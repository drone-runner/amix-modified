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
			else if (option == "-enterpoll" || upperoption == "ENTERPOLL")
			{
				this->enterpoll = true;
			}
			else if (option == "-fifo" || upperoption == "FIFO")
			{
				this->fifo = this->nextArg();
			}
			else
			{
				CommandlineParser::parseArg(option, upperoption);
			}
		}

	public:
		std::string fifo;
		std::string host;
		std::string room;
		std::string refFrom;
		std::string nick;
		std::string logFilename;
		//unsigned int port;
		std::string port;
		bool useAttr;
		unsigned int nickListWidth;
		bool askPassw;
		bool debug;
		bool coreDump;
		bool verbose;
		bool noHTML;
		bool bell;
		int antiidle;
		double period;
		LogState log;
		bool oldSchool;
		bool enterpoll;

		virtual void help(bool posix = true,
				FILE * fh = NULL,
				bool final = true)
		{
			if (fh == NULL)
			{
				fh = stderr;
			}

			CommandlineParser::help(posix, fh, false);

			fprintf(fh, "[-askpassw] [-nick <nick>] \\\n"
					"[-nicklistwidth <width of nicklist>] [-noattr] \\\n"
					"[-period <interval>] [-port <port>] [-room <room>] \\\n"
					"[-server <host>] [-log <log filename>] [-oldlog <logfilename>] \\\n"
					"[-antiidle <interval>] [-nohtml] [-old] [-enterpoll] -fifo <fifofile>%c",
					final ? '\n' : ' ');
		}

		AmiXCommandlineParser(int argc, char * argv[], bool final = true) :
			CommandlineParser(argc, argv, false)
	{
		this->host = "s1.polfan.pl";
#ifdef ABUSE
		this->room = "ABUSE";
		this->refFrom = "http://www.polfan.pl/app/?room=ABUSE";
#else
		this->room = "99fan";
		this->refFrom = "http://www.polfan.pl/app/?room=99fan";
#endif
		this->nick = "test";
		this->port = "14003"; // was int
		this->useAttr = true;
		this->nickListWidth = 30;
		this->askPassw = false;
		this->debug = false;
		this->coreDump = false;
		this->verbose = false;
		this->noHTML = false;
		this->bell = false;
		this->log = NOLOG;
		this->period = 1.0;
		this->antiidle = 0;
#ifdef ABUSE
		this->oldSchool = true;
#else
		this->oldSchool = false;
#endif
		this->enterpoll = false;
		this->fifo = "";

		if (final)
		{
			this->parse();
		}
	}
};

#endif

