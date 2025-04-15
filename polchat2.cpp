/*AmiX - polchat2.c - v. 0.1 - (c) by ABUKAJ (J.M.Kowalski)*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

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
#include "PolchatConnection.h"

part::part(tank & prt)
{
  this->source = & prt;

  int ptr = 0;

  int size = prt.len();

  const char * data = prt.get();

  this->headerlen = ((unsigned char) data[ptr++]) << 8;
  this->headerlen |= (unsigned char) data[ptr++];
  this->nstrings = ((unsigned char) data[ptr++]) << 8;
  this->nstrings |= (unsigned char) data[ptr++];

  this->header = new short int [this->headerlen];

  for (int i = 0; i < this->headerlen; i++)
  {
    this->header[i] = ((unsigned char) data[ptr++]) << 8;
    this->header[i] |= (unsigned char) data[ptr++];
  }

  this->strings = new std::string [this->nstrings];

  for (int i = 0; i < this->nstrings; i++)
  {
    int len = ((unsigned char) data[ptr++]) << 8;
    len |= (unsigned char) data[ptr++];
    this->strings[i] = std::string(data + ptr, len);
    ptr += len + 1;
  }

  if (ptr != size)
  {
    if (debug)
    {
      interface->nl();
      interface->put("Tank parse error!");
      /*partdump(prt);*/
    }
  }
}


//welcome message
part::part(std::string nick,
           std::string pass,
           std::string room,
           std::string roompass,
           std::string referer="http://www.polchat.pl/chat/room.phtml/?room=Sekciarz")
{
  this->source = NULL;
  this->headerlen = 1;
  this->header = new short int [1];
  this->header[0] = 0x0578;

  this->nstrings = 8;
  this->strings =new std::string [8];

  this->strings[0] = nick;
  this->strings[1] = pass;
  this->strings[2] = roompass;
  this->strings[3] = room;
  this->strings[4] = referer;
  this->strings[5] = "polchat.pl";
  this->strings[6] = "nlst=1&nnum=1&jlmsg=true&ignprv=false";
  this->strings[7] = VER;
}


part::part(std::string & str)
{
  this->source = NULL;
  this->header = new short int [1];

  this->headerlen = 1;
  this->header[0] = 0x019a;

  this->strings = new std::string [1];
  this->nstrings = 1;
  this->strings[0] = str;
}


//message
part::part(std::string & str, std::string & room)
{
  this->source = NULL;
  this->header = new short int [1];

  this->headerlen = 1;
  this->header[0] = 0x019a;

  this->strings = new std::string [2];
  this->nstrings = 2;
  this->strings[0] = str;
  this->strings[1] = room;
}

part::part(unsigned int hl, unsigned int ns, short * h, std::string * s)
{
  this->source = NULL;
  this->header = hl > 0 ? new short [hl] : NULL;
  for (this->headerlen = 0; this->headerlen < hl; this->headerlen++)
  {
    this->header[this->headerlen] = *h++;
  }

  this->strings = ns > 0 ? new std::string [ns] : NULL;
  for (this->nstrings = 0; this->nstrings < ns; this->nstrings++)
  {
    this->strings[this->nstrings] = *s++;
  }
}

void part::dump(amixInterface * interface, bool coredump)
{
  if (coredump && this->source != NULL)
  {
    this->source->dump(interface);
    return;
  }

  interface->nl();
  interface->put("HEADER: ");
  for (int i = 0; i < this->headerlen; i++)
  {
    interface->puthex(this->header[i], 4);
    interface->putchar(' ');
  }

  interface->nl();
  interface->put("STRINGS ");
  interface->puthex(this->nstrings, 4);
  for (int i = 0; i < this->nstrings; i++)
  {
    interface->nl();
    interface->putchar('"');
    interface->put(this->strings[i].c_str());
    interface->putchar('"');
  }
}

part::~part()
{
  if (this->source != NULL)
  {
    delete this->source;
  }

  if (this->strings != NULL)
  {
    delete [] this->strings;
  }

  if (this->header != NULL)
  {
    delete [] this->header;
  }
}

