// Fichier : input.h
// Description : Définition des types et structures pour la gestion
// des entrées du programme de réseau électrique

#ifndef INPUT_H
#define INPUT_H

#include "avl.h"

// Codes d'erreur pour la lecture des données
#define SUCCES 0
#define ERREUR_LECTURE 1
#define ERREUR_MEMOIRE 2
#define ERREUR_DONNEES 3

// Traitement de l'entrée standard
int traiter_entree(NoeudAVL** racine);

#endif // INPUT_H
