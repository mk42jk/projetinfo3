#ifndef AVL_H
#define AVL_H

#include <stdbool.h>

typedef struct NoeudAVL {
    int id_station;
    double capacite;
    double consommation;
    int hauteur;
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
} NoeudAVL;

NoeudAVL* creer_noeud(int id_station, double capacite);
void liberer_avl(NoeudAVL *racine);

NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station);
void maj_consommation(NoeudAVL *noeud, double consommation);
bool est_noeud_valide(const NoeudAVL *noeud);

void maj_hauteur(NoeudAVL *noeud);

int obtenir_hauteur(const NoeudAVL *noeud);
int obtenir_equilibre(const NoeudAVL *noeud);
NoeudAVL* rotation_droite(NoeudAVL *noeud);
NoeudAVL* rotation_gauche(NoeudAVL *noeud);

NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, double capacite);

#endif
