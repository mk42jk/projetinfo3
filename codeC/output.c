#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/output.h"

void ecrire_donnees(FILE* f, const NoeudAVL* racine) {
    if (!f || !racine) return;

    ecrire_donnees(f, racine->gauche);
    fprintf(f, "%d:%.0f:%.0f\n", 
            racine->id_station,
            racine->capacite,
            racine->consommation);
    ecrire_donnees(f, racine->droite);
}