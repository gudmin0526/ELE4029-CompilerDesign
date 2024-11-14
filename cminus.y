/****************************************************/
/* File: cminus.y                                   */
/* The C-MINUS Yacc/Bison specification file        */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

%}

%token LCURLY RCURLY
%token LPAREN RPAREN 
%token LBRACE RBRACE 
%token SEMI COMMA
%token INT VOID
%token ID NUM
%token ENDFILE ERROR

%nonassoc IF WHILE RETURN
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left EQ NE LT LE GT GE
%right ASSIGN
%left PLUS MINUS 
%left TIMES OVER

%% /* Grammar for C-MINUS */

program             : declaration_list 
                        { savedTree = $1; 
                          strcpy(savedTree->scpname, "global"); } 
                    ;
declaration_list    : declaration_list declaration
                        { YYSTYPE t = $1;
                          if (t != NULL)
                          { while (t->sibling != NULL)
                                t = t->sibling;
                            t->sibling = $2;
                            $$ = $1; }
                          else $$ = $2; }
                    | declaration { $$ = $1; }
                    ;
declaration         : var_declaration { $$ = $1; }
                    | fun_declaration { $$ = $1; }
                    ;
var_declaration     : type_specifier identifier SEMI 
                        { $$ = newStmtNode(VarDeclK);
                          $$->lineno = $2->lineno;  
                          $$->vartype = $1->vartype;

                          $$->type = Void;
                          $2->type = $1->type;

                          $$->attr.name = $2->attr.name; }
                    | type_specifier identifier LBRACE number RBRACE SEMI
                        { $$ = newStmtNode(VarDeclK);
                          $$->lineno = $2->lineno;
                          
                          $$->type = Void;
                          if ($1->type == Void)
                            $2->type = VoidArray;
                          else if ($1->type == Integer)
                            $2->type = IntegerArray; 
                          else
                            $2->type = Undetermined;
        
                          strcat($1->vartype, "[]");                            
                          $$->vartype = $1->vartype;        
                          $$->attr.name = $2->attr.name;                        
                          $$->child[0] = $4; }
                    ;
type_specifier      : int { $$ = $1; }
                    | void { $$ = $1; }
                    ;
fun_declaration     : type_specifier identifier LPAREN params RPAREN compound_stmt
                        { $$ = newStmtNode(FunDeclK);
                          $$->lineno = $2->lineno;  
                          $$->vartype = $1->vartype;

                          $$->type = Void;
                          $2->type = $1->type;

                          $$->attr.name = $2->attr.name;
                          $$->child[0] = $4;
                          $$->child[1] = $6;
                          $$->child[1]->isChildOfFunDecl = 1; }
                    ;
params              : param_list { $$ = $1; }
                    | void { $$ = newExpNode(ParamK);
                             $$->lineno = $1->lineno;
                             $$->type = Void;
                             $$->vartype = NULL; }
                    ;
param_list          : param_list COMMA param
                        { YYSTYPE t = $1;
                          if (t != NULL)
                          { while (t->sibling != NULL)
                                t = t->sibling;
                            t->sibling = $3;
                            $$ = $1; }
                          else $$ = $3; }
                    | param { $$ = $1; }
                    ;
param               : type_specifier identifier 
                        { $$ = newExpNode(ParamK);
                          $$->lineno = $2->lineno;   
                          $$->vartype = $1->vartype;

                          $2->type = $1->type;
                          $$->type = $2->type;

                          $$->attr.name = $2->attr.name; }
                    | type_specifier identifier LBRACE RBRACE
                        { $$ = newExpNode(ParamK);
                          $$->lineno = $2->lineno;  

                          if ($1->type == Void)
                            $2->type = VoidArray;
                          else if ($1->type == Integer)
                            $2->type = IntegerArray;
                          else
                            $2->type = Undetermined;
                          $$->type = $2->type;

                          strcat($1->vartype, "[]");
                          $$->vartype = $1->vartype;
                          $$->attr.name = $2->attr.name; }
                    ;
