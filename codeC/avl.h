// Fichier : avl.h
// Description : Définition de la structure et des fonctions pour l'arbre AVL
// permettant la gestion des stations électriques

#ifndef AVL_H
#define AVL_H

#include <stdbool.h>

// Structure d'un nœud de l'arbre AVL
typedef struct NoeudAVL {
    int id_station;        // Identifiant unique de la station
    float capacite;       // Capacité de production de la station
    float consommation;   // Consommation actuelle de la station
    int hauteur;          // Hauteur du nœud pour l'équilibrage
    struct NoeudAVL *gauche;  // Sous-arbre gauche
    struct NoeudAVL *droite;  // Sous-arbre droit
} NoeudAVL;

// Fonctions de gestion des nœuds
NoeudAVL* creer_noeud(int id_station, double capacite);
void liberer_avl(NoeudAVL *racine);

// Fonctions de recherche et mise à jour
NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station);
void maj_consommation(NoeudAVL *noeud, double consommation);
bool est_noeud_valide(const NoeudAVL *noeud);

// Fonctions de gestion de l'équilibrage
void maj_hauteur(NoeudAVL *noeud);

// Fonctions d'accès aux informations
int obtenir_hauteur(const NoeudAVL *noeud);
int obtenir_equilibre(const NoeudAVL *noeud);

// Fonctions de rotation
NoeudAVL* rotation_droite(NoeudAVL *noeud);
NoeudAVL* rotation_gauche(NoeudAVL *noeud);

// Fonction d'insertion
NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, double capacite);

#endif
