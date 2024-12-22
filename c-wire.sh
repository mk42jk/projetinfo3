#!/bin/bash

#Fonction pour l'affichage de l'aide
affiche_aide(){
     echo "====== C-WIRE AIDE ========"
     echo "L'utilisation: $0 <Fichier_Données> <Type_de_station> <type_de_consommateur> [id_centrale] [-h]"
     echo "Options:" #une ligne pour expliquer chaque argument 
     echo "Fichier_Données: Chemin vers le ficher des données CSV (Obligatoire)"
     echo "Type_de_station: Type de station (hvb, hva, lv) (Obligatoire)"
     echo "type_de_consommateur: Type de consommateur (comp, indiv, all) (Obligatoire)"
     echo "id_centrale: Identifiant de la centrale (Optionnelle)"
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

     if [[ $# -gt 5 ]]; then
          echo "Erreur: Nombre d'arguments trop élevées. (maximum 6 autorisés)."
          affiche_aide
     fi
     
     #Initialisation du nom des arguments
     local fichier_donnees=$1
     local type_de_station=$2
     local type_de_consommateur=$3
     local id_centrale=${4:-}

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
        if [[ "$type_de_station" == "hva" && ( "$type_de_consommateur" == "all" || "$type_de_consommateur" == "indiv" ) ]]; then
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
    for dossier in "tmp" "graphs" "tests"; do
        if [[ ! -d "$dossier" ]]; then
            echo "Création du dossier '$dossier'..."
            mkdir -p "$dossier"
            if [[ $? -ne 0 ]]; then
                echo "Erreur : Impossible de créer le dossier '$dossier'."
                exit 4 # Code 4 : Erreur de permission pour la création de dossier
            fi
        else
            #Si c'est le dossier tmp, on le vide :
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

        make clear
        make
        if [[ $? -ne 0 ]]; then
            echo "Erreur : Échec de la compilation du programme C."
            exit 5 # Code 5 : Erreur de compilation
        fi
        popd > /dev/null
    fi
    echo "Exécutable C vérifié et prêt."
}

#Fonction qui vérifie si gnuplot est installé.
verifier_installer_gnuplot(){
if ! command -v gnuplot &> /dev/null; then
        echo "Erreur : Gnuplot n'est pas installé. Veuillez l'installer avant d'exécuter ce script."
        exit 7 # Code d'erreur 7 pour absence de dépendance
    fi
}

#Fonction pour créer le fichier temporaire des données filtrer en fonction des cas
filtrer_donnees() {

    local fichier_donnees="$1"
    local type_station="$2"
    local type_consommateur="$3"
    local id_centrale="${4:-}"
    local fichier_filtre="tmp/filtered_data.csv"

    # Initialiser le fichier filtré avec une en-tête appropriée
    case "$type_station" in
        hva)
            echo "Station HV-A:Capacité:Consommation(entreprises)" > "$fichier_filtre"
            ;;
         hvb)
            echo "Station HV-B:Capacité:Consommation(entreprises)" > "$fichier_filtre"
            ;;
        lv)
            case "$type_consommateur" in
                all)
                    echo "Station:Capacité:Consommation(tous)" > "$fichier_filtre"
                    ;;
                comp)
                    echo "Station:Capacité:Consommation(entreprises)" > "$fichier_filtre"
                    ;;
                indiv)
                    echo "Station:Capacité:Consommation(particuliers)" > "$fichier_filtre"
                    ;;
            esac
            ;;
    esac

    echo "Filtrage des données pour :"
    echo "  Type de station       : $type_station"
    echo "  Type de consommateur  : $type_consommateur"
    echo "  Identifiant de centrale  : ${id_centrale:-Aucun}"
    
#Si les arguments sont "hvb comp".
if [[ "$type_station" == "hvb" ]]; then
        if [[ "$type_consommateur" == "comp" ]]; then 
            echo "Filtrage des données pour HVB avec consommateurs : 'comp'."
            if [[ -z "$id_centrale" ]]; then
               # Filtrer les lignes de capacité
                awk -F';' '
                    $2 != "-" && $7 != "-" && $4 == "-" { print $2 ":" $7 ":0" }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                # Filtrer les lignes de consommation
                awk -F';' '
                    $2 != "-" && $5 != "-" && $7 == "-" { print $2 ":0:" $8 }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
            else
                awk -F';' -v id="$id_centrale" '
                    $1 == id && $2 != "-" && $7 != "-" && $4 == "-" { print $2 ":" $7 ":0" }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                awk -F';' -v id="$id_centrale" '
                    $1 == id && $2 != "-" && $5 != "-" && $7 == "-" { print $2 ":0:" $8 }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
            fi
        else
            echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'hvb'."
            exit 3
        fi
