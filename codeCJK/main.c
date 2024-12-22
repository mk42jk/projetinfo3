// Fichier : main.c
// Description : Point d'entrée du programme de traitement des données

#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "output.h"
#include "avl.h"

int main(void) {
    NoeudAVL* racine = NULL;
    int resultat;

    // Lecture et traitement des données depuis stdin
    resultat = traiter_entree(&racine);
    if (resultat != SUCCES) {
        fprintf(stderr, "Erreur lors du traitement des données\n");
        liberer_avl(racine);
        return resultat;
    }

    // Écriture des résultats sur stdout
    ecrire_donnees(stdout, racine);

    // Libération de la mémoire
    liberer_avl(racine);
    return SUCCES;
}
