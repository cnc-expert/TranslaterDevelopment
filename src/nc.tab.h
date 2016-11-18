/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_NC_TAB_H_INCLUDED
# define YY_YY_NC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUM = 258,
    E = 259,
    EOB = 260,
    PROG_EOF = 261,
    LABL = 262,
    COMM = 263,
    MSG = 264,
    COMMA = 265,
    BGT = 266,
    BGE = 267,
    BLT = 268,
    BLE = 269,
    BNC = 270,
    DLY = 271,
    URT = 272,
    UCG = 273,
    MIR = 274,
    EPP = 275,
    RPT = 276,
    ERP = 277,
    DIS = 278,
    G = 279,
    M = 280,
    T = 281,
    F = 282,
    S = 283,
    N = 284,
    R = 285,
    I = 286,
    J = 287,
    K = 288,
    X = 289,
    Y = 290,
    Z = 291,
    OPEQUAL = 292,
    OPDIV = 293,
    OPMULT = 294,
    OPPLUS = 295,
    OPMINUS = 296,
    OPARENT = 297,
    CPARENT = 298,
    SIN = 299,
    COS = 300,
    TAN = 301,
    ARS = 302,
    ARC = 303,
    ART = 304,
    INT = 305,
    MOD = 306,
    SQR = 307,
    ABS = 308
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "src/nc.y" /* yacc.c:1909  */

	char* comment;
	char* numberOrVariable;
	
	void* cppString;
	void* list;
	int tokenCodeMathFunc;
	int tokenSingleLetterFunc;
	int tokenAxis;

#line 119 "nc.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_NC_TAB_H_INCLUDED  */
