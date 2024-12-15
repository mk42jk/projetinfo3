// Fichier : output.h
// Description : Fonctions pour l'écriture des données
// des stations électriques dans un fichier de sortie

#ifndef OUTPUT_H
#define OUTPUT_H

#include "avl.h"
#include "input.h"

// Écrit les données des stations dans le fichier spécifié au format CSV
void ecrire_donnees(FILE* f, const NoeudAVL* racine);

#endif
