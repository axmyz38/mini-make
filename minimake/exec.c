#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parsing.h"

struct line *rule_(struct  mmake *m, char *name)
{
  struct line *cur = m->first;

  while (cur )
  {
    if (cur->type == rule && strcmp(cur->data.rule.target, name) ==0)

    {
      
      return cur;
    }
    cur = cur->next;
  }

  return NULL;
}

int exec(struct mmake *m, char *name, char *need)
{
  struct line *exe = rule_(m, name);

  if (!exe)
  {
    if (need)
      printf("minimake: *** No rule to make target '%s', needed by '%s'.  Stop.\n", name, need);
    else 
      printf("minimake: *** No rule to make target '%s'.  Stop.\n", name);
    return 1;
  }

  for (int i =0; exe->data.rule.deps[i]; i++)
  {
    if (exec(m, exe->data.rule.deps[i], name) != 0)
      return 1;
  }

  struct line *cmd = exe->next;

  while (cmd && (cmd->type == empty || cmd->type == command))
  {
    if (cmd->type == command)
    {
      char *content = cmd->content;
      while (*content == '\t' || *content == ' ')
        content++;
      content[strcspn(content, "\n")] = '\0';
      content = expe(content, m, NULL);
      printf("%s\n",content);
      fflush(stdout);
      pid_t pid = fork();
      if (pid == 0)
      {
        execl("/bin/sh","/bin/sh", "-c", content,NULL);
        exit(EXIT_FAILURE);
      }

      int status;
      waitpid(pid,&status,0);
      int ret = WEXITSTATUS(status);
      if(ret!=0)
      {
        printf("minimake: *** [%s] Error %d\n", name, ret);
        return 1;
      }
    }
    cmd=cmd->next;
  }
  return 0;
}


