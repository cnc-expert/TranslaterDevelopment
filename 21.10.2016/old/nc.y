/*
	Это так называемый bison grammar file.
	Здесь описывается грамматика КС-языка (контекстно-свободного).
	Файл по структуре похож на входной файл для flex:
		он поделён на три части при помощи знака двойного процента - %%
	1. Первая часть - "пролог". Здесь описывается с какими токенами
		парсер будет работать, задаются разные директивы для генерации
		(например %defines), а также другая информация, о которой
		я расскажу в понедельник.
	2. Вторая часть - правила. Грамматика КС-языка может быть задана
		множеством правил в форме Бэкуса-Наура, т.е. в левой части
		правила стоит нетерминал, а в правой - последовательность
		из нетерминалов (grouping) и терминалов (tokens). Например:

			выражение :
				слагаемое плюс выражение
			;

		Аналогично - выражение-разность:

			выражение :
				слагаемое минус выражение
			;

		Правила с одной и той же левой частью для краткости могут быть
		объединены вертикальной чертой. Например:

			выражение :
				слагаемое плюс выражение
			|	слагаемое минус выражение
			;

	3. В третьей части файла может быть написан код на любом
	поддерживаемом Бизоном языке. Эта часть необязательна.
	Весь необходимый код можно вынести в отдельную единицу
	трансляции (и это будет правильно).
*/


/* Директива %defines сообщает о необходимости генерации
заголовочного файла с описанием констант-перечисления токенов,
что нужно для лексера. */
%defines


/* Объявление токенов, т.е. терминальных символов грамматики.
Задача лексера как раз в том, чтобы распознавать эти токены,
и при каждом обнаружении токена делать
return SOME_RECOGNIZED_TOKEN из yylex(). */
%token NUM EOB PROG_EOF
%token LABL COMM MSG COMMA
%token BGT BGE BLT BLE BNC DLY URT UCG MIR EPP RPT ERP DIS
%token G M T X Y Z F S N R I J K E
%token OPEQUAL OPDIV OPMULT OPPLUS OPMINUS OPARENT CPARENT
%token SIN COS TAN ARS ARC ART INT MOD SQR ABS


%%


/* Это - раздел для правил */ 


/* Начальный символ грамматики - это нетерминал в левой части
самого первого правила. В нашем случае - prog (программа для
системы ЧПУ NC). Программа - последовательность кадров (block).
Кадры заканчиваются символом LF, называемым end-of-block - EOB.
Число кадров в программе нам заранее не известно. Поэтому
используем рекурсивное определение */
prog:
	block EOB prog
|	PROG_EOF
;


/* Любой кадр программы - это либо кадр-комментарий, либо
submit_block. Подробнее о submit_block, labld_block, numberd_block
смотри на стр. 25 инструкции по системе ЧПУ NC - п. 2.4.4. */
block:
	COMM
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


/* Подробнее о том, что есть core_block смотри на стр. 26
инструкции по системе ЧПУ NC - п. 2.5. */
core_block:
	iso_block		/* Кадр ISO */
|	expr_block		/* Кадр назанчения */
|	tlc_block		/* Кадр с трёхбуквенным кодом */
;


/* Кадр ISO - просто последовательность "слов" - п. 2.4.3 инструкции */
iso_block:
	word
|	word iso_block
;

word:
	addr NUM
;

addr:
	axis | I | J | K | R | T | G | M
;

axis:
	X | Y | Z
;


/* Кадр назначения - это когда переменной присваивается
результат какого-то выражения */
expr_block:
	E OPEQUAL expr
;


/* В том, что есть expr (выражение) - закралась ошибка! Сможете исправить?*/
expr:
	signed_item
|	signed_item OPPLUS expr
|	signed_item OPMINUS expr
;

signed_item:
	item
|	OPPLUS item
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
|	func OPARENT expr CPARENT
|	OPARENT expr CPARENT
;


/* Математические функции - только от одного переменного */
func:
	SIN | COS | TAN | ARS | ARC | ART | INT | MOD
;


/* Трёхбуквенный код */
tlc_block:
	OPARENT tlc_body CPARENT
;


/* Для каждого 3-кода - свой синтаксис */
tlc_body:
	DIS COMMA MSG
|	DIS COMMA E
|	EPP COMMA LABL COMMA LABL
|	URT COMMA var_or_num
|	RPT COMMA var_or_num
|	ERP
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




/* Третий раздел, с кодом, выносим в отдельный файл - здесь ему не место. */

