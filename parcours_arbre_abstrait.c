#include <stdio.h>
#include "syntabs.h"
#include "util.h"

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;
int adresseGlobaleCourante = 0;

void parcours_n_prog(n_prog *n);
void parcours_l_instr(n_l_instr *n);
void parcours_instr(n_instr *n);
void parcours_instr_si(n_instr *n);
void parcours_instr_tantque(n_instr *n);
void parcours_instr_affect(n_instr *n);
void parcours_instr_appel(n_instr *n);
void parcours_instr_retour(n_instr *n);
void parcours_instr_ecrire(n_instr *n);
void parcours_l_exp(n_l_exp *n);
void parcours_exp(n_exp *n);
void parcours_varExp(n_exp *n);
void parcours_opExp(n_exp *n);
void parcours_intExp(n_exp *n);
void parcours_lireExp(n_exp *n);
void parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
void parcours_var(n_var *n);
void parcours_var_simple(n_var *n);
void parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
  portee = P_VARIABLE_GLOBALE;
  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void parcours_l_instr(n_l_instr *n)
{
  if(n){
  parcours_instr(n->tete);
  parcours_l_instr(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr(n_instr *n)
{
  if(n) {
    if(n->type == blocInst) parcours_l_instr(n->u.liste);
    else if(n->type == affecteInst) parcours_instr_affect(n);
    else if(n->type == siInst) parcours_instr_si(n);
    else if(n->type == tantqueInst) parcours_instr_tantque(n);
    else if(n->type == appelInst) parcours_instr_appel(n);
    else if(n->type == retourInst) parcours_instr_retour(n);
    else if(n->type == ecrireInst) parcours_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_si(n_instr *n)
{
  parcours_exp(n->u.si_.test);
  parcours_instr(n->u.si_.alors);
  if(n->u.si_.sinon){
    parcours_instr(n->u.si_.sinon);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  parcours_exp(n->u.tantque_.test);
  parcours_instr(n->u.tantque_.faire);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  if (rechercheExecutable(n->u.affecte_.var->nom) != -1) {
    parcours_var(n->u.affecte_.var);
    parcours_exp(n->u.affecte_.exp);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)
{
  int nbArgsAppel = 0;
  n_l_exp *tmp = n->args;
  //Calcule le nombre de paramètres de la fonction
  while (tmp != NULL) {
    tmp = tmp->queue;
    nbArgsAppel = nbArgsAppel + 1;
  }

  int ligneCourante = rechercheExecutable(n->fonction);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].complement == nbArgsAppel) {
      if (rechercheExecutable("main") != -1) {
        //entreeFonction();
        parcours_l_exp(n->args);
        //sortieFonction(1);
      }
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  parcours_exp(n->u.retour_.expression);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  parcours_exp(n->u.ecrire_.expression);
}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{
  if(n){
    parcours_exp(n->tete);
    parcours_l_exp(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_exp(n_exp *n)
{
  if(n->type == varExp) parcours_varExp(n);
  else if(n->type == opExp) parcours_opExp(n);
  else if(n->type == intExp) parcours_intExp(n);
  else if(n->type == appelExp) parcours_appelExp(n);
  else if(n->type == lireExp) parcours_lireExp(n);
}

/*-------------------------------------------------------------------------*/

void parcours_varExp(n_exp *n)
{
  parcours_var(n->u.var);
}

/*-------------------------------------------------------------------------*/
void parcours_opExp(n_exp *n)
{
  if( n->u.opExp_.op1 != NULL ) {
    parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    parcours_exp(n->u.opExp_.op2);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_intExp(n_exp *n)
{

}

/*-------------------------------------------------------------------------*/
void parcours_lireExp(n_exp *n)
{

}

/*-------------------------------------------------------------------------*/

void parcours_appelExp(n_exp *n)
{
  parcours_appel(n->u.appel);
}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  if( n ){
    parcours_dec(n->tete);
    parcours_l_dec(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)
{
  if(n){
    if(n->type == foncDec) {
      parcours_foncDec(n);
    }
    else if(n->type == varDec) {
      parcours_varDec(n);
    }
    else if(n->type == tabDec) {
      parcours_tabDec(n);
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_foncDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
    int taille = 0;
    //Calcule le nombre de paramètres de la fonction
    n_l_dec *tmp = n->u.foncDec_.param;
    while (tmp != NULL) {
      tmp = tmp->queue;
      taille = taille + 1;
    }
    ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_FONCTION, 0, taille);
    entreeFonction();
    portee = P_ARGUMENT;
    parcours_l_dec(n->u.foncDec_.param);
    portee = P_VARIABLE_LOCALE;
    parcours_l_dec(n->u.foncDec_.variables);
    parcours_instr(n->u.foncDec_.corps);
    sortieFonction(1);
  }


}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
    if (portee == P_VARIABLE_LOCALE) {
      ajouteIdentificateur(n->nom, P_VARIABLE_LOCALE, T_ENTIER, adresseLocaleCourante, 1);
      adresseLocaleCourante += 4;
    }
    if (portee == P_VARIABLE_GLOBALE) {
      ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_ENTIER, adresseGlobaleCourante, 1);
      adresseGlobaleCourante += 4;
    }
    if (portee == P_ARGUMENT) {
      ajouteIdentificateur(n->nom, P_ARGUMENT, T_ENTIER, adresseArgumentCourant, 1);
      adresseArgumentCourant += 4;
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
    ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
    adresseGlobaleCourante = adresseGlobaleCourante + 4 * n->u.tabDec_.taille;
  }
}

/*-------------------------------------------------------------------------*/

void parcours_var(n_var *n)
{
  if(n->type == simple) {
    parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    parcours_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/
void parcours_var_simple(n_var *n)
{
  //Verifier que c'est un entier (T_ENTIER)
  int ligneCourante = rechercheExecutable(n->nom);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].type == T_ENTIER) {

    } else {
      erreur("Problème de type");
    }
  } else {
    erreur("Variable non declarée");
  }
}

/*-------------------------------------------------------------------------*/
void parcours_var_indicee(n_var *n)
{
  //Verfier que c'est un tableau indicé de portée globale
  int ligneCourante = rechercheExecutable(n->nom);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].type == T_TABLEAU_ENTIER) {
      if (tabsymboles.tab[ligneCourante].portee == P_VARIABLE_GLOBALE) {
        parcours_exp(n->u.indicee_.indice);
      }
    }
  }
}
/*-------------------------------------------------------------------------*/
