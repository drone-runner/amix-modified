/*AmiX - image.cpp - J.M.Kowalski "ABUKAJ"*/
#include <string.h>

#include <string>
#include <map>

#include "image.h"

imageMapper imagemapper;

std::string imageMapper::src(const char *img)
{

  //XXX
  return "<emote>";

//  std::string result = "";
//  
//  const char * ptr = strstr(img, "src=\"") + 5;
//
//  while (*ptr != '"')
//  {
//    result += *ptr;
//    ptr++;
//  }
//
//  return result;
}

std::string imagetopic(const char *img)
{

  //XXX
  return "<emote>";
//  const char * ptr = strstr(img, "src=\"img/") + 9;
//
//  if (ptr == NULL)
//      return std::string(img);
//
//  const char * ptr2 = strstr(ptr, ".png\"");
//
//  if (ptr2 == NULL)
//      return std::string(img);
//
//  return "<" + std::string(ptr, ptr2 - ptr) + ">";
}

imageMapper::imageMapper()
{
//  this->mapping["img/pa.gif"] = "<pa>";
//  this->mapping["img/papa.gif"] = "<papa>";
//  this->mapping["img/biker.gif"] = "<biker>";
//  this->mapping["img/czesc.gif"] = "<czesc>";
}

const char * imageMapper::operator[](const char * img)
{
//  std::string s = this->src(img);
//  
//  if (s != "" && this->mapping.count(s) > 0)
//  {
//    return this->mapping[s];
//  }
//
//  return img;
  return "<emote>";
}