#Si les arguments sont "hva comp".
elif [[ "$type_station" == "hva" ]]; then
        if [[ "$type_consommateur" == "comp" ]]; then
            echo "Filtrage des données pour HVA avec consommateurs : 'comp'."
            if [[ -z "$id_centrale" ]]; then
                # Filtrer les lignes de capacité
                awk -F';' '
                    $3 != "-" && $7 != "-" && $4 == "-" { print $3 ":" $7 ":0" }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                # Filtrer les lignes de consommation
                awk -F';' '
                    $3 != "-" && $5 != "-" && $7 == "-" { print $3 ":0:" $8 }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
            else
                awk -F';' -v id="$id_centrale" '
                    $1 == id && $3 != "-" && $7 != "-" && $4 == "-" { print $3 ":" $7 ":0" }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                awk -F';' -v id="$id_centrale" '
                    $1 == id && $3 != "-" && $5 != "-" && $7 == "-" { print $3 ":0:" $8 }
                ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
            fi
        else
            echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'hva'."
            exit 3
        fi
#Si les arguments sont " lv comp".
elif [[ "$type_station" == "lv" ]]; then
        case "$type_consommateur" in
            comp)
                echo "Filtrage des données pour LV avec consommateurs : 'comp'."
                if [[ -z "$id_centrale" ]]; then
                    # Filtrer les lignes de capacité (corrigé)
                    awk -F';' '
                        $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    # Filtrer les lignes de consommation (entreprises)
                    awk -F';' '
                        $4 != "-" && $5 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                else
                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $5 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                fi
                ;;
            indiv)
                echo "Filtrage des données pour LV avec consommateurs : 'indiv'."
                if [[ -z "$id_centrale" ]]; then
                    # Filtrer les lignes de capacité (corrigé)
                    awk -F';' '
                        $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    # Filtrer les lignes de consommation (particuliers)
                    awk -F';' '
                        $4 != "-" && $6 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                else
                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $6 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                fi
                ;;
            all) #Si les arguments sont "lv all".
                echo "Filtrage des données pour LV avec consommateurs : 'all'."
                if [[ -z "$id_centrale" ]]; then
                    # Filtrer les lignes de capacité
                    awk -F';' '
                        $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    # Filtrer les lignes de consommation (entreprises)
                    awk -F';' '
                        $4 != "-" && $5 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    # Filtrer les lignes de consommation (particuliers)
                    awk -F';' '
                        $4 != "-" && $6 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                else
                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $7 != "-" { print $4 ":" $7 ":0" }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $5 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }

                    awk -F';' -v id="$id_centrale" '
                        $1 == id && $4 != "-" && $6 != "-" && $7 == "-" { print $4 ":0:" $8 }
                    ' "$fichier_donnees" >> "$fichier_filtre" || { echo "Erreur lors du filtrage avec awk."; exit 9; }
                fi

                ;;
            *)
                echo "Erreur : Type de consommateur '$type_consommateur' non valide pour 'lv'."
                exit 3
                ;;
        esac
    fi


    # Validation des données filtrées
    if [[ ! -s "$fichier_filtre" ]]; then
        echo "Erreur : Aucune donnée n'a été filtrée. Vérifiez vos critères de filtrage."
        exit 8
    fi
    echo "Filtrage terminé. Données filtrées enregistrées dans '$fichier_filtre'."
}

