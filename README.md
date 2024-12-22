## Description du projet

Ce projet est un programme Shell conçu pour traiter des fichiers de données de centrales électriques afin de déterminer si ces dernières sont en marge ou en surproduction.

---

## Fonctionnalités principales

1. **Validation des Entrées** :
   - Vérifie l'existence et le format du fichier d'entrée.
   - Valide les paramètres (types de stations, types de consommateurs, et identifiants de centrales).
   - Gère les combinaisons interdites entre les types de stations et de consommateurs.

2. **Organisation des Fichiers** :
   - Prépare et nettoie les dossiers nécessaires (`tmp` pour les fichiers temporaires et `graphs` pour les graphiques générés).

3. **Filtrage des Données** :
   - Trie et filtre les données en fonction des paramètres fournis :
     - Types de stations : `hvb`, `hva`, `lv`
     - Types de consommateurs : `comp`, `indiv`, `all`
     - Optionnellement, en fonction d'un identifiant de centrale.

4. **Analyse et Traitement** :
   - Appelle un programme C (`c-wire`) pour effectuer des calculs sur les données triées.
   - Génère des fichiers de résultats spécifiques pour chaque combinaison de paramètres.

5. **Génération de Graphiques** :
   - Produit des graphiques en histogrammes avec Gnuplot pour visualiser les stations les plus et les moins chargées.

---

## Pré-requis

Avant d'exécuter le programme, assurez-vous que votre environnement dispose des éléments suivants :

1. **Système d'exploitation** :
   - Linux ou macOS (Windows est supporté via WSL).

2. **Logiciels nécessaires** :
   - **Gnuplot** : Utilisé pour générer les graphiques.
     - Pour installer Gnuplot :
       - Sur Debian/Ubuntu : `sudo apt install gnuplot`
       - Sur macOS : `brew install gnuplot`
   - **Compilateur C (comme GCC)** : Utilisé pour compiler le programme C (`c-wire`).
     - Pour installer GCC :
       - Sur Debian/Ubuntu : `sudo apt install gcc`
       - Sur macOS : `brew install gcc`
   - **Make** : Nécessaire pour compiler avec le Makefile.
     - Pour installer Make :
       - Sur Debian/Ubuntu : `sudo apt install make`
       - Sur macOS : `brew install make`

3. **Permissions** :
   - Le script Shell (`c-wire.sh`) doit avoir les permissions d'exécution :
     ```bash
     chmod +x c-wire.sh
     ```

4. **Fichiers de données** :
   - Les fichiers d'entrée doivent être au format CSV avec des colonnes séparées par des points-virgules.

---

## Installation

Suivez les étapes ci-dessous pour installer et configurer correctement le projet :

1. **Cloner le dépôt** :  
   Téléchargez ou clonez le projet dans votre environnement local :  
   ```bash
   git clone <https://github.com/mk42jk/projetinfo3>
   cd <projetinfo3-main>
   ```
2. Naviguer jusqu'au projet
   Accédez au répertoire CodeC où se trouve le programme C (c-wire) :
```bash
cd CodeC
```
Compilez le programme C en utilisant la commande make :
```bash
make
```
Retour au répertoire principal :
```bash
cd -
```
Nettoyage des fichiers compilés :
```bash
cd CodeC
make clean
cd -
```
   
## Structure des Arguments

Le programme accepte entre 3 et 4 arguments :

1. **Fichier d'entrée** : Chemin vers un fichier CSV contenant les données.
2. **Type de station** : `hvb`, `hva`, ou `lv`.
3. **Type de consommateur** : `comp`, `indiv`, ou `all`.
4. **Identifiant de centrale** *(optionnel)* : Une valeur numérique entre 1 et 5.

### Combinaisons interdites :
- `hvb` avec `all` ou `indiv`
- `hva` avec `all` ou `indiv`

---

## Structure des Arguments

Le programme accepte entre 3 et 4 arguments :

1. **Fichier d'entrée** : Chemin vers un fichier CSV contenant les données.
2. **Type de station** : `hvb`, `hva`, ou `lv`.
3. **Type de consommateur** : `comp`, `indiv`, ou `all`.
4. **Identifiant de centrale** *(optionnel)* : Une valeur numérique entre 1 et 5.

### Combinaisons interdites :
- `hvb` avec `all` ou `indiv`
- `hva` avec `all` ou `indiv`

---

### Exemple du format CSV attendu

