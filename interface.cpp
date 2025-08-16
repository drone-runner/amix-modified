/*AmiX - interface.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>

#include <algorithm>

#include <curses.h>

#include "interface.h"
#include "polchat.h"
#include "temp.h"
#include "image.h"

amixInterface * interface = NULL;

int transformrgb(int red, int green, int blue)
{
  int col = 0;
  
  if (red > 0x007F)
  {
    col |= 0x01;
  }
  
  if (green > 0x007F)
  {
    col |= 0x02;
  }
  
  if (blue > 0x007F)
  {
    col |= 0x04;
  }
  return col;
}


void amixInterface::colouron(int c)
{
  if (!this->colourstack.empty())
  {
    this->window_attroff(COLOR_PAIR(this->colourstack.top()));
  }
  this->colourstack.push(c);
  this->window_attron(COLOR_PAIR(c));
}


void amixInterface::colouroff()
{
  if (!this->colourstack.empty())
  {
    this->window_attroff(COLOR_PAIR(this->colourstack.top()));
    this->colourstack.pop();
  }
  if (!this->colourstack.empty())
  {
    this->window_attron(COLOR_PAIR(this->colourstack.top()));
  }
}

void amixInterface::colourflush()
{
  while (!this->colourstack.empty())
  {
    this->window_attroff(COLOR_PAIR(this->colourstack.top()));
    this->colourstack.pop();

    if (!this->colourstack.empty())
    {
      this->window_attron(COLOR_PAIR(this->colourstack.top()));
    }
  }
}

void amixInterface::window_attron(int attr)
{
  if (this->useattr)
  {
    wattron(this->chatwindow, attr);
  }
}


void amixInterface::window_attroff(int attr)
{
  if (this->useattr)
  {
    wattroff(this->chatwindow, attr);
  }
}


amixInterface::amixInterface(bool oldSchool)
{
  this->oldSchool = oldSchool;
  this->history_ptr = this->history.begin();
  this->buffer_stored = false;

  this->inlen = 0;
  this->ptr = 0;
  this->buffer = "";
  this->utf8char = "";
  this->utf8charOut = "";
  this->consptr = 0;
  this->useattr = true;
  this->latin2 = false;
  this->window_updated = true;
  this->utf8charToGo = 0;

  this->nicklist_y = NICKLIST_Y;
  this->nicklist_w = 30;

  this->window_x = WINDOW_X;
  this->window_y = WINDOW_Y;

  this->console_x = CONSOLE_X;
  this->console_h = CONSOLE_H;

  this->title_x = TITLE_X;
  this->title_y = TITLE_Y;
  this->title_h = TITLE_H;

  initscr();
  noecho();
  cbreak();
  start_color();

  this->getsize();

  this->chatwindow = newwin(this->window_h, this->window_w, this->window_y, this->window_x);
  if (this->oldSchool)
  {
    wborder(this->chatwindow, ' ', ' ', '#', '#', '*', '*', '*', '*');
    mvwaddstr(this->chatwindow, 0, this->window_w / 2 - 3, " CHAT ");
  }
  scrollok(this->chatwindow, TRUE);
  if (this->oldSchool)
  {
    wsetscrreg(this->chatwindow, 1, this->window_h - 2);
    wmove(this->chatwindow, this->window_h - 2, 0);
  }
  else
  {
    wmove(this->chatwindow, this->window_h - 1, 0);
  }
  wrefresh(this->chatwindow);
  
  this->nickwindow = newwin(this->nicklist_h, this->nicklist_w, this->nicklist_y, this->nicklist_x);
  if (this->oldSchool)
  {
    wborder(this->nickwindow, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwaddstr(this->nickwindow, 0, this->nicklist_w / 2 - 4, " NICKS: ");
  }
  wrefresh(this->nickwindow);

  this->titlewindow = newwin(this->title_h, this->title_w, this->title_y, this->title_x);
  wrefresh(this->titlewindow);

  this->consolewindow = newwin(this->console_h, this->console_w, this->console_y, this->console_x);
  wmove(this->consolewindow, 1, 1);
  wrefresh(this->consolewindow);

  nodelay(this->consolewindow, TRUE);
  keypad(this->consolewindow, TRUE);

  init_pair(7, COLOR_YELLOW, COLOR_BLUE);
}


void amixInterface::getsize()
{
  getmaxyx(stdscr, this->scr_rows, this->scr_cols);

  this->window_h = this->scr_rows - this->window_y - this->console_h;
  this->window_w = this->scr_cols - this->nicklist_w - this->window_x;

  this->nicklist_x = this->scr_cols - this->nicklist_w;
  this->nicklist_h = this->scr_rows - this->nicklist_y;

  this->title_w = this->scr_cols - this->nicklist_w - this->title_x;

  this->console_y = this->scr_rows - this->console_h;
  this->console_w = this->scr_cols - this->console_x - this->nicklist_w;
}

void amixInterface::resize()
{
  this->getsize();

  mvwin(this->chatwindow, this->window_y, this->window_x);
  wresize(this->chatwindow, this->window_h, this->window_w);
  if (this->oldSchool)
  {
    wborder(this->chatwindow, ' ', ' ', '#', '#', '*', '*', '*', '*');
    mvwaddstr(this->chatwindow, 0, this->window_w / 2 - 3, " CHAT ");
    wsetscrreg(this->chatwindow, 1, this->window_h - 2);
  }
  this->window_redraw();
  
  mvwin(this->nickwindow, this->nicklist_y, this->nicklist_x);
  wresize(this->nickwindow, this->nicklist_h, this->nicklist_w);
  if (this->oldSchool)
  {
    wborder(this->nickwindow, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwaddstr(this->nickwindow, 0, this->nicklist_w / 2 - 4, " NICKS: ");
  }
  this->printnicklist();
  
  mvwin(this->titlewindow, this->title_y, this->title_x);
  wresize(this->titlewindow, this->title_h, this->title_w);
  this->printtitle();
  
  mvwin(this->consolewindow, this->console_y, this->console_x);
  wresize(this->consolewindow, this->console_h, this->console_w);
  this->console_update();
}


void amixInterface::window_redraw()
{
  std::list<line> & lines = chatrooms.currentroom().lines;

  if (this->oldSchool)
  {
    wmove(this->chatwindow, this->window_h - 2, 0);
  }

  for (int i = lines.size(); i < this->window_h - (this->oldSchool ? 2 : 0); i++)
  {
    this->nl();
    // is this loop below necessary?
    /*for (int j = 0; j < this->window_w; ++j)
    {
      waddch(this->chatwindow, ' ');
    }*/
  }
  
  for (std::list<line>::iterator it = lines.begin(); 
       it != lines.end();
       it++)

  {
    this->nl();
    this->put((*it).timestring.c_str());
    this->printpol((*it).text.c_str());
  }

  wnoutrefresh(this->chatwindow);
  this->window_updated = true;
}

