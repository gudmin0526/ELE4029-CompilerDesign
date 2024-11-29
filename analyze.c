/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 3;

/* functions for converting int to string */
static void intToStr(int num, char * buffer) 
{ char temp[12];
  int index = 0;  

  if (num == 0) {
      temp[index++] = '0';
  }

  // 숫자를 뒤집힌 순서로 변환
  while (num > 0) {
      temp[index++] = (num % 10) + '0';
      num /= 10;                       
  }

  temp[index] = '\0';

  // 문자열 뒤집기
  for (int i = 0; i < index; i++) {
      buffer[i] = temp[index - i - 1];
  }
  buffer[index] = '\0';
}

/* functions for checking what it returns from */
static void returnWhatFunctionIsItIn(char * scpname, char * buffer)
{ int i = 0;
  while (scpname[i] != '\0' && scpname[i] != '.') {
    buffer[i] = scpname[i];
    i++;
  }
  buffer[i] = '\0';
}

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { 
    preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure checkScope checks 
 * the scope of identifiers
 */
static void checkScope(TreeNode * t, int nthChild)
{ int i=0; 
  char nthChildStr[12];
  while (t != NULL) {
    int j=0;
    if (t->nodekind==StmtK)
    { switch (t->kind.stmt) {
        case IfK:
        case IfElseK:
        case WhileK:
          nthChild++;
        case ReturnK:
        case AssignK:
        { while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) {
              strcpy(t->child[j]->scpname, t->scpname);
              t->child[j]->parent = t;
            }
            j++;
          }
          if (t->sibling != NULL) {
            strcpy(t->sibling->scpname, t->scpname);
            t->sibling->parent = t->parent;
          }
          break;
        }
        case VarDeclK:
        { while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) {
              strcpy(t->child[j]->scpname, t->scpname);
              t->child[j]->parent = t;
            }
            j++;
          }
          if (t->sibling != NULL) {
            strcpy(t->sibling->scpname, t->scpname);
            t->sibling->parent = t->parent;
          }
          break;
        }
        case FunDeclK:
        { while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) {
              strcpy(t->child[j]->scpname, t->attr.name);
              t->child[j]->parent = t;
            }
            j++;
          }
          if (t->sibling != NULL) {
            strcpy(t->sibling->scpname, t->scpname);
            t->sibling->parent = t->parent;
          }
          break;
        }
        case CmpdK:
        { if (t->parent->kind.stmt != CmpdK)
            nthChild--;
          char scpname[MAXLENSCPNAME];
          strcpy(scpname, t->scpname);
          if (!t->isChildOfFunDecl) { /* funDecl에 속한 compound_stmt가 아닐 경우 */
            strcat(scpname, ".");
            intToStr(nthChild, nthChildStr);
            strcat(scpname, nthChildStr);
          }
          while (j < MAXCHILDREN) {
            if (t->child[j] != NULL)  {
              strcpy(t->child[j]->scpname, scpname);
              t->child[j]->parent = t;
            }
            j++;
          }
          if (t->sibling != NULL) {
            strcpy(t->sibling->scpname, t->scpname);
            t->sibling->parent = t->parent;
          }
          nthChild++;
          break;
        }
        default:
          break;
      }
    }
    else if (t->nodekind==ExpK)
    { switch (t->kind.exp) { /* exp들은 자체적으로 scope를 가지지 않으므로 부모 것을 상속 */
        case OpK:
        case ConstK:
        case IdK:
        case ParamK:
        case VarK:
        case CallK:
        case TypeK:
          while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) {
              strcpy(t->child[j]->scpname, t->scpname);
              t->child[j]->parent = t;
            }
            j++;
          }
          if (t->sibling != NULL) {
            strcpy(t->sibling->scpname, t->scpname);
            t->sibling->parent = t->parent;
          }
          break;        
        default:
          break;
      }
    }
    for (i=0;i<MAXCHILDREN;i++) {
      checkScope(t->child[i], nthChild);
    }
    t = t->sibling;
  }
} 

