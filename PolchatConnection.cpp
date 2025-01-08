/*AmiX - PolchatConnection.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <ctime>

#include <string>
#include <list>

//libboost-thread-dev
//libboost-system-dev
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "polchat1.h"
#include "polchat2.h"
#include "Renderable.h"
#include "PolchatConnection.h"

PolchatConnection * connection = NULL;

using boost::asio::ip::tcp;

void PolchatConnection::setState(ConnectionState s)
{
  this->stateMutex.lock();
  this->state = s;
  this->stateMutex.unlock();
}

void PolchatConnection::report(std::string msg)
{
  this->reportsMutex.lock();
  this->reports.push_back(msg);
  this->reportsMutex.unlock();
}

void PolchatConnection::queueDelayed(Renderable * msg)
{
  if (this->interval > 0)
  {
    bool only = this->delayed.empty();
    this->delayed.push_back(msg);
    if (only && this->running)
    {
      this->timer->expires_from_now(boost::posix_time::millisec(this->interval));
      this->timer->async_wait(boost::bind(&PolchatConnection::handleDelayed,
                                          this,
                                          boost::asio::placeholders::error));
    }
  }
  else
  {
    this->queueWrite(msg);
  }
}

void PolchatConnection::handleDelayed(const boost::system::error_code & error)
{
  if (error)
  {
    this->report("HANDLE DELAYED QUEUE - aborted.");
    return;
  }

  if (this->delayed.empty())
  {
    this->report("HANDLE EMPTY DELAYED QUEUE - Som-Ting-Wong!");
    return;
  }

  Renderable * msg = this->delayed.front();
  this->delayed.pop_front();
  this->queueWrite(msg);

  if (this->running && !this->delayed.empty())
  {
    this->timer->expires_from_now(boost::posix_time::millisec(this->interval));
    this->timer->async_wait(boost::bind(&PolchatConnection::handleDelayed,
                                        this,
                                        boost::asio::placeholders::error));
  }
}

void PolchatConnection::handleConnect(const boost::system::error_code & error,
                                      tcp::resolver::iterator iterator)
{
  this->report("Connect handling.");

  if (!error)
  {
    this->setState(CONNECTED);
    this->report("Connected. Listenning...");
    boost::asio::async_read(*this->socket,
                            boost::asio::buffer(this->lenBuffer,
                                                4),
                            boost::bind(&PolchatConnection::handleReadLength,
                                        this,
                                        boost::asio::placeholders::error));
  }
  else
  {
    this->report("Not connected.");
    this->socket->close();

    if (iterator != tcp::resolver::iterator())
    {
      this->report("Resolving...");
      tcp::endpoint endpoint = *iterator++;
      this->socket->async_connect(endpoint,
                                 boost::bind(&PolchatConnection::handleConnect,
                                             this,
                                             boost::asio::placeholders::error,
                                             iterator));
    }
//        else if (this->reconnect && this->running)
//        {
//          this->report("Retrying...");
//          this->connect();
//        }
    else
    {
      this->setState(DISCONNECTED);
    }
  }
}

void PolchatConnection::handleReadLength(const boost::system::error_code& error)
{
  if (!error)
  {
    this->length = ((this->lenBuffer[0] * 256 + this->lenBuffer[1]) * 256
                    + this->lenBuffer[2]) * 256 + this->lenBuffer[3] - 4;
    if (this->length == 0)
    {
      // empty tank ^^
      this->report("ERROR: Empty tank received.\n");

      if (this->running)
      {
        boost::asio::async_read(*this->socket,
                                boost::asio::buffer(this->lenBuffer,
                                                    4),
                                boost::bind(&PolchatConnection::handleReadLength,
                                            this,
                                            boost::asio::placeholders::error));
      }
    }
    else
    {
      this->buffer = new unsigned char [this->length];
      boost::asio::async_read(*this->socket,
                              boost::asio::buffer(this->buffer,
                                                  this->length),
                              boost::bind(&PolchatConnection::handleReadData,
                                          this,
                                          boost::asio::placeholders::error));
    }
  }
  else
  {
    this->handleError(error);
  }
}

void PolchatConnection::handleReadData(const boost::system::error_code& error)
{
  if (!error)
  {
    this->receivedMutex.lock();
    tank * tmpTank = new tank(this->length,
                              (char *) this->buffer,
                              false);
    if (tmpTank != NULL)
    {
      part * tmpPart = new part(*tmpTank);
      if (tmpPart != NULL)
      {
        if (tmpPart->headerlen == 0x0001
            && tmpPart->nstrings == 0x0000
            && tmpPart->header[0] == 0x0001)
        {
          connection->send(new part(0, 0));
        }

        this->received->push_back(tmpPart);
      }
      else
      {
        this->report("ERROR: Unable to allocate part.\n");
        delete tmpTank;
      }
    }
    else
    {
      this->report("ERROR: Unable to allocate tank.\n");
      delete [] this->buffer;
    }

    this->buffer = NULL;
    this->receivedMutex.unlock();

    if (this->running)
    {
      boost::asio::async_read(*this->socket,
                              boost::asio::buffer(this->lenBuffer,
                                                  4),
                              boost::bind(&PolchatConnection::handleReadLength,
                                          this,
                                          boost::asio::placeholders::error));
    }
  }
  else
  {
    delete [] this->buffer;
    this->buffer = NULL;
    this->handleError(error);
  }
}

void PolchatConnection::orderWrite()
{
  if (this->running)
  {
    unsigned long len = 0;
    for (std::list<Renderable *>::iterator it = this->toSend.begin();
         it != this->toSend.end();
         it++)
    {
      len += (*it)->size();
    }
    if (len > 0)
    {
      this->writeBuffer = new unsigned char [len];

      for (unsigned char * ptr = this->writeBuffer;
           !this->toSend.empty();
           this->toSend.pop_front())
      {
        Renderable * msg = this->toSend.front();
        ptr += msg->render(ptr);
        delete msg;
      }

      boost::asio::async_write(*this->socket,
                               boost::asio::buffer(this->writeBuffer,
                                                   len),
                               boost::bind(&PolchatConnection::handleWrite,
                                           this,
                                           boost::asio::placeholders::error));
    }
  }
}

void PolchatConnection::queueWrite(Renderable * msg)
{
  bool only = this->toSend.empty();
  this->toSend.push_back(msg);
  if (only)
  {
    this->orderWrite();
  }
}

void PolchatConnection::handleWrite(const boost::system::error_code& error)
{
  delete [] this->writeBuffer;
  this->writeBuffer = NULL;
  if (!error)
  {
    //delete this->toSend.front();
    //this->toSend.pop_front();
    if (!this->toSend.empty())
    {
      this->orderWrite();
    }
  }
  else
  {
    this->handleError(error);
  }
}

void PolchatConnection::handleError(const boost::system::error_code& error)
{
  this->setState(DISCONNECTED);
  this->report("An error has occured.");

  while (!this->toSend.empty())
  {
    delete this->toSend.front();
    this->toSend.pop_front();
  }

  this->timer->cancel();
  while (!this->delayed.empty())
  {
    delete this->delayed.front();
    this->delayed.pop_front();
  }
}

void PolchatConnection::stop()
{
  this->report("STOP SIGNAL RECEIVED.");
  this->running = false;
  this->timer->cancel();
}

void PolchatConnection::run()
{
  this->report("Listening for ASIO events.");
  this->io_service.run();
  while (this->running)
  {
    this->report("No ASIO events for the loop.");
    this->io_service.reset();
    boost::asio::deadline_timer t(this->io_service,
                                  boost::posix_time::seconds(1));
    t.wait();
    this->report("Waiting for ASIO events.");
    this->io_service.run();
  }
  this->report("Stopped ASIO events listening loop.");
  if (this->getState() != DISCONNECTED)
  {
    this->report("Oops... Not disconnected?");
  }
  this->socket->close();
  this->setState(DISCONNECTED);
}

PolchatConnection::PolchatConnection(std::string host,
                                     std::string port,
                                     double interval)
{
  this->host = host;
  this->port = port;
  this->interval = interval <= 0 ? 0 : (unsigned long) (1000 * interval);
//      this->reconnect = reconnect;
  this->running = true;
  this->writeBuffer = NULL;
  this->buffer = NULL;
  this->received = new std::list<part *>();
  this->query = NULL;
  this->socket = new tcp::socket(this->io_service);
  this->resolver = NULL; //tcp::resolver(this->io_service);
  this->thread = NULL;
  this->timer = new boost::asio::deadline_timer(this->io_service);

  this->setState(DISCONNECTED);
  //this->connect();
}

void PolchatConnection::connect()
{
  if (this->getState() == DISCONNECTED)
  {
    // WARNING: query & iterator would be deleted after the function
    // has finished
    //tcp::resolver::query query(this->host, this->port);
    this->setState(CONNECTING);
    this->report("Connecting...");

    if (this->resolver != NULL)
    {
      delete this->resolver;
    }
    this->resolver = new tcp::resolver(this->io_service);

    if (this->query != NULL)
    {
      delete this->query;
    }
    this->query = new tcp::resolver::query(this->host, this->port);

    tcp::resolver::iterator iterator = this->resolver->resolve(*this->query);
    tcp::endpoint endpoint = *iterator++;
    this->socket->async_connect(endpoint,
                                boost::bind(&PolchatConnection::handleConnect,
                                            this,
                                            boost::asio::placeholders::error,
                                            iterator));
    if (this->thread == NULL)
    {
      this->running = true;
      this->thread = new boost::thread(boost::bind(&PolchatConnection::run,
                                                   this));
    }
  }
  else
  {
    this->report("CONNECT WHEN NOT DISCONNECTED!!!");
  }
}

PolchatConnection::~PolchatConnection()
{
  this->close();
  fputs("THREAD REMOVED\n", stderr);

  if (this->timer != NULL)
  {
    this->timer->cancel();
    delete this->timer;
  }
  fputs("TIMER REMOVED\n", stderr);

  while (!this->delayed.empty())
  {
    delete this->delayed.front();
    this->delayed.pop_front();
  }
  fputs("delayed queue flushed\n", stderr);

  if (this->socket != NULL)
  {
    delete this->socket;
  }
  fputs("SOCKET DELETED\n", stderr);

  if (this->resolver != NULL)
  {
    delete this->resolver;
  }
  fputs("RESOLVER DELETED\n", stderr);

  if (this->query != NULL)
  {
    delete this->query;
  }
  fputs("QUERY DELETED\n", stderr);

  while (!this->toSend.empty())
  {
    delete this->toSend.front();
    this->toSend.pop_front();
  }
  fputs("toSend queue flushed\n", stderr);

  if (this->received != NULL)
  {
    while (!this->received->empty())
    {
      delete this->received->front();
      this->received->pop_front();
    }
    delete this->received;
  }
  fputs("received queue flushed and deleted\n", stderr);

  if (this->writeBuffer != NULL)
  {
    delete [] this->writeBuffer;
  }
  fputs("writeBuffer deleted\n", stderr);

  if (this->buffer != NULL)
  {
    delete [] this->buffer;
  }
  fputs("buffer deleted\n", stderr);
}

void PolchatConnection::send(Renderable * msg)
{
  this->io_service.post(boost::bind(&PolchatConnection::queueWrite,
                                    this,
                                    msg));
}

void PolchatConnection::sendDelayed(Renderable * msg)
{
  this->io_service.post(boost::bind(&PolchatConnection::queueDelayed,
                                    this,
                                    msg));
}

tank * PolchatConnection::getOneTank()
{
  tank * ptr;

  this->receivedMutex.lock();
  if (this->received->empty())
  {
    ptr = NULL;
  }
  else
  {
    ptr = this->received->front()->source;
    this->received->front()->source = NULL;
    delete this->received->front();
    this->received->pop_front();
  }
  this->receivedMutex.unlock();
  return ptr;
}

part * PolchatConnection::getOnePart()
{
  part * ptr;

  this->receivedMutex.lock();
  if (this->received->empty())
  {
    ptr = NULL;
  }
  else
  {
    ptr = this->received->front();
    this->received->pop_front();
  }
  this->receivedMutex.unlock();
  return ptr;
}

std::list<part *> * PolchatConnection::getAllParts()
{
  std::list<part *> * ptr;

  this->receivedMutex.lock();
  ptr = this->received;
  this->received = new std::list<part *>();
  this->receivedMutex.unlock();
  return ptr;
} 

void PolchatConnection::close()
{
  if (this->thread != NULL)
  {
    this->report("STOP SIGNAL SEND.");
    this->io_service.post(boost::bind(&PolchatConnection::stop,
                                      this));
    this->thread->join();
    delete this->thread;
    this->thread = NULL;
  }
}

std::string PolchatConnection::getReport()
{
  std::string msg = "";
  this->reportsMutex.lock();
  if (!this->reports.empty())
  {
    msg = this->reports.front();
    this->reports.pop_front();
  }
  this->reportsMutex.unlock();
  return msg;
}

PolchatConnection::ConnectionState PolchatConnection::getState()
{
  this->stateMutex.lock();
  ConnectionState result = this->state;
  this->stateMutex.unlock();
  return result;
}

