// Fichier : output.h
// Description : Fonctions pour l'écriture des données
// des stations électriques dans un fichier de sortie

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "avl.h"

// Fonction d'écriture des données
void ecrire_donnees(FILE* f, const NoeudAVL* racine);

#endif // OUTPUT_H
