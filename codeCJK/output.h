// Fichier : output.h
// Description : Fonctions pour l'écriture des données
// des stations électriques sur la sortie standard

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "avl.h"

/**
 * Écrit les données des stations sur le flux de sortie spécifié.
 * Format : id_station:capacite:consommation
 * 
 * @param f Flux de sortie (typiquement stdout)
 * @param racine Racine de l'arbre AVL contenant les données
 * @return Nombre de stations écrites, -1 en cas d'erreur
 */
int ecrire_donnees(FILE* f, const NoeudAVL* racine);

#endif // OUTPUT_H