void amixInterface::update_all()
{
  this->printtitle();
  this->printnicklist();
  this->window_redraw();
  this->console_update();
}

amixInterface::~amixInterface()
{
  delwin(this->chatwindow);
  delwin(this->nickwindow);
  delwin(this->consolewindow);
  delwin(this->titlewindow);
  endwin();
}


void amixInterface::nl()
{
  while (this->inlen < this->window_w - (this->oldSchool ? 2 : 0))
  {
    waddch(this->chatwindow, ' ');
    this->inlen++;
  }
  if (this->oldSchool)
  {
    waddch(this->chatwindow, '\n');
  }
  else
  {
    wmove(this->chatwindow, this->window_h, 0);
  }
  this->inlen = 0;
  this->window_updated = true;
}

std::string amixInterface::fromUTF8(std::string str)
{
  if (this->latin2)
  {
    return utf82isostring(str);
  }
  return str;
}

void amixInterface::putchar(unsigned char c)
{
  if (this->inlen >= this->window_w - (this->oldSchool ? 2 : 0))
  {
    this->nl();
  }
  this->utf8charOut += c;
  this->utf8charToGo += utf8charlen(c) - 1;
  if (this->utf8charToGo == 0)
  {
    this->inlen++;
    waddstr(this->chatwindow, this->fromUTF8(this->utf8charOut).c_str());
    this->utf8charOut = "";
    this->window_updated = true;
  }
}