# Fonction pour exécuter le traitement principal
traitement_principal() {
    local fichier_donnees=$1
    local type_de_station=$2
    local type_consommateur=$3
    local id_centrale=${4:-}

    echo "Lancement du traitement principal..."
    
    # Filtrage des données avant d'exécuter le programme C
    filtrer_donnees "$fichier_donnees" "$type_de_station" "$type_consommateur" "$id_centrale"
    verifier_executable_c

    # Préparation des paramètres pour le programme C
    local fichier_filtre="tmp/filtered_data.csv"
    local fichier_resultat="tmp/resultat.csv"
    echo "Exécution du programme C pour le traitement..."
    ./codeC/c-wire < "$fichier_filtre" > "$fichier_resultat"
    [[ $? -ne 0 ]] && { echo "Erreur : Le programme C a rencontré une erreur."; exit 6; }

    # Vérification que le fichier de résultat n'est pas vide
    if [[ ! -s "$fichier_resultat" ]]; then
        echo "Erreur : Le fichier de résultat est vide."
        exit 10
    fi

    # Définition de l'en-tête en fonction des arguments
    local header
    case "$type_de_station" in
        hvb|hva)
            if [[ "$type_consommateur" == "comp" ]]; then
                header="Station:Capacité:Consommation(entreprises)"
            fi
            ;;
        lv)
            case "$type_consommateur" in
                comp)
                    header="Station:Capacité:Consommation(entreprises)"
                    ;;
                indiv)
                    header="Station:Capacité:Consommation(particuliers)"
                    ;;
                all)
                    header="Station:Capacité:Consommation(tous)"
                    ;;
            esac
            ;;
    esac

    # On construit le nom du fichier final dans tests/
    local fichier_sortie="tests/${type_de_station}_${type_consommateur}.csv"
    if [[ -n "$id_centrale" ]]; then
        fichier_sortie="tests/${type_de_station}_${type_consommateur}_${id_centrale}.csv"
    fi

    # On déplace maintenant le résultat
    mv "$fichier_resultat" "$fichier_sortie" || {
        echo "Erreur lors du déplacement du fichier de résultat vers '$fichier_sortie'."
        exit 11
    }

    # On insère notre en-tête tout en conservant le contenu trié
    # Astuce: on place l'en-tête au-dessus du fichier existant
    if [[ -n "$header" ]]; then
        echo "$header" | cat - "$fichier_sortie" > "tmp/header_temp.csv" \
        && mv "tmp/header_temp.csv" "$fichier_sortie"
    fi

    # Enfin, si on est en lv all, on génère le fichier minmax dans 'tests/'
    if [[ "$type_de_station" == "lv" && "$type_consommateur" == "all" ]]; then
        generer_lv_all_minmax "$fichier_sortie"
    fi
}

generer_lv_all_minmax() {
    local fichier_input="$1"  # "tmp/lv_all.csv"
    local fichier_minmax="tests/lv_all_minmax.csv"
    local fichier_graphique="graphs/lv_minmax.png"

    echo "Station:Capacité:Consommation:Différence" > "$fichier_minmax"

    awk -F':' '
        NR > 1 {
            diff = $3 - $2
            if (diff < 0) diff = -diff
            print $1 ":" $2 ":" $3 ":" diff
        }
    ' "$fichier_input" | sort -t':' -k1,1 -k4,4nr | uniq > tmp/sorted_data_unique.csv

    # Extraire les 10 premiers (plus grandes différences) et les 10 derniers (plus petites différences)
    {
        head -n 10 tmp/sorted_data_unique.csv
        tail -n 10 tmp/sorted_data_unique.csv
    } >> "$fichier_minmax"

    echo "Fichier Min/Max généré : $fichier_minmax."

    # Génération du graphique avec Gnuplot
    gnuplot <<-EOF
        set terminal png size 1200,800
        set output '${fichier_graphique}'
        set title "Les 10 centrales en marge et les 10 centrales en sous-production"
        set xlabel "Stations"
        set ylabel "Consommation (kWh)"
        set style data histograms
        set style fill solid
        set boxwidth 0.8
        set grid ytics
        set datafile separator ':'

        plot '${fichier_minmax}' using 2:xtic(1) title "Capacité" lc rgb "green", \
             '' using 3 title "Consommation" lc rgb "red"
EOF

    echo "Graphique généré : $fichier_graphique."

    # Nettoyage du fichier temporaire
    rm -f tmp/sorted_data_unique.csv
}

main() {
 debut_temps=$(date +%s)
 validation_arguments "$@" # On vérifie d'abord les arguments avant de lancer le script
 preparer_dossiers         # Préparation des dossiers requis
 verifier_installer_gnuplot # On vérifie que gnuplot est installé
 traitement_principal "$@" # Lancement du traitement principal
 echo "Script terminé avec succès !"
 fin_temps=$(date +%s)
 execution_temps=$((fin_temps - debut_temps))
 echo "Temps d'exécution du script shell : $execution_temps secondes."
}

#Point d'entrée dans le script 
main "$@"
