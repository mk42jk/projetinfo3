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

verifier_installer_gnuplot() {
    if ! command -v gnuplot &> /dev/null; then
        echo "Erreur : Gnuplot n'est pas installé. Veuillez l'installer avant d'exécuter ce script."
        exit 7 # Code d'erreur 7 pour absence de dépendance
    fi
}


#fonction pour créer le fichier temporaire des données filtrer en fonction des cas
filtrer_donnees() {

    local fichier_donnees="$1"
    local type_station="$2"
    local type_consommateur="$3"
    local id_centrale="${4:-}"
    local fichier_filtre="tmp/filtered_data.csv"

 # Validation de l'argument 1 (Fichier_donnees) (juste au cas ou on le remet ici)
     if [[ ! -f "$fichier_donnees" ]]; then
        echo "Erreur: Le fichier de donnees '$fichier_donnees' n'existe pas, n'est pas un fichier, ou n'est pas accessible."
        affiche_aide
        exit 2 #code d'erreur 2 pour les fichier introuvable
    fi


    echo "Filtrage des données pour :"
    echo "  Type de station       : $type_station"
    echo "  Type de consommateur  : $type_consommateur"
    echo "  ID de centrale        : ${id_centrale:-Aucun}"

if [[ "$type_station" == "hvb" ]]; then
        if [[ "$type_consommateur" == "comp" ]]; then
            echo "Filtrage des données pour HVB avec consommateurs : 'comp'."
            if [[ -z "$id_centrale" ]]; then
                awk -F';' '$2 != "-" && $7 != "-" && $4 == "-" {
                    print $2 ";" $7 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            else
                awk -F';' -v id="$id_centrale" '$1 == id && $2 != "-" && $7 != "-" && $4 == "-" {
                    print $2 ";" $7 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            fi
        else
            echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'hvb'."
            exit 3
        fi

    elif [[ "$type_station" == "hva" ]]; then
        if [[ "$type_consommateur" == "comp" ]]; then
            echo "Filtrage des données pour HVA avec consommateurs : 'comp'."
            if [[ -z "$id_centrale" ]]; then
                awk -F';' '$3 != "-" && $7 != "-" && $4 == "-" {
                    print $3 ";" $7 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            else
                awk -F';' -v id="$id_centrale" '$1 == id && $3 != "-" && $7 != "-" && $4 == "-" {
                    print $3 ";" $7 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            fi
        else
            echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'hva'."
            exit 3
        fi

    elif [[ "$type_station" == "lv" ]]; then
        if [[ "$type_consommateur" == "comp" ]]; then
            echo "Filtrage des données pour LV avec consommateurs : 'comp'."
            if [[ -z "$id_centrale" ]]; then
                awk -F';' '$4 != "-" && $5 != "-" && $7 == "-" {
                    print $4 ";" $5 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            else
                awk -F';' -v id="$id_centrale" '$1 == id && $4 != "-" && $5 != "-" && $7 == "-" {
                    print $4 ";" $5 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            fi

        elif [[ "$type_consommateur" == "indiv" ]]; then
            echo "Filtrage des données pour LV avec consommateurs : 'indiv'."
            if [[ -z "$id_centrale" ]]; then
                awk -F';' '$4 != "-" && $6 != "-" && $7 == "-" {
                    print $4 ";" $6 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            else
                awk -F';' -v id="$id_centrale" '$1 == id && $4 != "-" && $6 != "-" && $7 == "-" {
                    print $4 ";" $6 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            fi

        elif [[ "$type_consommateur" == "all" ]]; then
            echo "Filtrage des données pour LV avec consommateurs : 'all'."
            if [[ -z "$id_centrale" ]]; then
                awk -F';' '$4 != "-" && ($5 != "-" || $6 != "-") {
                    print $4 ";" $5 ";" $6 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            else
                awk -F';' -v id="$id_centrale" '$1 == id && $4 != "-" && ($5 != "-" || $6 != "-") {
                    print $4 ";" $5 ";" $6 ";" $8
                }' "$fichier_donnees" | tr '-' '0' >> "$fichier_filtre"
            fi

        else
            echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'lv'."
            exit 3
        fi

    else
        echo "Erreur : Type de station '$type_station' inconnu."
        exit 3
    fi
    }

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
    local fichier_resultat="tmp/resultat.csv"
    echo "Exécution du programme C pour le traitement..."
    ./codeC/c-wire < "$fichier_filtre" > "$fichier_resultat"
    [[ $? -ne 0 ]] && { echo "Erreur : Le programme C a rencontré une erreur."; exit 6; }

    case "$type_station" in
        hvb)
            [[ "$type_consommateur" == "comp" ]] && mv "$fichier_resultat" "tmp/hvb_comp.csv"
            ;;
        hva)
            [[ "$type_consommateur" == "comp" ]] && mv "$fichier_resultat" "tmp/hva_comp.csv"
            ;;
        lv)
            case "$type_consommateur" in
                comp)
                    mv "$fichier_resultat" "tmp/lv_comp.csv"
                    ;;
                indiv)
                    mv "$fichier_resultat" "tmp/lv_indiv.csv"
                    ;;
                all)
                    mv "$fichier_resultat" "tmp/lv_all.csv"
                    generer_lv_all_minmax "tmp/lv_all.csv"
                    ;;
            esac
            ;;
    esac
}

generer_lv_all_minmax() {
    local fichier_input="$1"
    local fichier_minmax="tmp/lv_all_minmax.csv"
    local fichier_graphique="graphs/lv_minmax.png"

    echo "Station;Capacité;Consommation;Différence" > "$fichier_minmax"

    awk -F';' 'NR > 1 {
        diff = $3 - $2;
        abs_diff = (diff < 0) ? -diff : diff;
        print $1 ";" $2 ";" $3 ";" abs_diff;
    }' "$fichier_input" | sort -t';' -k4,4nr > tmp/sorted_data.csv

    (head -n 10 tmp/sorted_data.csv; tail -n 10 tmp/sorted_data.csv) >> "$fichier_minmax"
    echo "Fichier Min/Max généré : $fichier_minmax."

    # Génération du graphique avec Gnuplot
    gnuplot <<-EOF
        set terminal png size 1200,800
        set output '${fichier_graphique}'
        set title "Top 10 et Bottom 10 Stations LV"
        set xlabel "Stations"
        set ylabel "Consommation (kWh)"
        set style data histograms
        set style fill solid
        set boxwidth 0.8
        set grid ytics
        set datafile separator ';'

        plot '${fichier_minmax}' using 2:xtic(1) title "Capacité" lc rgb "green", \
             '' using 3 title "Consommation" lc rgb "red"
EOF

    echo "Graphique généré : $fichier_graphique."
}

main() {
     debut_temps=$(date +%s)
    validation_arguments "$@" # On vérifie d'abord les arguments avant de lancer le script
    preparer_dossiers         # Préparation des dossiers requis
    verifier_installer_gnuplot
    traitement_principal "$@" # Lancement du traitement principal
    generer_lv_all_minmax
    echo "Script terminé avec succès !"
     fin_temps=$(date +%s)
     execution_temps=$((fin_temps - debut_temps))
     echo "Temps d'exécution du script shell : $execution_temps secondes."
}

#Point d'entrée dans le script 
main "$@"



    
