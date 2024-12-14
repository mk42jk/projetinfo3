#!/bin/bash

print_help(){
     echo "Fonction d'aide exécutée."
     echo "Options:" #une ligne pour chaque fonction en c
     echo "  -h   Affichage de l'aide et quitter la page."
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
    
}


    