/* Procedure insertSymbol inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertSymbol(TreeNode * t)
{ /* insert scope if not found */
  ScopeList s = scp_lookup(t->scpname); 
  if (s == NULL) {
    scp_insert(t->scpname,t->parent->scpname);
    s = scp_lookup(t->scpname);
  }

  ScopeList save = s; /* for returing to the original scope, when not found at upper scopes */

  switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case FunDeclK:
        { BucketList bl = st_lookup(s->hashTable,t->attr.name);
          if (bl != NULL) {/* 다른 스코프에서 선언된건 알빠 아님 */
            fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d (already defined at line", t->attr.name, t->lineno);
            // Program to sequentially print all the line numbers
            LineList ll = bl->lines;
            while (ll != NULL) {
              fprintf(listing, " %d", ll->lineno);
              ll = ll->next;
            }
            fprintf(listing, ")\n");
            st_insert(s->hashTable,t->attr.name,t->vartype,"Function",t->scpname,t->lineno,0);
          }
          else {
            st_insert(s->hashTable,t->attr.name,t->vartype,"Function",t->scpname,t->lineno,location++);
            fn_insert(t->attr.name,t->vartype,NULL,NULL); /* 당장 param에 대한 정보는 없음 */
          }
          break;
        }
        case VarDeclK:
        { BucketList bl = st_lookup(s->hashTable,t->attr.name);

          if (bl != NULL) {/* 다른 스코프에서 선언된건 알빠 아님 */
            fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d (already defined at line", t->attr.name, t->lineno);
            // Program to sequentially print all the line numbers
            LineList ll = bl->lines;
            while (ll != NULL) {
              fprintf(listing, " %d", ll->lineno);
              ll = ll->next;
            }
            fprintf(listing, ")\n");
            st_insert(s->hashTable,t->attr.name,t->vartype,"Variable",t->scpname,t->lineno,0);
          }
          else {
            if (!strcmp(t->vartype, "void") || !strcmp(t->vartype, "void[]")) { /* void type variable -> Error */
              fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
              t->type = Undetermined;
            }
            st_insert(s->hashTable,t->attr.name,t->vartype,"Variable",t->scpname,t->lineno,location++);
          } 
          break;
        }
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case ParamK: /* 마찬가지로 자기 scope만 신경쓰면 된다 */
        { if (t->vartype == NULL)
            break;
          BucketList bl = st_lookup(s->hashTable,t->attr.name);
          if (bl != NULL) {/* 다른 스코프에서 선언된건 알빠 아님 */
            fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d (already defined at line", t->attr.name, t->lineno);
            // Program to sequentially print all the line numbers
            LineList ll = bl->lines;
            while (ll != NULL) {
              fprintf(listing, " %d", ll->lineno);
              ll = ll->next;
            }
            fprintf(listing, ")\n");
            st_insert(s->hashTable,t->attr.name,t->vartype,"Variable",t->scpname,t->lineno,0);
          }
          else {
            st_insert(s->hashTable,t->attr.name,t->vartype,"Variable",t->scpname,t->lineno,location++);
            fn_insert(t->scpname,NULL,t->attr.name,t->vartype); /* function overloading 안되므로 type 몰라도 된다 */
          }
          break;
        }
        case VarK:
        { while (s != NULL) { /* not found in symbol table at this scope */
            if (st_lookup(s->hashTable,t->attr.name) != NULL) { /* already in symbol table */ 
              st_insert(s->hashTable, t->attr.name,t->vartype,"Variable",t->scpname,t->lineno,0);
              break;
            }
            s = s->parent; /* search at parent scope */
          }
          if (s == NULL) { /* NOT FOUND IN ANY SCOPE -> Error */
            st_insert(save->hashTable, t->attr.name,"undetermined","Variable",t->scpname,t->lineno,location++);
            fprintf(listing, "Error: undeclared variable \"%s\" is used at line %d\n", t->attr.name, t->lineno);
          }
          break;
        }
        case CallK:
        { while (s != NULL) { /* not found in symbol table at this scope */
            if (st_lookup(s->hashTable,t->attr.name) != NULL) { /* already in symbol table */
              st_insert(s->hashTable, t->attr.name,t->vartype,"Function",t->scpname,t->lineno,0);
              break;
            }
            s = s->parent; /* search at parent scope */
          }
          if (s == NULL) { /* not found in any scope -> Error */
            st_insert(save->hashTable, t->attr.name,"undetermined","Function",t->scpname,t->lineno,location++);
            fn_insert(t->attr.name,"undetermined",NULL,NULL);
            fprintf(listing, "Error: undeclared function \"%s\" is used at line %d\n", t->attr.name, t->lineno);
          }
          break;
        }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ sl_fl_init();
  checkScope(syntaxTree, 0);
  traverse(syntaxTree,insertSymbol,nullProc);
  if (TraceAnalyze)
  { 
    printFuncList(listing);
    printScpList(listing);
    printSymTab(listing);
  }
}

/* Function isTypeInteger checks if symbol's 
 * type is integer. if not, return 0 
 */
static int isTypeInteger(TreeNode * t) 
{ if (t->type == Integer)
  return 1;

  if (t->kind.exp == ConstK) 
    return t->type == Integer;
  else if (t->kind.exp == VarK || t->kind.exp == CallK) {
    BucketList bl = find_symbol(t->attr.name, t->scpname);
    return bl && !strcmp(bl->type, "int");
  }
  return 0;
}

/* Function isTypeIntegerArray checks if symbol's 
 * type is integer array. if not, return 0 
 */
static int isTypeIntegerArray(TreeNode * t) 
{ if (t->type == IntegerArray)
    return 1;
    
  if (t->kind.exp == VarK || t->kind.exp == CallK) {
    BucketList bl = find_symbol(t->attr.name, t->scpname); /* symtab에 존재하는지 확인 */
    return bl && !strcmp(bl->type, "int[]");
  }
  return 0;
}

/* Function isTypeVoidKind checks if
 * symbol's type is void kind 
 */
static int isTypeVoidKind(TreeNode * t) 
{ BucketList bl = find_symbol(t->attr.name, t->scpname);
  if (t->kind.exp == VarK || t->kind.exp == CallK || t->kind.exp == CallK)
    return bl && (strcmp(bl->type, "void") == 0 || strcmp(bl->type, "void[]") == 0);
  else {
    if (t->type == Void || t->type == VoidArray)
      return 1;
  }
  return 0;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp) {
      case OpK:
        { TreeNode * c0 = t->child[0];
          TreeNode * c1 = t->child[1];
          if (c0->type != Integer || c1->type != Integer) {
            fprintf(listing, "Error: invalid operation at line %d\n", c0->lineno);
            t->type = Undetermined;
          }   
          else {
            t->type = Integer;
          }
          break;
        }
        case ParamK:
        { if (t->vartype == NULL) /* void parameter -> do nothing */
            break;
          if (isTypeVoidKind(t)) { /* void type variable -> Error */
            fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
            t->type = Void;
            break;
          }
          break;
        }
        case VarK:
        { if (isTypeInteger(t) && t->child[0] != NULL) { /* 1. Integer has child -> Error */
            fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). indexing can only allowed for int[] variables\n", t->lineno, t->attr.name);    
            t->type = Undetermined;
          } else if (isTypeInteger(t) && t->child[0] == NULL) { /* 2. Integer has no child -> Normal */
            t->type = Integer;
          } else if (isTypeIntegerArray(t) && t->child[0] != NULL && isTypeInteger(t->child[0])) { /* 3. Array has child and child is integer -> Integer */
            t->type = Integer;
          } else if (isTypeIntegerArray(t) && t->child[0] != NULL && !isTypeInteger(t->child[0])) { /* 4. Array has child but child is not integer -> Error */
            fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). indicies should be integer\n", t->lineno, t->attr.name);
            t->type = Integer;
          } else if (isTypeIntegerArray(t) && t->child[0] == NULL) { /* 4. Array has no child -> Integer Array */
            t->type = IntegerArray;
          } 
          break;
        }
        case CallK:
        { BucketList bl = find_symbol(t->attr.name, t->scpname); 
          if (bl == NULL) {/* symbol table에 존재하는지 확인 */
            fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name);
            t->type = Undetermined; 
            break;
          }

          if (!strcmp(bl->type, "undetermined")) { /* invalid function call */
            fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name); 
            t->type = Undetermined;
            break;
          }

          int numArgs = 0;
          FunctionList fl = fn_lookup(t->attr.name);
          ParamList p = fl->params;
          TreeNode * c = t->child[0]; /* args에 대한 type checking */
    
          while (c != NULL) {
            if (p == NULL) { /* params와 args의 길이가 안맞는 경우 */
              fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name); 
              t->type = Integer;
              return;
            }
            if (!((c->type == Integer && !strcmp(p->type, "int")) || (c->type == IntegerArray && !strcmp(p->type, "int[]")))) {
              fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name); 
              t->type = Integer;
              return;
            } /* params와 args의 타입이 불일치하는 경우 or Void 타입을 가진 경우 */
            numArgs++;
            p = p->next;
            c = c->sibling;
          }
          if (numArgs != fl->numParams) {
            fprintf(listing, "Error: Invalid function call at line %d (name : \"%s\")\n", t->lineno, t->attr.name); 
            t->type = Integer;
          }
          if (!strcmp(fl->type, "int"))
            t->type = Integer;
          else
            t->type = Void;
          break;
        }
        case ConstK:
        { t->type = Integer;
          break;
        }
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
        case IfElseK:
        case WhileK:
        { TreeNode * c = t->child[0];
          if (c == NULL || c->type != Integer) {
            fprintf(listing, "Error: invalid condition at line %d\n", t->child[1]->lineno);
            t->type = Undetermined;
            break;
          }
          t->type = Void;
          break;
        }
        case ReturnK:
        { char f_name[MAXLENSCPNAME];
          returnWhatFunctionIsItIn(t->scpname, f_name);
          FunctionList fl = fn_lookup(f_name);
          TreeNode * c = t->child[0];

          if (c == NULL && strcmp(fl->type, "void") != 0) {  
            fprintf(listing, "Error: Invalid return at line %d\n", t->lineno);
            t->type = Undetermined;
            break;
          } else if (c != NULL && (c->type != Integer || strcmp(fl->type, "int") != 0)) {
            fprintf(listing, "Error: Invalid return at line %d\n", t->lineno);
            t->type = Undetermined;
            break;
          }
          if (c == NULL)
            t->type = Void;
          else
            t->type = Integer;
          break;
        }
        case AssignK:
        { TreeNode * c0 = t->child[0];
          TreeNode * c1 = t->child[1];
          if (c0 == NULL || c1 == NULL) {
            fprintf(listing, "Error: invalid assignment at line %d\n", t->lineno);
            t->type = Undetermined;
            break;
          }
          if (c0->type == Integer && c1->type == Integer) {
            t->type = Integer;
          } else if (c0->type == IntegerArray && c1->type == IntegerArray) {
            t->type = IntegerArray;
          } else {
            fprintf(listing, "Error: invalid assignment at line %d\n", t->lineno);
            t->type = Undetermined;
            break;
          }
          break;
        }
        case VarDeclK:
        case FunDeclK:
        case CmpdK: 
        { t->type = Void;
          break;  
        }
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
