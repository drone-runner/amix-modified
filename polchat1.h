/*AmiX - polchat1.h - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_POLCHAT1_H
#define AMIX_POLCHAT1_H

#include <string>

#include "interface.h"
#include "Renderable.h"

class tank : public Renderable
{
  protected:
    unsigned long length;
    char * data;

  public:
    tank(unsigned long, char *, bool = true);
    virtual unsigned long render(unsigned char * &);
    virtual ~tank();
    void dump(amixInterface *);

    virtual unsigned long size()
    {
      return length + 4; 
    }

    unsigned long len()
    {
      return length;
    }

    const char * get()
    {
      return data;
    }

};

#endif
