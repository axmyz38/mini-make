#ifndef HEADER
#define HEADER

typedef enum t_type {
  empty,
  command,
  var,
  rule,
  commment,
  invalid
};

struct line {
  int size;
  t_type type;
  char *content;
  union {
    struct {char *name; char *value;} var;
    struct {char *target; char **deps;} rule;
  } data;
  struct line *next;
  struct line *prev;
};

struct mmake {
  int size;
  struct line *first;
};

#endif
