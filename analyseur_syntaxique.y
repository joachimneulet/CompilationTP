%{
    #include<stdlib.h>
    #include<stdio.h>
    #define YYDEBUG 1
    #include"syntabs.h" // pour syntaxe abstraite
    extern n_prog *n;   // pour syntaxe abstraite
    extern FILE *yyin;    // declare dans compilo
    extern int yylineno;  // declare dans analyseur lexical
    int yylex();          // declare dans analyseur lexical
    int yyerror(char *s); // declare ci-dessous
    int yydebug=0;
%}

%union{
    int type_entier;
    char* type_chaine;
    n_l_instr* n_l_instr;
    n_instr*  n_instr;
    n_exp* n_exp;
    n_l_exp* n_l_exp;
    n_var* n_var;
    n_l_dec* n_l_dec;
    n_dec* n_dec;
    n_prog* n_prog;
    n_appel* n_appel;
}

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
%token <type_chaine> IDENTIF
%token <type_entier> NOMBRE
%token VIRGULE

%type <n_prog> programme
%type <n_exp> expression
%type <n_exp> expression_et
%type <n_exp> expression_comp
%type <n_exp> expression_add
%type <n_exp> expression_mult
%type <n_exp> expression_non
%type <n_exp> atom
%type <n_l_exp> expression_liste
%type <n_instr> instruction
%type <n_instr> instruction_affect
%type <n_l_instr> instruction_liste
%type <n_dec> fonction_dec
%type <n_l_dec> fonction_dec_liste
%type <n_dec> variable_dec
%type <n_l_dec> variable_dec_liste
%type <n_l_dec> param_dec_liste
%type <n_dec> param_dec
%type <n_var> variable

%start programme

%%

programme : variable_dec_liste fonction_dec_liste { n = cree_n_prog($1, $2); }
    | fonction_dec_liste { n = cree_n_prog(NULL, $1); }
    ;

expression_liste : expression VIRGULE expression_liste { $$ = cree_n_l_exp($1, $3); }
    | expression { $$ = cree_n_l_exp($1, NULL); }
    ;

expression : expression OU expression_et { $$ = cree_n_exp_op(ou, $1, $3); }
    | expression_et { $$ = $1; }
    ;

expression_et : expression_et ET expression_comp { $$ = cree_n_exp_op(et, $1, $3); }
    | expression_comp { $$ = $1; }
    ;

expression_comp : expression_comp EGAL expression_add { $$ = cree_n_exp_op(egal, $1, $3); }
    | expression_comp INFERIEUR expression_add { $$ = cree_n_exp_op(inferieur, $1, $3); }
    | expression_add { $$ = $1; }
    ;

expression_add : expression_add PLUS expression_mult  { $$ = cree_n_exp_op(plus, $1, $3); }
    | expression_add MOINS expression_mult   { $$ = cree_n_exp_op(moins, $1, $3); }
    | expression_mult  { $$ = $1; }
    ;

expression_mult : expression_mult FOIS expression_non  { $$ = cree_n_exp_op(fois, $1, $3); }
    | expression_mult DIVISE expression_non   {  $$ = cree_n_exp_op(divise, $1, $3); }
    | expression_non   { $$ = $1; }
    ;

expression_non : NON expression_non  { $$ = cree_n_exp_op(non, $2, NULL); }
    | atom   { $$ = $1; }
    ;

atom : PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE  { $$ = $2; }
    | NOMBRE  { $$ = cree_n_exp_entier($1); }
    | variable { $$ = cree_n_exp_var($1); }
    | LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE { $$ = cree_n_exp_lire(); }
    | IDENTIF PARENTHESE_OUVRANTE expression_liste PARENTHESE_FERMANTE { $$ = cree_n_exp_appel(cree_n_appel($1, $3)); }
    | IDENTIF PARENTHESE_OUVRANTE PARENTHESE_FERMANTE { $$ = cree_n_exp_appel(cree_n_appel($1, NULL)); }
    ;

instruction_liste : instruction instruction_liste { $$ = cree_n_l_instr($1, $2); }
    | instruction { $$ = cree_n_l_instr($1, NULL); }
    ;

instruction : SI expression ALORS ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_instr_si($2, cree_n_instr_bloc($5), NULL); }
    | SI expression ALORS ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_instr_si($2, cree_n_instr_bloc($5), cree_n_instr_bloc($9)); }
    | TANTQUE expression FAIRE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_instr_tantque($2, cree_n_instr_bloc($5)); }
    | IDENTIF PARENTHESE_OUVRANTE expression_liste PARENTHESE_FERMANTE POINT_VIRGULE { $$ = cree_n_instr_appel(cree_n_appel($1, $3)); }
    | IDENTIF PARENTHESE_OUVRANTE PARENTHESE_FERMANTE POINT_VIRGULE { $$ = cree_n_instr_appel(cree_n_appel($1, NULL)); }
    | ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_instr_bloc($2); }
    | ECRIRE PARENTHESE_OUVRANTE expression PARENTHESE_FERMANTE POINT_VIRGULE { $$ = cree_n_instr_ecrire($3); }
    | RETOUR expression POINT_VIRGULE { $$ = cree_n_instr_retour($2); }
    | POINT_VIRGULE { $$ = cree_n_instr_vide(); }
    | instruction_affect { $$ = $1; }
    ;

instruction_affect : variable EGAL expression POINT_VIRGULE { $$ = cree_n_instr_affect($1, $3); }
    ;


fonction_dec : IDENTIF PARENTHESE_OUVRANTE param_dec_liste PARENTHESE_FERMANTE variable_dec_liste ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_dec_fonc($1, $3, $5, cree_n_instr_bloc($7)); }
    | IDENTIF PARENTHESE_OUVRANTE param_dec_liste PARENTHESE_FERMANTE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_dec_fonc($1, $3, NULL, cree_n_instr_bloc($6)); }
    | IDENTIF PARENTHESE_OUVRANTE PARENTHESE_FERMANTE variable_dec_liste ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_dec_fonc($1, NULL, $4, cree_n_instr_bloc($6)); }
    | IDENTIF PARENTHESE_OUVRANTE PARENTHESE_FERMANTE ACCOLADE_OUVRANTE instruction_liste ACCOLADE_FERMANTE { $$ = cree_n_dec_fonc($1, NULL, NULL, cree_n_instr_bloc($5)); }
    ;


fonction_dec_liste : fonction_dec fonction_dec_liste { $$ = cree_n_l_dec($1, $2); }
    | fonction_dec { $$ = cree_n_l_dec($1, NULL); }
    ;

variable_dec : ENTIER IDENTIF { $$ = cree_n_dec_var($2); }
    | ENTIER IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT { $$ = cree_n_dec_tab($2, $4); }
    ;

variable_dec_liste : variable_dec VIRGULE variable_dec_liste { $$ = cree_n_l_dec($1, $3); }
    | variable_dec POINT_VIRGULE { $$ = cree_n_l_dec($1, NULL); }
    ;

param_dec : ENTIER IDENTIF { $$ = cree_n_dec_var($2); }

param_dec_liste : param_dec VIRGULE param_dec_liste { $$ = cree_n_l_dec($1, $3); }
    | param_dec { $$ = cree_n_l_dec($1, NULL); }
    ;

variable : IDENTIF { $$ = cree_n_var_simple($1); }
    | IDENTIF CROCHET_OUVRANT expression CROCHET_FERMANT { $$ = cree_n_var_indicee($1, $3); }
    ;

%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
