%{
#include<stdlib.h>
#include<stdio.h>
#define YYDEBUG 1
//#include"syntabs.h" // pour syntaxe abstraite
//extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
%}
//...
//TODO: compléter avec la liste des terminaux
%token POINT_VIRGULE
%token PLUS
%token MOINS
%token FOIS
%token DIVISE
%token PARENTHESE_OUVRANTE
%token PARENTHESE_FERMANTE
%token CROCHET_OUVRANT
%token CROCHET_FERMANT
%token ACCOLADE_OUVRANTE
%token ACCOLADE_FERMANTE
%token EGAL
%token INFERIEUR
%token ET
%token OU
%token NON
%token SI
%token ALORS
%token SINON
%token TANTQUE
%token FAIRE
%token ENTIER
%token RETOUR
%token LIRE
%token ECRIRE
%token IDENTIF
%token NOMBRE
%token VIRGULE
%start programme

%%
programme : expression;
expression : PARENTHESE_OUVRANTE expression ACCOLADE_FERMANTE
  | expression OU expression_et
  | expression_et;
expression_et : expression_et ET expression_egal
  | expression_egal;
expression_egal : expression_egal EGAL expression_add
  | expression_egal INFERIEUR expression_add
  | expression_add;
expression_add : expression_add PLUS expression_mult
  | expression_add MOINS expression_mult
  | expression_mult;
expression_mult : expression_mult FOIS expression_non
  | expression_mult DIVISE expression_non
  | expression_non;
expression_non : NON expression_non
  | alphanum;
alphanum : NOMBRE
  | IDENTIF;

instruction : SI expression ALORS ACCOLADE_OUVRANTE instruction_liste  ACCOLADE_FERMANTE
  | SI expression ALORS ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
  | TANTQUE expression FAIRE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
  | IDENTIF PARENTHESE_OUVRANTE instruction_liste PARENTHESE_FERMANTE
  | ECRIRE PARENTHESE_OUVRANTE instruction_liste PARENTHESE_FERMANTE
  | ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
  | ;

instruction_liste : instruction POINT_VIRGULE
  | expression POINT_VIRGULE;


//TODO: compléter avec les productions de la grammaire

%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
