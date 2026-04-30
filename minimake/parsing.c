#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parsing.h"




t_type get_type(char *line) //donne le enum type 
{
  if (line[0] == '#' )
    return comment;
  if (line[0] == '\t')
      return command;
  if (line[0] == ' ')
    return invalid;

  int i = 0;
  while (line[i])
  {
    if (line[i] == ':')
        return rule;
    if (line[i] == '=')
        return var;
    i++;
      }
  return empty;

}



struct line *init_line(void) //initialsie une line 
{
  struct line *new = malloc(sizeof(struct line));
  if (!new)
    return -1;
  new->size = 0;
  new->type = 0;
  new->content = NULL;
  new->next = NULL;
  new->prev = NULL;
  return new;
}






void append(struct mmake *m, struct line *l) // s occupe du double chainage
{
  if (!m)
    return;
  if (!l)
    return ;
  
  if (m->size == 0)
  {
    m->size +=1;
    m->first = l;
    return; 
  }
  

  m->size += 1;

  struct line *current = m->first;

  while(current)
  {
    if (current->next == NULL)
    {
      l->prev = current;
      current->next = l;
      return;
    }
    current = current->next;

  }
  
}









struct mmake *parse(const char *filename) // fct de parsing
{
  
  struct mmake *new = malloc(sizeof(struct mmake));
  if (!new)
    return NULL;

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  
  fp = fopen( filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
  while ((read = getline(&line, &len, fp)) != -1) 
  {
    struct line *new_line = init_line();
    if (!new_line)
      return NULL;

    new_line->size = len;
    new_line->type = get_type(line);
    new_line->content = strdup(line);
    append(new,new_line);
  }
  fclose(fp);
  if (line)
     free(line);

  return new;
}
 

