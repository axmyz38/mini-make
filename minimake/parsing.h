#ifndef 
#define HEADER

typedef enum {
  empty,
  command,
  var,
  rule,
  commment,
  invalid
};

struct line {
  int size;
  enum type;
  struct line *next;
  struct line *prev;
},

struct mmake {
  int size;
  struct line *first;
},

#endif
