#ifndef AMIX_IMAGE_H
#define AMIX_IMAGE_H

#include <string>
#include <map>

class imageMapper
{
  protected:
    std::string src(const char *);
    std::map<std::string, const char *> mapping;

  public:
    imageMapper();
    const char * operator[](const char *);
};

extern imageMapper imagemapper;
std::string imagetopic(const char *);

#endif
