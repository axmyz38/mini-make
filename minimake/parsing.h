#ifndef 
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
  enum t_type type;
  char *content;
  struct line *next;
  struct line *prev;
};

struct mmake {
  int size;
  struct line *first;
};

#endif
