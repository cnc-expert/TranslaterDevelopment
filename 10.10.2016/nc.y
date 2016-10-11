%defines
%error-verbose

%union {
	double num;
	symrec *r;

}


%token<num> NUM
%token EOB PROG_EOF
%token<r> LABL COMM MSG
%token COMMA
%token BGT BGE BLT BLE BNC DLY URT UCG MIR EPP RPT ERP DIS
%token G M T X Y Z F S N R I J K E
%token OPEQUAL OPDIV OPMULT OPPLUS OPMINUS OPARENT CPARENT
%token SIN COS TAN ARS ARC ART INT MOD SQR ABS

%type<num> block

%%


prog:
	block EOB prog
|	PROG_EOF	{ return 0; }
;

block:
	COMM  { puts($1.r->str); $$=1234; }
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
;

expr_first_item_with_sign:
	expr
|	signed_item
|	signed_item OPPLUS expr
|	signed_item OPMINUS expr
;

expr:
	item
|	item OPPLUS expr
|	item OPMINUS expr
;

signed_item:
	OPPLUS item
|	OPMINUS item
;

item:
	factor
|	factor OPMULT item
|	factor OPDIV item
;

factor:
	NUM
|	E
|	func OPARENT expr_first_item_with_sign CPARENT
|	OPARENT expr_first_item_with_sign CPARENT
;

func:
	SIN | COS | TAN | ARS | ARC | ART | INT | MOD
;

tlc_block:
	OPARENT tlc_body CPARENT
;

tlc_body:
	DIS COMMA MSG { dis_msg($3.r->str) }
|	DIS COMMA E
|	EPP COMMA LABL COMMA LABL
|	URT COMMA var_or_num
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

