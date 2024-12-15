#include <stdio.h>
#include <stdlib.h>
#include "../include/input.h"
#include "../include/output.h"
#include "../include/avl.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fichier_csv> <type_station> <type_consommateur> [id_centrale]\n", argv[0]);
        return 1;
    }

    Parametres params;
    params.type_station = (TypeStation)atoi(argv[2]);
    params.type_consommateur = (TypeConsommateur)atoi(argv[3]);
    params.id_centrale = argc > 4 ? atoi(argv[4]) : -1;

    if (params.type_station > STATION_LV || params.type_consommateur > CONSOMMATEUR_TOUS) {
        fprintf(stderr, "Erreur : Valeurs de type invalides\n");
        return 1;
    }

    NoeudAVL* racine = NULL;
    if (traiter_fichier_entree(argv[1], &racine, &params) != 0) {
        fprintf(stderr, "Erreur lors du traitement du fichier\n");
        return 1;
    }

    ecrire_donnees(stdout, racine);
    liberer_avl(racine);
    return 0;
}
