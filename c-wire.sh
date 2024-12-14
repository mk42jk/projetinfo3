#!/bin/bash

#Fonction pour l'affichage de l'aide
print_help(){
     echo "Fonction d'aide exécutée."
     echo "L'utilisation: $0 <Fichier_Données> <Type_de_station> <type_de_consommateur> [ID_Centrale] [-h]"
     echo "Options:" #une ligne pour expliquer chaque argument 
     echo "Fichier_Données: Chemin vers le ficher des données CSV (Obligatoire)"
     echo "Type_de_station: Type de station (hvb, hva, lv) (Obligatoire)"
     echo "type_de_consommateur: Type de consommateur (comp, indiv, all) (Obligatoire)"
     echo "ID_Centrale: Identifiant de la centrale (Optionnelle)"
     echo "  -h   Affichage de l'aide et quitter la page."
     exit 0
}

temps_execution () {
    #Enregistre le temps au début de l'exécution
    local start_time=$(date +%s)

    #Enregistre le temps de fin d'exécution
    local end_time=$(date +%s)

    #Calcul le temps total 
    local execution_time=$((end_time-start_time))

    #Affiche le temps
    echo "Temps d'exécution total : $execution_time secondes."
    exit 0
}


    
