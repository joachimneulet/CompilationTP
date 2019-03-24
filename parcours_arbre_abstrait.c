#include <stdio.h>
#include <string.h>
#include "syntabs.h"
#include "util.h"

extern int portee;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;
int adresseGlobaleCourante = 0;
int afficheTabSymb = 0; //Si 1 : Affiche la table des symboles à chaque sortie de fonction
operande *varCourante = NULL;

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
  parcours_exp(n->u.si_.test);

  operande *var1 = NULL;
  operande *var2 = NULL;

  if (n->u.si_.test->u.opExp_.op1->type == varExp && n->u.si_.test->u.opExp_.op2->type == varExp) {
    var1 = code3a_new_var(n->u.si_.test->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
    var2 = code3a_new_var(n->u.si_.test->u.opExp_.op2->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
  } else if (n->u.si_.test->u.opExp_.op1->type == varExp && n->u.si_.test->u.opExp_.op2->type == intExp) {
    var1 = code3a_new_var(n->u.si_.test->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
    var2 = code3a_new_constante(n->u.si_.test->u.opExp_.op2->u.entier);
  } else if (n->u.si_.test->u.opExp_.op1->type == intExp && n->u.si_.test->u.opExp_.op2->type == intExp) {
    var1 = code3a_new_constante(n->u.si_.test->u.opExp_.op1->u.entier);
    var2 = code3a_new_constante(n->u.si_.test->u.opExp_.op2->u.entier);
  } else if (n->u.si_.test->u.opExp_.op1->type == appelExp && n->u.si_.test->u.opExp_.op2->type == varExp) {
    var1 = varCourante;
    var2 = code3a_new_var(n->u.si_.test->u.opExp_.op2->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
  }
  operande *etiquette0 = code3a_new_etiquette(_new_etiq());
  operande *etiquette1 = code3a_new_etiquette(_new_etiq());
  operande *etiquette2 = code3a_new_etiquette(_new_etiq());
  operande *etiquette3 = code3a_new_etiquette(_new_etiq());
  if (n->u.si_.test->u.opExp_.op == inferieur) {
    code3a_ajoute_instruction(jump_if_less, var1, var2, etiquette0, "Si inferieur goto");
  } else if (n->u.si_.test->u.opExp_.op == egal) {
    code3a_ajoute_instruction(jump_if_equal, var1, var2, etiquette0, "Si egal goto");
  }
  varCourante = code3a_new_temporaire();
  operande *constante1 = code3a_new_constante(0);
  code3a_ajoute_instruction(assign, constante1, NULL, varCourante, "Affectation");
  code3a_ajoute_instruction(jump, etiquette1, NULL, NULL, "Goto etiquette");
  code3a_ajoute_etiquette(etiquette0->u.oper_nom);
  code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, varCourante, "Affectation");
  code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  code3a_ajoute_instruction(jump_if_equal, varCourante, constante1, etiquette2, "Si egal goto");

  parcours_instr(n->u.si_.alors);

  if(!n->u.si_.sinon) {
    code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  }

  if(n->u.si_.sinon){
    code3a_ajoute_instruction(jump, etiquette3, NULL, NULL, "Goto etiquette");
    code3a_ajoute_etiquette(etiquette2->u.oper_nom);
    parcours_instr(n->u.si_.sinon);
    code3a_ajoute_etiquette(etiquette3->u.oper_nom);
  }



}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  parcours_exp(n->u.tantque_.test);

  operande *var1 = NULL;
  operande *var2 = NULL;
  if (n->u.tantque_.test->u.opExp_.op1->type == varExp && n->u.tantque_.test->u.opExp_.op2->type == varExp) {
    var1 = code3a_new_var(n->u.tantque_.test->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
    var2 = code3a_new_var(n->u.tantque_.test->u.opExp_.op2->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
  } else if (n->u.tantque_.test->u.opExp_.op1->type == varExp && n->u.tantque_.test->u.opExp_.op2->type == intExp) {
    var1 = code3a_new_var(n->u.tantque_.test->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
    var2 = code3a_new_constante(n->u.tantque_.test->u.opExp_.op2->u.entier);
  } else if (n->u.tantque_.test->u.opExp_.op1->type == intExp && n->u.tantque_.test->u.opExp_.op2->type == intExp) {
    var1 = code3a_new_constante(n->u.tantque_.test->u.opExp_.op1->u.entier);
    var2 = code3a_new_constante(n->u.tantque_.test->u.opExp_.op2->u.entier);
  }

  operande *etiquette0 = code3a_new_etiquette(_new_etiq());
  operande *etiquette1 = code3a_new_etiquette(_new_etiq());
  operande *etiquette2 = code3a_new_etiquette(_new_etiq());
  operande *etiquette3 = code3a_new_etiquette(_new_etiq());
  code3a_ajoute_etiquette(etiquette0->u.oper_nom);
  code3a_ajoute_instruction(jump_if_less, var1, var2, etiquette1, "Si inferieur goto");
  varCourante = code3a_new_temporaire();
  operande *constante1 = code3a_new_constante(0);
  code3a_ajoute_instruction(assign, constante1, NULL, varCourante, "Affectation");
  code3a_ajoute_instruction(jump, etiquette2, NULL, NULL, "Goto etiquette");
  code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, varCourante, "Affectation");
  code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  code3a_ajoute_instruction(jump_if_equal, varCourante, constante1, etiquette3, "Si egal goto");

  parcours_instr(n->u.tantque_.faire);

  code3a_ajoute_instruction(jump, etiquette0, NULL, NULL, "Goto etiquette");
  code3a_ajoute_etiquette(etiquette3->u.oper_nom);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  if (rechercheExecutable(n->u.affecte_.var->nom) != -1) {

    parcours_var(n->u.affecte_.var);
    parcours_exp(n->u.affecte_.exp);
    operande *result = NULL;
    if (n->u.affecte_.exp->type == intExp) {
      result = code3a_new_var(n->u.affecte_.var->nom, portee, adresseLocaleCourante);
      code3a_ajoute_instruction(assign, code3a_new_constante(n->u.affecte_.exp->u.entier), NULL, result, "Affectation");
    } else if (n->u.affecte_.exp->type == varExp) {
      operande* var1 = code3a_new_var(n->u.affecte_.exp->u.var->nom, portee, adresseLocaleCourante);
      result = code3a_new_var(n->u.affecte_.var->nom, portee, adresseLocaleCourante);
      code3a_ajoute_instruction(assign, var1, NULL, result, "Affectation");
    } else if (n->u.affecte_.exp->type == lireExp) {
      result = code3a_new_temporaire();
      code3a_ajoute_instruction(sys_read, NULL, NULL, result, "Expression lire : etape 1");
      code3a_ajoute_instruction(assign, result, NULL, code3a_new_var(n->u.affecte_.var->nom, portee, adresseLocaleCourante), "Expression lire : etape 2");
    } else if (n->u.affecte_.exp->type == opExp) {
      result = code3a_new_var(n->u.affecte_.var->nom, portee, adresseLocaleCourante);
      code3a_ajoute_instruction(assign, varCourante, NULL, result, "Affectation");
    }
    varCourante = result;
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
        parcours_l_exp(n->args);
      }
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  parcours_exp(n->u.retour_.expression);
  if (n->u.retour_.expression->type == varExp) {
    operande *var = code3a_new_var(n->u.retour_.expression->u.var->nom, portee, adresseLocaleCourante);
    code3a_ajoute_instruction(func_val_ret, var, NULL, NULL, "sauvegarder la valeur de retour");
    code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "Fin declaration de fonction");
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  parcours_exp(n->u.ecrire_.expression);
  if (n->u.ecrire_.expression->type == varExp) {
    varCourante = code3a_new_var(n->u.ecrire_.expression->u.var->nom, portee, adresseLocaleCourante);
  }
  code3a_ajoute_instruction(sys_write, varCourante, NULL, NULL, "Instruction ecrire");
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
  if (portee == P_VARIABLE_LOCALE) {
    operande *var1 = NULL;
    operande *var2 = NULL;
    if (n->u.opExp_.op1->type == varExp && n->u.opExp_.op2->type == varExp) {
      var1 = code3a_new_var(n->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
      var2 = code3a_new_var(n->u.opExp_.op2->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
    } else if (n->u.opExp_.op1->type == varExp && n->u.opExp_.op2->type == intExp) {
      var1 = code3a_new_var(n->u.opExp_.op1->u.var->nom, P_VARIABLE_LOCALE, adresseLocaleCourante);
      var2 = code3a_new_constante(n->u.opExp_.op2->u.entier);
    } else if (n->u.opExp_.op1->type == intExp && n->u.opExp_.op2->type == intExp) {
      var1 = code3a_new_constante(n->u.opExp_.op1->u.entier);
      var2 = code3a_new_constante(n->u.opExp_.op2->u.entier);
    }
    if (n->u.opExp_.op != egal && n->u.opExp_.op != inferieur) {
      varCourante = code3a_new_temporaire();
      code3a_ajoute_instruction(n->u.opExp_.op, var1, var2, varCourante, "Expression arith");
    }
  }

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

  code3a_ajoute_instruction(alloc, code3a_new_constante(1), NULL, NULL, "alloue place pour la valeur de retour");

  //On parcours les paramètres de la fonction
  n_l_exp *tmp = n->u.appel->args;
  while (tmp != NULL) {
    code3a_ajoute_instruction(func_param, code3a_new_var(tmp->tete->u.var->nom, portee, adresseArgumentCourant), NULL, NULL, "param");
    tmp = tmp->queue;
  }

  varCourante = code3a_new_temporaire();
  code3a_ajoute_instruction(func_call, code3a_new_etiquette(n->u.appel->fonction), NULL, varCourante, "call");
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

    code3a_ajoute_etiquette(n->nom);
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

    if (portee != P_ARGUMENT) {
      code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "Alloue de la memoire pour une variable");
    }

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

    int tailleTab = n->u.tabDec_.taille;

    code3a_ajoute_instruction(alloc, code3a_new_constante(tailleTab), code3a_new_var(n->nom, portee, adresseGlobaleCourante), NULL, "Alloue de la memoire pour un tableau");

    ajouteIdentificateur(n->nom, P_VARIABLE_GLOBALE, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
    adresseGlobaleCourante = adresseGlobaleCourante + 4 * tailleTab;
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
