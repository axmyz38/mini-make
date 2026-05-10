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


char *replace(char *str, char *pattern, char *rep)
{
  char *f = strstr(str,pattern);
  if (!f)
    return str;

  size_t new_l = strlen(str) - strlen(pattern) + strlen(rep) + 1;
  char *new = malloc(new_l+1);
  strncpy(new, str, f - str);
  new[f - str] = '\0';
  strcat(new, rep);
  strcat(new, f + strlen(pattern));
  free(str);
  return new;
}



char *w_space(const char *l) //enleve les espace et ptn de tab smr 
{
  if(!l)
    return NULL;

  while (*l == ' ' || *l == '\t')
    l++;
  char *w = strdup(l);
  int i = strlen(w)-1;
  while(i>=0 && (w[i] == ' ' || w[i] == '\t' || w[i] == '\n'))
    w[i--] = '\0';
  return w;
}

char *w_space_debut_fin(const char *l)// w_space sans tab :(
{
  if(!l)
    return NULL;

  while (*l == ' ' || *l == '\t' )
    l++;
  char *w = strdup(l);
  int i = strlen(w)-1;
  while(i>=0 && (w[i] == ' '  || w[i] == '\n'))
    w[i--] = '\0';
  return w;

}

char *expe(char *val, struct mmake *m, struct line *s)
{
  char *new = strdup(val);
  struct line *current = m->first;
  while (current && current!=s)
  {
    if (current->type == var)
    {
      char tab[300000];
      snprintf(tab, sizeof(tab), "${%s}", current->data.var.name);
      while (strstr(new, tab))
      {
        new = replace(new, tab, current->data.var.value);
      }
      char tab2[300000];
      snprintf(tab2, sizeof(tab2), "$(%s)", current->data.var.name);
      while (strstr(new, tab2))
        new = replace(new, tab2, current->data.var.value);
    }
    current = current->next;
  }

  char *p=NULL;
  while ((p = strstr(new, "${")))
  {
    char *e=strchr(p,'}');
    if(!e) return new;
    char pat[3000000];
    strncpy(pat,p,e - p +1);
    pat[e-p+1] = '\0';
    new = replace(new,pat,"");
  }
  p = NULL;
  while ((p = strstr(new, "$(")))
  {
    char *e = strchr(p, ')');
    if (!e) return new;
    char pat[3000000];
    strncpy(pat, p, e - p + 1);
    pat[e - p + 1] = '\0';
    new = replace(new, pat, "");
  }
  return new;
}

void data_clear(struct line *l, struct mmake *m)//rempli union data 
{
  if (l->type == var)
  {
    char *s = strchr(l->content, '=');
    char *tmp = strndup(l->content,s - l->content);
   
    l->data.var.name = w_space(tmp);
    free(tmp);
    s+=1;
    char *tmp2 = strdup(s);
    l->data.var.value = w_space_debut_fin(tmp2);
    free(tmp2);
    l->data.var.value[strcspn(l->data.var.value, "\n")] = '\0';
    char *old_v = l->data.var.value;
    l->data.var.value = expe(old_v, m,l);
    free(old_v);
  }
  else if (l->type == rule)
  {
    char *s = strchr(l->content, ':');
    char *tmp3 = strndup(l->content, s - l->content);
    l->data.rule.target = w_space(tmp3);
    free(tmp3);
    s+=1;
    char *copy2= strdup(s);
    const char *separators = " \t\n";
    char *strtoken = strtok(copy2, separators);
    int i = 0;
    while (strtoken != NULL) 
    { 
      i++; strtoken = strtok(NULL, separators);
    }
    free(copy2);
    l->data.rule.deps = malloc((i + 1) * sizeof(char *));
    char *copy3 = strdup(s);
    strtoken = strtok(copy3, separators);
    i = 0;
    while (strtoken != NULL)
    {
     
      l->data.rule.deps[i] = strdup(strtoken);
      strtoken = strtok(NULL, separators);
      i++;
    }
    l->data.rule.deps[i] = NULL;
    for (int j = 0; l->data.rule.deps[j]; j++)
    {
      char *old = l->data.rule.deps[j];
      l->data.rule.deps[j] = expe(old, m, l);
      free(old);
      
    }

    free(copy3); 
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
    data_clear(new_line,new);
    append(new,new_line);
  }
  fclose(fp);
  if (line)
     free(line);

  return new;
}


void free_m(struct mmake *m)
{
  if (!m) 
    return;

  struct line *cur = m->first;
  while(cur)
  {
    struct line *next = cur->next;
    free(cur->content);
    if (cur->type == var)
    {
      free(cur->data.var.name);
      free(cur->data.var.value);

    }
    else if (cur->type == rule)
    {
      free(cur->data.rule.target);
      for (int i = 0; cur->data.rule.deps[i]; i++)
      {
        free(cur->data.rule.deps[i]);
      }
      free(cur->data.rule.deps);
    }
    free(cur);
    cur = next;
  }
  free(m);

}
