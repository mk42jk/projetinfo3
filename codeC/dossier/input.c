// Fichier : input.c
// Description : Implémentation des fonctions de lecture et traitement
// des données d'entrée du réseau électrique

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

// Longueur maximale d'une ligne dans le fichier d'entrée
#define LONGUEUR_MAX_LIGNE 1024

// Traite une ligne de données du fichier d'entrée
static int traiter_ligne(char* ligne, NoeudAVL** racine) {
    if (!ligne || !racine) return ERREUR_LECTURE;

    char* token;
    int id_station;
    double capacite, consommation;
    
    // Format attendu : "id_station:capacite:consommation"
    token = strtok(ligne, ":");
    if (!token) return ERREUR_LECTURE;
    id_station = atoi(token);
    
    token = strtok(NULL, ":");
    if (!token) return ERREUR_LECTURE;
    capacite = atof(token);
    
    token = strtok(NULL, ":");
    if (!token) return ERREUR_LECTURE;
    consommation = atof(token);
    
    // Mise à jour de l'AVL
    *racine = inserer_noeud(*racine, id_station, capacite);
    if (*racine == NULL) return ERREUR_MEMOIRE;
    
    // Mise à jour de la consommation
    maj_consommation(chercher_noeud(*racine, id_station), consommation);
    
    return SUCCES;
}

// Traitement du fichier d'entrée
int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine) {
    if (!nom_fichier || !racine) return ERREUR_LECTURE;
    
    FILE* f = fopen(nom_fichier, "r");
    if (!f) return ERREUR_LECTURE;
    
    char ligne[LONGUEUR_MAX_LIGNE];
    int resultat = SUCCES;
    
    // Lecture ligne par ligne
    while (fgets(ligne, sizeof(ligne), f)) {
        // Suppression du retour à la ligne
        ligne[strcspn(ligne, "\r\n")] = 0;
        
        // Traitement de la ligne
        resultat = traiter_ligne(ligne, racine);
        if (resultat != SUCCES) break;
    }
    
    fclose(f);
    return resultat;
}
