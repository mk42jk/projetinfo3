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
     local fichier_donnees=$1
     local type_de_station=$2
     local type_de_consommateur=$3
     local ID_Centrale=${4:-}

     # Validation de l'argument 1 (Fichier_donnees)
     if [[ ! -f "$fichier_donnees" ]]; then
        echo "Erreur: Le fichier de donnes '$fichier_donnees' n'existe pas, n'est pas un fichier, ou n'est pas accessible."
        exit 1 #code d'erreur 1 pour la fonction validation_arguments
    fi

     # Validation de l'argument 2 (Type_de_station)
    case "$type_de_station" in 
         hvb|hva|lv)
            ;;
        *)
            echo "Erreur: Station Invalide '$type_de_station'. veuillez écrire 'hvb', 'hva', ou 'lv'."
            exit 1 #code d'erreur 1 pour la fonction validation_arguments
            ;;
    esac

     # Validation de l'argument 3 (Type_de_consommateur)
    case "$type_de_consommateur" in
    comp|indiv|all)
        # Vérifie s'il y a eu des combinaisons mal faite
        # hvb_all ou hvb_indiv pas possible
        if [[ "$type_de_station" == "hvb" && ( "$type_de_consommateur" == "all" || "$type_de_consommateur" == "indiv" ) ]]; then
            echo "Erreur : Le type de consommateur '$type_de_consommateur' n'est pas autorisé pour le type de station 'hvb'."
            exit 1 #code d'erreur 1 pour la fonction validation_arguments
        fi
        #hva_all ou hva_indiv pas possible également
        if [[ "$type_de_station" == "hva" && ( "$type_de_consommateur" == "all" || "$type_de_consommateur" == "indiv" ) ]]; then
            echo "Erreur : Le type de consommateur '$type_de_consommateur' n'est pas autorisé pour le type de station 'hva'."
            exit 1 #code d'erreur 1 pour la fonction validation_arguments
        fi
        ;;
    *)# pour le cas ou l'argument est mal écris 
        echo "Erreur : Type de consommateur invalide '$type_de_consommateur'. Il doit être 'comp', 'indiv', ou 'all'."
        exit 1 #code d'erreur 1 pour la fonction validation_arguments
        ;;
esac

}


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


main() {
    validation_arguments "$@" #On vérifie d'abord les arguments avant de lancer le script
    echo "Tout les arguments sont valide! Lançons le script !"
    #Reste de la fonction à finir
}

#Point d'entrée dans le script 
main "$@"


    
