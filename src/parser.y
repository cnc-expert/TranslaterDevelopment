%defines
%error-verbose

%code {
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
	void* classBlock;
	
}

%type<classBlock> word iso_block 
%type<cppString> factor signed_item item expr expr_first_item_with_sign expr_block 
%type<list> core_block tlc_block tlc_body numberd_block confirm_block labld_block block indented_block
%type<tokenCodeMathFunc> func func2 
%type<numberOrVariable> var_or_num
%type<tokenSingleLetterFunc> addr
%type<tokenAxis> axis

%token<numberOrVariable> NUM E
%token EOB PROG_EOF
%token<comment> LABL COMM MSG INDENT
%token COMMA
%token BNE BEQ BGT BGE BLT BLE BNC DLY URT UCG MIR EPP RPT ERP DIS UAO
%token<tokenSingleLetterFunc> G M T F S N R I J K b r
%token<tokenAxis> X Y Z
%token OPEQUAL OPDIV OPMULT OPPLUS OPMINUS OPARENT CPARENT
%token<tokenCodeMathFunc> SIN COS TAN ARS ARC ART INT MOD SQR ABS


%%


prog:
	block_list PROG_EOF { 
		while(ProcessEppBlock());
		ProcessRptBlock();
		ProcessJumpBlock();
		ProcessChamferBlock();
		ProcessG40Block();
		PrintProgramDeque(); 
		
		return 0; 
	}
;

block_list:
	indented_block EOB block_list { CreateProgramDeque($1); }
|   indented_block { CreateProgramDeque($1); }
;

indented_block:
	INDENT block { $$ = AddIndentationToBlock($1, $2); }
|	block
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
|	iso_block {$$ = CreateDequeForBlock($1); /*PrintCppString($1);*/}
|	expr_block {$$ = CreateDequeForBlockString($1);}
|	tlc_block
;

iso_block:
	word
|	word iso_block {$$ = ConcatWords($1, $2);}
;

word:
	addr { $$ = TranslateWordWithNumber($1, "", "0"); }
|	addr NUM { $$ = TranslateWordWithNumber($1, "", $2); }
|	addr OPPLUS NUM { $$ = TranslateWordWithNumber($1, "+", $3); }
|	addr OPMINUS NUM { $$ = TranslateWordWithNumber($1, "-", $3); }
|	addr E { $$ = TranslateWordWithVariable($1, $2); }

;

addr:
	axis { $$ = $1; } | I | J | K | R | S | F 
|	T | G | M | r | b
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
		{ $$ = SubstituteNCvarAsCppString($1); }
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
	DIS COMMA MSG
		{ $$=CreateDefinedDequeForComments($3); }
|	DIS COMMA E
		{ $$ = CreateDISWithVarBlock($3); } 
|	EPP COMMA LABL COMMA LABL
		{ $$=CreateEPPBlock($3,$5);}
|	UAO COMMA var_or_num
		{ $$ = ChooseCoordinateSystem($3); }
|	URT COMMA var_or_num
		{ $$ = CreateURTBlock($3); }
|	RPT COMMA var_or_num
		{ $$ = CreateRPTDeque($3); }
|	ERP
		{ $$ = CreateERPDeque(); }
|	DLY COMMA var_or_num
		{ $$ = CreateDelayDeque($3); }
|	UCG COMMA NUM COMMA word word COMMA word word
		{ $$ = CreateDefinedDequeForBlockString(""); }
|	MIR
		{ $$ = CreateDefinedDequeForBlockString("G50.1"); }
|	MIR COMMA
		{ $$ = CreateDefinedDequeForBlockString("G50.1"); }
|	MIR COMMA axis
		{$$ = CreateMIRBlock($3);}
|	MIR COMMA axis COMMA axis 
		{$$ = CreateDefinedDequeForBlockString("G51.1 X0 Y0");} 
|	BNC COMMA LABL
		{ $$ = CreateBNCBlock($3); }
|	BGT COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("GT", $3, $5, $7); }
|	BGE COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("GE", $3, $5, $7); }
|	BLT COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("LT", $3, $5, $7); }
|	BLE COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("LE", $3, $5, $7); }
|	BNE COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("NE", $3, $5, $7); }
|	BEQ COMMA var_or_num COMMA var_or_num COMMA LABL
		{ $$ = CreateJumpBlock("EQ", $3, $5, $7); }
;

var_or_num:
	E
|	NUM
|	/* Empty means zero */ { $$ = "0"; }
;

