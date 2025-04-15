/*HTTP client - client.c - v. 0.1 - J.M.Kowalski "ABUKAJ"*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "interface.h"
#ifdef ABUSE
#include "abuse-version.h"
#else
#include "version.h"
#endif
#include "polchat.h"

#define TM const struct tm
#define MAX_CL 10240
#define VALID_CACHE 10
#define SA const struct sockaddr
#define WHOIS_PORT 43

//int checkupdate()
//{
//  char buffer[1024];
//  char *string = NULL;
//  int sfd = -1;
//  struct addrinfo hints, *res, *tres;
//  int length = 0;
//  int state = 0;
// 
//   memset(&hints, 0, sizeof(hints));
//   hints.ai_family = PF_UNSPEC;
//   hints.ai_socktype = SOCK_STREAM;
//   
//   if (0 == getaddrinfo(AMIXSERVER, "http", &hints, &res))
//   {
//     for (tres = res; tres; tres = tres->ai_next)
//     {
//       if ((sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != -1)
//       {
//         if ((connect(sfd, res->ai_addr, res->ai_addrlen)) < 0)
//         {
//           interface->nl();
//           interface->put("Connection to " AMIXSERVER " failed...");
//           close(sfd);
//           sfd = -1;
//           continue;
//         }
//       }
//       else
//       {
//         interface->nl();
//         interface->put("Unable to create socket :-(...");
//       }
//       break;
//     }
//   
//     if (sfd >= 0)
//     {
//       strcpy(buffer, "GET " AMIXPATH "  HTTP/1.1\r\nHost: " AMIXSERVER "\r\n\r\n");
//                                               
//       if (write(sfd, buffer, strlen(buffer)))
//       {
//         while ((state != 6) && read(sfd, buffer, 1))
//         {
//           switch (state)
//           {
//             case 0:
//               if (buffer[0] == '\r')
//               {
//                 state = 1;
//               }
//               else
//               {
//                 /*putchar(buffer[0]);*/
//               }
//               break;
//             case 1:
//               if (buffer[0] == '\n')
//               {
//                 state = 2;
//                 /*putchar('\n');*/
//               }
//               else
//               {
//                 state = 0;
//                 /*putchar('\r');
//                 putchar(buffer[0]);*/
//               }
//               break;
//             case 2:
//               if (buffer[0] == '\r')
//               {
//                 state = 3;
//               }
//               break;
//             case 3:
//               if (buffer[0] == '\n')
//               {
//                 state = 4;
//               }
//               else
//               {
//                 state = 2;
//               }
//               break;
//             case 4:
//               if (buffer[0] == '\r')
//               {
//                 state = 5;
//               }
//               else
//               {
//                 state = 2;
//               }
//               break;
//             case 5:
//               if (buffer[0] == '\n')
//               {
//                 state = 6;
//               }
//               else
//               {
//                 state = 2;
//               }
//               break;
//           }
//         }
//
//         if (NULL != (string = readline(sfd)))
//         {
//           if (strcmp(VERCODE, string) < 0)
//           {
//             //TODO: rethink it
//             interface->nl();
//             interface->printpol("<b><blink>UWAGA!</blink> Jest nowa wersja programu AmiX!</b>");
//             free(string);
//           }
//           while (NULL != (string = readline(sfd)))
//           {
//             interface->nl();
//             interface->printpol(string);
//             free(string);
//           }
//           
//           interface->print();
//           interface->update();                            
//         }
//       }
//       else
//       {
//         interface->nl();
//         interface->put("Unable to send http request.");
//       }
//       close(sfd);
//     }
//     else
//     {
//       if (debug)
//       {
//         interface->nl();
//         interface->put("Unable to connect " AMIXSERVER " .");
//       }
//     }
//     freeaddrinfo(res);
//   }
//   else
//   {
//     if (debug)
//     {
//       interface->nl();
//       interface->put("Resolver problem (unable to resolve " AMIXSERVER ").");
//     }
//   }
//  
//  return length;
//}


char *readline(int sfd)
{
  int state = 0;
  int inbuf = 0;
  int length = 0;
  static char buffer[1024];
  char c;
  char *string = NULL;
  char *tmp;
  
  while ((state != 2) && read(sfd, buffer + inbuf, 1))
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