compound_stmt       : LCURLY local_declarations statement_list RCURLY
                        { $$ = newStmtNode(CmpdK); 
                          $$->lineno = lineno;
                          $$->type = Void;
                          $$->child[0] = $2;
                          $$->child[1] = $3; }
                    ;
local_declarations  : local_declarations var_declaration
                        { YYSTYPE t = $1;
                          if (t != NULL)
                          { while (t->sibling != NULL)
                                t = t->sibling;
                            t->sibling = $2;
                            $$ = $1; }
                          else $$ = $2; }
                    | { $$ = NULL; }
                    ;
statement_list      : statement_list statement
                        { YYSTYPE t = $1;
                          if (t != NULL)
                          { while (t->sibling != NULL)
                                t = t->sibling;
                            t->sibling = $2;
                            $$ = $1; }
                            else $$ = $2; }
                    | { $$ = NULL; }
                    ;
statement           : expression_stmt { $$ = $1; }
                    | compound_stmt   { $$ = $1; }
                    | selection_stmt  { $$ = $1; }
                    | iteration_stmt  { $$ = $1; }
                    | return_stmt     { $$ = $1; }
                    ;
expression_stmt     : expression SEMI { $$ = $1; }
                    | SEMI { $$ = NULL; }
                    ;
selection_stmt      : IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
                        { $$ = newStmtNode(IfK);                             
                          $$->lineno = lineno;
                          $$->type = Void;
                          $$->child[0] = $3;
                          $$->child[1] = $5; }
                    | IF LPAREN expression RPAREN statement ELSE statement
                        { $$ = newStmtNode(IfElseK);
                          $$->lineno = lineno;
                          if ($5->type == $7->type)
                            $$->type = $5->type;          
                          else
                            $$->type = Undetermined;              
                          $$->child[0] = $3;
                          $$->child[1] = $5;
                          $$->child[2] = $7; }
                    ;
iteration_stmt      : WHILE LPAREN expression RPAREN statement
                        { $$ = newStmtNode(WhileK);
                          $$->lineno = lineno;
                          $$->child[0] = $3;
                          $$->type = Void;
                          $$->child[1] = $5; }
                    ;
return_stmt         : RETURN SEMI 
                        { $$ = newStmtNode(ReturnK);
                          $$->type = Void; 
                          $$->lineno = lineno; }
                    | RETURN expression SEMI
                        { $$ = newStmtNode(ReturnK); 
                          $$->lineno = lineno;
                          $$->type = $2->type;
                          $$->child[0] = $2; }
                    ;
expression          : var assignop expression 
                        { $$ = newStmtNode(AssignK);
                          $$->lineno = $1->lineno;
                          $$->child[0] = $1;
                          $$->child[1] = $3;
                          if ($1->type == $3->type)
                            $$->type = $1->type;
                          else
                            $$->type = Undetermined;
              
                          $$->attr.op = $2->attr.op; }
                    | simple_expression { $$ = $1; }
                    ;
var                 : identifier 
                        { $$ = newExpNode(VarK);
                          $$->lineno = $1->lineno;
                          $$->type = Undetermined;
                          $$->attr.name = $1->attr.name; }
                    | identifier LBRACE expression RBRACE
                        { $$ = newExpNode(VarK);
                          $$->lineno = $1->lineno;
                          $$->type = Undetermined;
                          $$->attr.name = $1->attr.name;
                          $$->child[0] = $3; }
                    ;
simple_expression   : additive_expression relop additive_expression
                        { $$ = newExpNode(OpK); 
                          $$->lineno = $1->lineno;
                          $$->child[0] = $1;
                          $$->child[1] = $3;
                          if ($1->type == Integer && $3->type == Integer)
                            $$->type = Integer;
                          else
                            $$->type = Undetermined;
                          $$->attr.op = $2->attr.op; }
                    | additive_expression { $$ = $1; }
                    ;
