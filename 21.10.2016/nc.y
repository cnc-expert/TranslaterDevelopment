%defines
%error-verbose

%code {
	#include <stdio.h>
	#include "lex.yy.h"
	#include "main.h"
	#include "translator.h"
	}

%union {
	char* comm;
	
	void* cppString;
	int tokencode;
	}

%type<cppString> factor signed_item item expr expr_first_item_with_sign
%type<tokencode> func func2
%type<comm> block
%type<comm> var_or_num

%token<comm> NUM E
%token EOB PROG_EOF
%token<comm> LABL COMM MSG
%token COMMA
%token BGT BGE BLT BLE BNC DLY URT UCG MIR EPP RPT ERP DIS
%token G M T X Y Z F S N R I J K 
%token OPEQUAL OPDIV OPMULT OPPLUS OPMINUS OPARENT CPARENT
%token<tokencode> SIN COS TAN ARS ARC ART INT MOD SQR ABS


%%


prog:
	block EOB prog
|	PROG_EOF	{ return 0; }
;

block:
	COMM  { printf("\n %s", $$); } /*{ puts($1.r->str); $$=1234; }*/
|	submit_block
;

submit_block:
	OPDIV labld_block
|	labld_block
;

labld_block:
	LABL numberd_block
|	numberd_block
;

numberd_block:
	N NUM core_block
|	core_block
;

core_block:
	/* empty */
|	iso_block
|	expr_block
|	tlc_block
;

iso_block:
	word
|	word iso_block
;

word:
	addr NUM
|	addr OPPLUS NUM
|	addr OPMINUS NUM
|	addr E
;

addr:
	axis | I | J | K | R
|	T | G | M
;

axis:
	X | Y | Z
;

expr_block:
	E OPEQUAL expr_first_item_with_sign 
	{ PrintCPPstring ( CreateExpressionBlock($1, $3) ); }
;

expr_first_item_with_sign:
	expr
|	signed_item
|	signed_item OPPLUS expr 
		{$$ = BinaryOperation($1, '+', $3);}
|	signed_item OPMINUS expr
		{$$ = BinaryOperation($1, '-', $3);}
;

expr:
	item
|	item OPPLUS expr {$$ = BinaryOperation($1, '+', $3);}
|	item OPMINUS expr {$$ = BinaryOperation($1, '-', $3);}
;

signed_item:
	OPPLUS item {$$ = $2;}
|	OPMINUS item {$$ = MinusOperation($2);}
;

item:
	factor
|	factor OPMULT item {$$ = BinaryOperation($1, '*', $3);}
|	factor OPDIV item {$$ = BinaryOperation($1, '/', $3);}
;

factor:
	NUM {$$=CharToCppString($1);}
|	E {$$=CharToCppString($1);}
|	func OPARENT expr_first_item_with_sign CPARENT 
		{ $$ = Func($1,$3);}
|	func2 OPARENT expr_first_item_with_sign COMMA expr_first_item_with_sign  CPARENT
		{ $$ = Func2($1, $3, $5);}
|	OPARENT expr_first_item_with_sign CPARENT 
		{$$ = PutInBrackets($2); }
;



func:
	SIN | COS | TAN | ARS | ARC | ART | INT 
;

func2:
	MOD;

tlc_block:
	OPARENT tlc_body CPARENT 
;

tlc_body:
	DIS COMMA MSG /*{ dis_msg($3.r->str); }*/
|	DIS COMMA E
|	EPP COMMA LABL COMMA LABL
|	URT COMMA var_or_num { }
|	RPT COMMA var_or_num 
|	ERP {  }
|	DLY COMMA var_or_num
|	UCG COMMA NUM COMMA word word COMMA word word
|	MIR
|	MIR COMMA
|	MIR COMMA axis
|	MIR COMMA axis COMMA axis
|	BNC COMMA LABL
|	BGT COMMA var_or_num COMMA var_or_num COMMA LABL
|	BGE COMMA var_or_num COMMA var_or_num COMMA LABL
|	BLT COMMA var_or_num COMMA var_or_num COMMA LABL
|	BLE COMMA var_or_num COMMA var_or_num COMMA LABL
;

var_or_num:
	E
|	NUM
;

/*

"ASDF"

(RPT,5)
   (RPT,E2)
   	  ...
   (ERP)


   (EPP, ASDF, QWER)

(ERP)


#100=0
N12 IF [#100 GE 5] GOTO 13  (stack of block number pair)
#100=#100+1

	#101=0
	N14 IF [#101 GE 5] GOTO 15
	#101=#101+1

    GOTO 14
    N15



	#102=16
	GOTO 278 (hashtable: label -> block number)
	N16


GOTO 12
N13
*/

