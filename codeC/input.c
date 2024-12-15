// Fichier : input.c
// Description : Implémentation des fonctions de lecture et traitement
// des données d'entrée du réseau électrique

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

// Longueur maximale d'une ligne dans le fichier d'entrée
#define LONGUEUR_MAX_LIGNE 1024

// Traite une ligne de données du fichier CSV d'entrée
int traiter_ligne(char* ligne, NoeudAVL** racine, const Parametres* params) {
    // Vérification des paramètres d'entrée
    if (!ligne || !racine || !params) {
        return ERREUR_LECTURE;
    }

    char* fragment;
    int centrale, station = -1, entreprise = -1;
    double capacite = 0.0, consommation = 0.0;
    
    // Lecture de l'ID de la centrale
    fragment = strtok(ligne, ";");
    if (!fragment) return 0;
    centrale = atoi(fragment);

    if (params->id_centrale != -1 && centrale != params->id_centrale) {
        return 0;
    }

    // Lecture selon le type de station
    switch (params->type_station) {
        case STATION_HVB:
            // Lecture de l'ID de la station HVB
            fragment = strtok(NULL, ";");
            if (!fragment) return 0;
            if (fragment[0] != '-') {
                station = atoi(fragment);
                // Avancer jusqu'à la colonne Company
                fragment = strtok(NULL, ";"); // HV-A
                fragment = strtok(NULL, ";"); // LV
                fragment = strtok(NULL, ";"); // Company
                if (fragment && fragment[0] != '-') {
                    entreprise = atoi(fragment);
                }
                // Lire la capacité ou la consommation
                fragment = strtok(NULL, ";"); // Individual
                fragment = strtok(NULL, ";"); // Capacity
                if (fragment && fragment[0] != '-') {
                    capacite = atof(fragment);
                }
                fragment = strtok(NULL, ";"); // Load
                if (fragment && fragment[0] != '-') {
                    consommation = atof(fragment);
                }
            }
            break;
            
        case STATION_HVA:
            strtok(NULL, ";"); // Skip HV-B
            fragment = strtok(NULL, ";"); // HV-A
            if (fragment && fragment[0] != '-') {
                station = atoi(fragment);
                fragment = strtok(NULL, ";"); // LV
                fragment = strtok(NULL, ";"); // Company
                if (fragment && fragment[0] != '-') {
                    entreprise = atoi(fragment);
                }
                fragment = strtok(NULL, ";"); // Individual
                fragment = strtok(NULL, ";"); // Capacity
                if (fragment && fragment[0] != '-') {
                    capacite = atof(fragment);
                }
                fragment = strtok(NULL, ";"); // Load
                if (fragment && fragment[0] != '-') {
                    consommation = atof(fragment);
                }
            }
            break;
            
        case STATION_LV:
            strtok(NULL, ";"); // Skip HV-B
            strtok(NULL, ";"); // Skip HV-A
            fragment = strtok(NULL, ";"); // LV
            if (fragment && fragment[0] != '-') {
                station = atoi(fragment);
                fragment = strtok(NULL, ";"); // Company
                if (fragment && fragment[0] != '-') {
                    entreprise = atoi(fragment);
                }
                fragment = strtok(NULL, ";"); // Individual
                fragment = strtok(NULL, ";"); // Capacity
                if (fragment && fragment[0] != '-') {
                    capacite = atof(fragment);
                }
                fragment = strtok(NULL, ";"); // Load
                if (fragment && fragment[0] != '-') {
                    consommation = atof(fragment);
                }
            }
            break;
    }

    // Traitement des données lues
    if (station != -1) {
        if (capacite > 0.0) {
            // Insertion ou mise à jour de la station
            *racine = inserer_noeud(*racine, station, capacite);
        }
        if (consommation > 0.0 && 
            ((entreprise != -1 && (params->type_consommateur == CONSOMMATEUR_ENTREPRISE || params->type_consommateur == CONSOMMATEUR_TOUS)) ||
             (entreprise == -1 && (params->type_consommateur == CONSOMMATEUR_PARTICULIER || params->type_consommateur == CONSOMMATEUR_TOUS)))) {
            // Mise à jour de la consommation
            NoeudAVL* noeud = chercher_noeud(*racine, station);
            if (noeud) {
                maj_consommation(noeud, consommation);
            }
        }
    }

    return 0;
}

// Traitement complet du fichier d'entrée
int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine, const Parametres* params) {
    // Ouverture du fichier en lecture
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        return ERREUR_LECTURE;
    }

    char ligne[LONGUEUR_MAX_LIGNE];
    int resultat = 0;

    // Lecture et traitement ligne par ligne
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Ignore les lignes vides
        if (strlen(ligne) <= 1) continue;
        
        // Traitement de la ligne courante
        resultat = traiter_ligne(ligne, racine, params);
        if (resultat != 0) {
            break;
        }
    }

    fclose(fichier);
    return resultat;
}
