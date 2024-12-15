// Fichier : input.h
// Description : Définition des types et structures pour la gestion
// des entrées du programme de réseau électrique

#ifndef INPUT_H
#define INPUT_H

#include "avl.h"

#define ERREUR_LECTURE -1

// Types de stations électriques
typedef enum {
    STATION_HVB,    // Haute Tension B
    STATION_HVA,    // Haute Tension A
    STATION_LV      // Basse Tension
} TypeStation;

// Types de consommateurs d'électricité
typedef enum {
    CONSOMMATEUR_ENTREPRISE,    // Entreprises et industries
    CONSOMMATEUR_PARTICULIER,   // Clients particuliers
    CONSOMMATEUR_TOUS          // Tous types de consommateurs
} TypeConsommateur;

// Structure pour les paramètres de lecture des données
typedef struct {
    TypeStation type_station;
    TypeConsommateur type_consommateur;
} Parametres;

// Fonctions de traitement des fichiers et des données
int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine, const Parametres* params);
int traiter_ligne(char* ligne, NoeudAVL** racine, const Parametres* params);

#endif
