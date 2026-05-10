#ifndef HEADER
#define HEADER

enum t_type {
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

enum t_type get_type(char *line);
struct line *init_line(void);
void append(struct mmake *m, struct line *l);
char *w_space(const char *l);
void data_clear(struct line *l, struct mmake *m);
struct mmake *parse(const char *filename);
int exec(struct mmake *m, char *name, char *need);
char *expe(char *val, struct mmake *m, struct line *s);


#endif
