/*ascii - temp.c - v. 0.5 - Jakub Mateusz Kowalski*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include "temp.h"

/*****************************************************************************\
\*****************************************************************************/
void clearscreen(){
    int a;

    for (a = 0; a < 25; a++){
        putchar('\n');
        }

    }


/*****************************************************************************\
\*****************************************************************************/
void mypause(){
    int a;

    a = 0;
    puts("Nacisnij ENTER/RETURN");
    do{
        a = getchar();
        fflush(stdin);
        }
        while (a == 0);

    a = 0;
    }


/*****************************************************************************\
\*****************************************************************************/
int ncsstrcmp(const char *cs, const char *ct)
{
  while (*cs != '\0' && tolower(*cs) == tolower(*ct))
  {
    cs++; ct++;
  }
  return tolower(*cs) - tolower(*ct);
}

int ncsstrcmp(std::string cs, std::string ct)
{
  int len = std::min(cs.length(), ct.length());
  for (int i = 0; i < len; ++i)
  {
    int tmp = tolower(cs[i]) - tolower(ct[i]);

    if (tmp != 0)
    {
      return tmp;
    }
  }
  return cs.length() - ct.length();
}

/*****************************************************************************\
\*****************************************************************************/
int ncsstrncmp(const char *cs, const char *ct, int n)
{
  if (n == 0)
  {
    return 0;
  }
  while (*cs != '\0' && tolower(*cs) == tolower(*ct) && --n > 0)
  {
    cs++; ct++;
  }
  return tolower(*cs) - tolower(*ct);
}

int ncsstrncmp(std::string cs, std::string ct, int n)
{
  int len = std::min((const unsigned long int) std::min(cs.length(), ct.length()), (const unsigned long int) n);

  for (int i = 0; i < len; ++i)
  {
    int tmp = tolower(cs[i]) - tolower(ct[i]);

    if (tmp != 0)
    {
      return tmp;
    }
  }
  if (len < n)
  {
    return cs.length() - ct.length();
  }
  return 0;
}

/*****************************************************************************\
\*****************************************************************************/
char inttohex(int n){
  if (n >= 0x00)
    {
    if (n < 0x0A)
      {
      return '0' + n;
      }
    else if (n < 0x0010)
      {
      return ('A' - 0x0A) + n;
      }
    }
  return '#';
  /*
  switch (n){
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 0:
      return '0';
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
    }
  return 0;*/
  }


