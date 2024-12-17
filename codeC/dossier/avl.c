// Fichier : avl.c
// Description : Implémentation des fonctions de gestion de l'arbre AVL
// pour le stockage et la manipulation des données des stations

#include "avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Fonction pour obtenir la valeur maximale entre deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fonction pour obtenir la valeur minimale entre deux entiers
int min(int a, int b) {
    return (a < b) ? a : b;
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
    if (!est_noeud_valide(racine) || !est_noeud_valide(racine->gauche)) {
        return racine;
    }
    
    NoeudAVL *pivot = racine->gauche;
    NoeudAVL *sous_arbre = pivot->droite;

    // Effectue la rotation
    pivot->droite = racine;
    racine->gauche = sous_arbre;

    // Met à jour les hauteurs dans le bon ordre (de bas en haut)
    racine->hauteur = max(obtenir_hauteur(racine->gauche), obtenir_hauteur(racine->droite)) + 1;
    pivot->hauteur = max(obtenir_hauteur(pivot->gauche), racine->hauteur) + 1;

    return pivot;
}

// Effectue une rotation à gauche sur un nœud
NoeudAVL* rotation_gauche(NoeudAVL *racine) {
    if (!est_noeud_valide(racine) || !est_noeud_valide(racine->droite)) {
        return racine;
    }
    
    NoeudAVL *pivot = racine->droite;
    NoeudAVL *sous_arbre = pivot->gauche;

    // Effectue la rotation
    pivot->gauche = racine;
    racine->droite = sous_arbre;

    // Met à jour les hauteurs dans le bon ordre (de bas en haut)
    racine->hauteur = max(obtenir_hauteur(racine->gauche), obtenir_hauteur(racine->droite)) + 1;
    pivot->hauteur = max(obtenir_hauteur(pivot->gauche), racine->hauteur) + 1;

    return pivot;
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
        // Mise à jour de la capacité si le nœud existe déjà
        racine->capacite = capacite;
        return racine;
    }

    // Met à jour la hauteur du nœud courant
    racine->hauteur = 1 + max(obtenir_hauteur(racine->gauche), obtenir_hauteur(racine->droite));

    // Calcule le facteur d'équilibre
    int equilibre = obtenir_equilibre(racine);

    // Vérifie les cas de déséquilibre et effectue les rotations nécessaires
    
    // Cas Gauche-Gauche : rotation simple à droite
    if (equilibre > 1 && id_station < racine->gauche->id_station) {
        return rotation_droite(racine);
    }
    
    // Cas Droite-Droite : rotation simple à gauche
    if (equilibre < -1 && id_station > racine->droite->id_station) {
        return rotation_gauche(racine);
    }
    
    // Cas Gauche-Droite : double rotation (gauche puis droite)
    if (equilibre > 1 && id_station > racine->gauche->id_station) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }
    
    // Cas Droite-Gauche : double rotation (droite puis gauche)
    if (equilibre < -1 && id_station < racine->droite->id_station) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }

    return racine;
}


NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station) {
    // Cas de base : racine nulle ou nœud trouvé
    if (!est_noeud_valide(racine) || racine->id_station == id_station) {
        return racine;
    }
    
    // Recherche récursive dans le sous-arbre approprié
    if (id_station < racine->id_station) {
        return chercher_noeud(racine->gauche, id_station);
    }
    return chercher_noeud(racine->droite, id_station);
}
