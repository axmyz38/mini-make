#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parsing.h"




enum t_type get_type(char *line) //donne le enum type 
{
  if (line[0] == '#' )
    return commment;
  if (line[0] == '\t')
      return command;
  if (line[0] == '\n')
    return empty;


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
    return NULL;
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




char *w_space(const char *l) //enleve les espace et ptn de tab smr 
{
  while (*l == ' ' || *l == '\t')
    l++;
  char *w = strdup(l);
  int i = strlen(w)-1;
  while(i>=0 && (w[i] == ' ' || w[i] == '\t' || w[i] == '\n'))
    w[i--] = '\0';
  return w;
}

char *expe(char *val, struct mmake *m)
{
  char *new = strdup(val);
  struct line *current = m->first;
  while (current)
  {
    if (current->type == var)
    {
      char tab[300000];
      snprintf(tab, sizeof(tab), "${%s}", current->data.var.name);
      while (strstr(new, tab))
      {
        new = expe(new, tab, current->data.var.value);
      }
    }
    current = current->next;
  }
  return new;
}

void data_clear(struct line *l)//rempli union data 
{
  if (l->type == var)
  {
    char *s = strchr(l->content, '=');
    l->data.var.name = w_space(strndup(l->content,s - l->content));
    s+=1;
    l->data.var.value = w_space(strdup(s));
    l->data.var.value[strcspn(l->data.var.value, "\n")] = '\0';
  }
  else if (l->type == rule)
  {
    char *s = strchr(l->content, ':');
    l->data.rule.target = w_space(strndup(l->content, s - l->content));
    s+=1;
    char *copy = strdup(s);
    const char * separators = " \t\n";
    char * strtoken = strtok ( copy, separators );
    int i = 0;
    while ( strtoken != NULL ) 
    {
      strtoken = strtok ( NULL, separators );
      i++;
    }
    l->data.rule.deps = malloc ((i+1) * sizeof(char *));
    i = 0;
    char *copy2 = strdup(s);
    strtoken = strtok(copy2, separators);
    while ( strtoken != NULL ) 
    {
      l->data.rule.deps[i] = strdup(strtoken);
      strtoken = strtok ( NULL, separators );
      i++;
    }
    l->data.rule.deps[i] = NULL;
    free(copy);
    free(copy2);

  }
 }


struct mmake *parse(const char *filename) // fct de parsing 
{  
  struct mmake *new = malloc(sizeof(struct mmake));
  if (!new)
    return NULL;
  new->size = 0;
  new->first = NULL;
  

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  
  fp = fopen( filename, "r");
    if (fp == NULL)
        return NULL;
  while ((read = getline(&line, &len, fp)) != -1) 
  {
    struct line *new_line = init_line();
    if (!new_line)
      return NULL;

    new_line->size = len;
    new_line->type = get_type(line);
    new_line->content = strdup(line);
    data_clear(new_line);
    append(new,new_line);
  }
  fclose(fp);
  if (line)
     free(line);

  return new;
}