```csv
Power plant;HV-B Station;HV-A Station;LV Station;Company;Individual;Capacity;Load
1;-;-;-;-;-;2707288990;-
1;1;-;-;-;-;916264250;-
1;1;-;-;71;-;-;213302747
1;1;1;-;-;-;916264250;-
1;-;1;-;61;-;-;226765650
1;-;1;1;-;-;241999040;-
1;-;-;1;1;-;-;96811846
1;-;-;1;-;21;-;28198359
1;-;-;1;-;22;-;26074220
1;-;-;1;-;23;-;20274618
1;-;1;2;-;-;382635838;-
1;-;-;2;2;-;-;87678035
1;-;-;2;-;24;-;25315611
1;-;-;2;-;25;-;27415657
1;-;-;2;-;26;-;21164380
1;-;1;3;-;-;291629372;-
```


## Utilisation

### Lancer le programme :

```bash
./c-wire.sh <fichier_csv> <type_station> <type_consommateur> [id_centrale]
```

### Demander de l'aide :

```bash
./c-wire.sh -h
```

### Exemples d'exécution :

1. Avec 3 paramètres :
   ```bash
   ./c-wire.sh input/data.csv hvb comp
   ```

2. Avec 4 paramètres :
   ```bash
   ./c-wire.sh input/data.csv lv all 2
   ```

---

## Détails des Fonctions

### `affiche_aide()`
Affiche une aide détaillée sur la syntaxe de la commande et les paramètres disponibles, ainsi que les combinaisons interdites.

### `validation_arguments()`
Vérifie :
- Que le fichier d'entrée existe et est accessible.
- Que les paramètres de station et de consommateur sont valides.
- Que les combinaisons interdites ne sont pas utilisées.

### `preparer_dossiers()`
Crée et nettoie les dossiers nécessaires pour le traitement des données (`tmp`, `graphs`).

### `verifier_executable_c()`
Vérifie la présence de l'exécutable C (`c-wire`). Si nécessaire, tente de le compiler en utilisant `make`.

### `filtrer_donnees()`
Trie et filtre les données du fichier d'entrée en fonction des paramètres fournis :
- **HVB** :
  - Consommateurs `comp` uniquement.
- **HVA** :
  - Consommateurs `comp` uniquement.
- **LV** :
  - Consommateurs `comp`, `indiv`, ou `all`.
  - Combine les données des entreprises et des particuliers si `all` est sélectionné.

Enregistre les données triées dans un fichier temporaire (`tmp/filtered_data.csv`).

### `traitement_principal()`
- Filtre les données avec `filtrer_donnees()`.
- Exécute le programme C (`c-wire`) sur les données filtrées.
- Organise les fichiers de sortie selon le type de station et de consommateur :
  - `tmp/hvb_comp.csv`
  - `tmp/hva_comp.csv`
  - `tmp/lv_comp.csv`
  - `tmp/lv_indiv.csv`
  - `tmp/lv_all.csv`

### `generer_lv_all_minmax()`
Spécifique à `lv all` :
- Calcule les différences entre la capacité et la consommation.
- Trie les stations par différence, sélectionne les 10 plus chargées et les 10 moins chargées.
- Génère un graphique en histogramme (`graphs/lv_minmax.png`) avec Gnuplot.

---

## Structure des Fichiers

### Fichiers d'entrée
- **Format** : CSV avec des colonnes séparées par des points-virgules.
- Les colonnes doivent inclure des informations sur les stations, les types de consommateurs, la capacité, et la consommation.

### Répertoires générés :
- **`tmp/`** : Contient les fichiers temporaires générés pendant le traitement.
- **`graphs/`** : Contient les graphiques générés.
- **`tests/`** : Contient les fichiers de résultats finaux.

---

## Gestion des Erreurs

Le programme gère les erreurs suivantes :
- **Fichier non trouvé** : Si le fichier d'entrée n'existe pas.
- **Extension incorrecte** : Si le fichier d'entrée n'est pas au format `.csv`.
- **Paramètres invalides** : Si le type de station ou de consommateur est incorrect.
- **Combinaisons interdites** : Si une combinaison non autorisée est utilisée.
- **Compilation échouée** : Si le programme C ne peut pas être compilé.
- **Gnuplot non installé** : Si Gnuplot est requis mais absent.

---

## Exemple de Résultats

1. **Fichiers générés** :
   - `tmp/hvb_comp.csv`
   - `tmp/hva_comp.csv`
   - `tmp/lv_comp.csv`
   - `tmp/lv_indiv.csv`
   - `tmp/lv_all.csv`
   - `tmp/lv_all_minmax.csv`

2. **Graphique généré** :
   - `graphs/lv_minmax.png` : Histogramme représentant les 10 stations les plus et les moins chargées.
