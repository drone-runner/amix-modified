/*AmiX - Renderable.h - (c) by ABUKAJ (J.M.Kowalski)*/
#ifndef AMIX_RENDERABLE_H
#define AMIX_RENDERABLE_H

class Renderable
{
  public:
    virtual unsigned long render(unsigned char * &) = 0;
    virtual unsigned long size() = 0;

    // must be defined somewhere if it has to be pure virtual
    virtual ~Renderable()
    {
    };

};

#endif
