%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <errno.h>
    #include <limits.h>
    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

/* WRITEME: Write any definitions here. You can find information on
            definitions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC10 */
DIGIT    [0-9]*
ID       [a-zA-Z][a-zA-Z0-9]*


/* WRITEME: If you are going to use start conditions, write them here.
            You can find more information on start conditions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC11
            You will probably want to use exclusive conditions rather
            than inclusive conditions (use the %x instead of the %s). */
%x comment

%%
        int line_num = 1;

"/*"         BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++line_num;
<comment><<EOF>>        yyerror("dangling comment");
<comment>"*"+"/"        BEGIN(INITIAL);


  /* WRITEME: Write your Flex token rules here */

"print"     { return T_PRINT; }
"return"    { return T_RETURN; }
"new"       { return T_NEW; }
"extends"   { return T_EXTEND; }
"if"        { return T_IF; }
"else"      { return T_ELSE; }
"while"     { return T_WHILE; }
"do"        { return T_DO; }

"->"        { return T_ARROW; }
","         { return T_COMMA; }
"."         { return T_DOT; }
"{"         { return T_OPEN_BRACKET; }
"}"         { return T_CLOSE_BRACKET; }
"("         { return T_OPEN_PAREN; }
")"         { return T_CLOSE_PAREN; }
"="         { return T_ASSIGN; }

"integer"   { return T_INT; }
"boolean"   { return T_BOOL; }
"none"      { return T_NONE; }
"true"      { return T_TRUE; }
"false"     { return T_FALSE; }

"or"        { return T_OR; }
"and"       { return T_AND; }
"equals"    { return T_EQUAL; }
">="        { return T_GREATER_EQUAL; }
">"         { return T_GREATER; }
"+"         { return T_PLUS; }
"-"         { return T_MINUS; }

"*"         { return T_MULTIPLY; }
"/"         { return T_DIVIDE; }
"not"       { return T_NOT; }
";"         { return T_SEMI_COLON; }

{ID}        { yylval.identifier_ptr = new IdentifierNode(yytext); return T_ID; }
{DIGIT}     { yylval.integer_ptr = new IntegerNode(atoi(yytext)); return T_NUMBER; }

[ \t\n]+    /* eat up whitespace */
.           { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