void amixInterface::puthex(unsigned int n, unsigned int len)
{
  if (this->inlen + len >= this->window_w - (this->oldSchool ? 2 : 0))
  {
    this->nl();
  }
  while (len-- > 0)
  {
    this->putchar(inttohex((n >> (len * 4)) & 0x0F));
  }
  this->window_updated = true;
}


void amixInterface::put(const char *word)
{
  if (word != NULL && *word != '\0')
  {
    if (*word == '\n')
    {
      this->nl();
    }
    else if (verbose || (*word != '<'))
    {
      int len = utf8strlen((unsigned char *) word);
      if (this->inlen + len <= this->window_w - (this->oldSchool ? 2 : 0))
      {
        waddstr(this->chatwindow, this->fromUTF8(word).c_str());
        inlen += len;
      }
      else 
      {
        this->nl();
        while (*word != '\0')
        {
          //TODO: rethink it
          this->putchar((unsigned char) *word);
          word++;
        }
      }
    }
    else if (0 == ncsstrcmp(word, "<b>"))
    {
      this->window_attron(A_BOLD);
    }
    else if (0 == ncsstrcmp(word, "</b>"))
    {
      this->window_attroff(A_BOLD);
    }
    else if (0 == ncsstrcmp(word, "<u>"))
    {
      this->window_attron(A_UNDERLINE);
    }
    else if (0 == ncsstrcmp(word, "</u>"))
    {
      this->window_attroff(A_UNDERLINE);
    }
    else if (0 == ncsstrcmp(word, "<blink>"))
    {
      this->window_attron(A_BLINK);
    }
    else if (0 == ncsstrcmp(word, "</blink>"))
    {
      this->window_attroff(A_BLINK);
    }
    else
    {
      int len = utf8strlen((unsigned char *) word);
      if (this->inlen + len <= this->window_w - (this->oldSchool ? 2 : 0))
      {
        waddstr(this->chatwindow, this->fromUTF8(word).c_str());
        this->inlen += len;
      }
      else 
      {
        this->nl();
        while (*word != '\0')
        {
          //TODO: rethink it
          this->putchar((unsigned char) *word);
          word++;
        }
      }
    }
  }
  this->window_updated = true;
}


void amixInterface::putforce(const char *word)
{
  waddstr(this->chatwindow, this->fromUTF8(word).c_str());
  this->window_updated = true;
}


void amixInterface::print()
{
  wnoutrefresh(this->chatwindow);

  this->window_updated = true;
}


char *readtoken(const char *string){
  static int ptr = 0;
  static int done = 0;
  static int link = 0;
  int mode = 0;
  int len = 0;
  int start = 0;
  char c;
  char *result = NULL;

  if (done)
    {
    ptr = 0;
    done = 0;
    }
  else
    {
    start = ptr;
    c = string[ptr];
    switch (c)
      {
      case '\0':
        done = -1;
        mode = 1;
        break;
      case '<':
        mode = 2;
        break;
      case '&':
        mode = 3;
        break;
      case ' ':
      case '\n':
      case '\t':
      case '\r':
      case '\f':
      case '\b':
      case '\a':
        mode = 5;
        break;
      default:
        mode = 0;
        break;
      }

    switch (mode)
      {
      case 0:
        while (!isspace(string[ptr]) && '\0' != string[ptr] && '<' != string[ptr] && (link || ('&' != string[ptr])))
          {
          ptr++;
          }
        len = ptr - start;
        if (NULL != (result = (char *) calloc(len + 1, sizeof(char))))
          {
          memcpy(result, string + start, len);
          result[len] = '\0';
          return result;
          }
        break;
      case 1:
        if (NULL != (result = (char *) calloc(1, sizeof(char))))
          {
          result[0] = '\0';
          return result;
          }
        break;
      case 2:
        while ('\0' != string[ptr] && '>' != string[ptr])
          {
          ptr++;
          }
        if (string[ptr] == '>')
          {
          ptr++;
          }
        len = ptr - start;
        if (NULL != (result = (char *) calloc(len + 1, sizeof(char))))
          {
          memcpy(result, string + start, len);
          result[len] = '\0';
          if (0 == ncsstrncmp(result, "<a", 2))
            {
            link = -1;
            }
          if (0 == ncsstrcmp(result, "</a>"))
            {
            link = 0;
            }             
          return result;
          }
        break;
      case 3:
        while ('\0' != string[ptr] && ';' != string[ptr])
          {
          ptr++;
          }
        if (string[ptr] == ';')
          {
          ptr++;
          }
        len = ptr - start;
        if (NULL != (result = (char *) calloc(len + 1, sizeof(char))))
          {
          memcpy(result, string + start, len);
          result[len] = '\0';
          return result;
          }                 
        break;
      case 5:
        while (isspace(string[ptr++]))
          {
          }
        ptr--;
        if (NULL != (result = (char *) calloc(2, sizeof(char))))
          {
          result[0] = ' ';
          result[1] = '\0';
          return result;
          }
        break;
      }
    }
 
  return result;
  }


