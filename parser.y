%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"
    
    #define YYDEBUG 1
    #define YYINITDEPTH 10000

    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
%}

%error-verbose

/* WRITEME: Copy your token and precedence specifiers from Project 3 here */
/* reserved words */
%token T_PRINT T_RETURN T_NEW T_EXTEND
%token T_IF T_ELSE T_WHILE T_DO
%token T_OPEN_BRACKET T_CLOSE_BRACKET
%token T_OPEN_PAREN T_CLOSE_PAREN
%token T_ASSIGN
%token T_SEMI_COLON

/* class tokens */
%token T_ARROW T_COMMA T_DOT

/* type token */
%token T_INT T_BOOL T_NONE
%token T_TRUE T_FALSE

/* others */
%token T_ID T_NUMBER

/* operators */
%token T_OR T_AND
%token T_EQUAL T_GREATER T_GREATER_EQUAL
%token T_PLUS T_MINUS
%token T_MULTIPLY T_DIVIDE
%token T_NOT

/* operators */
%left T_OR
%left T_AND
%left T_EQUAL T_GREATER T_GREATER_EQUAL
%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE
%precedence T_NOT


/* WRITEME: Specify types for all nonterminals and necessary terminals here */
%type <program_ptr>             Start
%type <class_list_ptr>          Classes
%type <class_ptr>               Class
%type <identifier_ptr>          T_ID
%type <declaration_list_ptr>    Members Declaration
%type <method_list_ptr>         Methods     
%type <method_ptr>              Method
%type <parameter_list_ptr>      Parameters ParametersExtra
%type <type_ptr>                Type ReturnType
%type <methodbody_ptr>          MethodBody
%type <statement_list_ptr>      Statements ElseBlock Statement1
%type <returnstatement_ptr>     ReturnCall
// %type <parameter_ptr>
%type <declaration_ptr>         Member
%type <identifier_list_ptr>     MemberExtra
%type <expression_ptr>          Expression
%type <assignment_ptr>          Assignment
// %type <call_ptr>                
%type <methodcall_ptr>          MethodCall
%type <ifelse_ptr>              IfBlock
%type <while_ptr>               WhileLoop
%type <dowhile_ptr>             DoWhileLoop
%type <print_ptr>               PrintCall
// %type <plus_ptr>                T_PLUS
// %type <minus_ptr>               T_MINUS
// %type <times_ptr>               T_MULTIPLY
// %type <divide_ptr>              T_DIVIDE
// %type <greater_ptr>             T_GREATER
// %type <greaterequal_ptr>        T_GREATER_EQUAL
// %type <equal_ptr>               T_EQUAL
// %type <and_ptr>                 T_AND
// %type <or_ptr>                  T_OR
// %type <not_ptr>                 T_NOT
// %type <negation_ptr>            T_MINUS
%type <expression_list_ptr>     Arguments
// %type <memberaccess_ptr>        Expression
// %type <variable_ptr>            
// %type <integerliteral_ptr>      
%type <integer_ptr>             T_NUMBER
%type <booleanliteral_ptr>      T_TRUE T_FALSE
%type <new_ptr>                 T_NEW
%type <integertype_ptr>         T_INT
%type <booleantype_ptr>         T_BOOL
// %type <objecttype_ptr>          T_ID
%type <none_ptr>                T_NONE
// %type <statement_ptr>           
// %type <base_char_ptr>           
// %type <base_int> 


%%

/* WRITEME: This rule is a placeholder. Replace it with your grammar
            rules from Project 3 */
Start : Classes          { astRoot = new ProgramNode($1); }
      ;

Classes : Class Classes  { $$ = $2; $$->push_front($1); }
        | Class          { $$ = new std::list<ClassNode*>(); $$->push_front($1); }
        ;

Class : T_ID T_OPEN_BRACKET Members Methods T_CLOSE_BRACKET                { $$ = new ClassNode($1, NULL, $3, $4); }
      | T_ID T_EXTEND T_ID T_OPEN_BRACKET Members Methods T_CLOSE_BRACKET  { $$ = new ClassNode($1, $3, $5, $6); }
      ;

Members : Members Member                                      { $$ = $1; $$->push_back($2); }
        | %empty                                              { $$ = new std::list<DeclarationNode*>(); }
        ;

Member : Type T_ID MemberExtra T_SEMI_COLON                   { $3->push_front($2); $$ = new DeclarationNode($1, $3); }
       ;

MemberExtra : T_COMMA T_ID MemberExtra                        { $$ = $3; $$->push_front($2); }
            | %empty                                          { $$ = new std::list<IdentifierNode*>(); }
            ;

Methods : Method Methods                                      { $$ = $2; $$->push_front($1); }
        | %empty                                              { $$ = new std::list<MethodNode*>(); }
        ;

Method : T_ID T_OPEN_PAREN Parameters T_CLOSE_PAREN T_ARROW
         ReturnType T_OPEN_BRACKET MethodBody T_CLOSE_BRACKET { $$ = new MethodNode($1, $3, $6, $8); }
       ;

MethodBody : Declaration Statements ReturnCall                { $$ = new MethodBodyNode($1, $2, $3); }
           ;

Parameters : Type T_ID ParametersExtra                        { $$ = $3; $$->push_front(new ParameterNode($1, $2)); }
           | %empty                                           { $$ = new std::list<ParameterNode*>(); }
           ;

ParametersExtra : T_COMMA Type T_ID ParametersExtra           { $$ = $4; $$->push_front(new ParameterNode($2, $3)); }
                | %empty                                      { $$ = new std::list<ParameterNode*>(); }
                ;

