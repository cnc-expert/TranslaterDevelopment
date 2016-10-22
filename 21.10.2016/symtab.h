#ifndef SYMTAB_H
#define SYMTAB_H


enum type {
	typ_msg,
	typ_comm,
	typ_label
};


typedef struct symrec {
	char *str;
	enum type t;
	symrec *next;
} symrec;


void symtab_put(const char *s, enum type t);


char *symtab_getstr(symrec *r);

#endif

