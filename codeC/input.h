#ifndef INPUT_H
#define INPUT_H

#include "avl.h"

#define ERREUR_LECTURE -1

typedef enum {
    STATION_HVB,
    STATION_HVA,
    STATION_LV
} TypeStation;

typedef enum {
    CONSOMMATEUR_ENTREPRISE,
    CONSOMMATEUR_PARTICULIER,
    CONSOMMATEUR_TOUS
} TypeConsommateur;

typedef struct {
    TypeStation type_station;
    TypeConsommateur type_consommateur;
    int id_centrale;
} Parametres;

int traiter_fichier_entree(const char* nom_fichier, NoeudAVL** racine, const Parametres* params);
int traiter_ligne(char* ligne, NoeudAVL** racine, const Parametres* params);

#endif