void amixInterface::printtitle()
{
  for (int i = 0; i < this->title_w; i++)
  {
    mvwaddch(this->titlewindow, 0, i, ' ');
    mvwaddch(this->titlewindow, 1, i, ' ');
  }

  int written = 0;
  for (std::list<roomname>::iterator it = chatrooms.roomlist.begin();
       it != chatrooms.roomlist.end();
       it++)
  {
    std::map<std::string, chatroom> & rooms = (*it).room?chatrooms.room:chatrooms.priv;

    chatroom & cr = rooms[(*it).name];
    int len = utf8strlen(cr.name);

    if (written + len + 2 > this->title_w)
    {
      mvwaddnstr(this->titlewindow, 0, written, "...", this->title_w - written);      
      break;
    }

    if (!(*it).room && this->useattr)
    {
      wattron(this->titlewindow, A_UNDERLINE);
    }

    if (it == chatrooms.current)
    {
      if (this->useattr)
      {
        wattron(this->titlewindow, A_BOLD);
      }

      mvwaddch(this->titlewindow, 0, written++, '>');
    }
    else
    {
      mvwaddch(this->titlewindow, 0, written++, ':');
    }

    mvwaddstr(this->titlewindow, 0, written, this->fromUTF8(cr.name).c_str());

    written += len;

    if (it == chatrooms.current)
    {
      mvwaddch(this->titlewindow, 0, written++, '<');

      if (this->useattr)
      {
        wattroff(this->titlewindow, A_BOLD);
      }
    }
    else
    {
      mvwaddch(this->titlewindow, 0, written++, ':');
    }

    if (!(*it).room && this->useattr)
    {
      wattroff(this->titlewindow, A_UNDERLINE);
    }
  }

  mvwaddnstr(this->titlewindow, 1, 0, this->fromUTF8(chatrooms.currentroom().description).c_str(), this->title_w);
  wnoutrefresh(this->titlewindow);
  this->window_updated = true;
}


