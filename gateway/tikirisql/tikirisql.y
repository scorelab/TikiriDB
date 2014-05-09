%{
/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory -
 * University of Colombo School of Computing.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the tikiridb system.
 */

/**
 * \file
 *         Tikiridb Query Parser (lex/yacc).
 * \author
 *         nayanajith laxaman <nml@ucsc.cmb.ac.lk>
 *
 */
#include "parser-helper.h"
#include <stdio.h>
#include <stdlib.h>
//#define YYSTYPE char *
#define YYDEBUG 1

#define write(A){printf("%s\n",A);}

#define DEFAULT_PORT 25601
#define DEFAULT_HOST "::1"
//#define DEFAULT_HOST "10.16.80.206"


int port=DEFAULT_PORT;
char *host = DEFAULT_HOST;

%}
   /*Configuration specific tokents*/
%token PORT HOST

   /*yystype to be union of string and int */
%union
{
  int number;
  unsigned char *string;
//  int subtok;
}

	/* symbolic tokens <xxxx> defines the type which should incorporate in union*/
%token <string> NAME
%token <string> STRING
%token <number> INTNUM 

	/* literal keyword tokens */
%left  OR AND NOT
%left  <string>   COMPARISON /*= <> < > <= >=*/
%left '+' '-' '*' '/'

%token WHERE 

%token SELECT 
%token FROM 

%token CREATE
%token STORE
%token SIZE 
%token AS 
%token ON 
%token EVENT 
%token DO 
%token DELETE 
%token QUERY

	/*tikiridb specific*/
%token SAMPLE
%token FOR
%token PERIOD


	/*SELECT nodeid,light,temp FROM sensors SAMPLE PERIOD 1s FOR 10s*/
%%

sql:
   | set_host_port{yyinit();}
   | select_statement{yyinit();}
   | create_statement{yyinit();}
   | event_statement{yyinit();}
   | delete_statement{yyinit();}
   ;



select_statement:
   SELECT column_commalist from_clause sample_period for_clause ';'
   {
      send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause sample_period ';'
   {
      send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause where_clause sample_period for_clause ';'
   {
      send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause where_clause sample_period ';'
   {
      send_query_to_sf(host,port);
   }
   ;

create_statement:
   CREATE STORE store_name SIZE INTNUM AS '(' nested_select ')' ';'
   {
      //send_query_to_sf(host,port);
   }
   ;

event_statement:
   ON EVENT event_name '(' event_param ')' DO '(' nested_select ')' ';'
   {
      //send_query_to_sf(host,port);
   }
   ;

delete_statement:
   DELETE QUERY INTNUM ';'
   {
   	//set_for_period($3);
   }
   ;

nested_select:
   SELECT column_commalist from_clause sample_period for_clause 
   {
      //send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause sample_period 
   {
      //send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause where_clause sample_period for_clause 
   {
      //send_query_to_sf(host,port);
   }
   | SELECT column_commalist from_clause where_clause sample_period 
   {
      //send_query_to_sf(host,port);
   }
   ;

column_commalist:
   column 
   | column_commalist ',' column 
   ;

column:
   NAME {
	add_field($1);
	}
   ;

from_clause:
   FROM table 
   ;

table:
   NAME{
	add_table($1);
	}
   ;     

store_name:
   NAME{
	add_table($1);
	}
   ; 

event_name:
   NAME{
	add_table($1);
	}
   ;

event_param:
   INTNUM{
	//add_table($1);
	}
   ;

sample_period:
   SAMPLE PERIOD INTNUM{
   set_for_period($3);
	}
   ;

for_clause:
   FOR INTNUM{
   set_sample_period($2);
	}
   ;



   /*Start WHERE*/
where_clause:
   WHERE search_condition
   ;

search_condition:
   search_condition OR search_condition
   | search_condition AND search_condition
   | NOT search_condition
   | '(' search_condition ')'
   | predicate
   ;

predicate:
   comparison_predicate
   ;

comparison_predicate:
   scalar_exp COMPARISON scalar_exp
   {
   add_comparison($2);
   }
   ;

scalar_exp:
   scalar_exp '+' scalar_exp
   | scalar_exp '-' scalar_exp
   | scalar_exp '*' scalar_exp
   | scalar_exp '/' scalar_exp
   | lvalue
   | rvalue
   | '(' scalar_exp ')'
   ;

rvalue:
   INTNUM {
   add_rvalue($1);
   }
   ;

lvalue:
   NAME {
   add_lvalue($1);
	}
   ;
   /*End WHERE*/

set_host_port:
   set_port set_host ';'
   | set_host set_port ';'
   ;

set_port:
   PORT INTNUM {
   port=$2;
   printf("Setting PORT: %d\n",port);
	}
   ;

set_host:
   HOST NAME {
   host=$2;
   printf("Setting HOST: %s\n",host);
	}
   ;
%%

