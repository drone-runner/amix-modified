/*AmiX - PolchatConnection.h - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_POLCHATCONNECTION_H
#define AMIX_POLCHATCONNECTION_H

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

using boost::asio::ip::tcp;

class PolchatConnection
{
  public:
    enum ConnectionState {CONNECTING, CONNECTED, DISCONNECTED};

  private:
//    bool reconnect;
    bool running;
    std::list<Renderable *> toSend;
    std::list<part *> * received;
    boost::mutex receivedMutex;
    std::string host;
    std::string port;
    boost::asio::io_service io_service;
    tcp::resolver * resolver;
    boost::thread * thread;
    tcp::socket * socket;
    unsigned char lenBuffer[4];
    unsigned char * buffer;
    unsigned long length;
    unsigned char * writeBuffer;
    tcp::resolver::query * query;
    std::list<std::string> reports;
    boost::mutex reportsMutex;
    ConnectionState state;
    boost::mutex stateMutex;
    std::list<Renderable *> delayed;
    unsigned long interval;
    boost::asio::deadline_timer * timer;

    void setState(ConnectionState);
    void report(std::string);
    void queueDelayed(Renderable *);
    void handleDelayed(const boost::system::error_code &);
    void handleConnect(const boost::system::error_code &,
                       tcp::resolver::iterator);
    void handleReadLength(const boost::system::error_code &);
    void handleReadData(const boost::system::error_code &);
    void orderWrite();
    void queueWrite(Renderable *);
    void handleWrite(const boost::system::error_code &);
    void handleError(const boost::system::error_code &);
    void stop();
    void run();

  public:
    PolchatConnection(std::string, std::string, double = 0);
    void connect();
    ~PolchatConnection();
    void send(Renderable *);
    void sendDelayed(Renderable *);
    tank * getOneTank();
    part * getOnePart();
    std::list<part *> * getAllParts();
    void close();
    std::string getReport();
    ConnectionState getState();
};

extern PolchatConnection * connection;

#endif
