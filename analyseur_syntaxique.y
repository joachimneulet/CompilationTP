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

%union{ n_l_instr_ n_l_instr_; n_instr_ n_instr_; n_exp_ n_exp_; n_l_exp_ n_l_exp_; n_var_ n_var_; n_l_dec_ n_l_dec_; n_dec_ n_dec_; n_prog_ n_prog_; n_appel_ n_appel_}
%token POINT_VIRGULE //DONE
%token PLUS //DONE
%token MOINS //DONE
%token FOIS //DONE
%token DIVISE //DONE
%token PARENTHESE_OUVRANTE //DONE
%token PARENTHESE_FERMANTE //DONE
%token CROCHET_OUVRANT
%token CROCHET_FERMANT
%token ACCOLADE_OUVRANTE //DONE
%token ACCOLADE_FERMANTE //DONE
%token EGAL //DONE
%token INFERIEUR //DONE
%token ET //DONE
%token OU //DONE
%token NON //DONE
%token SI //DONE
%token ALORS //DONE
%token SINON //DONE
%token TANTQUE //DONE
%token FAIRE //DONE
%token ENTIER //DONE
%token RETOUR //DONE
%token LIRE //DONE
%token ECRIRE //DONE
%token <char[]> IDENTIF //DONE
%token <int> NOMBRE //DONE
%token VIRGULE

%start programme

%%

programme : fonction_liste;

expression_liste : expression expression_liste | ;

expression : expression OU expression_et
	|	expression_et
	;

expression_et : expression_et ET expression_egal
	|	expression_egal
	;

expression_egal : expression_egal EGAL expression_add
	|	expression_egal INFERIEUR expression_add
	|	expression_add
	;

expression_add : expression_add PLUS expression_mult
	|	expression_add MOINS expression_mult
	|	expression_mult
	;

expression_mult : expression_mult FOIS expression_non
	|	expression_mult DIVISE expression_non
	|	expression_non
	;

expression_non : NON expression_non
	|	alphanum
	;

alphanum : NOMBRE
	|	IDENTIF
	;

instruction_liste : instruction instruction_liste
	| ;

instruction : SI expression ALORS ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	|	SI expression ALORS ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	|	TANTQUE expression FAIRE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	|	IDENTIF PARENTHESE_OUVRANTE expression_liste PARENTHESE_FERMANTE
	|	ECRIRE PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE
	|	ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	| ECRIRE PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE POINT_VIRGULE
	| LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE POINT_VIRGULE
	| RETOUR expression POINT_VIRGULE
	| POINT_VIRGULE
	;

variable : ENTIER IDENTIF | ENTIER IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT;

fonction : IDENTIF PARENTHESE_OUVRANTE expression_liste PARENTHESE_FERMANTE variable ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	| IDENTIF PARENTHESE_OUVRANTE expression_liste PARENTHESE_FERMANTE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE
	;

fonction_liste : fonction fonction_liste | ;



%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
