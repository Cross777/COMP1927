// crawl.c ... build a graph of part of the web
// Written by John Shepherd, September 2015
// Uses the cURL library and functions by Vincent Sanders <vince@kyllikki.org>
// Modified by Timothy Hor (z5019242) & Raymond Osborne (z5020527)

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include "stack.h"
#include "set.h"
#include "graph.h"
#include "url.h"
#include "url_file.h"

#define BUFSIZE 1024

#define CSEURL "http://www.cse.unsw.edu.au/"

void setFirstURL(char *, char *);
void normalise(char *, char *, char *, char *, int);
void strlower(char *);

int main(int argc, char **argv)
{
   URL_FILE *handle;
   char buffer[BUFSIZE];
   char baseURL[BUFSIZE];
   char firstURL[BUFSIZE];
   char next[BUFSIZE];
   int  maxURLs;

   if (argc > 2) {
      strcpy(baseURL,argv[1]);
      setFirstURL(baseURL,firstURL);
      maxURLs = atoi(argv[2]);
      if (maxURLs < 40) maxURLs = 40;
   }
   else {
      fprintf(stderr, "Usage: %s BaseURL MaxURLs\n",argv[0]);
      exit(1);
   }
      
   // create ToDo list
   Stack todoList = newStack();

   // add firstURL to the ToDo list
   pushOnto(todoList, firstURL);
   
   // initialise Graph to hold up to maxURLs
   Graph G = newGraph(maxURLs);

   // initialise set of Seen URLs
   Set seenURLs = newSet();

   // while (ToDo list not empty and Graph not filled)
   while (!emptyStack(todoList) && nVertices(G) <= maxURLs) {
      // showStack(todoList); // T/D
      // grab Next URL from ToDo list
      strcpy(next, popFrom(todoList));
      // printf("next = %s\n", next); // T/D

      // open URL
      URL_FILE *currURL = url_fopen(next,"r");

      // if (not allowed) continue
      int charsToCheck = strlen(CSEURL);
      if (strncmp(next, CSEURL, charsToCheck) != 0) {
         continue;
      }

      // foreach line in the opened URL
      char line[BUFSIZE];
      while (url_fgets(line, BUFSIZE, currURL) != NULL) {
         // printf("line = %s", line); // T/D
         // foreach URL on that line
         char url[BUFSIZE];
         char *foundURL;
         foundURL = nextURL(line);
         // printf("foundURL = %s ", foundURL); // T/D
         while (foundURL != NULL) {
            getURL(foundURL, url, BUFSIZE-1);
            // if (Graph not filled or both URLs in Graph)
            if (nVertices(G) <= maxURLs || isConnected(G, next, url)) {
               // add an edge from Next to this URL
               addEdge(G, next, url);
            }
            // if (this URL not Seen already)
            if (!isElem(seenURLs, url)) {
               // add it to the Seen set
               insertInto(seenURLs, url);
               //add it to the ToDo list
               pushOnto(todoList, url);
            }
            foundURL = nextURL(foundURL);
         }
      }

      // close the opened URL
      url_fclose(currURL);

      sleep(1);
   }
  
   // showGraph(G, 0); // T/D

   if (!(handle = url_fopen(firstURL, "r"))) {
      fprintf(stderr,"Couldn't open %s\n", next);
      exit(1);
   }
   while(!url_feof(handle)) {
      url_fgets(buffer,sizeof(buffer),handle);
      //fputs(buffer,stdout);
      strlower(buffer);
      char *cur, link[BUFSIZE], full_link[BUFSIZE];
      cur = buffer;
      while ((cur = nextURL(cur)) != NULL) {
         getURL(cur, link, BUFSIZE-1);
         normalise(link, next, baseURL, full_link, BUFSIZE-1);
         printf("Found %s\n",full_link);
         cur += strlen(link);
      }
   }
   url_fclose(handle);
   sleep(1);
   return 0;
}

// normalise(In,From,Base,Out,N)
// - converts the URL in In into a regularised version in Out
// - uses the base URL to handle URLs like "/..."
// - uses From (the URL used to reach In) to handle "../"
//   - assumes that From has already been normalised
void normalise(char *in, char *from, char *base, char *out, int n)
{
   if (strstr(in, "http") != NULL)
      strcpy(out, in);
   else if (in[0] == '/') {
      strcpy(out, base);
      strcat(out, in);
   }
   else {
      strcpy(out, base);
      strcat(out, "/");
      strcat(out, in);
   }
}

// setFirstURL(Base,First)
// - sets a "normalised" version of Base as First
// - modifies Base to a "normalised" version of itself
void setFirstURL(char *base, char *first)
{
   char *c;
   if ((c = strstr(base, "/index.html")) != NULL) {
      strcpy(first,base);
      *c = '\0';
   }
   else if (base[strlen(base)-1] == '/') {
      strcpy(first,base);
      strcat(first,"index.html");
      base[strlen(base)-1] = '\0';
   }
   else {
      strcpy(first,base);
      strcat(first,"/index.html");
   }
}

// strlower(Str)
// - set all chars in Str to lower-case
void strlower(char *s)
{
   for ( ; *s != '\0'; s++)
      *s = tolower(*s);
}
