// Fichier : avl.c
// Description : Implémentation des fonctions de gestion de l'arbre AVL
// pour le stockage et la manipulation des données des stations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Fonctions utilitaires privées
static int max(int a, int b) {
    return (a > b) ? a : b;
}

int obtenir_hauteur(const NoeudAVL *noeud) {
    return noeud ? noeud->hauteur : 0;
}

int obtenir_equilibre(const NoeudAVL *noeud) {
    return noeud ? obtenir_hauteur(noeud->gauche) - obtenir_hauteur(noeud->droite) : 0;
}

void maj_hauteur(NoeudAVL *noeud) {
    if (!noeud) return;
    noeud->hauteur = 1 + max(obtenir_hauteur(noeud->gauche),
                            obtenir_hauteur(noeud->droite));
}

// Rotations pour l'équilibrage
NoeudAVL* rotation_droite(NoeudAVL *y) {
    if (!y || !y->gauche) return y;
    
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;

    x->droite = y;
    y->gauche = T2;

    maj_hauteur(y);
    maj_hauteur(x);

    return x;
}

NoeudAVL* rotation_gauche(NoeudAVL *x) {
    if (!x || !x->droite) return x;
    
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;

    y->gauche = x;
    x->droite = T2;

    maj_hauteur(x);
    maj_hauteur(y);

    return y;
}

// Crée un nouveau nœud AVL
NoeudAVL* creer_noeud(int id_station, double capacite) {
    NoeudAVL* noeud = (NoeudAVL*)malloc(sizeof(NoeudAVL));
    if (!noeud) return NULL;
    
    noeud->id_station = id_station;
    noeud->capacite = capacite;
    noeud->consommation = 0.0f;
    noeud->hauteur = 1;
    noeud->gauche = noeud->droite = NULL;
    
    return noeud;
}

// Libère la mémoire de l'arbre
void liberer_avl(NoeudAVL *racine) {
    if (!racine) return;
    liberer_avl(racine->gauche);
    liberer_avl(racine->droite);
    free(racine);
}

// Met à jour la consommation d'un nœud
void maj_consommation(NoeudAVL *noeud, double consommation) {
    if (!noeud) return;
    noeud->consommation += consommation;
}

// Insère ou met à jour un nœud dans l'arbre
NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, double capacite) {
    // Insertion normale BST
    if (!racine)
        return creer_noeud(id_station, capacite);

    // Mise à jour si le nœud existe déjà
    if (id_station == racine->id_station) {
        if (capacite > 0) {  // Ne mettre à jour que si capacite > 0
            racine->capacite = capacite;
        }
        return racine;
    }

    if (id_station < racine->id_station)
        racine->gauche = inserer_noeud(racine->gauche, id_station, capacite);
    else
        racine->droite = inserer_noeud(racine->droite, id_station, capacite);

    // Mise à jour de la hauteur
    maj_hauteur(racine);

    // Équilibrage
    int equilibre = obtenir_equilibre(racine);

    // Cas Gauche-Gauche
    if (equilibre > 1 && id_station < racine->gauche->id_station)
        return rotation_droite(racine);

    // Cas Droite-Droite
    if (equilibre < -1 && id_station > racine->droite->id_station)
        return rotation_gauche(racine);

    // Cas Gauche-Droite
    if (equilibre > 1 && id_station > racine->gauche->id_station) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }

    // Cas Droite-Gauche
    if (equilibre < -1 && id_station < racine->droite->id_station) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }

    return racine;
}

// Recherche un nœud dans l'arbre
NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station) {
    if (!racine || racine->id_station == id_station)
        return racine;

    if (id_station < racine->id_station)
        return chercher_noeud(racine->gauche, id_station);
    
    return chercher_noeud(racine->droite, id_station);
}
