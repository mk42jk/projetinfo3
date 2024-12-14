#include <stdio.h>
#include "../include/avl.h"
#include "../include/input.h"
#include "../include/output.h"

#define ERREUR_ECRITURE 5

int main(int argc, char* argv[]) {
    Parametres params = {STATION_HVB, '\0', -1};
    int ret = analyser_arguments(argc, argv, &params);
    if (ret != 0) return ret;

    NoeudAVL* racine = NULL;
    ret = traiter_fichier_entree(argv[1], &racine, &params);
    if (ret != 0) {
        liberer_avl(racine);
        return ret;
    }

    char nom_fichier[256];
    generer_nom_fichier(nom_fichier, sizeof(nom_fichier), &params);

    const char* type_station = obtenir_type_station_str(params.type_station);
    const char* type_cons = obtenir_type_consommateur_str(params.type_consommateur);

    if (ecrire_fichier_sortie(nom_fichier, racine, type_station, type_cons) != 0) {
        liberer_avl(racine);
        return ERREUR_ECRITURE;
    }

    if (params.type_station == STATION_LV && params.type_consommateur == 't') {
        char nom_minmax[256];
        snprintf(nom_minmax, sizeof(nom_minmax), "lv_all_minmax.csv");
        if (ecrire_fichier_minmax(nom_minmax, racine) != 0) {
            liberer_avl(racine);
            return ERREUR_ECRITURE;
        }
    }

    liberer_avl(racine);
    return 0;
}
