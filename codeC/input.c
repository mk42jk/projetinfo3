// Fichier : input.c
// Description : Implémentation des fonctions de lecture et traitement
// des données d'entrée du réseau électrique

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

// Longueur maximale d'une ligne dans le fichier d'entrée
#define LONGUEUR_MAX_LIGNE 1024

// Traite une ligne de données de l'entrée standard
static int traiter_ligne(char* ligne, NoeudAVL** racine) {
    if (!ligne || !racine) return ERREUR_LECTURE;

    char* elmt;
    int id_station;
    long capacite, consommation;  // Changé en long pour correspondre à la structure
    
    // Format attendu : "id_station:capacite:consommation"
    elmt = strtok(ligne, ":");
    if (!elmt) return ERREUR_LECTURE;
    id_station = atoi(elmt);
    
    elmt = strtok(NULL, ":");
    if (!elmt) return ERREUR_LECTURE;
    capacite = atol(elmt);  // Conversion explicite en long
    
    elmt = strtok(NULL, ":");
    if (!elmt) return ERREUR_LECTURE;
    consommation = atol(elmt);  // Conversion explicite en long
    
    // Validation des données
    if (capacite < 0 || consommation < 0) {
        return ERREUR_DONNEES;
    }
    
    // Mise à jour de l'AVL uniquement si capacite > 0
    if (capacite > 0) {
        *racine = inserer_noeud(*racine, id_station, capacite);
        if (*racine == NULL) return ERREUR_MEMOIRE;
    }
    
   // Mise à jour de la consommation uniquement si consommation > 0
    if (consommation > 0) {
        NoeudAVL* noeud = chercher_noeud(*racine, id_station);
        if (!noeud) return ERREUR_MEMOIRE;
        maj_consommation(noeud, consommation);
    }

    return SUCCES;
}

// Traitement de l'entrée standard
int traiter_entree(NoeudAVL** racine) {
    if (!racine) return ERREUR_LECTURE;
    
    char ligne[LONGUEUR_MAX_LIGNE];
    int resultat = SUCCES;
    int nb_stations = 0;  // Compteur de stations
    
    rewind(stdin); // Remettre la lecture depuis stdin

    // Lire et ignorer la première ligne (en-tête)
    if (!fgets(ligne, sizeof(ligne), stdin)) {
        fprintf(stderr, "Erreur : impossible de lire la première ligne\n");
        return ERREUR_LECTURE;
    }
    
    // Lecture depuis l'entrée standard
    while (fgets(ligne, sizeof(ligne), stdin)) {
        // Supprimer le retour à la ligne
        ligne[strcspn(ligne, "\n")] = 0;
        
        resultat = traiter_ligne(ligne, racine);
        if (resultat != SUCCES) {
            fprintf(stderr, "Erreur lors du traitement de la ligne %d\n", nb_stations + 1);
            break;
        }
        nb_stations++;
    }
    
    if (resultat == SUCCES && nb_stations == 0) {
        fprintf(stderr, "Attention : aucune donnée n'a été lue\n");
        return ERREUR_LECTURE;
    }
    
    return resultat;
}
