#include <stdio.h>
#include <string.h>
#include "syntabs.h"
#include "util.h"

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;
int adresseGlobaleCourante = 0;
int afficheTabSymb = 0; //Si 1 : Affiche la table des symboles à chaque sortie de fonction
operande *varTempCourante = NULL;

void parcours_n_prog(n_prog *n);
void parcours_l_instr(n_l_instr *n);
void parcours_instr(n_instr *n);
void parcours_instr_si(n_instr *n);
void parcours_instr_tantque(n_instr *n);
void parcours_instr_affect(n_instr *n);
void parcours_instr_appel(n_instr *n);
void parcours_instr_retour(n_instr *n);
void parcours_instr_ecrire(n_instr *n);
operande *parcours_l_exp(n_l_exp *n);
operande *parcours_exp(n_exp *n);
operande *parcours_varExp(n_exp *n);
operande *parcours_opExp(n_exp *n);
operande *parcours_intExp(n_exp *n);
operande *parcours_lireExp(n_exp *n);
operande *parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
operande *parcours_var(n_var *n);
operande *parcours_var_simple(n_var *n);
operande *parcours_var_indicee(n_var *n);
operande *parcours_appel(n_appel *n);

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
  portee = P_VARIABLE_GLOBALE;
  code3a_init();
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
  operande *oper1 = parcours_exp(n->u.tantque_.test->u.opExp_.op1);
  operande *oper2 = parcours_exp(n->u.tantque_.test->u.opExp_.op2);

  operande *etiquette0 = code3a_new_etiquette_auto();
  operande *etiquette1 = code3a_new_etiquette_auto();
  operande *etiquette2 = code3a_new_etiquette_auto();

  operande *constante0 = code3a_new_constante(0);
  operande *constante1 = code3a_new_constante(-1);
  varTempCourante = code3a_new_temporaire();

  code3a_ajoute_instruction(assign, constante1, NULL, varTempCourante, "Affectation");

  if (n->u.si_.test->u.opExp_.op == inferieur) {
    code3a_ajoute_instruction(jump_if_less, oper1, oper2, etiquette2, "Si inferieur goto");
  } else if (n->u.si_.test->u.opExp_.op == egal) {
    code3a_ajoute_instruction(jump_if_equal, oper1, oper2, etiquette2, "Si egal goto");
  }

  code3a_ajoute_instruction(assign, constante0, NULL, varTempCourante, "Affectation");
  code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  code3a_ajoute_instruction(jump_if_equal, varTempCourante, constante0, etiquette1, "Si egal goto");

  parcours_instr(n->u.si_.alors);

  if(!n->u.si_.sinon) {
    code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  }

  if(n->u.si_.sinon) {

    code3a_ajoute_instruction(jump, etiquette0, NULL, NULL, "Goto etiquette");
    code3a_ajoute_etiquette(etiquette1->u.oper_nom);
    parcours_instr(n->u.si_.sinon);
    code3a_ajoute_etiquette(etiquette0->u.oper_nom);
  }



}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  operande *etiquette0 = code3a_new_etiquette_auto();
  operande *etiquette1 = code3a_new_etiquette_auto();
  operande *etiquette2 = code3a_new_etiquette_auto();

  operande *constante0 = code3a_new_constante(0);
  operande *constante1 = code3a_new_constante(-1);

  varTempCourante = code3a_new_temporaire();
  code3a_ajoute_etiquette(etiquette0->u.oper_nom);

  operande *oper1 = parcours_exp(n->u.tantque_.test->u.opExp_.op1);
  operande *oper2 = parcours_exp(n->u.tantque_.test->u.opExp_.op2);

  code3a_ajoute_instruction(assign, constante1, NULL, varTempCourante, "Affectation");

  if (n->u.si_.test->u.opExp_.op == inferieur) {
    code3a_ajoute_instruction(jump_if_less, oper1, oper2, etiquette2, "Si inferieur goto");
  } else if (n->u.si_.test->u.opExp_.op == egal) {
    code3a_ajoute_instruction(jump_if_equal, oper1, oper2, etiquette2, "Si egal goto");
  }

  code3a_ajoute_instruction(assign, constante0, NULL, varTempCourante, "Affectation");
  code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  code3a_ajoute_instruction(jump_if_equal, varTempCourante, constante0, etiquette1, "Si egal goto");

  parcours_instr(n->u.tantque_.faire);

  code3a_ajoute_instruction(jump, etiquette0, NULL, NULL, "Goto etiquette");
  code3a_ajoute_etiquette(etiquette1->u.oper_nom);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  int ligneCourante = rechercheExecutable(n->u.affecte_.var->nom);
  if (ligneCourante != -1) {

    operande *result =  parcours_var(n->u.affecte_.var);
    operande *oper = parcours_exp(n->u.affecte_.exp);

    code3a_ajoute_instruction(assign, oper, NULL, result, "Affectation");

  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
}
/*-------------------------------------------------------------------------*/