assignop            : ASSIGN { $$ = newExpNode(OpK);
                               $$->lineno = lineno;
                               $$->attr.op = ASSIGN; }
relop               : LE { $$ = newExpNode(OpK);
                           $$->lineno = lineno;
                           $$->attr.op = LE; } 
                    | LT { $$ = newExpNode(OpK);
                           $$->lineno = lineno;                    
                           $$->attr.op = LT; } 
                    | GT { $$ = newExpNode(OpK);
                           $$->lineno = lineno;                    
                           $$->attr.op = GT; } 
                    | GE { $$ = newExpNode(OpK);
                           $$->lineno = lineno;                                    
                           $$->attr.op = GE; } 
                    | EQ { $$ = newExpNode(OpK);
                           $$->lineno = lineno;                    
                           $$->attr.op = EQ; } 
                    | NE { $$ = newExpNode(OpK);
                           $$->lineno = lineno;                                        
                           $$->attr.op = NE; } 
                    ;
additive_expression : additive_expression addop term 
                        { $$ = newExpNode(OpK); 
                          $$->lineno = $1->lineno;                    
                          $$->child[0] = $1;
                          $$->child[1] = $3;
                          if ($1->type == Integer && $3->type == Integer)
                            $$->type = Integer;
                          else
                            $$->type = Undetermined;
                          $$->attr.op = $2->attr.op; }
                    | term { $$ = $1; }
                    ;
addop               : PLUS { $$ = newExpNode(OpK);
                             $$->lineno = lineno;                    
                             $$->attr.op = PLUS; } 
                    | MINUS { $$ = newExpNode(OpK);
                              $$->lineno = lineno;                    
                              $$->attr.op = MINUS; } 
                    ;
term                : term mulop factor
                        { $$ = $2;
                          $$->child[0] = $1;
                          $$->child[1] = $3; 
                          $$->lineno = $1->lineno;
                          if ($1->type == Integer && $3->type == Integer)
                            $$->type = Integer;
                          else
                            $$->type = Undetermined;
                          $$->attr.op = $2->attr.op; }
                    | factor { $$ = $1; }
                    ;
mulop               : TIMES { $$ = newExpNode(OpK);
                              $$->lineno = lineno;                    
                              $$->attr.op = TIMES; } 
                    | OVER  { $$ = newExpNode(OpK);
                              $$->lineno = lineno;                    
                              $$->attr.op = OVER; } 
                    ;
factor              : LPAREN expression RPAREN { $$ = $2; }
                    | var  { $$ = $1; }
                    | call { $$ = $1; }
                    | number { $$ = $1; }
                    ;
call                : identifier LPAREN args RPAREN
                        { $$ = newExpNode(CallK);
                          $$->lineno = $1->lineno;
                          $$->type = Undetermined;                        
                          $$->attr.name = $1->attr.name;
                          $$->child[0] = $3; }
                    ;
args                : arg_list { $$ = $1; }
                    | { $$ = NULL; }
                    ;
arg_list            : arg_list COMMA expression
                        { YYSTYPE t = $1;
                          if (t != NULL)
                          { while (t->sibling != NULL)
                                t = t->sibling;
                            t->sibling = $3;
                            $$ = $1; }
                          else $$ = $3; }
                    | expression { $$ = $1; }
                    ;
identifier          : ID { $$ = newExpNode(IdK);
                           $$->attr.name = copyString(tokenString);
                           $$->type = Undetermined;
                           $$->lineno = lineno; }
                    ;
number              : NUM { $$ = newExpNode(ConstK);
                            $$->attr.val = atoi(tokenString);
                            $$->type = Integer;
                            $$->lineno = lineno; }
                    ;
int                 : INT { $$ = newExpNode(TypeK); 
                            $$->vartype = copyString(tokenString);
                            $$->type = Integer;
                            $$->lineno = lineno; }
                    ;
void                : VOID { $$ = newExpNode(TypeK); 
                             $$->vartype = copyString(tokenString);
                             $$->type = Void;
                             $$->lineno = lineno; }
                    ;
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

