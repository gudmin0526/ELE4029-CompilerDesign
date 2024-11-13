/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* SIZE is the size of the hash table */
#define TABLESIZE 100

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

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
     BucketList hashTable[TABLESIZE];
   } * ScopeList;

/* The record in function lists for
 * each functions, including name,
 * and its type 
 */
typedef struct ParamListRec
   { char * name;
     char * type;
     struct ParamListRec * next;
   } * ParamList;

/* The list of global functions
 * because c-minus only allowed
 * functions at global scope 
 */
typedef struct FunctionListRec
   { char * name;
     char * type;
     ParamList params;
     int numParams;
     struct FunctionListRec * next;
   } * FunctionList;


/* initialize scope list and
 * function list
 */
void sl_fl_init();

/* Procedure sp_insert inserts scope
 * into scope list indentified by name 
 */
void scp_insert( char * name, char * parent );

/* Function sp_lookup returns l
 * or NULL if scope name not found
 */
ScopeList scp_lookup ( char * name );

/* Procedure fn_insert inserts functions and
 * Params into function list 
 */
void fn_insert( char * name, char * type, char * paramName , char * paramType );

/* Function fn_lookup returns 
 * number of parameters or -1 if not exists
 */
FunctionList fn_lookup(char * name);

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( BucketList * hashTable, char * name, char * type, char * kind,  char * scpname, int lineno, int loc );

/* Function st_lookup returns the BucketlList 
 * or NULL if not found
 */
BucketList st_lookup ( BucketList * hashTable, char * name );

BucketList find_symbol ( char * name, char * scpname );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printFuncList(FILE * listing);
void printSymTab(FILE * listing);
void printScpList(FILE * listing);

#endif
