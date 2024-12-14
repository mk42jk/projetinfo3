#ifndef AVL_H
#define AVL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATION_HVB,
    STATION_HVA,
    STATION_LV
} TypeStation;

typedef struct {
    TypeStation type_station;
    char type_consommateur;
    int id_centrale;
} Parametres;

typedef struct NoeudAVL {
    int id_station;
    int capacite;
    int consommation;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

typedef struct {
    int id_station;
    int capacite;
    int consommation;
} DonneesSortie;

NoeudAVL* creer_noeud(int id_station, int capacite);
void liberer_avl(NoeudAVL *racine);

NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station);
void maj_consommation(NoeudAVL *noeud, int consommation);
bool est_noeud_valide(const NoeudAVL *noeud);

bool est_equilibre(const NoeudAVL *racine);
void maj_hauteur(NoeudAVL *noeud);
int obtenir_balance(const NoeudAVL *noeud);

int obtenir_hauteur(const NoeudAVL *noeud);
int obtenir_equilibre(const NoeudAVL *noeud);
NoeudAVL* rotation_droite(NoeudAVL *noeud);
NoeudAVL* rotation_gauche(NoeudAVL *noeud);

NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, int capacite);

int traiter_ligne(char* ligne, NoeudAVL** racine, Parametres* params);
void generer_nom_fichier(char* buffer, size_t taille, const Parametres* params);

int ecrire_fichier_sortie(const char* nom_fichier, const NoeudAVL* racine, 
                         const char* type_station, const char* type_consommation);
int ecrire_fichier_minmax(const char* nom_fichier, const NoeudAVL* racine);
void avl_vers_tableau(const NoeudAVL* racine, DonneesSortie** tableau, 
                     size_t* taille, size_t* capacite);
void trier_par_capacite(DonneesSortie* tableau, size_t taille);
void trier_par_difference_absolue(DonneesSortie* tableau, size_t taille);

#endif // AVL_H
