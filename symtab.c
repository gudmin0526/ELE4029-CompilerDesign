/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % TABLESIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
static ScopeList sl;
static FunctionList fl;

/* initialize scope list and
 * function list
 */
void sl_fl_init()
{ /* init for global scope */
  sl = (ScopeList) malloc(sizeof(struct ScopeListRec));
  sl->name = "global";
  sl->next = NULL;
  sl->parent = NULL;

  /* inserts built-in input, output into symbol table */
  st_insert(sl->hashTable,"input","int","Function","global",0,0);
  st_insert(sl->hashTable,"output","void","Function","global",0,1);
  scp_insert("input","global");
  scp_insert("output","global");
  ScopeList s_output = scp_lookup("output");
  st_insert(s_output->hashTable,"value","int","Variable","output",0,2);

  /* init for built-in input function */
  fl = (FunctionList) malloc(sizeof(struct FunctionListRec));
  fl->name = "input";
  fl->type = "int";
  fl->numParams = 0;
  fl->params = NULL;

  /* init for built-in output function */
  FunctionList f = (FunctionList) malloc(sizeof(struct FunctionListRec));
  f->name = "output";
  f->type = "void";
  f->numParams = 1;
  f->next = NULL;

  ParamList p = (ParamList) malloc(sizeof(struct ParamListRec));
  p->name = "value";
  p->next = NULL;
  p->type = "int";
  f->params = p;

  fl->next = f;
}

/* Procedure scp_insert inserts scope
 * into scope list indentified by name 
 */
void scp_insert( char * name, char * parname )
{ ScopeList l = sl;
  ScopeList parent = NULL;
  
  while ((l->next != NULL)) {
    if (!strcmp(l->name, parname)) parent = l; /* find parent scope */
    l = l->next; /* find the position where new scope be inserted */
  }
  if (!strcmp(l->name, parname)) parent = l; /* find parent scope at the last node */

  ScopeList s = (ScopeList) malloc(sizeof(struct ScopeListRec));
  s->name = name;
  s->next = NULL;
  s->parent = parent;
  l->next = s;
}

/* Function scp_lookup returns l
 * or NULL if scope name not found
 */
ScopeList scp_lookup ( char * name )
{ ScopeList s = sl;
  while ((s != NULL) && (strcmp(name,s->name) != 0))
    s = s->next;
  return s;
}

/* Procedure fn_insert inserts functions and
 * Params into function list 
 */
void fn_insert ( char * name, char * type, char * paramName, char * paramType )
{ FunctionList f = fl;
  while ((f->next != NULL) && (strcmp(name,f->name) != 0))
    f = f->next;
  if (strcmp(name,f->name) != 0) /* function not in list */
  { FunctionList nf = (FunctionList) malloc(sizeof(struct FunctionListRec));
    nf->name = name;
    nf->type = type;
    nf->numParams = 0;
    nf->next = NULL;
    nf->params = NULL;

    f->next = nf;
  }
  else /* if exists, append param to the left */
  { ParamList p = (ParamList) malloc(sizeof(struct ParamListRec));
    p->name = paramName;
    p->type = paramType;
    p->next = NULL;
    f->numParams++;
    ParamList temp = f->params;
    if (temp == NULL)
      f->params = p;
    else {
      while (temp->next != NULL)
        temp = temp->next;
      temp->next = p; 
    }
  }
}

/* Function fn_lookup returns 
 * number of parameters or -1 if not exists
 */
FunctionList fn_lookup(char * name) 
{ FunctionList f = fl;
  while ((f->next != NULL) && (strcmp(name,f->name) != 0))
    f = f->next;
  if (strcmp(name,f->name) == 0) /* function is in the list */
    return f;
  else
    return NULL;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( BucketList * hashTable, char * name, char * type, 
                char * kind, char * scpname, int lineno, int loc )
{ int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->type = type;
    l->scpname = scpname;
    l->kind = kind;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h]; /* insert front */
    hashTable[h] = l; }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
}

/* Function st_lookup returns the BucketlList 
 * or NULL if not found
 */
BucketList st_lookup ( BucketList * hashTable, char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return NULL;
  else return l;
}

BucketList find_symbol ( char * name, char * scpname )
{ ScopeList s = scp_lookup(scpname);
  while (s != NULL) {
    BucketList l = st_lookup(s->hashTable, name);
    if (l != NULL)
      return l;
    s = s->parent;
  }
  return NULL;
}

/* Procedure printFunctionList prints a formatted 
 * listing of the function list contents 
 * to the specified output file.
 */
void printFuncList(FILE * listing)
{ FunctionList f = fl;
  fprintf(listing,"\n< Functions >\n");
  fprintf(listing, "Function Name   Return Type     Param Number    Param Types\n");
  fprintf(listing, "--------------  --------------  --------------  --------------\n");
  while (f != NULL) {
    fprintf(listing, "%-14s  ", f->name);
    fprintf(listing, "%-14s  ", f->type);
    fprintf(listing, "%-12d  ", f->numParams);
    ParamList p = f->params;
    while (p != NULL)
    { fprintf(listing,"%5s ",p->type);
      p = p->next;
    }
    fprintf(listing, "\n");
    f = f->next;
  }
  fprintf(listing,"\n");
}

/* Procedure printScpList prints a formatted 
 * listing of the scope list contents 
 * to the specified output file. 
 */
void printScpList(FILE * listing)
{ ScopeList s = sl;
  fprintf(listing,"\n< Scopes >\n");
  fprintf(listing, "Scope Name      Parent Scope\n");
  fprintf(listing, "--------------  --------------\n");
  while (s != NULL) {
    fprintf(listing, "%-14s  ", s->name);
    if (s->parent != NULL)          
      fprintf(listing, "%-14s  ", s->parent->name);
    else
      fprintf(listing, "%-14s  ", "NULL");
    s = s->next;
    fprintf(listing, "\n");
  }
  fprintf(listing,"\n");
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ ScopeList s = sl;
  fprintf(listing,"\n< Symbol Table >\n");
  fprintf(listing, "Symbol Name     Symbol Kind     Symbol Type     Scope Name      Location    Line Numbers\n");
  fprintf(listing, "--------------  --------------  --------------  --------------  ----------  -----------------\n");
  while (s != NULL) {
    int i;
    for (i=0;i<TABLESIZE;++i)
    { if (s->hashTable[i] != NULL)
      { BucketList l = s->hashTable[i];
        while (l != NULL)
        { LineList t = l->lines;
          fprintf(listing, "%-14s  ", l->name);
          fprintf(listing, "%-14s  ", l->kind);
          fprintf(listing, "%-14s  ", l->type);
          fprintf(listing, "%-14s  ", l->scpname);
          fprintf(listing, "%-9d  ", l->memloc);
          while (t != NULL)
          { fprintf(listing,"%4d ",t->lineno);
            t = t->next;
          }
          fprintf(listing,"\n");
          l = l->next;
        }
      }
    }
    s = s->next;
  }
} /* printSymTab */
