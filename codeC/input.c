#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/input.h"

#define LONGUEUR_MAX_LIGNE 1024

int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine, const Parametres* params) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        return ERREUR_LECTURE;
    }

    char ligne[LONGUEUR_MAX_LIGNE];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (traiter_ligne(ligne, racine, params) != 0) {
            fclose(fichier);
            return ERREUR_LECTURE;
        }
    }

    fclose(fichier);
    return 0;
}

int traiter_ligne(char* ligne, NoeudAVL** racine, const Parametres* params) {
    char* token;
    int centrale, station = -1, entreprise = -1;
    double capacite = 0.0, consommation = 0.0;
    
    // Lecture de l'ID de la centrale
    token = strtok(ligne, ";");
    if (!token) return 0;
    centrale = atoi(token);

    if (params->id_centrale != -1 && centrale != params->id_centrale) {
        return 0;
    }

    // Lecture selon le type de station
    switch (params->type_station) {
        case STATION_HVB:
            // Lecture de l'ID de la station HVB
            token = strtok(NULL, ";");
            if (!token) return 0;
            if (token[0] != '-') {
                station = atoi(token);
                // Avancer jusqu'à la colonne Company
                token = strtok(NULL, ";"); // HV-A
                token = strtok(NULL, ";"); // LV
                token = strtok(NULL, ";"); // Company
                if (token && token[0] != '-') {
                    entreprise = atoi(token);
                }
                // Lire la capacité ou la consommation
                token = strtok(NULL, ";"); // Individual
                token = strtok(NULL, ";"); // Capacity
                if (token && token[0] != '-') {
                    capacite = atof(token);
                }
                token = strtok(NULL, ";"); // Load
                if (token && token[0] != '-') {
                    consommation = atof(token);
                }
            }
            break;
            
        case STATION_HVA:
            strtok(NULL, ";"); // Skip HV-B
            token = strtok(NULL, ";"); // HV-A
            if (token && token[0] != '-') {
                station = atoi(token);
                token = strtok(NULL, ";"); // LV
                token = strtok(NULL, ";"); // Company
                if (token && token[0] != '-') {
                    entreprise = atoi(token);
                }
                token = strtok(NULL, ";"); // Individual
                token = strtok(NULL, ";"); // Capacity
                if (token && token[0] != '-') {
                    capacite = atof(token);
                }
                token = strtok(NULL, ";"); // Load
                if (token && token[0] != '-') {
                    consommation = atof(token);
                }
            }
            break;
            
        case STATION_LV:
            strtok(NULL, ";"); // Skip HV-B
            strtok(NULL, ";"); // Skip HV-A
            token = strtok(NULL, ";"); // LV
            if (token && token[0] != '-') {
                station = atoi(token);
                token = strtok(NULL, ";"); // Company
                if (token && token[0] != '-') {
                    entreprise = atoi(token);
                }
                token = strtok(NULL, ";"); // Individual
                token = strtok(NULL, ";"); // Capacity
                if (token && token[0] != '-') {
                    capacite = atof(token);
                }
                token = strtok(NULL, ";"); // Load
                if (token && token[0] != '-') {
                    consommation = atof(token);
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
