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
 *         nayanajith Laxaman <nml@ucsc.cmb.ac.lk>
 *
 */



#include "tikirisqly.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>

int in_parse = 0; /*parser is unser parsing?*/
#define TOK(name) { in_parse=0; return name; }

//#define DEBUG 1

#ifdef DEBUG
#define LOG_DEBUG(...) printf("[ DEBUG ] "__VA_ARGS__)
#define LOG_ERROR(...) printf("[ ERROR ] "__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#endif

int lineno = 1; /**/
void yyerror(char *s); 
void start_yyerror(); 
void yyinit(); /*(init) discard its current Lex buffer*/
void parse_string(char *string);
static YY_BUFFER_STATE in_buffer;
//static char *in_buffer;

/*
int
yywrap()
 { 
   return 1; 
 }
*/

/*---------------------------------------------------------------------------*/
/*show help of tikirisql*/
int 
show_help()
{
   FILE *file = fopen("help.txt", "r");
   char ch;
   if (file) 
   {
      while((ch = fgetc( file )) != EOF) 
      {
         printf("%c", ch);
      }
   } 
   else 
   {
      printf("Help File does not exist.\n");
   }
   fclose(file);
}

/*---------------------------------------------------------------------------*/
/*quit from parser*/
void 
quit()
{
	printf("Bye\n");
  	exit(0); 
}

/*---------------------------------------------------------------------------*/
void
set_file(char *file){
   printf("file:%s\n\n",file);
}

/*---------------------------------------------------------------------------*/
void 
source_in(char source[])
{
   if((yyin = fopen(source,"r")) == NULL)
   {
      perror(source);
      while(1)
      {
         printf("tsql> ");
      }

   }
}


/*---------------------------------------------------------------------------*/
/*display the promt; if parser is in-parse ->, if parser is free tsql> is displayed*/
void 
prompt()
{
   if(in_parse)
   {
      printf("tsql> ");
   }
   else
   {
      printf("   -> ");
   }
}

%}
   /*exclusive start condition*/
%x START 
%%

   /*command line input*/
<START>-h        |
       "-?"      |
       --help    {start_yyerror();}

<START>-H        |
       --host    {
         yylval.string=strdup(yytext); 
         TOK(HOST);
       }

<START>-P        |
       --port    {
         yylval.string=strdup(yytext); 
         TOK(PORT);
       }

<START>-F[ ][A-Za-z:*][A-Za-z0-9_:.]*         |
       --file[ ][A-Za-z:*][A-Za-z0-9_:.]*     {
         strtok(yytext, " ");
         set_file(strtok(NULL, " "));
       }
   /*<START>.        start_yyerror();*/


   /*Configuration specific tokents*/
PORT     TOK(PORT)
HOST     TOK(HOST)

   /* literal keyword tokens */

FROM        TOK(FROM)
SELECT      TOK(SELECT)

CREATE      TOK(CREATE)
STORE       TOK(STORE)
SIZE        TOK(SIZE)
AS          TOK(AS)
ON          TOK(ON)
EVENT       TOK(EVENT)
DO          TOK(DO)
DELETE      TOK(DELETE)
QUERY       TOK(QUERY)

   /* where cause */
WHERE       TOK(WHERE)
OR          TOK(OR)
AND         TOK(AND)
NOT         TOK(NOT)



	/*tikiridb specific*/
SAMPLE	   TOK(SAMPLE)
PERIOD	   TOK(PERIOD)
FOR	      TOK(FOR)


	/*help*/
help	|
\\h	{
		show_help();
}


   /*quit*/
quit |
\\q  {
	  quit();
}


   /*source in*/
source[ ]*[A-Za-z*][A-Za-z0-9_]* |
\\s[ ]*[A-Za-z*][A-Za-z0-9_]*    {
   source_in(yytext);
}

   /*comparison*/

"="   |
"<>"  |
"<"   |
">"   |
"<="  |
">="  { 
   yylval.string=strdup(yytext); 
   return COMPARISON; 
}


<*>[+*/(),;]  { 
   in_parse=1; 
   return yytext[0]; 
}


<*>[A-Za-z:*][A-Za-z0-9_:.-]* { 
                  yylval.string=strdup(yytext); 
                  return NAME; 
               }

   /* numbers */
<*>[0-9]+            |
   [0-9]+"."[0-9]*   |
   "."[0-9]*         { 
                  yylval.number=atoi(yytext); 
                  return INTNUM; 
               }


   /* strings  */
'[^'\n]*'   {
   char *pszBuffer;
   int c;

   pszBuffer = (char*)strdup(yytext);
   c = input();
   unput(c);  /* just peeking */
   if(c  !=  '\'')   
   {
      yylval.string=(char*)strdup(pszBuffer);
      free(pszBuffer);
      return STRING;
   }
   else
   {
      free(pszBuffer);
      yymore() ;
      printf("%s",yytext);
   }
}
   /* whitespace */

\n  { 
   lineno++;
   prompt();
}

[ \t\r]+    ;  /* whitespace */

"--".*      ;  /* comment */


   /* anything else */
.        yyerror("invalid character");
%%



/*---------------------------------------------------------------------------*/
/**/
void 
yyerror(char *s)
{
   in_parse = 1;
   prompt();
   printf("%d: %s at %s\n", lineno, s, yytext);
   prompt();
}


/*---------------------------------------------------------------------------*/
/**/
void
start_yyerror()
{
   printf("Usage is:\ntikirisql [--help | -h | -? ] [(-H | --host) hostname] [(-P | --port) port] [(--file | -F) filename]\n\n");
   exit(0);
}

   /*init lex*/
void
yyinit(){
   //YY_FLUSH_BUFFER;
   //BEGIN (INITIAL);
   //BEGIN (0);
   //yywrap();
   //printf("tsql> ");
   //printf("---");

   //in_buffer = NULL;
}

/*---------------------------------------------------------------------------*/
/*parsing the string*/
void
parse_string(char *string){
      in_buffer = yy_scan_string (string);/*read buffer in to yy_buffer*/
      yyparse ();/*parse the string*/
//      yy_delete_buffer(in_buffer);/*delete yy_buffer*/
}

/*---------------------------------------------------------------------------*/
/* main method of parser */
main(int argc, char *argv[])
{
 if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
      perror("Warning: failed to ignore SIGPIPE");
    }
    
   if (argc > 1) {
      BEGIN(START); /*change parser in to START state*/

      LOG_DEBUG("args: %d\n",argc);
      char buffer[256];
      bzero(buffer,256);

      int i;
      for(i=1;i<argc;i++){
         strcat(buffer,argv[i]); /*read input strings into buffer*/
         if (i < argc-1)
         strcat(buffer," ");/*input word seperation*/
      }
      strcat(buffer,";");/*input word seperation*/
      LOG_DEBUG("string: %s\n",buffer);

      parse_string(buffer);

      BEGIN (INITIAL); /*change parser in to query processing state*/
    }

   printf("\nWelcome to the TikiriSQL client.  Commands end with ;.\n");
   printf("TikiriSQL version: 0.0.1 (beta release)\n\n");
   printf("Type 'help;' or '\\h' for help.\n");

   in_parse=1;
   prompt();
   /* parse all the text until quit */
   //get_key_command();
   while(1)
   {
      char buffer[256];
      bzero(buffer,256);
      fgets(buffer,255,stdin); /*read standard input into buffer*/

      if (strcmp(buffer,"\n")!=0){
         parse_string(buffer);
      }else{
         prompt();
      }
    }
}
