// Fichier : output.c
// Description : Implémentation des fonctions d'écriture des données
// des stations électriques dans le fichier de sortie

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/output.h"

// Fonction récursive pour l'écriture des données d'un sous-arbre
static void ecrire_donnees_rec(FILE* f, const NoeudAVL* noeud) {
    if (!noeud) return;

    // Parcours infixe (gauche-racine-droite) pour maintenir l'ordre
    ecrire_donnees_rec(f, noeud->gauche);
    
    // Écriture des données du nœud courant au format CSV
    fprintf(f, "%d,%.2f,%.2f\n", 
            noeud->id_station,
            noeud->capacite,
            noeud->consommation);
    
    ecrire_donnees_rec(f, noeud->droite);
}

// Fonction principale d'écriture des données dans le fichier
void ecrire_donnees(FILE* f, const NoeudAVL* racine) {
    if (!f) return;
    
    // Écriture de l'en-tête du fichier CSV
    fprintf(f, "ID,Capacite,Consommation\n");
    
    // Écriture récursive de toutes les données
    ecrire_donnees_rec(f, racine);
}