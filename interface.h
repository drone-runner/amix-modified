#ifndef AMIX_INTERFACE_H
#define AMIX_INTERFACE_H

#include <ctime>

#include <string>
#include <stack>
#include <list>

#include <curses.h>

#include "polchat.h"

#define NICKLIST_Y 0

#define WINDOW_X 0
#define WINDOW_Y 2 

#define CONSOLE_X 0
#define CONSOLE_H 1

#define TITLE_X 0
#define TITLE_Y 0
#define TITLE_H 2

//#define BUFFSIZE 10240

class amixInterface
{
  protected:
    WINDOW * chatwindow;
    WINDOW * nickwindow;
    WINDOW * titlewindow;
    WINDOW * consolewindow;
    int nicklist_x;
    int nicklist_y;
    int nicklist_h;
    int console_x;
    int console_y;
    int console_w;
    int console_h;
    int title_x;
    int title_y;
    int title_w;
    int title_h;
    int window_x;
    int window_y;
    int window_w;
    int window_h;
    int scr_rows;
    int scr_cols;
    bool window_updated;
    bool oldSchool;

    int inlen;
    int ptr;
    int consptr;
    std::string buffer;
    std::string utf8char;
    std::string utf8charOut;
    int utf8charToGo;
    int utf8left;
    void getsize();
    void buffer_stored_flush();
    std::stack<int> colourstack;
    std::list<std::string> history;
    std::list<std::string>::iterator history_ptr;
    bool buffer_stored;
    static const int history_size = 100;
    std::string fromUTF8(std::string);

  public:
    int nicklist_w;
    bool useattr;

    amixInterface(bool = false);
    ~amixInterface();
    void colouron(int);
    void colouroff();
    void colourflush();
    void window_attron(int);
    void window_attroff(int);
    void resize();
    void window_redraw();
    void update();
    void update_all();
    void nl();
    void putchar(unsigned char);
    void puthex(unsigned int, unsigned int);
    void put(const char *);
    void putforce(const char *);
    void print();
    void printtitle();
    void printpol(const char *);
    bool console_input();
    std::string get_input();
    void printnicklist();
    std::string input_password();
    std::string input(std::string, std::string);
    void console_update();
};

char *readtoken(const char *);
int transformrgb(int, int, int);

extern amixInterface * interface;

#endif