void amixInterface::printpol(const char *string)
{
  int mode = 0;
  int tokens = -1;
  char *token = NULL;
  char *ptr;
  unsigned int tmp;
  int col;
  
  if (string != NULL)
  {
    token = readtoken(string);
    while (tokens)
    {
      switch (mode)
      {
        case 0:
          if (token == NULL)
          {
            tokens = 0;
          }
          else if (*token == '\0')
          {
            mode = 1;
          }
          else if (0 == ncsstrncmp(token, "<font", 5))
          {
            if (NULL != (ptr = strstr(token, "color=")))
            {
              ptr += 6; /*+= strlen("color=");*/
              if (0 == ncsstrncmp(ptr, "red", 3) || 0 == ncsstrncmp(ptr, "\"red\"", 5))
              {
                this->colouron(COLOR_RED);
              }
              else if (0 == ncsstrncmp(ptr, "orange", 6))
              {
                this->colouron(COLOR_YELLOW);
              }
              else
              {
                sscanf(ptr, "#%x", &tmp);
                col = transformrgb((tmp >> 16) & 0x00FF, (tmp >> 8) & 0x00FF, tmp & 0x00FF);
                if (col != COLOR_WHITE && col != COLOR_BLACK)
                {
                  this->colouron(col);
                }
              }
            }
            mode = 2/*2*/;
          }
          else if (0 == ncsstrcmp(token, "</font>"))
          {
            this->colouroff();
            mode = 2/*2*/;
          }
          else if (0 == ncsstrcmp(token, "<i>"))
          {
            mode = 2;
          }
          else if (0 == ncsstrcmp(token, "</i>"))
          {
            mode = 2;
          }
          else if (0 == ncsstrncmp(token, "<a", 2))
          {
            mode = 2;
          }
          else if (0 == ncsstrcmp(token, "</a>"))
          {
            mode = 2;
          }
          else if (0 == ncsstrncmp(token, "<img", 4))
          {
            this->put(imagetopic(token).c_str());
            mode = 2;
          }
          else if (0 == ncsstrcmp(token, "</img>"))
          {
            mode = 2;
          }
          else if (0 == ncsstrcmp(token, "&quot;"))
          {
            this->put("\"");
            mode = 1;
          }
          else if (0 == ncsstrcmp(token, "&amp;"))
          {
            this->put("&");
            mode = 1;
          }
          else if (0 == ncsstrcmp(token, "&gt;"))
          {
            this->put(">");
            mode = 1;
          }
          else if (0 == ncsstrcmp(token, "&lt;"))
          {
            this->put("<");
            mode = 1;
          }
          else 
          {
            this->put(token);
            mode = 1;
          }
          break;
        case 1:
          free(token);
          token = readtoken(string);
          mode = 0;
          break;
        case 2:
          if (verbose || nohtml)
          {
            this->put(token);
          }
          mode = 1;
          break;
      }
    }
  }
  else
  {
    if (debug)
    {
      this->put("\n");
      this->put("Error: NULL ptr given to printpol()");
    }
  }
  this->colourflush();
}


std::string amixInterface::get_input()
{
   std::string tmp = this->buffer;

   this->buffer_stored_flush();

   this->history.push_front(this->buffer);

   if (this->history.size() > this->history_size)
   {
     this->history.pop_back();
   }
   this->history_ptr = this->history.begin();

   this->buffer = "";
   this->ptr = 0;
   this->consptr = 0;
   this->console_update();

   return tmp;
}

