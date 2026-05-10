#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parsing.h"



void p_help(void)
{
  printf("Usage: minimake [OPTIONS] [TARGETS]\n\
    OPTIONS can be:\n\
        \"-h\"            to display the helper\n\
        \"-p\"            to print the rules and variables\n\
        \"-f\" <filename> to change the make config file, \"Makefile\" by default\n\
    TARGETS are target name of makefile rule to execute\n"
);
}


void p_print(struct mmake *m)
{
  if (!m)
    return;
  
  printf("# variables\n");

  struct line *current = m->first; 

  

  while (current)
  {
    if (current->type == var)
    {
      printf("'%s' = '%s'\n", current->data.var.name, current->data.var.value);
      current = current->next;
      
    }
    else 
      current = current->next;

  }

  printf("\n# rules\n");

  current = m->first;

  while (current)
  {
    if (current->type == rule)
    {
      printf("(%s):", current->data.rule.target);
      for (int i = 0; current->data.rule.deps[i]; i++)
      {
        printf(" [%s]", current->data.rule.deps[i]);
      }
      printf("\n");
      
      struct line *next = current->next;
      while(next && (next->type == command || next->type == empty))
      {
        if (next->type == command)
        {
        char *content = next->content;
        while (*content == '\t' || *content == ' ')
            content++;
        content[strcspn(content, "\n")] = '\0';
        printf("\t'%s'\n", content);
        }
        next = next->next;
      }
      
    }
    current = current->next;
  }
}











int main(int argc, char *argv[])
{
  int pprint = 0;

  char *filename = "Makefile";
  int fil = 0;
  

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-h")==0)
    {
      p_help();
      
      return 0;
    }
  }

  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-p")==0)
    {
            pprint++;
    }
    else if (strcmp(argv[i], "-f")==0)
    {
      if (i+1>=argc)
      {
        printf("pas de fichier \n");
        
        return 2;
      }
      filename = strdup(argv[i+1]);
      fil++;
      i++;
      
                  
    }

  }
  
  struct stat st;
  if (stat(filename, &st) != 0)
  {
    fprintf(stderr, "minimake: *** No such file: '%s'.  Stop.\n", filename);
    if (fil) free(filename);
    return 3;
  }


  struct mmake *m = parse(filename);
  if (!m) 
  {
    printf("minimake: *** No targets.  Stop.\n");
    if(fil)
      free(filename);
    return 2;
  }
  if (pprint != 0)
  {
    p_print(m);
    free_m(m);
    if(fil)
      free(filename);
    return 0;

  }

  int target = 0;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], " -h") == 0)
      continue;

    if (strcmp(argv[i], "-f")==0)
    {
       i++;
       continue;
    }

    target =1;
    exec(m, argv[i], NULL);
  
  }
  
  if (!target)
  {
    struct line *cur = m->first;

    while(cur && cur->type != rule)
    {
      cur = cur->next;
    }
    if (!cur)
    {
      printf("minimake: *** No targets.  Stop.\n");
      
      free_m(m);
      if(fil)
        free(filename);
      return 2;

    }
    int r= exec( m, cur->data.rule.target, NULL);
    free_m(m);
    if(fil)
      free(filename);
    return r;
    
  }

  free_m(m);
  if(fil)
    free(filename);
  return 0;
}