operande *parcours_appel(n_appel *n)
{
  code3a_ajoute_instruction(alloc, code3a_new_constante(1), NULL, NULL, "alloue place pour la valeur de retour");

  int nbArgsAppel = 0;
  n_l_exp *tmp = n->args;
  //Calcule le nombre de paramètres de la fonction
  while (tmp != NULL) {
    operande *param = parcours_exp(tmp->tete);
    code3a_ajoute_instruction(func_param, param, NULL, NULL, "param");
    tmp = tmp->queue;
    nbArgsAppel = nbArgsAppel + 1;
  }

  //Rajoute un "f" devant le nom de la fonction
  char *prefixe = "f";
  char *fFonction = (char *) malloc(1 + strlen(prefixe) + strlen(n->fonction));
  strcpy(fFonction, prefixe);
  strcat(fFonction, n->fonction);

  int ligneCourante = rechercheExecutable(n->fonction);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].complement == nbArgsAppel) {
      if (rechercheExecutable("main") != -1) {

        parcours_l_exp(n->args);
        varTempCourante = code3a_new_temporaire();
        code3a_ajoute_instruction(func_call, code3a_new_etiquette(fFonction), NULL, varTempCourante, "call");
        return varTempCourante;
      } else {
        code3a_ajoute_instruction(func_call, code3a_new_etiquette(fFonction), NULL, NULL, "call");
      }
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  parcours_exp(n->u.retour_.expression);
  if (n->u.retour_.expression->type == varExp) {
    operande *var = code3a_new_var(n->u.retour_.expression->u.var->nom, portee, adresseGlobaleCourante);
    code3a_ajoute_instruction(func_val_ret, var, NULL, NULL, "sauvegarder la valeur de retour");
    code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin declaration de fonction");
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  operande *oper = parcours_exp(n->u.ecrire_.expression);
  code3a_ajoute_instruction(sys_write, oper, NULL, NULL, "Instruction ecrire");
}

/*-------------------------------------------------------------------------*/

operande *parcours_l_exp(n_l_exp *n)
{
  operande *result = NULL;
  if(n){
    parcours_exp(n->tete);
    parcours_l_exp(n->queue);
  }
  return result;
}

/*-------------------------------------------------------------------------*/

operande *parcours_exp(n_exp *n)
{
  if(n->type == varExp) return parcours_varExp(n);
  else if(n->type == opExp) return parcours_opExp(n);
  else if(n->type == intExp) return parcours_intExp(n);
  else if(n->type == appelExp) return parcours_appelExp(n);
  else if(n->type == lireExp) parcours_lireExp(n);
}

/*-------------------------------------------------------------------------*/

operande *parcours_varExp(n_exp *n)
{
  return parcours_var(n->u.var);
}

/*-------------------------------------------------------------------------*/
operande *parcours_opExp(n_exp *n)
{
  operande *oper1 = NULL;
  operande *oper2 = NULL;
  operande *result = NULL;

  if( n->u.opExp_.op1 != NULL ) {
    oper1 = parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    oper2 = parcours_exp(n->u.opExp_.op2);
  }

  result = code3a_new_temporaire();
  code3a_ajoute_instruction(n->u.opExp_.op, oper1, oper2, result, "Expression arith");
  return result;

}

/*-------------------------------------------------------------------------*/

operande *parcours_intExp(n_exp *n)
{
  return code3a_new_constante(n->u.entier);
}

/*-------------------------------------------------------------------------*/
operande *parcours_lireExp(n_exp *n)
{
  operande *result = code3a_new_temporaire();
  code3a_ajoute_instruction(sys_read, NULL, NULL, result, "Expression lire");
  return result;
}

/*-------------------------------------------------------------------------*/

operande *parcours_appelExp(n_exp *n)
{
  return parcours_appel(n->u.appel);
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

    //Rajoute un "f" devant le nom de la fonction
    char *prefixe = "f";
    char *fFonction = (char *) malloc(1 + strlen(prefixe) + strlen(n->nom));
    strcpy(fFonction, prefixe);
    strcat(fFonction, n->nom);

    code3a_ajoute_etiquette(fFonction);
    code3a_ajoute_instruction(func_begin, NULL, NULL, NULL, "Debut declaration de fonction");

    ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_FONCTION, 0, taille);
    entreeFonction();
    portee = P_ARGUMENT;
    parcours_l_dec(n->u.foncDec_.param);
    portee = P_VARIABLE_LOCALE;
    parcours_l_dec(n->u.foncDec_.variables);
    parcours_instr(n->u.foncDec_.corps);
    sortieFonction(afficheTabSymb);

    code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin declaration de fonction");

  }


}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
    if (portee == P_VARIABLE_LOCALE) {
      ajouteIdentificateur(n->nom, P_VARIABLE_LOCALE, T_ENTIER, adresseLocaleCourante, 1);
      code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "Alloue de la memoire pour une variable");
      adresseLocaleCourante += 4;
    }
    if (portee == P_VARIABLE_GLOBALE) {
      ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_ENTIER, adresseGlobaleCourante, 1);
      code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseGlobaleCourante), NULL, "Alloue de la memoire pour une variable");
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
  int ligneCourante = rechercheDeclarative(n->nom);
  if (ligneCourante == -1) {

    int tailleTab = n->u.tabDec_.taille;

    code3a_ajoute_instruction(alloc, code3a_new_constante(tailleTab), code3a_new_var(n->nom, P_VARIABLE_GLOBALE, adresseGlobaleCourante), NULL, "Alloue de la memoire pour un tableau");

    ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
    adresseGlobaleCourante = adresseGlobaleCourante + 4 * tailleTab;
  }
}

