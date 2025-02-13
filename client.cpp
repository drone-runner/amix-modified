/*AmiX - client.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <clocale>

#include <string>

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
#include "version.h"

#include "AmiXCommandlineParser.h"
#include "PolchatConnection.h"

#define EPUSECS 6000

WINDOW *stdscr;

int main(int argc, char *argv[])
{

reco:

  short nickStatus[2] = {0x0582, 0x0001};
  AmiXCommandlineParser options(argc, argv);

  part *ppart = NULL;

  // polchat.cpp globals
  askpassw = options.askPassw ? -1 : 0;
  cud = options.cud ? -1 : 0;
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

//  if (!options.latin2)
//  {
//    setlocale(LC_ALL, "");
//  }

  try
  {
    interface = new amixInterface(options.oldSchool);

    interface->nicklist_w = options.nickListWidth;
    interface->useattr = options.useAttr;
    interface->latin2 = options.latin2;
    interface->resize();

    interface->nl();
    interface->window_attron(COLOR_PAIR(7) | A_BOLD);
    interface->put(" " VER " ");
    interface->window_attroff(COLOR_PAIR(7) | A_BOLD);
    interface->nl();
    interface->put(" Linuxowy klient Polchatu");
    interface->nl();
    interface->put(" By ABUKAJ (J.M.Kowalski - amiga@buziaczek.pl)");
    interface->nl();
    interface->put(" status: freeware (badz giftware ;-D)");
    interface->nl();
    interface->put(" autor nie ponosi odpowiedzialnosci za ewentualne");
    interface->nl();
    interface->put(" szkody wywolane uzyciem programu w tej wersji");
    interface->nl();
    interface->put(" Oficjalna strona projektu: http://amix.org.pl");
    interface->nl();
    interface->put(" (Wersja zmodyfikowana pod Polfan, by DMB).");
    interface->nl();
    interface->nl();

    switch (rand() % 3)
    {
      case 0:
        interface->put(" Cieszmy się, że mamy 94Nata - to dzięki niej istnieje Polfan i AmiX wciąz daje radę.");
        interface->put(" To jest wersja zmodyfikowana pod Polfan przez DMB. Dobrej zabawy na Polfanie ^^.");
        break;

      case 1:
        interface->put(" Wyłącz komputer i wyjdź z domu.");
        interface->nl();
        interface->put(" Może pod twoim blokiem nawalają się dresiarze!?");
        break;

      case 2:
        interface->put("1f j00 (4n 1234d 7|-|15, j00 n33d 70 937 |41d");
        interface->put("3v3ryb0dy st4y c00l, th1s 1s Polfan!!!");
        break;
    }
    interface->nl();
    interface->print();
    interface->update();

//    if (cud)
//    {
//      interface->put(" Wersja zmodyfikowana pod Polfan nie wspiera aktualizacji.");
//      //checkupdate();
//    }
//    else
//    {
//      interface->nl();
//      interface->put(" Sprawdzanie uaktualnien pominiete.");
//    }
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
			    enterpollcnt = EPUSECS;
			    goto reco;
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
	  goto reco;
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
        else if (0 == ncsstrncmp(inputstring, "/help ", 6) || 0 == ncsstrncmp(inputstring, "/help", 6))
        {
          //TODO: retink it
          chatrooms.currentroom().msg("<B>/borg</B> - wysyla tekst: &quot;"
                        "I'm cybernetic organism - living tissue over metal endoskeleton.&quot;",
                        true);
          chatrooms.currentroom().msg("<B>/exit</B> - komenda analogiczna do polchatowego /quit, ale "
                        "dodatkowo konczy prace programu",
                        true);
          chatrooms.currentroom().msg("<B>/reco</B> - przerwij polaczenie i podlacz na nowo",
			true);
          chatrooms.currentroom().msg("<B>/help</B> - wyswietla tekst pomocy",
                        true);
          chatrooms.currentroom().msg("<B>/lama</B> - wysyla tekst: &quot;" 
                        "thankfully alert gauchos were able to save the llama "
                        "before it was swept into the blades of the turbine&quot;",
                        true);
          chatrooms.currentroom().msg("<B>/ver</B>  - podaje wersje programu", true); 
          interface->print();
        }
        else if ((0 == ncsstrncmp(inputstring, "/part ", 6) || 0 == ncsstrncmp(inputstring, "/part", 6)) && !(*(chatrooms.current)).room)
        {
          chatrooms.part((*(chatrooms.current)).name, true);
          interface->update_all();
        }         
        else
        {
          if (0 == ncsstrncmp(inputstring, "/lama ", 6) || 0 == ncsstrncmp(inputstring, "/lama", 6))
          {
            inputstring = "thankfully alert gauchos were able to save the llama"
                          "before it was swept into the blades of the turbine";
          }
          else if (0 == ncsstrncmp(inputstring, "/borg ", 6) || 0 == ncsstrncmp(inputstring, "/borg", 6))
          {
            inputstring = "I'm cybernetic organism - living tissue over metal endoskeleton.";
          }
          else if (0 == ncsstrncmp(inputstring, "/jedi ", 6) || 0 == ncsstrncmp(inputstring, "/jedi", 6))
          {
            inputstring = "May the Force be with you, my young padawan...";
          }         

          
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

      //if (!run)
      //{
      //  NOT A BUG BUT A FEATURE - it is not pleasant to escape without goodbye.
      //  connection->close();
      //}
    }
        
    delete connection;
    delete interface;
  }
  catch (std::exception& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
  }
 
  closelog(); 
  puts("AmiX: Koniec pracy na dzis, polecam sie na przyszlosc");
  return 0;
}
