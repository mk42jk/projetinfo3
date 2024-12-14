#!/bin/bash

#Fonction pour l'affichage de l'aide
Affiche_aide(){
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

#Fonctions pour vérifier les arguments 
validation_arguments() {
     #Si -h est dans les arguments
     for arg in "$@"; do
          if [[ "$arg" == "-h" ]]; then
               Affiche_aide
          fi
     done

     #Vérification du nombre d'arguments (4 minimum)
     if [[ $# -lt 3 ]]; then #Nombre d'Arguments strictement inférieurs à 3
          echo "Erreur: Nombre d'arguments insuffisants."
          affiche_aide
     fi

     #Initialisation du nom des arguments
     local Fichier_donnees










temps_execution () {
    #Enregistre le temps au début de l'exécution
    local debut_temps=$(date +%s)

    #Enregistre le temps de fin d'exécution
    local fin_temps=$(date +%s)

    #Calcul le temps total 
    local execution_temps=$((fin_temps-debut_temps))

    #Affiche le temps
    echo "Temps d'exécution total : $execution_temps secondes."
    exit 0
}


    
