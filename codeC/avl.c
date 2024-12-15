// Fichier : avl.c
// Description : Implémentation des fonctions de gestion de l'arbre AVL
// pour le stockage et la manipulation des données des stations

#include "../include/avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Fonction pour obtenir la valeur maximale entre deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fonction pour obtenir la valeur maximale entre trois entiers
int max3(int a, int b, int c) {
    return max(max(a, b), c);
}

// Fonction pour obtenir la valeur minimale entre deux entiers
int min(int a, int b) {
    return (a < b) ? a : b;
}

// Fonction pour obtenir la valeur minimale entre trois entiers
int min3(int a, int b, int c) {
    return min(min(a, b), c);
}

// Vérifie si un nœud est valide
bool est_noeud_valide(const NoeudAVL *noeud) {
    return noeud != NULL;
}

// Crée un nouveau nœud AVL avec les données spécifiées
NoeudAVL* creer_noeud(int id_station, double capacite) {
    NoeudAVL* nouveau_noeud = (NoeudAVL*)malloc(sizeof(NoeudAVL));
    
    if (nouveau_noeud != NULL) {
        nouveau_noeud->id_station = id_station;
        nouveau_noeud->capacite = capacite;
        nouveau_noeud->consommation = 0.0;
        nouveau_noeud->hauteur = 1;
        nouveau_noeud->gauche = NULL;
        nouveau_noeud->droite = NULL;
    }
    return nouveau_noeud;
}

// Libère la mémoire allouée pour un arbre AVL
void liberer_avl(NoeudAVL *racine) {
    if (!est_noeud_valide(racine)) {
        return;
    }
    liberer_avl(racine->gauche);
    liberer_avl(racine->droite);
    free(racine);
}

// Obtient la hauteur d'un nœud
int obtenir_hauteur(const NoeudAVL *noeud) {
    if (est_noeud_valide(noeud)) {
        return noeud->hauteur;
    }
    return 0;
}

// Obtient l'équilibre d'un nœud
int obtenir_equilibre(const NoeudAVL *noeud) {
    if (est_noeud_valide(noeud)) {
        return obtenir_hauteur(noeud->gauche) - obtenir_hauteur(noeud->droite);
    }
    return 0;
}

// Effectue une rotation à droite sur un nœud
NoeudAVL* rotation_droite(NoeudAVL *racine) {
    NoeudAVL *pivot;
    int eq_a, eq_p;

    pivot = racine->gauche;
    racine->gauche = pivot->droite;
    pivot->droite = racine;
    
    eq_a = obtenir_equilibre(racine);
    eq_p = obtenir_equilibre(pivot);
    racine->hauteur = max3(obtenir_hauteur(racine->gauche), obtenir_hauteur(racine->droite), 0) + 1;
    pivot->hauteur = max3(obtenir_hauteur(pivot->gauche), racine->hauteur, 0) + 1;
    
    return pivot;
}

// Effectue une rotation à gauche sur un nœud
NoeudAVL* rotation_gauche(NoeudAVL *racine) {
    NoeudAVL *pivot;
    int eq_a, eq_p;

    pivot = racine->droite;
    racine->droite = pivot->gauche;
    pivot->gauche = racine;
    
    eq_a = obtenir_equilibre(racine);
    eq_p = obtenir_equilibre(pivot);
    racine->hauteur = max3(obtenir_hauteur(racine->gauche), obtenir_hauteur(racine->droite), 0) + 1;
    pivot->hauteur = max3(obtenir_hauteur(pivot->droite), racine->hauteur, 0) + 1;
    
    return pivot;
}

// Met à jour la hauteur d'un nœud
void maj_hauteur(NoeudAVL *noeud) {
    if (!est_noeud_valide(noeud)) {
        return;
    }
    noeud->hauteur = max(obtenir_hauteur(noeud->gauche), obtenir_hauteur(noeud->droite)) + 1;
}

// Met à jour la consommation d'un nœud
void maj_consommation(NoeudAVL *noeud, double consommation) {
    if (est_noeud_valide(noeud)) {
        noeud->consommation += consommation;
    }
}

// Insère un nouveau nœud dans l'arbre AVL
NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, double capacite) {
    // Cas de base : arbre vide
    if (!est_noeud_valide(racine)) {
        return creer_noeud(id_station, capacite);
    }

    // Insertion récursive selon l'id de la station
    if (id_station < racine->id_station) {
        racine->gauche = inserer_noeud(racine->gauche, id_station, capacite);
    } else if (id_station > racine->id_station) {
        racine->droite = inserer_noeud(racine->droite, id_station, capacite);
    } else {
        return racine; // Doublon : on ne fait rien
    }

    // Mise à jour de la hauteur
    maj_hauteur(racine);

    // Rééquilibrage si nécessaire
    int equilibre = obtenir_equilibre(racine);
    
    // Cas de déséquilibre gauche-gauche
    if (equilibre > 1 && id_station < racine->gauche->id_station) {
        return rotation_droite(racine);
    }
    
    // Cas de déséquilibre droite-droite
    if (equilibre < -1 && id_station > racine->droite->id_station) {
        return rotation_gauche(racine);
    }
    
    // Cas de déséquilibre gauche-droite
    if (equilibre > 1 && id_station > racine->gauche->id_station) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }
    
    // Cas de déséquilibre droite-gauche
    if (equilibre < -1 && id_station < racine->droite->id_station) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }

    return racine;
}

// Recherche un nœud dans l'arbre AVL
NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station) {
    if (!est_noeud_valide(racine) || racine->id_station == id_station) {
        return racine;
    }
    
    if (id_station < racine->id_station) {
        return chercher_noeud(racine->gauche, id_station);
    }
    return chercher_noeud(racine->droite, id_station);
}