/*AmiX - client.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <clocale>

#include <string>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>

#include <boost/asio.hpp>
#include <curses.h>

#include "polchat.h"
#include "polchat2.h"
#include "interface.h"
#include "temp.h"
#ifdef ABUSE
#include "abuse-version.h"
#else
#include "version.h"
#endif

#include "AmiXCommandlineParser.h"
#include "PolchatConnection.h"

#include "fifo.h"

#define EPUSECS 6000
#define MAXFIFOFILES 1024

WINDOW *stdscr;
extern bool fion = false;
extern int fifd = -1;
extern int c;

int main(int argc, char *argv[])
{

	short nickStatus[2] = {0x0582, 0x0001};
	AmiXCommandlineParser options(argc, argv);

	part *ppart = NULL;

	// polchat.cpp globals
	askpassw = options.askPassw ? -1 : 0;
	debug = options.debug ? -1 : 0;
	coredump = options.coreDump ? -1 : 0;
	verbose = options.verbose || options.coreDump ? -1 : 0;
	nohtml = options.noHTML ? -1 : 0;
	bell = options.bell ? -1 : 0;
	period = options.period;
	antiidle = options.antiidle;
	nick = options.nick;
	// abuse
	int enterpollcnt = EPUSECS;
	// fifo
	const char* fifo = options.fifo.c_str();

	if (strlen (fifo)) {
		(void)printf("Connect to fifo %s...\n", fifo);
		fion = true;
		fifd = open(fifo, O_NONBLOCK);
		if (fifd == -1) {
			abort();
		}
	}

	setlocale(LC_ALL, "");

	srand(time(NULL));

	switch (options.log)
	{
		case LOG:
			openlog(options.logFilename.c_str());
			break;

		case OLDLOG:
			openoldlog(options.logFilename.c_str());
			break;

		default:
			;
	}

	try
	{
		interface = new amixInterface(options.oldSchool);

		interface->nicklist_w = options.nickListWidth;
		interface->useattr = options.useAttr;
		interface->resize();

		interface->nl();
		interface->window_attron(COLOR_PAIR(7) | A_BOLD);
		interface->put(" " VER " ");
		interface->window_attroff(COLOR_PAIR(7) | A_BOLD);

		//witaski
		// szczala bumsik --->
#ifdef ABUSE
		interface->nl();
		interface->nl();
		interface->put("* DMB ABUSE VERSION *");
		interface->nl();
		interface->nl();
#endif
		interface->nl();
		interface->print();
		interface->update();

		if (askpassw)
		{
			pass = interface->input_password();
		}

		init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
		init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
		init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
		init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
		init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);

		boost::asio::io_service io_service;

		connection = new PolchatConnection(options.host, options.port, options.period);
		while (run || connected)
		{
			if (options.enterpoll) {
				--enterpollcnt;
				if (enterpollcnt <= 0) {
					delete connection;
					delete interface;
					closelog();
					execv(argv[0], argv);
				}
			}
			usleep(1);
			switch (connection->getState())
			{
				case PolchatConnection::DISCONNECTED:
					connected = 0;
					if (run)
					{
						connection->connect();
						//TODO: rethink reconnect
						//if (roomname != NULL)
						//{
						//  ppart = new part(nick, pass, roomname, "", reffrom);
						//}
						//else
						//{
						ppart = new part(nick, pass, options.room, "", options.refFrom);
						//}
						// is this possible to send on disconnected?
						connection->send(ppart);
					}
					break;

				case PolchatConnection::CONNECTED:
					connected = -1;
					if (NULL != (ppart = connection->getOnePart()))
					{
						processpart(ppart);
						delete ppart;
						ppart = NULL;
						interface->print();
					}

					if (last == 0)
					{
						last = time(NULL);
					}

					if (options.antiidle > 0)
					{
						if (connected && (options.antiidle < difftime(time(NULL), last)))
						{
							std::string noop = "/noop";
							part * tmp;
							if (NULL != (tmp = new part(noop)))
							{
								connection->send(tmp);
								last = time(NULL);
								antiidle_sent = true;
							}
						}
					}
					break;

				case PolchatConnection::CONNECTING:
				default:
					break;
			}

			std::string msg = connection->getReport();
			while (msg != "")
			{
				interface->nl();
				interface->put(msg.c_str());
				interface->print();
				interface->update();
				fprintf(stderr, "%s\n", msg.c_str());
				msg = connection->getReport();
			}

			interface->update();

			/*czy jest cos na wejsciu?*/
			if (!fion) {
				if (interface->console_input())
				{
					std::string inputstring = interface->get_input();
					if (0 == ncsstrncmp(inputstring, "/quit ", 6) || 0 == ncsstrncmp(inputstring, "/quit", 6))
					{
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/exit ", 6) || 0 == ncsstrncmp(inputstring, "/exit", 6))
					{
						run = 0;
						inputstring.replace(0, 5, "/quit");
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					if (0 == ncsstrncmp(inputstring, "/reco ", 6) || 0 == ncsstrncmp(inputstring, "/reco", 6))
					{
						delete connection;
						delete interface;
						closelog();
						execv(argv[0], argv);
					}
					else if (0 == ncsstrncmp(inputstring, "/busy ", 6) || 0 == ncsstrncmp(inputstring, "/busy", 6))
					{
						nickStatus[1] = 0x0001;
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/free ", 6) || 0 == ncsstrncmp(inputstring, "/free", 6))
					{
						nickStatus[1] = 0x0000;
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/status ", 8))
					{
						nickStatus[1] = atoi(&inputstring[8]);
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/info ", 6))
					{
						size_t first_nonspace = inputstring.find_first_not_of(' ', 5);
						size_t last_nonspace = inputstring.find_last_not_of(' ');

						if (first_nonspace != std::string::npos)
						{
							std::string infonick = inputstring.substr(first_nonspace, last_nonspace + 1 - first_nonspace);

							std::list<nicknode>::iterator it_end = chatrooms.currentroom().nicklist.end();
							for (std::list<nicknode>::iterator it = chatrooms.currentroom().nicklist.begin();
									it != it_end;
									it++)
							{
								if ((*it).nick == infonick)
								{
									chatrooms.currentroom().msg((*it).nick + " uses " + (*it).client, true);
									break;
								}
							}
						}
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/next ", 6) || 0 == ncsstrncmp(inputstring, "/next", 6))
					{
						chatrooms.next();
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/prev ", 6) || 0 == ncsstrncmp(inputstring, "/prev", 6))
					{
						chatrooms.prev();
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/debugon ", 9) || 0 == ncsstrncmp(inputstring, "/debugon", 9))
					{
						debug = -1;
						interface->nl();
						interface->put("DEBUG MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/debugoff ", 10) || 0 == ncsstrncmp(inputstring, "/debugoff", 10))
					{
						debug = 0;
						interface->nl();
						interface->put("DEBUG MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/verboseon ", 11) || 0 == ncsstrncmp(inputstring, "/verboseon", 11))
					{
						verbose = -1;
						interface->nl();
						interface->put("VERBOSE MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/verboseoff ", 12) || 0 == ncsstrncmp(inputstring, "/verboseoff", 12))
					{
						verbose = 0;
						interface->nl();
						interface->put("VERBOSE MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/coredumpon ", 12) || 0 == ncsstrncmp(inputstring, "/coredumpon", 12))
					{
						verbose = coredump = -1;
						interface->nl();
						interface->put("COREDUMP MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/coredumpoff ", 13) || 0 == ncsstrncmp(inputstring, "/coredumpoff", 13))
					{
						verbose = coredump = 0;
						interface->nl();
						interface->put("COREDUMP MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/ver ", 5) || 0 == ncsstrncmp(inputstring, "/ver", 5))
					{
						//TODO: rethink it 
						chatrooms.currentroom().msg("<B>" VER "</B>", true);
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/helpa ", 7) || 0 == ncsstrncmp(inputstring, "/helpa", 7))
					{
						//TODO: retink it
						chatrooms.currentroom().msg("<B>/exit</B> - EXIT ", true);
						chatrooms.currentroom().msg("<B>/reco</B> - RECONNECT", true);
						chatrooms.currentroom().msg("<B>/ver</B>  - VERSION", true); 
						interface->print();
					}
					else if ((0 == ncsstrncmp(inputstring, "/part ", 6) || 0 == ncsstrncmp(inputstring, "/part", 6)) && !(*(chatrooms.current)).room)
					{
						chatrooms.part((*(chatrooms.current)).name, true);
						interface->update_all();
					}         
					else
					{

						if ((*(chatrooms.current)).room)
						{
							ppart = new part(inputstring, chatrooms.currentroom().name);
						}
						else
						{
							inputstring = "/msg " + chatrooms.currentroom().name + " " + inputstring;
							ppart = new part(inputstring);
						}
						connection->sendDelayed(ppart);
					}
				} 
			} else {
				/*czy jest cos na wejsciu?*/
				getfactualkey();
				if (c != -1)
				{
					std::string inputstring = interface->get_input();
					if (0 == ncsstrncmp(inputstring, "/quit ", 6) || 0 == ncsstrncmp(inputstring, "/quit", 6))
					{
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/exit ", 6) || 0 == ncsstrncmp(inputstring, "/exit", 6))
					{
						run = 0;
						inputstring.replace(0, 5, "/quit");
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					if (0 == ncsstrncmp(inputstring, "/reco ", 6) || 0 == ncsstrncmp(inputstring, "/reco", 6))
					{
						delete connection;
						delete interface;
						closelog();
						execv(argv[0], argv);
					}
					else if (0 == ncsstrncmp(inputstring, "/busy ", 6) || 0 == ncsstrncmp(inputstring, "/busy", 6))
					{
						nickStatus[1] = 0x0001;
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/free ", 6) || 0 == ncsstrncmp(inputstring, "/free", 6))
					{
						nickStatus[1] = 0x0000;
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/status ", 8))
					{
						nickStatus[1] = atoi(&inputstring[8]);
						ppart = new part(2, 0, nickStatus);
						connection->send(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/info ", 6))
					{
						size_t first_nonspace = inputstring.find_first_not_of(' ', 5);
						size_t last_nonspace = inputstring.find_last_not_of(' ');

						if (first_nonspace != std::string::npos)
						{
							std::string infonick = inputstring.substr(first_nonspace, last_nonspace + 1 - first_nonspace);

							std::list<nicknode>::iterator it_end = chatrooms.currentroom().nicklist.end();
							for (std::list<nicknode>::iterator it = chatrooms.currentroom().nicklist.begin();
									it != it_end;
									it++)
							{
								if ((*it).nick == infonick)
								{
									chatrooms.currentroom().msg((*it).nick + " uses " + (*it).client, true);
									break;
								}
							}
						}
						ppart = new part(inputstring);
						connection->sendDelayed(ppart);
					}
					else if (0 == ncsstrncmp(inputstring, "/next ", 6) || 0 == ncsstrncmp(inputstring, "/next", 6))
					{
						chatrooms.next();
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/prev ", 6) || 0 == ncsstrncmp(inputstring, "/prev", 6))
					{
						chatrooms.prev();
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/debugon ", 9) || 0 == ncsstrncmp(inputstring, "/debugon", 9))
					{
						debug = -1;
						interface->nl();
						interface->put("DEBUG MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/debugoff ", 10) || 0 == ncsstrncmp(inputstring, "/debugoff", 10))
					{
						debug = 0;
						interface->nl();
						interface->put("DEBUG MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/verboseon ", 11) || 0 == ncsstrncmp(inputstring, "/verboseon", 11))
					{
						verbose = -1;
						interface->nl();
						interface->put("VERBOSE MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/verboseoff ", 12) || 0 == ncsstrncmp(inputstring, "/verboseoff", 12))
					{
						verbose = 0;
						interface->nl();
						interface->put("VERBOSE MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/coredumpon ", 12) || 0 == ncsstrncmp(inputstring, "/coredumpon", 12))
					{
						verbose = coredump = -1;
						interface->nl();
						interface->put("COREDUMP MODE ON");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/coredumpoff ", 13) || 0 == ncsstrncmp(inputstring, "/coredumpoff", 13))
					{
						verbose = coredump = 0;
						interface->nl();
						interface->put("COREDUMP MODE OFF");
						interface->print();
					}
					else if (0 == ncsstrncmp(inputstring, "/ver ", 5) || 0 == ncsstrncmp(inputstring, "/ver", 5))
					{
						//TODO: rethink it 
						chatrooms.currentroom().msg("<B>" VER "</B>", true);
						interface->print();
					}
					else if ((0 == ncsstrncmp(inputstring, "/part ", 6) || 0 == ncsstrncmp(inputstring, "/part", 6)) && !(*(chatrooms.current)).room)
					{
						chatrooms.part((*(chatrooms.current)).name, true);
						interface->update_all();
					}         
					else
					{
						if ((*(chatrooms.current)).room)
						{
							ppart = new part(inputstring, chatrooms.currentroom().name);
						}
						else
						{
							inputstring = "/msg " + chatrooms.currentroom().name + " " + inputstring;
							ppart = new part(inputstring);
						}
						connection->sendDelayed(ppart);
					}
				} 

			}
		}
		//if (!run)
		//{
		//  NOT A BUG BUT A FEATURE - it is not pleasant to escape without goodbye.
		//  connection->close();
		//}

		delete connection;
		delete interface;
	}
	catch (std::exception& e)
	{
		fprintf(stderr, "Exception: %s\n", e.what());
	}

	closelog(); 
	puts("QUITTED");
	return 0;
}
