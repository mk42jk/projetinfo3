// Fichier : main.c
// Description : Programme principal de gestion du réseau électrique
// Lit les données d'entrée, les traite et génère un rapport de sortie

#include <stdio.h>
#include <stdlib.h>
#include "../include/input.h"
#include "../include/output.h"
#include "../include/avl.h"

int main(int argc, char* argv[]) {
    // Vérification des arguments de la ligne de commande
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fichier_csv> <type_station> <type_consommateur> [id_centrale]\n", argv[0]);
        return 1;
    }

    // Initialisation des paramètres de traitement
    Parametres params;
    params.type_station = (TypeStation)atoi(argv[2]);
    params.type_consommateur = (TypeConsommateur)atoi(argv[3]);
    params.id_centrale = argc > 4 ? atoi(argv[4]) : -1;

    if (params.type_station > STATION_LV || params.type_consommateur > CONSOMMATEUR_TOUS) {
        fprintf(stderr, "Erreur : Valeurs de type invalides\n");
        return 1;
    }

    // Création de l'arbre AVL vide
    NoeudAVL* racine = NULL;

    // Traitement du fichier d'entrée et construction de l'arbre
    if (traiter_fichier_entree(argv[1], &racine, &params) != 0) {
        fprintf(stderr, "Erreur lors du traitement du fichier\n");
        return 1;
    }

    // Génération du rapport de sortie
    ecrire_donnees(stdout, racine);

    // Libération de la mémoire
    liberer_avl(racine);
    return 0;
}