/*****************************************************************************\
\*****************************************************************************/
unsigned int iso2unicode(unsigned char c)
  {
  if (c < 128)
    {
    return (unsigned int) c;
    }
  switch (c)
    {
    case 0x0080:
      return 0x0080;
    case 0x0081:
      return 0x0081;
    case 0x0082:
      return 0x0082;
    case 0x0083:
      return 0x0083;
    case 0x0084:
      return 0x0084;
    case 0x0085:
      return 0x0085;
    case 0x0086:
      return 0x0086;
    case 0x0087:
      return 0x0087;
    case 0x0088:
      return 0x0088;
    case 0x0089:
      return 0x0089;
    case 0x008A:
      return 0x008A;
    case 0x008B:
      return 0x008B;
    case 0x008C:
      return 0x008C;
    case 0x008D:
      return 0x008D;
    case 0x008E:
      return 0x008E;
    case 0x008F:
      return 0x008F;
    case 0x0090:
      return 0x0090;
    case 0x0091:
      return 0x0091;
    case 0x0092:
      return 0x0092;
    case 0x0093:
      return 0x0093;
    case 0x0094:
      return 0x0094;
    case 0x0095:
      return 0x0095;
    case 0x0096:
      return 0x0096;
    case 0x0097:
      return 0x0097;
    case 0x0098:
      return 0x0098;
    case 0x0099:
      return 0x0099;
    case 0x009A:
      return 0x009A;
    case 0x009B:
      return 0x009B;
    case 0x009C:
      return 0x009C;
    case 0x009D:
      return 0x009D;
    case 0x009E:
      return 0x009E;
    case 0x009F:
      return 0x009F;
    case 0x00A0:
      return 0x00A0;
    case 0x00A1:
      return 0x0104;
    case 0x00A2:
      return 0x02D8;
    case 0x00A3:
      return 0x0141;
    case 0x00A4:
      return 0x00A4;
    case 0x00A5:
      return 0x013D;
    case 0x00A6:
      return 0x015A;
    case 0x00A7:
      return 0x00A7;
    case 0x00A8:
      return 0x00A8;
    case 0x00A9:
      return 0x0160;
    case 0x00AA:
      return 0x015E;
    case 0x00AB:
      return 0x0164;
    case 0x00AC:
      return 0x0179;
    case 0x00AD:
      return 0x00AD;
    case 0x00AE:
      return 0x017D;
    case 0x00AF:
      return 0x017B;
    case 0x00B0:
      return 0x00B0;
    case 0x00B1:
      return 0x0105;
    case 0x00B2:
      return 0x02DB;
    case 0x00B3:
      return 0x0142;
    case 0x00B4:
      return 0x00B4;
    case 0x00B5:
      return 0x013E;
    case 0x00B6:
      return 0x015B;
    case 0x00B7:
      return 0x02C7;
    case 0x00B8:
      return 0x00B8;
    case 0x00B9:
      return 0x0161;
    case 0x00BA:
      return 0x015F;
    case 0x00BB:
      return 0x0165;
    case 0x00BC:
      return 0x017A;
    case 0x00BD:
      return 0x02DD;
    case 0x00BE:
      return 0x017E;
    case 0x00BF:
      return 0x017C;
    case 0x00C0:
      return 0x0154;
    case 0x00C1:
      return 0x00C1;
    case 0x00C2:
      return 0x00C2;
    case 0x00C3:
      return 0x0102;
    case 0x00C4:
      return 0x00C4;
    case 0x00C5:
      return 0x0139;
    case 0x00C6:
      return 0x0106;
    case 0x00C7:
      return 0x00C7;
    case 0x00C8:
      return 0x010C;
    case 0x00C9:
      return 0x00C9;
    case 0x00CA:
      return 0x0118;
    case 0x00CB:
      return 0x00CB;
    case 0x00CC:
      return 0x011A;
    case 0x00CD:
      return 0x00CD;
    case 0x00CE:
      return 0x00CE;
    case 0x00CF:
      return 0x010E;
    case 0x00D0:
      return 0x0110;
    case 0x00D1:
      return 0x0143;
    case 0x00D2:
      return 0x0147;
    case 0x00D3:
      return 0x00D3;
    case 0x00D4:
      return 0x00D4;
    case 0x00D5:
      return 0x0150;
    case 0x00D6:
      return 0x00D6;
    case 0x00D7:
      return 0x00D7;
    case 0x00D8:
      return 0x0158;
    case 0x00D9:
      return 0x016E;
    case 0x00DA:
      return 0x00DA;
    case 0x00DB:
      return 0x0170;
    case 0x00DC:
      return 0x00DC;
    case 0x00DD:
      return 0x00DD;
    case 0x00DE:
      return 0x0162;
    case 0x00DF:
      return 0x00DF;
    case 0x00E0:
      return 0x0155;
    case 0x00E1:
      return 0x00E1;
    case 0x00E2:
      return 0x00E2;
    case 0x00E3:
      return 0x0103;
    case 0x00E4:
      return 0x00E4;
    case 0x00E5:
      return 0x013A;
    case 0x00E6:
      return 0x0107;
    case 0x00E7:
      return 0x00E7;
    case 0x00E8:
      return 0x010D;
    case 0x00E9:
      return 0x00E9;
    case 0x00EA:
      return 0x0119;
    case 0x00EB:
      return 0x00EB;
    case 0x00EC:
      return 0x011B;
    case 0x00ED:
      return 0x00ED;
    case 0x00EE:
      return 0x00EE;
    case 0x00EF:
      return 0x010F;
    case 0x00F0:
      return 0x0111;
    case 0x00F1:
      return 0x0144;
    case 0x00F2:
      return 0x0148;
    case 0x00F3:
      return 0x00F3;
    case 0x00F4:
      return 0x00F4;
    case 0x00F5:
      return 0x0151;
    case 0x00F6:
      return 0x00F6;
    case 0x00F7:
      return 0x00F7;
    case 0x00F8:
      return 0x0159;
    case 0x00F9:
      return 0x016F;
    case 0x00FA:
      return 0x00FA;
    case 0x00FB:
      return 0x0171;
    case 0x00FC:
      return 0x00FC;
    case 0x00FD:
      return 0x00FD;
    case 0x00FE:
      return 0x0163;
    case 0x00FF:
      return 0x02D9;
    }
  return 0;
  }


