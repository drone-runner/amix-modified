/*ABUKAJtools - temp.h - v. 0.4 - Jakub Mateusz Kowalski*/
#ifndef AMIX_TEMP_H
#define AMIX_TEMP_H

#include <string>

void clearscreen();
void mypause();
int ncsstrcmp(const char *, const char *);
int ncsstrcmp(std::string, std::string);
int ncsstrncmp(const char *, const char *, int);
int ncsstrncmp(std::string, std::string, int);
char inttohex(int);
unsigned char unicode2iso(unsigned int);
unsigned int iso2unicode(unsigned char);
char *iso2utf8string(char *);
std::string iso2utf8string(std::string);
bool isutf8charbeginning(unsigned int);
int utf8charlen(const unsigned char *);
int utf8charlen(unsigned int);
char *utf82isostring(char *);
std::string utf82isostring(std::string);
int utf8strlen(const unsigned char *);
int utf8strlen(std::string);
char *clonestring(const char *);
char *freadline(FILE *);
char *remcontrols(char *);
std::string lowercase(std::string);

#endif
