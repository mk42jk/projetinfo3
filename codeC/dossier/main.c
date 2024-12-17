// Fichier : main.c
// Description : Programme principal de gestion du réseau électrique
// Lit les données d'entrée, les traite et génère un rapport de sortie

#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "output.h"
#include "avl.h"

int main(int argc, char* argv[]) {
    // Vérification des arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier_donnees_filtrees>\n", argv[0]);
        return 1;
    }

    // Initialisation de l'arbre AVL
    NoeudAVL* racine = NULL;

    // Traitement du fichier d'entrée
    int resultat = traiter_fichier_entree(argv[1], &racine);
    if (resultat != SUCCES) {
        fprintf(stderr, "Erreur lors du traitement du fichier\n");
        return resultat;
    }

    // Écriture des résultats sur la sortie standard
    ecrire_donnees(stdout, racine);

    // Libération de la mémoire
    liberer_avl(racine);
    return SUCCES;
}