/*****************************************************************************\
\*****************************************************************************/
unsigned char unicode2iso(unsigned int c)
  {
  if (c < 128)
    {
    return (int) c;
    }
  switch (c)
    {
    case 0x0080:
      return 0x0080;
    case 0x0081:
      return 0x0081;
    case 0x0082:
      return 0x0082;
    case 0x0083:
      return 0x0083;
    case 0x0084:
      return 0x0084;
    case 0x0085:
      return 0x0085;
    case 0x0086:
      return 0x0086;
    case 0x0087:
      return 0x0087;
    case 0x0088:
      return 0x0088;
    case 0x0089:
      return 0x0089;
    case 0x008A:
      return 0x008A;
    case 0x008B:
      return 0x008B;
    case 0x008C:
      return 0x008C;
    case 0x008D:
      return 0x008D;
    case 0x008E:
      return 0x008E;
    case 0x008F:
      return 0x008F;
    case 0x0090:
      return 0x0090;
    case 0x0091:
      return 0x0091;
    case 0x0092:
      return 0x0092;
    case 0x0093:
      return 0x0093;
    case 0x0094:
      return 0x0094;
    case 0x0095:
      return 0x0095;
    case 0x0096:
      return 0x0096;
    case 0x0097:
      return 0x0097;
    case 0x0098:
      return 0x0098;
    case 0x0099:
      return 0x0099;
    case 0x009A:
      return 0x009A;
    case 0x009B:
      return 0x009B;
    case 0x009C:
      return 0x009C;
    case 0x009D:
      return 0x009D;
    case 0x009E:
      return 0x009E;
    case 0x009F:
      return 0x009F;
    case 0x00A0:
      return 0x00A0;
    case 0x00A4:
      return 0x00A4;
    case 0x00A7:
      return 0x00A7;
    case 0x00A8:
      return 0x00A8;
    case 0x00AD:
      return 0x00AD;
    case 0x00B0:
      return 0x00B0;
    case 0x00B4:
      return 0x00B4;
    case 0x00B8:
      return 0x00B8;
    case 0x00C1:
      return 0x00C1;
    case 0x00C2:
      return 0x00C2;
    case 0x00C4:
      return 0x00C4;
    case 0x00C7:
      return 0x00C7;
    case 0x00C9:
      return 0x00C9;
    case 0x00CB:
      return 0x00CB;
    case 0x00CD:
      return 0x00CD;
    case 0x00CE:
      return 0x00CE;
    case 0x00D3:
      return 0x00D3;
    case 0x00D4:
      return 0x00D4;
    case 0x00D6:
      return 0x00D6;
    case 0x00D7:
      return 0x00D7;
    case 0x00DA:
      return 0x00DA;
    case 0x00DC:
      return 0x00DC;
    case 0x00DD:
      return 0x00DD;
    case 0x00DF:
      return 0x00DF;
    case 0x00E1:
      return 0x00E1;
    case 0x00E2:
      return 0x00E2;
    case 0x00E4:
      return 0x00E4;
    case 0x00E7:
      return 0x00E7;
    case 0x00E9:
      return 0x00E9;
    case 0x00EB:
      return 0x00EB;
    case 0x00ED:
      return 0x00ED;
    case 0x00EE:
      return 0x00EE;
    case 0x00F3:
      return 0x00F3;
    case 0x00F4:
      return 0x00F4;
    case 0x00F6:
      return 0x00F6;
    case 0x00F7:
      return 0x00F7;
    case 0x00FA:
      return 0x00FA;
    case 0x00FC:
      return 0x00FC;
    case 0x00FD:
      return 0x00FD;
    case 0x0102:
      return 0x00C3;
    case 0x0103:
      return 0x00E3;
    case 0x0104:
      return 0x00A1;
    case 0x0105:
      return 0x00B1;
    case 0x0106:
      return 0x00C6;
    case 0x0107:
      return 0x00E6;
    case 0x010C:
      return 0x00C8;
    case 0x010D:
      return 0x00E8;
    case 0x010E:
      return 0x00CF;
    case 0x010F:
      return 0x00EF;
    case 0x0110:
      return 0x00D0;
    case 0x0111:
      return 0x00F0;
    case 0x0118:
      return 0x00CA;
    case 0x0119:
      return 0x00EA;
    case 0x011A:
      return 0x00CC;
    case 0x011B:
      return 0x00EC;
    case 0x0139:
      return 0x00C5;
    case 0x013A:
      return 0x00E5;
    case 0x013D:
      return 0x00A5;
    case 0x013E:
      return 0x00B5;
    case 0x0141:
      return 0x00A3;
    case 0x0142:
      return 0x00B3;
    case 0x0143:
      return 0x00D1;
    case 0x0144:
      return 0x00F1;
    case 0x0147:
      return 0x00D2;
    case 0x0148:
      return 0x00F2;
    case 0x0150:
      return 0x00D5;
    case 0x0151:
      return 0x00F5;
    case 0x0154:
      return 0x00C0;
    case 0x0155:
      return 0x00E0;
    case 0x0158:
      return 0x00D8;
    case 0x0159:
      return 0x00F8;
    case 0x015A:
      return 0x00A6;
    case 0x015B:
      return 0x00B6;
    case 0x015E:
      return 0x00AA;
    case 0x015F:
      return 0x00BA;
    case 0x0160:
      return 0x00A9;
    case 0x0161:
      return 0x00B9;
    case 0x0162:
      return 0x00DE;
    case 0x0163:
      return 0x00FE;
    case 0x0164:
      return 0x00AB;
    case 0x0165:
      return 0x00BB;
    case 0x016E:
      return 0x00D9;
    case 0x016F:
      return 0x00F9;
    case 0x0170:
      return 0x00DB;
    case 0x0171:
      return 0x00FB;
    case 0x0179:
      return 0x00AC;
    case 0x017A:
      return 0x00BC;
    case 0x017B:
      return 0x00AF;
    case 0x017C:
      return 0x00BF;
    case 0x017D:
      return 0x00AE;
    case 0x017E:
      return 0x00BE;
    case 0x02C7:
      return 0x00B7;
    case 0x02D8:
      return 0x00A2;
    case 0x02D9:
      return 0x00FF;
    case 0x02DB:
      return 0x00B2;
    case 0x02DD:
      return 0x00BD;
    default:
      return '#';
    }
  return 0;
  }


