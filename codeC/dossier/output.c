// Fichier : output.c
// Description : Implémentation des fonctions d'écriture des données
// des stations électriques dans le fichier de sortie

#include <stdio.h>
#include <stdlib.h>
#include "../include/output.h"

// Fonction récursive pour l'écriture des données
static void ecrire_donnees_rec(FILE* f, const NoeudAVL* noeud) {
    if (!noeud) return;
    
    // Parcours infixe pour maintenir l'ordre
    ecrire_donnees_rec(f, noeud->gauche);
    
    // Écriture au format id_station:capacite:consommation
    fprintf(f, "%d:%.2f:%.2f\n", 
            noeud->id_station,
            noeud->capacite,
            noeud->consommation);
    
    ecrire_donnees_rec(f, noeud->droite);
}

// Fonction principale d'écriture des données
void ecrire_donnees(FILE* f, const NoeudAVL* racine) {
    if (!f || !racine) return;
    ecrire_donnees_rec(f, racine);
}