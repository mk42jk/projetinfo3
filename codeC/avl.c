#include "avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int max3(int a, int b, int c) {
    return max(max(a, b), c);
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int min3(int a, int b, int c) {
    return min(min(a, b), c);
}

bool est_noeud_valide(const NoeudAVL *noeud) {
    return noeud != NULL;
}

NoeudAVL* creer_noeud(int id_station, double capacite) {
    NoeudAVL* nouveau_noeud = (NoeudAVL*)malloc(sizeof(NoeudAVL));
    
    if (nouveau_noeud == NULL) {
        printf("Erreur : Échec de l'allocation mémoire pour un nouveau nœud\n");
        return NULL;
    }
    
    nouveau_noeud->id_station = id_station;
    nouveau_noeud->capacite = capacite;
    nouveau_noeud->consommation = 0.0;
    nouveau_noeud->hauteur = 1;
    
    nouveau_noeud->gauche = NULL;
    nouveau_noeud->droite = NULL;
    
    return nouveau_noeud;
}

void liberer_avl(NoeudAVL *racine) {
    if (!est_noeud_valide(racine)) {
        return;
    }
    
    liberer_avl(racine->gauche);
    liberer_avl(racine->droite);
    
    free(racine);
}

int obtenir_hauteur(const NoeudAVL *noeud) {
    if (est_noeud_valide(noeud)) {
        return noeud->hauteur;
    }
    return 0;
}

int obtenir_equilibre(const NoeudAVL *noeud) {
    if (est_noeud_valide(noeud)) {
        return obtenir_hauteur(noeud->gauche) - obtenir_hauteur(noeud->droite);
    }
    return 0;
}

NoeudAVL* rotation_droite(NoeudAVL *racine) {
    NoeudAVL *pivot;
    int eq_a, eq_p;
    
    pivot = racine->gauche;
    racine->gauche = pivot->droite;
    pivot->droite = racine;
    
    eq_a = obtenir_equilibre(racine);
    eq_p = obtenir_equilibre(pivot);
    
    racine->hauteur = eq_a - min(eq_p, 0) + 1;
    pivot->hauteur = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);
    
    return pivot;
}

NoeudAVL* rotation_gauche(NoeudAVL *racine) {
    NoeudAVL *pivot;
    int eq_a, eq_p;
    
    pivot = racine->droite;
    racine->droite = pivot->gauche;
    pivot->gauche = racine;
    
    eq_a = obtenir_equilibre(racine);
    eq_p = obtenir_equilibre(pivot);
    
    racine->hauteur = eq_a - max(eq_p, 0) - 1;
    pivot->hauteur = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);
    
    return pivot;
}

NoeudAVL* inserer_noeud(NoeudAVL *racine, int id_station, double capacite) {
    if (!est_noeud_valide(racine)) {
        return creer_noeud(id_station, capacite);
    }

    if (id_station < racine->id_station) {
        racine->gauche = inserer_noeud(racine->gauche, id_station, capacite);
    } else if (id_station > racine->id_station) {
        racine->droite = inserer_noeud(racine->droite, id_station, capacite);
    } else {
        return racine;
    }

    racine->hauteur = 1 + max(obtenir_hauteur(racine->gauche),obtenir_hauteur(racine->droite));

    int equilibre = obtenir_equilibre(racine);

    // Cas gauche-gauche
    if (equilibre > 1 && id_station < racine->gauche->id_station) {
        return rotation_droite(racine);
    }

    // Cas droite-droite
    if (equilibre < -1 && id_station > racine->droite->id_station) {
        return rotation_gauche(racine);
    }

    // Cas gauche-droite
    if (equilibre > 1 && id_station > racine->gauche->id_station) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }

    // Cas droite-gauche
    if (equilibre < -1 && id_station < racine->droite->id_station) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }

    return racine;
}

NoeudAVL* chercher_noeud(NoeudAVL *racine, int id_station) {
    if (!est_noeud_valide(racine) || racine->id_station == id_station) {
        return racine;
    }

    if (id_station < racine->id_station) {
        return chercher_noeud(racine->gauche, id_station);
    }
    return chercher_noeud(racine->droite, id_station);
}

void maj_consommation(NoeudAVL *noeud, double consommation) {
    if (est_noeud_valide(noeud)) {
        noeud->consommation += consommation;
    }
}

void maj_hauteur(NoeudAVL *noeud) {
    if (!est_noeud_valide(noeud)) {
        return;
    }
    noeud->hauteur = max(obtenir_hauteur(noeud->gauche),
                        obtenir_hauteur(noeud->droite)) + 1;
}
