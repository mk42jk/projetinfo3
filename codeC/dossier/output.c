// Fichier : output.c
// Description : Implémentation des fonctions d'écriture des données
// des stations électriques sur la sortie standard

#include <stdio.h>
#include <stdlib.h>
#include "output.h"

// Fonction récursive pour l'écriture des données
static void ecrire_donnees_rec(FILE* f, const NoeudAVL* noeud, int* compte) {
    if (!noeud) return;
    
    // Parcours infixe pour maintenir l'ordre croissant des ID
    ecrire_donnees_rec(f, noeud->gauche, compte);
    
    // Écriture au format id_station:capacite:consommation
    fprintf(f, "%d:%.2f:%.2f\n", 
            noeud->id_station,
            noeud->capacite,
            noeud->consommation);
    
    (*compte)++;
    
    ecrire_donnees_rec(f, noeud->droite, compte);
}

// Fonction principale d'écriture des données
int ecrire_donnees(FILE* f, const NoeudAVL* racine) {
    if (!f) {
        fprintf(stderr, "Erreur : flux de sortie invalide\n");
        return -1;
    }
    
    if (!racine) {
        fprintf(stderr, "Attention : aucune donnée à écrire\n");
        return 0;
    }
    
    // Compteur de stations écrites
    int compte = 0;
    
    // Écriture des données (sans en-tête, géré par le shell)
    ecrire_donnees_rec(f, racine, &compte);
    
    // Retourne le nombre de stations écrites
    return compte;
}