bool amixInterface::console_input()
{
  int updated = 0;
  int tptr;
  int tlen = 0;
 
  int c;
  while (ERR != (c = wgetch(this->consolewindow)))
  {
    switch (c)
    {
      case '\n':
      case '\r':
        {
          return true;
        }
      case KEY_UP:
        if (! this->buffer_stored)
        {
          this->history.push_front(this->buffer);
          this->history_ptr = this->history.begin();
          this->buffer_stored = true;
        }
        if (this->history_ptr != this->history.end())
        {
          this->history_ptr++;

          if (this->history_ptr == this->history.end())
          {
            this->history_ptr--;
            break;
          }
          this->buffer = *this->history_ptr;
        }
        this->ptr = this->buffer.length();
        this->consptr = utf8strlen(this->buffer);
        updated = -1;
        break;
      case KEY_DOWN:
        if (this->buffer_stored && this->history_ptr != this->history.begin())
        {
          this->history_ptr--;

          this->buffer = *this->history_ptr;

          this->ptr = this->buffer.length();
          this->consptr = utf8strlen(this->buffer);
          updated = -1;
        }
        break;
      case KEY_HOME:
        this->ptr = 0;
        this->consptr = 0;
        updated = -1;
        break;
      case KEY_END:
        this->ptr = this->buffer.length();
        this->consptr = utf8strlen(this->buffer);
        updated = -1;
        break;
      case KEY_BACKSPACE:
      case 0x007F: /*backspace mapuje na DEL?*/
        if (this->ptr > 0)
        {
          this->buffer_stored_flush();

          int tmp = 1;
          while (not isutf8charbeginning((unsigned char) this->buffer[ptr - tmp]) && this->ptr - tmp > 0)
          {
            tmp++;
          }
          this->ptr -= tmp;
          this->buffer.erase(this->ptr, tmp);

          this->consptr--;
          updated = -1;
        }
        break;
      case KEY_DL:
      case KEY_DC:
        if (this->ptr >= 0 && this->ptr < this->buffer.length())
        {
          this->buffer_stored_flush();

          int tmp = 1;
          while (not isutf8charbeginning((unsigned char) this->buffer[this->ptr + tmp])
                 && this->ptr + tmp < this->buffer.length())
          {
            tmp++;
          }

          this->buffer.erase(this->ptr, tmp);
          updated = -1;
        }
        break;
      case KEY_LEFT:
        if (this->ptr > 0)
        {
          do
          {
            this->ptr--;
          }
          while (not isutf8charbeginning((unsigned char) this->buffer[this->ptr]) && this->ptr > 0);

          this->consptr--;
          if (this->consptr + 1 > this->window_w)
          {
            updated = -1;
          }
          else
          {
            wmove(this->consolewindow, 0, this->consptr);
          }
        }
        break;
      case KEY_RESIZE:
        this->resize();
        break;
      case KEY_RIGHT:
        if (this->ptr < this->buffer.length())
        {
          do
          {
            this->ptr++;
          }
          while (not isutf8charbeginning((unsigned char) this->buffer[this->ptr])
                 && this->ptr < this->buffer.length());

          this->consptr++;
          if (this->consptr > this->window_w)
          {
            updated = -1;
          }
          else
          {
            wmove(this->consolewindow, 0, this->consptr);
          }
        }
        break;
      case '\t':
        this->buffer_stored_flush();

        tptr = this->ptr - 1;
        tlen = 1;

        while (tptr >= 0 && this->buffer[tptr] != ' ')
        {
          tptr--;
          tlen++;
        }
        tptr++;/*korekta dlugosci*/
        tlen--;
        {
          std::string nick = chatrooms.currentroom().getnickbyprefix((char *) this->buffer.c_str() + tptr, tlen);
        
          nick.erase(0, tlen);
          this->buffer.insert(ptr, nick);
          this->ptr += nick.length();
          this->consptr += utf8strlen(nick);
        }

        updated = -1;
        break;
      case KEY_F(1):
      case KEY_SLEFT:
      case KEY_SHOME:
        chatrooms.prev();
        this->printtitle();
        this->window_redraw();
        this->printnicklist();
        break;
      case KEY_F(2):
      case KEY_SRIGHT:
      case KEY_SEND:
        chatrooms.next();
        this->printtitle();
        this->window_redraw();
        this->printnicklist();
        break;
      case KEY_NPAGE:
      case KEY_PPAGE:
        break;
      default:
        if (this->latin2)
        {
          this->utf8char = iso2utf8string(std::string(1, c));
        }
        else
        {
          this->utf8char += c;

          //TODO: rethink it
          this->utf8left += utf8charlen(c) - 1;
        }
        
        if (this->utf8left == 0)
        {
          this->buffer_stored_flush();

          this->buffer.insert(this->ptr, this->utf8char);
          this->ptr += this->utf8char.length();
          this->utf8char = "";
          this->consptr++;
          updated = -1;
        }
        break;
    }
  }

  if (updated)
  {
    this->console_update();
  }
  return false;
}


void amixInterface::buffer_stored_flush()
{
  if (this->buffer_stored)
  {
    this->history.pop_front();
    this->buffer_stored = false;
  }
}


void amixInterface::console_update()
{
  int utf8d = 0;

  int d = this->consptr >= this->console_w?1 + this->consptr - this->console_w:0;
  
  for (int i = d; i != 0; i--)
  {
    utf8d += utf8charlen((unsigned char) this->buffer[utf8d]);
  }

  wmove(this->consolewindow, 0, 0);

  int j = 0;
  int i = 0;
  int utf8left = 0;
  std::string utf8char = "";

  while (i + utf8d < this->buffer.length() && j < this->console_w - 1)
  {
    unsigned char c = this->buffer[utf8d + i];
    i++;

    utf8char += c;
    utf8left += utf8charlen(c) - 1;

    if (utf8left == 0)
    {
      waddstr(this->consolewindow, this->fromUTF8(utf8char).c_str());
      utf8char = "";
      j++;
    }
  }
  while (j++ < this->console_w)
  {
    waddch(this->consolewindow, ' ');
  }
  wmove(this->consolewindow, 0, std::min(this->consptr, this->console_w));
  wnoutrefresh(this->consolewindow);
  
  this->window_updated = true;
}


