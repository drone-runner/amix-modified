/*AmiX - AmiXCommandlineParser.h - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_AMIXCOMMANDLINEPARSER_H
#define AMIX_AMIXCOMMANDLINEPARSER_H

#include <cstdio>

#include <string>

#include "CommandlineParser.h"

enum LogState {NOLOG, LOG, OLDLOG};

class AmiXCommandlineParser : public CommandlineParser
{
  protected:
    void portParse()
    {
      const char * onerror = "ERROR: Port must be a natural number no greater"
                             " than 65535.\n";
      unsigned long port = this->ulParse(onerror);
      if (port > 65535)
      {
        fputs(onerror, stderr);
        this->parseError = true;
      }
      else
      {
        this->port = port;
      }
    }

    virtual void parseArg(std::string & option, std::string & upperoption)
    {
      if (option == "-askpassw" || upperoption == "ASKPASSW")
      {
        this->askPassw = true;
      }
      else if (option == "-noattr" || upperoption == "NOATTR")
      {
        this->useAttr = false;
      }
      else if (option == "-latin2" || upperoption == "LATIN2")
      {
        this->latin2 = true;
      }
      else if (option == "-checkupdates" || upperoption == "CHECKUPDATES")
      {
        this->cud = true;
      }
      else if (option == "-debug" || upperoption == "DEBUG")
      {
        this->debug = true;
      }
      else if (option == "-coredump" || upperoption == "COREDUMP")
      {
        this->coreDump = true;
      }
      else if (option == "-verbose" || upperoption == "VERBOSE")
      {
        this->verbose = true;
      }
      else if (option == "-nohtml" || upperoption == "NOHTMLFORMATTING"
               || option == "-nohtmlformatting" || upperoption == "NOHTML")
      {
        this->noHTML = true;
      }
      else if (option == "-bell" || upperoption == "BELL")
      {
        this->bell = true;
      }
      else if (option == "-reffrom" || upperoption == "REFFROM")
      {
        this->refFrom = this->nextArg();
      }
      else if (option == "-room" || upperoption == "ROOM")
      {
        this->room = this->nextArg();
      }
      else if (option == "-server" || upperoption == "SERVER")
      {
        this->host = this->nextArg();
      }
      else if (option == "-nick" || upperoption == "NICK")
      {
        this->nick = this->nextArg();
      }
      else if (option == "-port" || upperoption == "PORT")
      {
        //this->portParse();
        this->port = this->nextArg();
      }
      else if (option == "-nicklistwidth" || upperoption == "NICKLISTWIDTH")
      {
        this->nickListWidth = this->plParse("ERROR: Nick list width must be a "
                                            "positive integer.\n");
      }
      else if (option == "-antiidle" || upperoption == "ANTIIDLE")
      {
        this->antiidle = this->plParse("ERROR: Antiidle period must be a "
                                       "positive integer.\n");
      }
      else if (option == "-period" || upperoption == "PERIOD")
      {
        this->period = this->udParse("ERROR: antiflood period width must be "
                                     "greater or equal to 0.\n");
      }
      else if (option == "-log" || upperoption == "LOG")
      {
        this->log = LOG;
        this->logFilename = this->nextArg();
      }
      else if (option == "-oldlog" || upperoption == "OLDLOG")
      {
        this->log = OLDLOG;
        this->logFilename = this->nextArg();
      }
      else if (option == "-old" || upperoption == "OLD")
      {
        this->oldSchool = true;
      }
      else
      {
        CommandlineParser::parseArg(option, upperoption);
      }
    }

  public:
    std::string host;
    std::string room;
    std::string refFrom;
    std::string nick;
    std::string logFilename;
    //unsigned int port;
    std::string port;
    bool useAttr;
    bool latin2;
    unsigned int nickListWidth;
    bool askPassw;
    bool cud;
    bool debug;
    bool coreDump;
    bool verbose;
    bool noHTML;
    bool bell;
    int antiidle;
    double period;
    LogState log;
    bool oldSchool;

    virtual void help(bool posix = true,
                      FILE * fh = NULL,
                      bool final = true)
    {
      if (fh == NULL)
      {
        fh = stderr;
      }

      CommandlineParser::help(posix, fh, false);

      if (posix)
      {
        fprintf(fh, "[-askpassw] [-nick <nick>] \\\n"
                    "[-nicklistwidth <width of nicklist>] [-noattr] \\\n"
                    "[-period <interval>] [-port <port>] [-room <room>] \\\n"
                    "[-server <host>] [-log <log filename>] [-oldlog "
                    "<logfilename>] \\\n"
                    "[-antiidle <interval>] [-nohtml] [-latin2] [-old]%c",
                    final ? '\n' : ' ');
      }
      else
      {
        fprintf(fh, "SERVER/K,PORT/K/N,ROOM/K,NICK/K,DEBUG/S,VERBOSE/S,"
                    "COREDUMP/S,BELL/S,PERIOD/K/N,NICKLISTWIDTH/K/N,NOATTR/S,"
                    "ASKPASSW/S,CHECKUPDATES/S,LOG/K,OLDLOG/K,ANTIIDLE/K/N,"
                    "NOHTML/S,REFFROM/K,LATIN2/S,OLD/S%c", final ? '\n' : ',');
      }
    }

    AmiXCommandlineParser(int argc, char * argv[], bool final = true) :
        CommandlineParser(argc, argv, false)
    {
      this->host = "s1.polfan.pl";
      this->room = "AI";
      this->refFrom = "http://www.polfan.pl/app/?room=AI";
      this->nick = "Uzytkownik_AmiX";
      this->port = "14003"; // was int
      this->useAttr = true;
      this->latin2 = false;
      this->nickListWidth = 30;
      this->askPassw = false;
      this->cud = false;
      this->debug = false;
      this->coreDump = false;
      this->verbose = false;
      this->noHTML = false;
      this->bell = false;
      this->log = NOLOG;
      this->period = 1.0;
      this->antiidle = 0;
      this->oldSchool = false;

      if (final)
      {
        this->parse();
      }
    }
};

#endif

