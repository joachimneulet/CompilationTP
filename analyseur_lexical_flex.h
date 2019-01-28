#ifndef __ANALYSEUR_LEXICAL__
#define __ANALYSEUR_LEXICAL__

int yylex(void);
int yylval(void);
void nom_token( int token, char *nom, char *valeur );

#endif
