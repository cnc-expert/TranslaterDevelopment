%defines
%error-verbose

%code {
	#include <stdio.h>
	#include "lex.yy.h"
	#include "main.h"
	#include "translator.h"

}

%union {
	char* comment;
	char* numberOrVariable;
	
	void* cppString;
	void* list;
	int tokenCodeMathFunc;
	int tokenSingleLetterFunc;
	int tokenAxis;
}

%type<cppString> factor signed_item item expr expr_first_item_with_sign expr_block word iso_block 
%type<list> core_block tlc_block tlc_body numberd_block confirm_block labld_block block
%type<tokenCodeMathFunc> func func2 
%type<numberOrVariable> var_or_num
%type<tokenSingleLetterFunc> addr
%type<tokenAxis> axis

%token<numberOrVariable> NUM E
%token EOB PROG_EOF
%token<comment> LABL COMM MSG
%token COMMA
%token BGT BGE BLT BLE BNC DLY URT UCG MIR EPP RPT ERP DIS
%token<tokenSingleLetterFunc> G M T F S N R I J K
%token<tokenAxis> X Y Z
%token OPEQUAL OPDIV OPMULT OPPLUS OPMINUS OPARENT CPARENT
%token<tokenCodeMathFunc> SIN COS TAN ARS ARC ART INT MOD SQR ABS


%%


prog:
	block_list PROG_EOF { 
		while(ProcessEppBlock());
		PrintProgramDeque(); 
		return 0; 
	}
;

block_list:
	block EOB block_list { CreateProgramDeque($1); }
|   block{ CreateProgramDeque($1); }
;

block:
	COMM  { $$ = CreateDefinedDequeForComments($1); }
|	confirm_block
;

confirm_block:
	OPDIV labld_block {$$ = AddOPDIVtoBlocks($2);}
|	labld_block
;

labld_block:
	LABL numberd_block {$$ = AddLabelToDequeOfBlock($2, $1);}
|	numberd_block
;

numberd_block:
	N NUM core_block {$$ = EnumerateBlocks($3, $2);}
|	core_block
;

core_block:
	{ $$ = CreateDefinedDequeForBlockString(""); } /* for empty block */
|	iso_block {$$ = CreateDequeForBlockString($1); /*PrintCppString($1);*/}
|	expr_block {$$ = CreateDequeForBlockString($1);}
|	tlc_block
;

iso_block:
	word
|	word iso_block {$$ = ConcatCppString($1, $2);}
;

word:
	addr NUM { $$ = TranslateWordWithNumber($1, "", $2); }
|	addr OPPLUS NUM { $$ = TranslateWordWithNumber($1, "+", $3); }
|	addr OPMINUS NUM { $$ = TranslateWordWithNumber($1, "-", $3); }
|	addr E { $$ = TranslateWordWithVariable($1, $2); }
;

addr:
	axis { $$ = $1; } | I | J | K | R
|	T | G | M
;

axis:
	X | Y | Z
;

expr_block:
	E OPEQUAL expr_first_item_with_sign
		{ $$ = TranslateExpressionBlock($1, $3); }
;

expr_first_item_with_sign:
	expr
|	signed_item
|	signed_item OPPLUS expr 
		{ $$ = ExecuteArithmeticOperation($1, '+', $3); }
|	signed_item OPMINUS expr
		{ $$ = ExecuteArithmeticOperation($1, '-', $3); }
;

expr:
	item
|	item OPPLUS expr 
		{ $$ = ExecuteArithmeticOperation($1, '+', $3); }
|	item OPMINUS expr 
		{ $$ = ExecuteArithmeticOperation($1, '-', $3); }
;

signed_item:
	OPPLUS item { $$ = $2; }
|	OPMINUS item { $$ = ExecuteNegativeOperation($2); }
;

item:
	factor
|	factor OPMULT item 
		{ $$ = ExecuteArithmeticOperation($1, '*', $3); }
|	factor OPDIV item 
		{ $$ = ExecuteArithmeticOperation($1, '/', $3); }
;

factor:
	NUM 
		{ $$ = ConvertCharToCppString($1); }
|	E 
		{ $$ = ConvertCharToCppString($1); }
|	func OPARENT expr_first_item_with_sign CPARENT 
		{ $$ = TranslateFunction($1, $3); }
|	func2 OPARENT expr_first_item_with_sign COMMA expr_first_item_with_sign CPARENT
		{ $$ = TranslateFunctionWithTwoArguments($1, $3, $5); }
|	OPARENT expr_first_item_with_sign CPARENT 
		{ $$ = PutExpressionInBrackets($2); }
;



func:
	SIN | COS | TAN | ARS | ARC | ART | INT 
;

func2:
	MOD
;

tlc_block:
	OPARENT tlc_body CPARENT { $$ = $2; }
;

tlc_body:
	DIS COMMA MSG {$$=CreateDefinedDequeForComments($3); }
|	DIS COMMA E {$$ = CreateDefinedDequeForBlockString("");}
|	EPP COMMA LABL COMMA LABL { $$=CreateEPPBlock($3,$5);}
|	URT COMMA var_or_num { $$=CreateURTBlock($3);}
|	RPT COMMA var_or_num {}
|	ERP {  }
|	DLY COMMA var_or_num {$$ = CreateDelayDeque($3);}
|	UCG COMMA NUM COMMA word word COMMA word word {}
|	MIR {$$ = CreateDefinedDequeForBlockString("G52.2");}
|	MIR COMMA {$$ = CreateDefinedDequeForBlockString("G52.2");}
|	MIR COMMA axis {}
|	MIR COMMA axis COMMA axis {}
|	BNC COMMA LABL {}
|	BGT COMMA var_or_num COMMA var_or_num COMMA LABL {}
|	BGE COMMA var_or_num COMMA var_or_num COMMA LABL {}
|	BLT COMMA var_or_num COMMA var_or_num COMMA LABL {}
|	BLE COMMA var_or_num COMMA var_or_num COMMA LABL {}
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

