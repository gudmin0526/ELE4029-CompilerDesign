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

/* SIZE is the size of the hash table */
#define SIZE 30

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { char * name;    /* name */
     char * scpname; /* scope name */
     char * type;    /* type */
     char * kind;    /* kind */
     int memloc;     /* memory location for variable */
     LineList lines; /* line numbers */
     struct BucketListRec * next;
   } * BucketList;

/* The record that stores information for 
 * each scope, organized as a structure 
 * containing a hash table
 */
typedef struct ScopeListRec
   { char * name;
     struct ScopeListRec * next;
     struct ScopeListRec * parent;
     BucketList hashTable[SIZE];
   } * ScopeList;

/* The record in function lists for
 * each functions, including name,
 * and its type 
 */
typedef struct ParamListRec
   { char * name;
     char * type;
   } * ParamList;

/* The list of global functions
 * because c-minus only allowed
 * functions at global scope 
 */
typedef struct FunctionListRec
   { char * name;
     char * type;
     ParamList params;
   } * FunctionList;


/* the hash table */
static ScopeList scopeList;

/* initialize global scope which
 * is the first element of scope list
*/
void scpl_init()
{ 
  scopeList = (ScopeList) malloc(sizeof(struct ScopeListRec));
  scopeList->name = "global";
  scopeList->next = NULL;
  scopeList->parent = NULL;
}

/* Procedure sp_insert inserts scope
 * into scope list indentified by name 
 */
void scp_insert( char * name, char * parname )
{ ScopeList l = scopeList;
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

/* Function sp_lookup returns l
 * or NULL if scope name not found
 */
void * scp_lookup ( char * name )
{ ScopeList s = scopeList;
  while ((s != NULL) && (strcmp(name,s->name) != 0))
    s = s->next;
  return s;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( void ** hashTable, char * name, char * type, 
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
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( void ** hashTable, char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;
}

void printScpList(FILE * listing)
{ 
  ScopeList s = scopeList;
  fprintf(listing,"< Scopes >\n");
  fprintf(listing, "Scope Name       Parent Scope \n");
  fprintf(listing, "---------------  ---------------\n");
  while (s != NULL) {
    fprintf(listing, "%-14s  ", s->name);
    if (s->parent != NULL)          
      fprintf(listing, "%-14s  ", s->parent->name);
    else
      fprintf(listing, "%-14s  ", "NULL");
    s = s->next;
  }
  fprintf(listing,"\n");
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ ScopeList s = scopeList;
  fprintf(listing,"< Symbol Table >\n");
  fprintf(listing, "Symbol Name       Symbol Kind       Symbol Type      Scope Name       Location    Line Numbers\n");
  fprintf(listing, "----------------  ----------------  ---------------  ---------------  ----------  -----------------\n");
  while (s != NULL) {
    int i;
    for (i=0;i<SIZE;++i)
    { if (s->hashTable[i] != NULL)
      { BucketList l = s->hashTable[i];
        while (l != NULL)
        { LineList t = l->lines;
          fprintf(listing, "%-16s  ", l->name);
          fprintf(listing, "%-16s  ", l->kind);
          fprintf(listing, "%-16s  ", l->type);
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