/*****************************************************************************\
  nie przetwarza poprawnie sekwencji ponaddwubajtowych! - TODO
\*****************************************************************************/

char *iso2utf8string(char *string)
{
  unsigned char *result = NULL;
  int src = 0;
  int dst = 0;
  int len = 0/* 10*/;
  unsigned int c;
  
  if (NULL != string)
  {
    while ('\0' != (c = iso2unicode((unsigned char) string[src++])))
    {
      if (c < 0x0080)
      {
        len++;
      }
      else if (c < 0x0800)
      {
        len += 2;
      }
      else if (c < 0x010000)
      {
        len += 3;
      }
      else if (c < 0x0200000)
      {
        len += 4;
      }
      else if (c < 0x04000000)
      {
        len += 5;
      }
      else if (c < 0x080000000)
      {
        len += 6;
      }
    }
    if (NULL != (result = (unsigned char *) calloc(len + 1, sizeof(char))))
    {
      src = 0;
      while (dst <= len)
      {
        c = iso2unicode((unsigned char) string[src++]);
        if (c < 0x0080)
        {
          result[dst++] = c & 0x00FF;
        }
        else if (c < 0x0800)
        {
          result[dst++] = 0x00C0 | (0x001F & (c >> 6));
          result[dst++] = 0x0080 | (0x003F & c);
        }
      }
    }
  }
  return (char *) result;
}

std::string iso2utf8string(std::string string)
{
  std::string result = "";
  unsigned int c;
  

  for (std::string::iterator it = string.begin();
       it != string.end();
       it++)
  {
    c = iso2unicode((unsigned int) *it);
    if (c < 0x0080)
    {
      result += c & 0x00FF;
    }
    else if (c < 0x0800)
    {
      result += 0x00C0 | (0x001F & (c >> 6));
      result += 0x0080 | (0x003F & c);
    }
  }

  return result;
}

