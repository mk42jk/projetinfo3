// Fichier : input.h
// Description : Définition des types et structures pour la gestion
// des entrées du programme de réseau électrique

#ifndef INPUT_H
#define INPUT_H

#include "avl.h"

// Codes d'erreur
#define SUCCES 0
#define ERREUR_LECTURE 1
#define ERREUR_MEMOIRE 2

// Fonction de traitement du fichier d'entrée
int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine);

#endif // INPUT_H
