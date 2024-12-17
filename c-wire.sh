#!/bin/bash

#Fonction pour l'affichage de l'aide
affiche_aide(){
     echo "Fonction d'aide exécutée."
     echo "L'utilisation: $0 <Fichier_Données> <Type_de_station> <type_de_consommateur> [ID_Centrale] [-h]"
     echo "Options:" #une ligne pour expliquer chaque argument 
     echo "Fichier_Données: Chemin vers le ficher des données CSV (Obligatoire)"
     echo "Type_de_station: Type de station (hvb, hva, lv) (Obligatoire)"
     echo "type_de_consommateur: Type de consommateur (comp, indiv, all) (Obligatoire)"
     echo "ID_Centrale: Identifiant de la centrale (Optionnelle)"
     echo "  -h   Affichage de l'aide et quitter la page."
     exit 0 # 0 signifie succès
}

#Fonctions pour vérifier les arguments 
validation_arguments() {
     #Si -h est dans les arguments
     for arg in "$@"; do
          if [[ "$arg" == "-h" ]]; then
               affiche_aide
          fi
     done

     #Vérification du nombre d'arguments (4 minimum, 6 maximum)
     if [[ $# -lt 3 ]]; then #Nombre d'Arguments strictement inférieurs à 3
          echo "Erreur: Nombre d'arguments insuffisants. (minimum 3 requis)"
          affiche_aide
     fi

     if [[ $# -gt 6 ]]; then
          echo "Erreur: Nombre d'arguments trop élevées. (maximum 6 autorisés)."
          affiche_aide
     fi
     
     #Initialisation du nom des arguments
     local fichier_donnees=$1
     local type_de_station=$2
     local type_de_consommateur=$3
     local ID_Centrale=${4:-}

     # Validation de l'argument 1 (Fichier_donnees)
     if [[ ! -f "$fichier_donnees" ]]; then
        echo "Erreur: Le fichier de donnees '$fichier_donnees' n'existe pas, n'est pas un fichier, ou n'est pas accessible."
        affiche_aide
        exit 2 #code d'erreur 2 pour les fichier introuvable
    fi

     # Validation de l'argument 2 (Type_de_station)
    case "$type_de_station" in 
         hvb|hva|lv)
            ;;
        *)
            echo "Erreur: Station Invalide '$type_de_station'. Veuillez écrire 'hvb', 'hva', ou 'lv'."
            affiche_aide
            exit 3 #Argument manquant ou invalide
            ;;
    esac

     # Validation de l'argument 3 (Type_de_consommateur)
    case "$type_de_consommateur" in
    comp|indiv|all)
        # Vérifie s'il y a eu des combinaisons mal faite
        # hvb_all ou hvb_indiv pas possible
        if [[ "$type_de_station" == "hvb" && ( "$type_de_consommateur" == "all" || "$type_de_consommateur" == "indiv" ) ]]; then
            echo "Erreur : Le type de consommateur 'all ou indiv' n'est pas autorisé pour le type de station 'hvb'."
            affiche_aide
            exit 3 #Argument invalide
        fi
        #hva_all ou hva_indiv pas possible également
        if [[ "$type_de_station" == "hva" && ( "$type_consommateur" == "all" || "$type_de_consommateur" == "indiv" ) ]]; then
            echo "Erreur : Le type de consommateur 'all ou indiv' n'est pas autorisé pour le type de station 'hva'."
            affiche_aide
            exit 3 #Argument invalide
        fi
        ;;
    *) # pour le cas ou l'argument est mal écris 
        echo "Erreur : Type de consommateur invalide '$type_de_consommateur'. Il doit être 'comp', 'indiv', ou 'all'."
        affiche_aide
        exit 3 #Argument invalide
        ;;
esac

}