/*****************************************************************************\
\*****************************************************************************/

bool isutf8charbeginning(unsigned int c)
{
  return (c < 0x0080 || c > 0x00BF);
}

/*****************************************************************************\
\*****************************************************************************/

int utf8charlen(const unsigned char *str)
{
  int c = *str;
  if (c < 0x0080) //0???????
  {
    return 1;
  }
  if (c < 0x00C0) //10??????
  {
    return 0;
  }
  if (isutf8charbeginning(str[1]))
  {
    return -1;
  }
  if (c < 0x00E0) //110?????
  {
    return 2;
  }
  if (isutf8charbeginning(str[2]))
  {
    return -2;
  }
  if (c < 0x00F0) //1110????
  {
    return 3;
  }
  return -3;
  if (isutf8charbeginning(str[3]))
  {
    return -3;
  }
  if (c < 0x00F8) //11110???
  {
    return 4;
  }
  if (isutf8charbeginning(str[4]))
  {
    return -4;
  }
  if (c < 0x00FC) //111110??
  {
    return 5;
  }
  if (isutf8charbeginning(str[5]))
  {
    return -5;
  }
  if (c < 0x00FE) //1111110?
  {
    return 6;
  }
  return -6;
}

/*****************************************************************************\
\*****************************************************************************/

int utf8charlen(unsigned int c)
{
  if (c < 0x0080) //0???????
  {
    return 1;
  }
  if (c < 0x00C0) //10??????
  {
    return 0;
  }
  if (c < 0x00E0) //110?????
  {
    return 2;
  }
  if (c < 0x00F0) //1110????
  {
    return 3;
  }
  return -1;
  if (c < 0x00F8) //11110???
  {
    return 4;
  }
  if (c < 0x00FC) //111110??
  {
    return 5;
  }
  if (c < 0x00FE) //1111110?
  {
    return 6;
  }
  return -6;
}

/*****************************************************************************\
  nie przetwarza poprawnie sekwencji ponaddwubajtowych! - TODO
\*****************************************************************************/
char *utf82isostring(char *string)
{
  unsigned char *result = NULL;
  int src = 0;
  int dst = 0;
  int len = 0;
  unsigned char res;
  unsigned int tmp = 0;
  unsigned int c;
  
  if (NULL != string)
  {
    while ('\0' != (c = (unsigned char) string[src++]))
    {
      if (isutf8charbeginning(c))
      {
        len++;
      }
    }
    if (NULL != (result = (unsigned char *) calloc(len + 1, sizeof(char))))
    {
      src = 0;
      while (dst <= len)
      {
        c = (unsigned char) string[src++];
        if (c < 0x0080)
        {
          res = c /*& 0x007F*/;
        }
        else if (c < 0x00E0) //110?????
        {
          tmp = c & 0x001F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          res = unicode2iso(tmp);
        }
        else if (c < 0x00F0) //1110????
        {
          tmp = c & 0x000F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          res = unicode2iso(tmp);
        }
        else if (c < 0x00F8) //11110???
        {
          tmp = c & 0x0007;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          res = unicode2iso(tmp);
        }
        else if (c < 0x00FC) //111110??
        {
          tmp = c & 0x0003;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          res = unicode2iso(tmp);
        }
        else if (c < 0x00FE) //1111110?
        {
          tmp = c & 0x0001;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          tmp <<= 6;
          tmp |= (unsigned char) string[src++] & 0x003F;
          res = unicode2iso(tmp);
        }
        //TODO - co jak c = FE, FF?
        result[dst++] = res;
      }
    }
  }
  return (char *) result;
}