void processpart(part *ppart)
{
  short type;
  short headerlen;
  short nstrings;
  int i;
  char *ptr;
  unsigned int tmp;
  unsigned int tempt[7];
  
  if (NULL != ppart)
  {
    headerlen = ppart->headerlen;
    nstrings = ppart->nstrings;
    if (headerlen > 0)
    {
      type = (ppart->header[0] & 0xFFFF);
      switch (type)
      {
        case 0x0000: /*heh, chyba nie wejdzie??*/
          if (debug)
          {
            interface->nl();
            interface->put("Unknown part header");
            if (!verbose)
            {
              ppart->dump(interface);
            }
          }
          break;
        case 0x0001:/*ECHO REQUEST*/
          if (headerlen != 0x0001 || nstrings != 0x0000)
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }     
          break;
        case 0x0262:/*MSG*/                
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            if (bell)
            {
              beep();
              /*interface->put("\a");
              interface->nl();*/
            }
            if (!verbose)
            {
              if (!antiidle_sent || ppart->strings[0] != POLCHAT_NOOP_REPLY_MSG)
              {
                chatrooms.roommsg(ppart->strings[1], ppart->strings[0]);
                printlog("-msg-", ppart->strings[0], ppart->strings[1]);
              }
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0263:/*Priv. msg*/
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            chatrooms.privmsg(ppart->strings[1], ppart->strings[0]);
            // PRIV FROM
          }
          else if (headerlen == 0x0001 && nstrings == 0x0003)
          {
            chatrooms.privmsg(ppart->strings[2], ppart->strings[0]);
            // PRIV TO
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0266: /*client config*/
          if (headerlen == 0x0001 || nstrings == 0x0001)
          {
            if (debug)
            {
              interface->nl();
              interface->put("CLIENT CONFIG: ");
              interface->put(ppart->strings[0].c_str());
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0267:/*nick entered*/
          if (headerlen == 0x0002 && nstrings == 0x0003)
          {
            std::string &room = ppart->strings[1];

            chatrooms.room[room].addnick(ppart->strings[0],
                                         ppart->strings[2],
                                         ppart->header[1]);
            if ((*(chatrooms.current)).name == room)
            {
              interface->printnicklist();
            }
            if ((ppart->header[1] & 0x00ff8c) != 0x0000 && debug)
            {
              interface->nl();
              interface->put("Unknown status of: ");
              interface->put(ppart->strings[0].c_str());
              interface->put(" : ");
              interface->puthex(ppart->header[1], 4);
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0268:/*nick left*/
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            std::string &room = ppart->strings[1];

            chatrooms.room[room].removenick(ppart->strings[0]);
            if ((*(chatrooms.current)).name == room)
            {
              interface->printnicklist();
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0269:/*NICK update*/
          if (headerlen == 0x0002 && nstrings == 0x0002)
          {
            std::string &room = ppart->strings[1];

            chatrooms.room[room].updatenick(ppart->strings[0], ppart->header[1]);
            if ((*(chatrooms.current)).name == room)
            {
              interface->printnicklist();
            }
            if ((ppart->header[1] & 0x00ff8c) != 0x0000 && debug)
            {
              interface->nl();
              interface->put("Unknown status of: ");
              interface->put(ppart->strings[0].c_str());
              interface->put(" : ");
              interface->puthex(ppart->header[1], 4);
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }  
            }
          }
          break;
        case 0x026a:/*I have absolutly no idea - chyba ze wlazlem jako ja???*/
          if (headerlen == 0x0002 && nstrings == 0x0001)
          {
            if ((verbose || debug) && ppart->header[1] != 0x0004)
            {
              interface->nl();
              interface->put(" 0004 != ");
              interface->puthex(ppart->header[1], 4);
              interface->nl();
              interface->put(" NICK:");

              if (!verbose)
              {
                interface->nl();
                interface->put(ppart->strings[0].c_str());
              }
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x026b:/*nicklist*/
          if (headerlen >= 5 && ppart->header[1] == 0x0001 &&
            ppart->header[2] == 0x0001 && ppart->header[3] == 0x0001 &&
            ppart->header[4] == 0x0000)
          {
            chatroom & room = chatrooms.room[ppart->strings[0]];
            for (i = 1; i < nstrings; i += 2)
            {
              room.addnick(ppart->strings[i],
                           ppart->strings[i + 1],
                           ppart->header[i + 4]);
              if (((ppart->header[i + 4] & 0x00ff8c) != 0x0000 || ppart->header[i + 5] != 0x0000) && debug)
              {
                interface->nl();
                interface->put("Unknown status of: ");
                interface->put(ppart->strings[i].c_str());
                interface->put(" : ");
                interface->puthex(ppart->header[i + 4], 4);
                interface->put(" : ");
                interface->puthex(ppart->header[i + 5], 4);
              }
            }
            if (room.name == (*(chatrooms.current)).name)
            {
              interface->printnicklist();
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x026C: /*#nicks*/
          if (headerlen == 0x0002 && nstrings == 0x0000)
          {
            nickn = ppart->header[1];
            if (debug)
            {
              interface->nl();
              interface->put("W pokoju: 0x");
              interface->puthex(nickn, 4);
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0271: /*ROOMINFO*/
          if (headerlen == 0x0002 && nstrings == 0x0002)
          {
            chatrooms.room[ppart->strings[0]].setdesc(ppart->strings[1]);
            interface->printtitle();
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0272:/*PIERDOLY... konfiguracja pokoju ;-D*/
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            if (NULL != (ptr = strstr((char *) ppart->strings[0].c_str(), "color_user=")))
            {
              ptr += 11;
              sscanf(ptr, "#%x", &tmp);
              colourt[0] = transformrgb((tmp >> 16) & 0x00FF, (tmp >> 8) & 0x00FF, tmp & 0x00FF);
            }
            if (NULL != (ptr = strstr((char *) ppart->strings[0].c_str(), "color_op=")))
            {
              ptr += 9;
              sscanf(ptr, "#%x", &tmp);
              colourop = transformrgb((tmp >> 16) & 0x00FF, (tmp >> 8) & 0x00FF, tmp & 0x00FF);
            }
            if (NULL != (ptr = strstr((char *) ppart->strings[0].c_str(), "color_guest=")))
            {
              ptr += 12;
              tmp = sscanf(ptr, "#%x #%x #%x #%x #%x #%x #%x", &tempt[0],
                           &tempt[1], &tempt[2], &tempt[3], &tempt[4], &tempt[5],
                           &tempt[6]);
              for (i = 0; i < tmp; i++)
              {
                colourt[i + 1] = transformrgb((tempt[i] >> 16) & 0x00FF, (tempt[i] >> 8) & 0x00FF, tempt[i] & 0x00FF);
              }
            }
          }    
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0276:/*MSG powitalny*/
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            chatrooms.join(ppart->strings[1]);
            if (!verbose)
            {
              chatrooms.roommsg(ppart->strings[1], ppart->strings[0]);
              printlog("-hi-", ppart->strings[0], ppart->strings[1]);
              interface->colouroff();
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        case 0x0277:/*MSG pozegnalny*/
          if (headerlen == 0x0001 && nstrings == 0x0002)
          {
            chatrooms.part(ppart->strings[1], false);
            interface->update_all();

            if (!verbose)
            {
              chatrooms.currentroom().msg(ppart->strings[0], true);
              printlog("-bye-", ppart->strings[0], ppart->strings[1]);
              interface->colouroff();
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            } 
          }
          break;
        case 0x0591:
          if (debug && !verbose)
          {
            interface->nl();
            interface->put("REKLAMY");
            ppart->dump(interface);
          }
          break;
        case (short) 0xffff:/*MSG -*/
          if (headerlen == 0x0001 && nstrings == 0x0001)
          {
            if (!verbose)
            {
              //TODO: rethink it
              chatrooms.currentroom().addline(ppart->strings[0].c_str());
              line & tmp = chatrooms.currentroom().lines.back();
              interface->put(tmp.timestring.c_str());
              interface->printpol(tmp.text.c_str());

              std::string dummy = "";

              printlog("---", ppart->strings[0], dummy);
            }
            connected = 0;
            //TODO ensure if it works?
            connection->close();
            if (POLCHAT_BAD_PASSWORD_MSG == ppart->strings[0])
            {
              pass = interface->input_password();
            }
            else if (POLCHAT_NICK_BUSY_MSG == ppart->strings[0])
            {
              nick = interface->input(" Nick:", nick);;
            }
          }
          else
          {
            if (debug)
            {
              interface->nl();
              interface->put("Unknown part header");
              if (!verbose)
              {
                ppart->dump(interface);
              }
            }
          }
          break;
        default:
          if (debug)
          {
            interface->nl();
            interface->put("Unknown part header");
            if (!verbose)
            {
              ppart->dump(interface);
            }
          }
          break;
      }
    }
    else
    {
      if (debug)
      {
        interface->nl();
        interface->put("Unknown part header");
        if (!verbose)
        {
          ppart->dump(interface);
        }
      }
      if (headerlen == 0x0000 && nstrings == 0x0000)
      {
        interface->nl();
        interface->put("<blink>");
        interface->put("CONNECTION LOST :-( /*???*/");
        interface->put("</blink>");
        interface->put("0x000000000000");
        connected = 0;
        //TODO ensure if it works?
        connection->close();
      }
    }     
    if (verbose)
    {
      ppart->dump(interface, coredump);
    }
  }
  else
  {
    if (debug)
    {
      interface->nl();
      interface->put("Error: NULL ptr given to processpart()");
    }
  }
}

unsigned long part::size()
{
  unsigned long size = 8 + 2 * this->headerlen;
  for (long i = 0; i < this->nstrings; i++)
  {
    size += this->strings[i].length() + 3;
  }
  return size;
}

unsigned long part::render(unsigned char * & buffer)
{
  unsigned long tmp = this->size();
  if (buffer == NULL)
  {
    buffer = new unsigned char [tmp];
  }

  buffer[3] = (unsigned char) (tmp % 256);
  tmp /= 256;
  buffer[2] = (unsigned char) (tmp % 256);
  tmp /= 256;
  buffer[1] = (unsigned char) (tmp % 256);
  tmp /= 256;
  buffer[0] = (unsigned char) tmp;

  unsigned char * ptr = &buffer[4];
  *ptr++ = (unsigned char) (this->headerlen / 256);
  *ptr++ = (unsigned char) (this->headerlen % 256);
  *ptr++ = (unsigned char) (this->nstrings / 256);
  *ptr++ = (unsigned char) (this->nstrings % 256);

  for (long i = 0; i < this->headerlen; i++)
  {
    *ptr++ = (unsigned char) (this->header[i] / 256);
    *ptr++ = (unsigned char) (this->header[i] % 256);
  }

  for (long i = 0; i < this->nstrings; i++)
  {
    tmp = this->strings[i].length();
    *ptr++ = (unsigned char) (tmp / 256);
    *ptr++ = (unsigned char) (tmp % 256);
    strcpy((char *) ptr, this->strings[i].c_str());
    ptr += tmp;
    *ptr++ = '\0';
  }
  return this->size();
}

