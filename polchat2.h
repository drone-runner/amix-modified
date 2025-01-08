/*AmiX - polchat2.h - v. 0.2 - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_POLCHAT2_H
#define AMIX_POLCHAT2_H

#include <string>

#include "polchat1.h"
#include "Renderable.h"
#include "interface.h"

#define POLCHAT_BAD_PASSWORD_MSG "nieprawidłowe hasło lub identyfikator użytkownika"
#define POLCHAT_NOOP_REPLY_MSG "<font color=red>** nieznana komenda <b>/noop</b></font>"
#define POLCHAT_NICK_BUSY_MSG "wybrany identyfikator jest aktualnie w użyciu"

class part : public Renderable
{
  public:
    short headerlen;
    short nstrings;
    short * header;
    std::string * strings;
    tank * source;

    part(tank &);
    //part(const char *);
    part(std::string &);
    //part(const char *, std::string &);
    part(std::string &, std::string &);
    part(std::string, std::string, std::string, std::string, std::string);
    part(unsigned int, unsigned int, short * = NULL, std::string * = NULL);
    virtual unsigned long render(unsigned char * &);
    virtual unsigned long size();
    virtual ~part();
    void dump(amixInterface *, bool = false);
};

void processpart(part *);

#endif