std::string utf82isostring(std::string string)
{
  std::string result = "";
  unsigned char res;
  unsigned int tmp = 0;
  unsigned int c;
  
  std::string::iterator it = string.begin();

  while (it != string.end())
  {
    c = (unsigned char) *it;
    it++;
    if (c < 0x0080)
    {
      res = c /*& 0x007F*/;
    }
    else if (c < 0x00E0) //110?????
    {
      tmp = c & 0x001F;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      res = unicode2iso(tmp);
    }
    else if (c < 0x00F0) //1110????
    {
      tmp = c & 0x000F;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      res = unicode2iso(tmp);
    }
    else if (c < 0x00F8) //11110???
    {
      tmp = c & 0x0007;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      res = unicode2iso(tmp);
    }
    else if (c < 0x00FC) //111110??
    {
      tmp = c & 0x0003;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      res = unicode2iso(tmp);
    }
    else if (c < 0x00FE) //1111110?
    {
      tmp = c & 0x0001;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      tmp <<= 6;
      tmp |= (unsigned char) *it & 0x003F;
      it++;
      res = unicode2iso(tmp);
    }
    //TODO - co jak c = FE, FF?
    result += res;
  }
  
  return result;
}

int utf8strlen(const unsigned char *string)
{
  int len = 0;

  //unsigned int c;
  
  if (NULL != string)
  {
    //while ('\0' != (c = (unsigned char) *string))
    //{
    //  if (isutf8charbeginning(c))
    //  {
    //    len++;
    //  }
    //  string++;
    while ('\0' != *string)
    {
      int tmp = utf8charlen(string);

      if (tmp < 1)
      {
        return -1;
      }

      string += tmp;
      len++;
    }
    return len;
  }
  return -1;
}

int utf8strlen(std::string string)
{
  int len = 0;
  const unsigned char * str = (const unsigned char *) string.c_str();

  while ('\0' != *str)
  {
    int tmp = utf8charlen(str);

    if (tmp < 1)
    {
      return -1;
    }

    str += tmp;
    len++;
  }
  return len;
}

/*****************************************************************************\
\*****************************************************************************/

char *clonestring(const char *string)
  {
  char *result = NULL;

  if (string != NULL && NULL != (result = (char *) malloc(1 + strlen(string))))
    {
    strcpy(result, string);
    }
  return result;
  }


/*****************************************************************************\
\*****************************************************************************/

char *freadline(FILE *fh)
  {
  int state = 0;
  int inbuf = 0;
  int length = 0;
  static char buffer[1024];
  char c;
  char *string = NULL;
  char *tmp;
  
  while ((state != 2) && fread(buffer + inbuf, sizeof(char), 1, fh))
    {
    c = buffer[inbuf];
    switch (c)
      {
      case '\n':
        state = 2;
        break;
      default:
        state = 1;
        if (inbuf >= 1024)
          {
          if (NULL != string)
            {
            if (NULL != (tmp = (char *) realloc(string, length + 1024)))
              {
              string = tmp;
              strncpy(string + length, buffer, 1024);
              length += 1024;
              }
            else
              {
              free(string);
              string = NULL;
              }
            }
          else
            {
            if (NULL != (string = (char *) calloc(1024, sizeof(char))))
              {
              strncpy(string, buffer, 1024);
              length = 1024;
              }
            }
          inbuf = 0;
          }
        buffer[inbuf++] = c;
        break;
      }
    }
  if (state != 0)
    {
    if (NULL != string)
      {
      if (NULL != (tmp = (char *) realloc(string, length + inbuf + 1)))
        {
        string = tmp;
        strncpy(string + length, buffer, inbuf);
        length += inbuf;
        string[length] = '\0';
        }
      else
        {
        free(string);
        string = NULL;
        }
      }
    else
      {
      if (NULL != (string = (char *) calloc(inbuf + 1, sizeof(char))))
        {
        strncpy(string, buffer, inbuf);
        length = inbuf;
        string[inbuf] = '\0';
        }
      }
    }
  return string;
  }


/*****************************************************************************\
\*****************************************************************************/
char *remcontrols(char *string)
{
  unsigned char *ptr;
  for (ptr = (unsigned char *) string; *ptr != '\0'; ptr++)
  {
    if (*ptr < 0x0020 || (*ptr >= 0x0080 && *ptr < 0x00A0))
    {
      *ptr = '$';
    }
  }
  return string;
}

/*****************************************************************************\
\*****************************************************************************/
std::string lowercase(std::string src)
{
  std::string res = "";
  for (const char * c = src.c_str(); *c != '\0'; c++)
  {
    res += tolower(*c);
  }
  return res;
}
