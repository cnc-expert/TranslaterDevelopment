#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

extern int linenum;

void yyerror (const char *s);
char* AddToSymbolTable(char* yytext);


#ifdef __cplusplus
}
#endif

#endif // MAIN_H

