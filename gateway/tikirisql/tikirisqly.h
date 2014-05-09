/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PORT = 258,
     HOST = 259,
     NAME = 260,
     STRING = 261,
     INTNUM = 262,
     NOT = 263,
     AND = 264,
     OR = 265,
     COMPARISON = 266,
     WHERE = 267,
     SELECT = 268,
     FROM = 269,
     CREATE = 270,
     STORE = 271,
     SIZE = 272,
     AS = 273,
     ON = 274,
     EVENT = 275,
     DO = 276,
     DELETE = 277,
     QUERY = 278,
     SAMPLE = 279,
     FOR = 280,
     PERIOD = 281
   };
#endif
/* Tokens.  */
#define PORT 258
#define HOST 259
#define NAME 260
#define STRING 261
#define INTNUM 262
#define NOT 263
#define AND 264
#define OR 265
#define COMPARISON 266
#define WHERE 267
#define SELECT 268
#define FROM 269
#define CREATE 270
#define STORE 271
#define SIZE 272
#define AS 273
#define ON 274
#define EVENT 275
#define DO 276
#define DELETE 277
#define QUERY 278
#define SAMPLE 279
#define FOR 280
#define PERIOD 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 63 "tikirisql.y"

  int number;
  unsigned char *string;
//  int subtok;



/* Line 2068 of yacc.c  */
#line 110 "tikirisqly.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