# Fonction pour vérifier et préparer les dossiers requis
preparer_dossiers() {
    # Vérification et création des dossiers
    for dossier in "tmp" "graphs"; do
        if [[ ! -d "$dossier" ]]; then
            echo "Création du dossier '$dossier'..."
            mkdir -p "$dossier"
            if [[ $? -ne 0 ]]; then
                echo "Erreur : Impossible de créer le dossier '$dossier'."
                exit 4 # Code 4 : Erreur de permission pour la création de dossier
            fi
        else
            if [[ "$dossier" == "tmp" ]]; then
                echo "Nettoyage du dossier '$dossier'..."
                rm -rf "$dossier"/*
                if [[ $? -ne 0 ]]; then
                    echo "Erreur : Impossible de nettoyer le dossier '$dossier'."
                    exit 4 # Code 4 : Erreur de permissions pour le nettoyage du dossier
                fi 
            fi
        fi
    done
    echo "Les dossiers requis sont prêts."
}

# Vérifie et compile le programme C si besoin
verifier_executable_c() {
    local executable="codeC/c-wire"  # Chemin de l'exécutable
    local source="codeC/main.c"      # Chemin du fichier source

    if [[ ! -f "$executable" || "$executable" -ot "$source" ]]; then
        echo "Compilation de l'exécutable C..."
        pushd "codeC" > /dev/null
        make
        if [[ $? -ne 0 ]]; then
            echo "Erreur : Échec de la compilation du programme C."
            exit 5 # Code 5 : Erreur de compilation
        fi
        popd > /dev/null
    fi
    echo "Exécutable C vérifié et prêt."
}

#Fonction pour vérifier la présence de gnuplot et l'installer le cas échéant
 verifier_installer_gnuplot() {
    if ! command -v gnuplot &> /dev/null; then
        echo "Gnuplot n'est pas installé. Téléchargement..."

        # Vérifier le système d'exploitation et installer gnuplot en conséquence
        if [ -x "$(command -v apt-get)" ]; then
        
            sudo apt-get update
            sudo apt-get install -y gnuplot

        else
            echo "Installation manuelle requise."
        fi
    else
        echo "Gnuplot est déjà installé."
    fi
}

#fonction pour créer le fichier temporaire des données filtrer en fonction des cas
filtrer_donnees() {

    local fichier_donnees="$1"
    local type_station="$2"
    local type_consommateur="$3"
    local ID_centrale="${4:-}"
    local fichier_filtre="tmp/filtered_data.csv"

 # Validation de l'argument 1 (Fichier_donnees) (juste au cas ou on le remet ici)
     if [[ ! -f "$fichier_donnees" ]]; then
        echo "Erreur: Le fichier de donnees '$fichier_donnees' n'existe pas, n'est pas un fichier, ou n'est pas accessible."
        affiche_aide
        exit 2 #code d'erreur 2 pour les fichier introuvable
    fi

    #PENSER A RETIRER LE HEAD -N 1 Pour le c

    echo "Filtrage des données pour :"
    echo "  Type de station       : $type_station"
    echo "  Type de consommateur  : $type_consommateur"
    echo "  ID de centrale        : ${ID_centrale:-Aucun}"

    # Colonnes de filtrage
    local colonne_station colonne_consommateur
    case "$type_station" in
        hvb) colonne_station=2 ;;
        hva) colonne_station=3 ;;
        lv)  colonne_station=4 ;;
        *) 
            echo "Erreur : Type de station inconnu '$type_station'."
            exit 3
            ;;
    esac

    case "$type_consommateur" in
        comp) colonne_consommateur=5 ;;
        indiv) colonne_consommateur=6 ;;
        all) colonne_consommateur="5,6" ;; # Toutes les colonnes consommateurs
        *)
            echo "Erreur : Type de consommateur inconnu '$type_consommateur'."
            exit 3
            ;;
    esac
# A refaire ce soir au cas par cas plutot que un switch case 
    #  commande de filtrage
    local filtre=""

    # Filtrer par type de station
    filtre+="\$$colonne_station != \"-\""

    # Filtrer par ID de centrale (si il est fourni)
    if [[ -n "$ID_centrale" ]]; then
        filtre+=" && \$1 == \"$ID_centrale\""
    fi

    # Filtrer par type de consommateur
    if [[ "$type_consommateur" != "all" ]]; then
        filtre+=" && \$$colonne_consommateur != \"-\""
    fi

    # Exécution du filtrage avec awk
    awk -F';' "BEGIN {OFS=\":\"} $filtre {print \$colonne_station, \$7, \$8}" "$fichier_donnees" > "$fichier_filtre"

    if [[ $? -ne 0 ]]; then
        echo "Erreur : Filtrage des données échoué."
        exit 6
    fi

    echo "Données filtrées enregistrées dans '$fichier_filtre'."


# cette partie du code n'a pas sa place ici, à changer et le mettre dans traitement principal ou une autre fonction fichier_sortie

     # Tri des données par capacité croissante et génération du fichier de sortie
    local fichier_sortie="tmp/${type_station}_${type_consommateur}.csv"
    echo "Tri des données par capacité croissante et création du fichier de sortie '$fichier_sortie'..."

    # Créer le fichier avec les colonnes et les données triées par capacité croissante
    sort -t: -k2,2n "$fichier_filtre" > "$fichier_sortie"

    if [[ $? -ne 0 ]]; then
        echo "Erreur : Échec du tri ou de la création du fichier de sortie."
        exit 7
    fi

    echo "Fichier généré et trié avec succès : '$fichier_sortie'."
}

# fin de la partie mal située

# Fonction pour exécuter le traitement principal
traitement_principal() {
    local fichier_donnees=$1
    local type_de_station=$2
    local type_de_consommateur=$3
    local ID_centrale=${4:-}

    echo "Lancement du traitement principal..."
    
      # Filtrage des données avant d'exécuter le programme C
    filtrer_donnees "$fichier_donnees" "$type_de_station" "$type_de_consommateur" "$ID_centrale"

     verifier_executable_c

    # on prépare des paramètres pour le programme C
    local fichier_filtre="tmp/filtered_data.csv"
    local params="$fichier_filtre $type_de_station $type_de_consommateur"
    if [[ -n "$ID_centrale" ]]; then
        params="$params $ID_centrale"
    fi

    # Exécution de l'exécutable C
      echo "Exécution du programme C pour le calcul des consommations..."
    ./codeC/c-wire $params
    if [[ $? -ne 0 ]]; then
        echo "Erreur : Le programme C a rencontré une erreur."
        exit 6 # Code 6 : Erreur lors de l'exécution du programme C
    fi

#Voir avec jibril pour faire le fichier de sortie en fonction des cas
# surement refaire une autre fonction

#gnuplot graphique ("$fichier" temporaire car pas de fichier sommé)

     
        local data_file="tests/all_min_max.csv"
        local plot_file="graphs/graph.png"
        
        
        set terminal png size 1100,800
        set output ‘graph.png’
        set title « Les 10 Postes LV les plus chargés et les moins chargés"
        set xlabel "Postes LV"
        set ylabel "Consommation (kWh)"
        set boxwidth 0.8
        set style fill solid
        set grid ytics

        set datafile separator ';'

      
        set style line 1 lc rgb "red"
        set style line 2 lc rgb "green"


        gnuplot -e " plot '${data_file}' using 2:xtic(1) title "Consommation en marge" with histograms ls 2 'green' every ::0::9, \
             '' using 3:xtic(1) title "Consommation en surproduction" with histograms ls 1 'red' every ::10::19"
}

    echo "Traitement principal terminé avec succès."
}

main() {
     debut_temps=$(date +%s)
    validation_arguments "$@" # On vérifie d'abord les arguments avant de lancer le script
    preparer_dossiers         # Préparation des dossiers requis
    verifier_installer_gnuplot
    traitement_principal "$@" # Lancement du traitement principal
    echo "Script terminé avec succès !"
     fin_temps=$(date +%s)
     execution_temps=$((fin_temps - debut_temps))
     echo "Temps d'exécution du script shell : $execution_temps secondes."
}

#Point d'entrée dans le script 
main "$@"



    