/* Type Section */
Type : T_INT                                                  { $$ = new IntegerTypeNode(); $$->basetype = bt_integer; }
     | T_BOOL                                                 { $$ = new BooleanTypeNode(); $$->basetype = bt_boolean; }
     | T_ID                                                   { $$ = new ObjectTypeNode($1); $$->basetype = bt_object; }
     ;

ReturnType : Type                                             { $$ = $1; }
           | T_NONE                                           { $$ = new NoneNode(); $$->basetype = bt_none; }
           ;

/* Statement Section */
Statements : Statement1                            { $$ = $1; }
           | %empty                                { $$ = new std::list<StatementNode*>(); }
           ;

Statement1 : Assignment  T_SEMI_COLON  Statements  { $$ = $3; $$->push_front($1); }
           | MethodCall  T_SEMI_COLON  Statements  { $$ = $3; $$->push_front(new CallNode($1)); }
           | PrintCall   T_SEMI_COLON  Statements  { $$ = $3; $$->push_front($1); }
           | WhileLoop                 Statements  { $$ = $2; $$->push_front($1); }
           | DoWhileLoop T_SEMI_COLON  Statements  { $$ = $3; $$->push_front($1); }
           | IfBlock                   Statements  { $$ = $2; $$->push_front($1); }
           ;

Assignment : T_ID T_ASSIGN Expression                                           { $$ = new AssignmentNode($1, NULL, $3); }
           | T_ID T_DOT T_ID T_ASSIGN Expression                                { $$ = new AssignmentNode($1, $3, $5); }
           ;

Declaration : Declaration Type T_ID MemberExtra T_SEMI_COLON                    { $$ = $1; $4->push_front($3); $$->push_back(new DeclarationNode($2, $4)); }
            | %empty                                                            { $$ = new std::list<DeclarationNode*>(); }
            ;

PrintCall : T_PRINT Expression                                                  { $$ = new PrintNode($2); }
          ;

WhileLoop : T_WHILE Expression T_OPEN_BRACKET Statement1 T_CLOSE_BRACKET        { $$ = new WhileNode($2, $4); }
          ;


DoWhileLoop : T_DO T_OPEN_BRACKET Statements T_CLOSE_BRACKET T_WHILE T_OPEN_PAREN Expression T_CLOSE_PAREN { $$ = new DoWhileNode($3, $7); }
            ;

IfBlock : T_IF Expression T_OPEN_BRACKET Statement1 T_CLOSE_BRACKET ElseBlock   { $$ = new IfElseNode($2, $4, $6); }
        ;

ElseBlock : T_ELSE T_OPEN_BRACKET Statements T_CLOSE_BRACKET                    { $$ = $3; }
          | %empty                                                              { $$ = NULL; }
          ;

ReturnCall : T_RETURN Expression T_SEMI_COLON                                   { $$ = new ReturnStatementNode($2); }
           | %empty                                                             { $$ = NULL; }
           ;

/* Expression Section */
Expression : Expression T_PLUS Expression                     { $$ = new PlusNode($1, $3); $$->basetype = bt_integer; }
           | Expression T_MINUS Expression                    { $$ = new MinusNode($1, $3); $$->basetype = bt_integer; }
           | Expression T_MULTIPLY Expression                 { $$ = new TimesNode($1, $3); $$->basetype = bt_integer; }
           | Expression T_DIVIDE Expression                   { $$ = new DivideNode($1, $3); $$->basetype = bt_integer; }
           | Expression T_GREATER Expression                  { $$ = new GreaterNode($1, $3); $$->basetype = bt_boolean; }
           | Expression T_GREATER_EQUAL Expression            { $$ = new GreaterEqualNode($1, $3); $$->basetype = bt_boolean; }
           | Expression T_EQUAL Expression                    { $$ = new EqualNode($1, $3); $$->basetype = bt_boolean; }
           | Expression T_AND Expression                      { $$ = new AndNode($1, $3); $$->basetype = bt_boolean; }
           | Expression T_OR Expression                       { $$ = new OrNode($1, $3); $$->basetype = bt_boolean; }
           | T_NOT Expression                                 { $$ = new NotNode($2); $$->basetype = bt_boolean; }
           | T_MINUS Expression     %prec T_NOT               { $$ = new NegationNode($2); $$->basetype = bt_integer; }
           | T_ID                                             { $$ = new VariableNode($1); }
           | T_ID T_DOT T_ID                                  { $$ = new MemberAccessNode($1, $3); }
           | MethodCall                                       { $$ = $1; }
           | T_OPEN_PAREN Expression T_CLOSE_PAREN            { $$ = $2; }
           | T_NUMBER                                         { $$ = new IntegerLiteralNode($1); $$->basetype = bt_integer; }
           | T_TRUE                                           { $$ = new BooleanLiteralNode(new IntegerNode(1)); $$->basetype = bt_boolean; }
           | T_FALSE                                          { $$ = new BooleanLiteralNode(new IntegerNode(0)); $$->basetype = bt_boolean; }
           | T_NEW T_ID                                       { $$ = new NewNode($2, NULL); $$->basetype = bt_object; }
           | T_NEW T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN  { $$ = new NewNode($2, $4); $$->basetype = bt_object; }
           ;

MethodCall : T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN             { $$ = new MethodCallNode($1, NULL, $3); }
           | T_ID T_DOT T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN  { $$ = new MethodCallNode($1, $3, $5); }
           ;

Arguments : Arguments T_COMMA Expression  { $$ = $1; $$->push_back($3); }
          | Expression                    { $$ = new std::list<ExpressionNode*>(); $$->push_back($1); }
          | %empty                        { $$ = new std::list<ExpressionNode*>(); }
          ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(0);
}
