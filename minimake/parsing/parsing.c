#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parsing.h"








struct mmake *new(void)
{
  
  struct mmake *new = malloc(sizeof(struct mmake));
  if (!new)
    return -1;

  FILE *file;
  char *line = NULL;
  size_t line = 0;
  ssize_t nread;

  struct line *naw = malloc(sizeof(struct line));

  if (!naw)
    return -1;

  fp = fopen("Makefile", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
  int i = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
  
    if (strchr(line,'='))
      int i = strchr(line,'=');
    while (isspace(line[i]))
    {
      i++;
    }
    
    if (strncmp(line[i], "gcc", 3))
      naw->type = 
          
        
  }
  if (line)
     free(line);

    exit(EXIT_SUCCESS);
}
 
void type(char *line)
{
  if (line[0] == '#' || line[0] == ' ')
      return comment;
  if (line[0] == '\t')
      return command;
  int i = 0;
  while (line)
  {
    if (line[i] == ':')
        return rule;
    if (line[i] == '=')
        return var;
  }
  
}