/*-------------------------------------------------------------------------*/

operande *parcours_var(n_var *n)
{
  if(n->type == simple) {
    return parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    parcours_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/
operande *parcours_var_simple(n_var *n)
{
  //Verifier que c'est un entier (T_ENTIER)
  int ligneCourante = rechercheExecutable(n->nom);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].type == T_ENTIER) {
      return code3a_new_var(n->nom, tabsymboles.tab[ligneCourante].portee, tabsymboles.tab[ligneCourante].adresse);
    } else {
      erreur("Problème de type");
    }
  } else {
    erreur("Variable non declarée");
  }
}

/*-------------------------------------------------------------------------*/
operande *parcours_var_indicee(n_var *n)
{
  //Verfier que c'est un tableau indicé de portée globale
  int ligneCourante = rechercheExecutable(n->nom);
  if (ligneCourante != -1) {
    if (tabsymboles.tab[ligneCourante].type == T_TABLEAU_ENTIER) {
      if (tabsymboles.tab[ligneCourante].portee == P_VARIABLE_GLOBALE) {
          operande *indice = parcours_exp(n->u.indicee_.indice);
          if (n->u.indicee_.indice->type == varExp) {
            varTempCourante = code3a_new_temporaire();
            code3a_ajoute_instruction(assign, indice, NULL, varTempCourante, "Affectation");
            indice = varTempCourante;
          }
          operande *result =  code3a_new_var(n->nom, tabsymboles.tab[ligneCourante].portee, tabsymboles.tab[ligneCourante].adresse);
          result->u.oper_var.oper_indice = indice;
          return result;
      }
    }
  }
}
/*-------------------------------------------------------------------------*/
