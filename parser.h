#ifndef __PARSER_H
#define __PARSER_H

#include <stdbool.h>
#include <wchar.h>

#ifdef      DEBUG
void enter();
void leave();
#else
#define enter(x) ;
#define leave(x) ;
#endif //   DEBUG

bool expecti(wchar_t c);
bool expect(wchar_t c);
bool noexpect(wchar_t *c);
extern wchar_t next;
void error(char *);
void scani();
void scan();

#endif /* __PARSER_H */
