/*AmiX - polchat.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <boost/asio.hpp>
#include <curses.h>

#include "temp.h"
#include "polchat.h"
#include "interface.h"
#ifdef ABUSE
#include "abuse-version.h"
#else
#include "version.h"
#endif

int debug = 0;
int verbose = 0;
int nohtml = 0;
int coredump = 0;
int bell = 0;
int run = -1;
int connected = 0;
int askpassw = 0;
std::string pass = "";
std::string nick = "Uzytkownik_AmiX";
std::string fifo = "";
int antiidle = 0;
bool antiidle_sent = false;
time_t last = 0;

double period = 1.0;
FILE *logfh = NULL;
short nickn;
chat chatrooms;

int colourt[8] = {COLOR_BLACK, COLOR_BLUE, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_BLACK, COLOR_CYAN, COLOR_BLUE};
int colourop = COLOR_RED;

bool enterpoll = false;

nicknode::nicknode(std::string & nn, std::string & nc, short ns)
{
  this->nick = nn;
  this->client = nc;
  this->status = ns;
}

line::line(std::string & str)
{
  char buffer[15];
  time_t tmp;

  tmp = time(NULL);
  strftime(buffer, 14, "%H:%M:%S ", localtime(&tmp));

  this->timestring = buffer;
  this->text = str;
}

void chatroom::addnick(std::string & nick,
                       std::string & client,
                       short status)
{
  std::list<nicknode>::iterator it;
  bool found = false;
  
  for (it = this->nicklist.begin(); 
       it != this->nicklist.end();
       it++)
  {
    int res = ncsstrcmp(nick.c_str(), (*it).nick.c_str());

    if (res <= 0)
    {
      found = res == 0;
      break;
    }
  }

  if (!found)
  {
    this->nicklist.insert(it, nicknode(nick, client, status));
  }
  else
  {
    if (debug)
    {
      interface->nl();
      interface->put("NICK: ");
      interface->put(nick.c_str());
      interface->put(" already found in room: ");
      interface->put(this->name.c_str());
    }
  }
}

void chatroom::updatenick(std::string & nick,
                          short status)
{
  //bool found = false;
  std::list<nicknode>::iterator it;
  
  for (it = this->nicklist.begin(); 
       it != this->nicklist.end();
       it++)
  {
    int res = ncsstrcmp(nick.c_str(), (*it).nick.c_str());

    if (res == 0)
    {
      //found = true;
      (*it).status = status;
      break;
    }

    if (res < 0)
    {
      break;
    }
  }

  //updatenick message is sent BEFORE nick entered and AFTER nick left
  //if (!found && debug)
  //{
  //  interface->nl();
  //  interface->put("(updatenick) NICK: ");
  //  interface->put(nick.c_str());
  //  interface->put(" not found in room: ");
  //  interface->put(this->name.c_str());
  //}
}


void chatroom::removenick(std::string & nick)
{
  bool found = false;
  std::list<nicknode>::iterator it;
  
  for (it = this->nicklist.begin(); 
       it != this->nicklist.end();
       it++)
  {
    int res = ncsstrcmp(nick.c_str(), (*it).nick.c_str());

    if (res == 0)
    {
      found = true;
      this->nicklist.erase(it);
      break;
    }

    if (res < 0)
    {
      break;
    }
  }

  if (!found && debug)
  {
    interface->nl();
    interface->put("(removenick) NICK: ");
    interface->put(nick.c_str());
    interface->put(" not found in room: ");
    interface->put(this->name.c_str());
  }
}


std::string chatroom::getnickbyprefix(char * prefix, int len)
{
  for (std::list<nicknode>::iterator it = this->nicklist.begin(); 
       it != this->nicklist.end();
       it++)
  {
    int res = ncsstrncmp(prefix, (*it).nick.c_str(), len);

    if (res == 0)
    {
      return (*it).nick;
    }

    if (res < 0)
    {
      break;
    }
  }
  return std::string(prefix, len);
}

void chatroom::addline(std::string text)
{
  this->lines.push_back(line(text));
  if (this->lines.size() > MSGSTOREMAX)
  {
    this->lines.pop_front();
  }
}


void chat::join(std::string name)
{
  if (this->room.count(name) != 0)
  {
    if (debug)
    {
      this->currentroom().msg("YOU ARE ALREADY IN ROOM: " + name, true);
    }
    return;
  }
  this->roomlist.push_back(roomname(name));
  this->room[name] = chatroom(name);
}

void chat::part(std::string name, bool priv=false)
{
  if (priv)
  {
    this->priv.erase(name);
  }
  else
  {
    this->room.erase(name);
  }

  roomname toremove = roomname(name, !priv);

  if (toremove == *(this->current))
  {
    this->current = this->roomlist.erase(this->current);
  }
  else
  {
    this->roomlist.remove(toremove);
  }

  if (this->current == this->roomlist.end())
  {
    this->current--;
  }
}

void chat::prev()
{
  if (this->current != this->roomlist.begin())
  {
    this->current--;
  }
  interface->update_all();
}

void chat::next()
{
  this->current++;
  if (this->current == this->roomlist.end())
  {
    this->current--;
  }
  interface->update_all();
}

void chatroom::msg(std::string msg, bool display)
{
  this->addline(msg);
  if (display)
  {
    line & tmp = this->lines.back();
    interface->nl();
    interface->put(tmp.timestring.c_str());
    interface->printpol(tmp.text.c_str());
  }
}


void chat::privmsg(std::string name, std::string msg)
{
  std::string lowname = lowercase(name);

  if (this->priv.count(lowname) == 0)
  {
    std::string tmp = "Private chat with " + name;
    this->priv[lowname] = chatroom(lowname, tmp);
    this->roomlist.push_back(roomname(lowname, false));

    //TODO: refactoring
    interface->printtitle();
  }
  this->priv[lowname].msg(msg, !(*(this->current)).room && (*(this->current)).name == lowname);
}

void chat::roommsg(std::string name, std::string msg)
{
  if (this->room.count(name) == 0)
  {
    if (debug)
    {
      this->currentroom().msg("YOU ARE NOT IN ROOM: " + name, true);
    }
    return;
  }
  else
  {
    this->room[name].msg(msg, (*(this->current)).room && (*(this->current)).name == name);
  }
}


int openlog(const char *logfn)
{
  closelog();
  logfh = fopen(logfn, "w");
  if (logfh != NULL)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}


int openoldlog(const char *logfn)
{
  closelog();
  logfh = fopen(logfn, "a");
  if (logfh != NULL)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}



void closelog()
{
  if (logfh != NULL)
  {
    fclose(logfh);
    logfh = NULL;
  }
}


int printlog(const char *descr, std::string & msg, std::string & room)
{
  time_t t;
  static char buffer[15];
  if (logfh != NULL)
  {
    t = time(NULL);
    strftime(buffer, 14, "%H:%M:%S ", localtime(&t));

    fputs("<div class=\"amix_log\">\n <span class=\"amix_log_date\">\n  ", logfh);
    fputs(buffer, logfh);
    fputs("\n </span>\n <span class=\"amix_log_desc\">\n  ", logfh);
    fputs(descr, logfh);
    fputs("\n </span>\n <span class=\"amix_log_room\">\n  ", logfh);
    fputs(room.c_str(), logfh);
    fputs("\n </span>\n <span class=\"amix_log_msg\">\n  ", logfh);
    fputs(msg.c_str(), logfh);
    fputs("\n </span>\n</div>\n", logfh);
  }
  return -1; /*nie chce mi sie sprawdzac czy nie bylo EOF*/
}
