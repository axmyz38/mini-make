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
      while(next && next->type == command)
      {
        char *content = next->content+1;
        const char *t = strchr(content, '\n');
        content[strcspn(content, "\n")] = '\0';
        printf("\t'%s'\n", content);
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

  if (argc == 1)
  {
    parse("Makefile");
    return 0; 
  }

  for (int i = 0; i < argc; i++)
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
      i++;
      
                  
    }

  }

  if (pprint !=0)
  {
    p_print(parse(filename));
    return 0;
  }
  parse(filename);
  return 0;

}


