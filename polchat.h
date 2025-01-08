/*AmiX - polchat.h - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_POLCHAT_H
#define AMIX_POLCHAT_H

#include <ctime>

#include <string>
#include <map>
#include <list>

#define MSGSTOREMAX 100

class nicknode
{
  public:
    short status;
    std::string nick;
    std::string client;

    nicknode(std::string &, std::string &, short);
};

class line
{
  public:
    std::string timestring;
    std::string text;

    line(std::string &);
};

class chatroom
{
  public:
    std::string name;
    std::string description;
    std::list<nicknode> nicklist;
    std::list<line> lines;

    chatroom(std::string n, std::string d="")
    {
      this->name = n;
      this->description = d;
    }

    chatroom()
    {
      //dummy;
    }

    void addnick(std::string &, std::string &, short);
    void removenick(std::string &);
    void updatenick(std::string &, short);
    std::string getnickbyprefix(char *, int);
    void addline(std::string);
    void msg(std::string, bool);
    void setdesc(std::string desc)
    {
      this->description = desc;
    }
};

class roomname
{
  public:
    std::string name;
    bool room;

    roomname(std::string & n, bool r=true)
    {
      this->name = n;
      this->room =r;
    }

    bool operator==(roomname cmp)
    {
      return cmp.name == this->name && cmp.room == this->room;
    }
};

class chat
{
  public:
    std::map<std::string, chatroom> room;
    std::map<std::string, chatroom> priv;
    std::list<roomname> roomlist;
    std::list<roomname>::iterator current;
    chatroom none;

    chat()
    {
      this->join("");
      this->room[""].setdesc("Polchat system room");
      this->current = this->roomlist.begin();
      this->none = chatroom("NOT CONNECTED", "not connected yet");
    }

    void join(std::string);
    void part(std::string, bool);
    void privmsg(std::string, std::string);
    void roommsg(std::string, std::string);
    void prev();
    void next();

    chatroom & currentroom()
    {
      if (this->current == this->roomlist.end())
      {
        return this->none;
      }
      roomname & tmp = *(this->current);
      if (tmp.room)
      {
        return this->room[tmp.name];
      }
      return this->priv[tmp.name];
    }
};

int openlog(const char *);
int openoldlog(const char *);
void closelog();
int printlog(const char *, std::string &, std::string &);

extern int cud;
extern int debug;
extern int verbose;
extern int nohtml;
extern int coredump;
extern int bell;
extern int run;
extern int connected;
extern int antiidle;
extern bool antiidle_sent;
extern time_t last;
extern double period;
extern int askpassw;
extern std::string pass;
extern std::string nick;
extern int colourt[];
extern int colourop;
extern short nickn;

extern chat chatrooms;

#endif
