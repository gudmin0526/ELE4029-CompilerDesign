/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* Procedure spl_init initialize
 * scope list with global scope which
 * is the first element of scope list
 */
void scpl_init();

/* Procedure sp_insert inserts scope
 * into scope list indentified by name 
 */
void scp_insert( char * name, char * parent );

/* Function sp_lookup returns l
 * or NULL if scope name not found
 */
void * scp_lookup ( char * name );

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( void ** hashTable, char * name, char * type, char * kind,  char * scpname, int lineno, int loc );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( void ** hashTable, char * name );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);
void printScpList(FILE * listing);

#endif