void amixInterface::printnicklist()
{
  int i = this->oldSchool ? 1 : 0;

  std::list<nicknode> & nicklist = chatrooms.currentroom().nicklist;

  for (std::list<nicknode>::iterator it = nicklist.begin(); 
       it != nicklist.end() && i < nicklist_h - (this->oldSchool ? 1 : 0);
       it++)
  {
    int nicklen = 3 + (this->oldSchool ? 1 : 0);
    int colour = colourt[((*it).status & 0x0070) >> 4];

    if (colour != COLOR_WHITE && colour != COLOR_BLACK && this->useattr)
    {
      wattron(this->nickwindow, COLOR_PAIR(colour));
    }
    
    if ((*it).status & 0x0002)
    {
      mvwprintw(this->nickwindow, i, this->oldSchool ? 1 : 0, "OP ");
    }
    else
    {
      mvwprintw(this->nickwindow, i, this->oldSchool ? 1 : 0, "   ");
    }

    if (((*it).status & 0x0001) && this->useattr)
    {
      wattron(this->nickwindow, A_UNDERLINE);
    }
    
    if (this->useattr)
    {
      wattron(this->nickwindow, A_BOLD);
    }  

    mvwaddnstr(this->nickwindow, i, nicklen, this->fromUTF8((*it).nick).c_str(), this->nicklist_w - (this->oldSchool ? 1 : 0) - nicklen);
    nicklen += utf8strlen((*it).nick);

    if (this->useattr)
    {
      wattroff(this->nickwindow, A_BOLD);
    }

    if (nicklen < this->nicklist_w - 3 - (this->oldSchool ? 1 : 0) && (*it).client != "unknown")
    {
      if (this->useattr)
      {
        wattron(this->nickwindow, A_DIM);
      }

      mvwaddnstr(this->nickwindow, i, nicklen, (":" + this->fromUTF8((*it).client)).c_str(), this->nicklist_w - (this->oldSchool ? 1 : 0) - nicklen);
      nicklen += 1 + utf8strlen((*it).client);

      if (this->useattr)
      {
        wattroff(this->nickwindow, A_DIM);
      }
    }

    if (nicklen > this->nicklist_w)
    {
      mvwaddstr(this->nickwindow, i, this->nicklist_w - 3 - (this->oldSchool ? 1 : 0), "...");
    }

    if (((*it).status & 0x0001) && this->useattr)
    {
      wattroff(this->nickwindow, A_UNDERLINE);
    }

    for (int j = nicklen; j < this->nicklist_w - (this->oldSchool ? 1 : 0); j++)
    {
      mvwaddch(this->nickwindow, i, j, ' ');
    }
    i++;
    
    if (colour != COLOR_WHITE && colour != COLOR_BLACK && this->useattr)
    {
      wattroff(this->nickwindow, COLOR_PAIR(colour) | A_BOLD);
    }  
  }

  while (i < this->nicklist_h - (this->oldSchool ? 1 : 0))
  {
    wmove(this->nickwindow, i, this->oldSchool ? 1 : 0);
    for (int j =  this->oldSchool ? 2 : 0; j < this->nicklist_w; j++)
    {
      waddch(this->nickwindow, ' ');
    }
    i++;
  }
  wnoutrefresh(this->nickwindow);
  this->window_updated = true;
}

void amixInterface::update()
{
  if (this->window_updated)
  {
    doupdate();
  }
  this->window_updated = false; //?
}

std::string amixInterface::input_password()
{
  int state = 0;
  char c;
  std::string string = "";

  this->put(" Password:");
  this->nl();
  wnoutrefresh(this->chatwindow);
  doupdate();
  nodelay(this->consolewindow, FALSE);

  while ((state != 2) && ERR != (c = wgetch(this->consolewindow)))
  {
    switch (c)
    {
      case '\n':
        state = 2;
        break;
      default:
        string += c;
        break;
    }                           
  }
  
  nodelay(this->consolewindow, TRUE);
  return string;
}

std::string amixInterface::input(std::string msg, std::string init="")
{
  this->put(msg.c_str());
  this->nl();
  wnoutrefresh(this->chatwindow);

  this->buffer = init;
  this->ptr = this->buffer.length();
  this->consptr = utf8strlen(this->buffer);
  this->console_update();

  doupdate();
  while (!this->console_input())
  {
    this->update();
  }
  return interface->get_input();
}

