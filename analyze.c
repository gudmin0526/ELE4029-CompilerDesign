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
static int location = 0;

/* functions for converting int to string */
void intToStr(int num, char * buffer) {
    char temp[12];
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
    printf("scope: %s, %d\n", t->scpname, t->nodekind);
    if (t->nodekind==StmtK)
    { switch (t->kind.stmt) {
        case IfK:
        case IfElseK:
        case WhileK:
        case ReturnK:
        case AssignK:
          while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) 
              strcpy(t->child[j]->scpname, t->scpname);
            j++;
          }
          if (t->sibling != NULL)
            strcpy(t->sibling->scpname, t->scpname);
          nthChild = 0;         
          break;
        case VarDeclK:
        case FunDeclK:          
          while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) 
              strcpy(t->child[j]->scpname, t->attr.name);
            j++;
          }
          if (t->sibling != NULL)
            strcpy(t->sibling->scpname, t->scpname);
          break;
        case CmpdK:
          char scpname[MAXLENSCPNAME];
          strcpy(scpname, t->scpname);
          if (!t->isChildOfFunDecl) { /* funDecl에 속한 compound_stmt가 아닐 경우 */
            strcat(scpname, ".");
            intToStr(nthChild, nthChildStr);
            strcat(scpname, nthChildStr);
            printf("is not CHildOfFunDecl\n");
          }
          while (j < MAXCHILDREN) {
            if (t->child[j] != NULL) 
              strcpy(t->child[j]->scpname, scpname);
            j++;
          }
          if (t->sibling != NULL) {
            nthChild++;
            strcpy(t->sibling->scpname, t->scpname);
          }
          break; 
          break;
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
            if (t->child[j] != NULL) 
              strcpy(t->child[j]->scpname, t->scpname);
            j++;
          }
          if (t->sibling != NULL)
            strcpy(t->sibling->scpname, t->scpname);
          nthChild = 0;
          break;        
        default:
          break;
      }
    }
    for (i=0;i<MAXCHILDREN;i++)
      checkScope(t->child[i], nthChild);
    t = t->sibling;
  }
} 

/* Procedure insertScope inserts
 * scope at the scope list
 */
static void insertScope(TreeNode * t) {
  switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case FunDeclK:
          
        case VarDeclK:
        case CmpdK:
        default:
          break;
      }
  }
}


/* Procedure insertSymbol inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertSymbol(TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case AssignK:
        case FunDeclK:
        case VarDeclK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0);
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0);
          break;
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
{  
  scpl_init();
  checkScope(syntaxTree, 0);
  traverse(syntaxTree,insertSymbol,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          break;
        case TypeK:
        /* NEED TO IMPLEMENT */
        case ConstK:
        /* NEED TO IMPLEMENT */ 
        case IdK:
          t->type = Integer;
          break;
        case ParamK:
        /* NEED TO IMPLEMENT */
        case VarK:
        /* NEED TO IMPLEMENT */
        case CallK:
        /* NEED TO IMPLEMENT */
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case IfElseK:
        /* NEED TO IMPLEMENT */
        case WhileK:
        /* NEED TO IMPLEMENT */
        case ReturnK:
        /* NEED TO IMPLEMENT */
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
        case VarDeclK:
        /* NEED TO IMPLEMENT */
        case FunDeclK:
        /* NEED TO IMPLEMENT */
        case CmpdK:
        /* NEED TO IMPLEMENT */
          break;
          break;
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
