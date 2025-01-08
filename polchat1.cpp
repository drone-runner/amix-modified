/*AmiX - polchat1.cpp - (c) by ABUKAJ (J.M.Kowalski)*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <string>

//libboost-thread-dev
//libboost-system-dev
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "polchat.h"
#include "polchat1.h"
#include "polchat2.h"
#include "interface.h"
#include "temp.h"


tank::~tank()
{
  if (NULL != data)
  {
    delete data;
  }
}

unsigned long tank::render(unsigned char * & buffer)
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
  memcpy(&buffer[4], data, length);
  return this->size();
}

tank::tank(unsigned long len, char * dat, bool clone)
{
  length = len;
  if (clone)
  {
    data = new char[len];
    if (data != NULL)
    {
      memcpy(data, dat, len);
    }
  }
  else
  {
    data = dat;
  }
}

void tank::dump(amixInterface * interface)
{
  int size;
  int i, j;

  interface->nl();
  interface->put("Size: 0x");
  interface->puthex((unsigned long) length, 8);
  size = length;
  for (i = 0; i < size / 32; i++)
  {
    interface->nl();
    interface->put("0x");
    interface->puthex(i * 32, 4);
    for (j = 0; j < 32; j++)
    {
      interface->putchar(' ');
      interface->puthex((unsigned char) data[i * 32 + j], 2);
    }
    interface->put("  ");
    for (j = 0; j < 32; j++)
    {
      if (isgraph((unsigned char) data[i * 32 + j]))
      {
        interface->putchar((unsigned char) data[i * 32 + j]);
      }
      else
      {
        interface->putchar('.');
      }
    }
  }

  interface->nl();
  interface->put("0x");
  interface->puthex(i * 32, 4);
  for (j = 0; j < size % 32; j++)
  {
    interface->putchar(' ');
    interface->puthex((unsigned char) data[i * 32 + j], 2);
  }
  for (; j < 32; j++)
  {
    interface->put("   ");
  }
  interface->put("  ");
  for (j = 0; j < size % 32; j++)
  {
    if (isgraph((unsigned char) data[i * 32 + j]))
    {
      interface->putchar((unsigned char) data[i * 32 + j]);
    }
    else
    {
      interface->putchar('.');
    }
  }
}
