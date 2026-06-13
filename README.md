# [i] Système de Maintenance Prédictive pour Machines Électriques

## Conception d'un système de maintenance prédictive pour machines électriques (moteurs/alternateurs) basé sur la fusion de données multi-capteurs et l'algorithme Random Forest

> [!] Travail de Fin d'Études — Master 2 Génie Électrique, Électroénergétique et Systèmes Embarqués

---

## [#] Table des matières

1. [Description du projet](#description-du-projet)
2. [Architecture du système](#architecture-du-système)
3. [Structure du répertoire](#structure-du-répertoire)
4. [Installation](#installation)
5. [Utilisation](#utilisation)
6. [Datasets](#datasets)
7. [Implémentation embarquée](#implémentation-embarquée)
8. [Résultats](#résultats)

---

## Description du projet

Ce projet implémente un système complet de maintenance prédictive pour les machines électriques rotatives (moteurs asynchrones, alternateurs synchrones), depuis l'acquisition des données capteurs jusqu'au déploiement embarqué sur microcontrôleur STM32.

### Problématique

> "Dans quelle mesure la fusion de données multi-capteurs (vibrations, température, courant statorique) couplée à un algorithme de classification supervisée de type Random Forest, déployé sur une cible embarquée STM32, permet-elle de diagnostiquer de manière fiable et en temps réel les défauts naissants des machines électriques rotatives ?"

### Capteurs et données fusionnées

| Capteur | Grandeur physique | Interface |
|---|---|---|
| Accéléromètre ADXL345 (MEMS) | Vibrations mécaniques | SPI |
| Sonde PT100 + INA125 | Température bobinage/carter | ADC (pont de Wheatstone) |
| Capteur Hall ACS712-30A | Courant statorique | ADC |

### Caractéristiques extraites (12 features)

- Domaine temporel : RMS, Kurtosis, Facteur de crête, Skewness, Peak-to-Peak, Écart-type
- Domaine fréquentiel : FFT → Fréquence dominante, Énergie spectrale
- Paramètres physiques : Température (°C), Courant RMS (A), Puissance estimée (W), Vitesse (RPM)

---

## Architecture du système

```
┌─────────────────────────────┐
│    MACHINE ÉLECTRIQUE       │
│  ┌────────┐ ┌─────┐ ┌────┐ │
│  │ADXL345 │ │PT100│ │Hall│ │
│  └───┬────┘ └──┬──┘ └─┬──┘ │
└──────┼─────────┼──────┼────┘
       │         │      │
  ┌────▼───┐ ┌───▼──┐ ┌─▼──┐
  │Filtre  │ │Pont  │ │Amp │
  │Anti-   │ │Wheat.│ │0-  │
  │Repli.  │ │+INA  │ │3.3V│
  └────┬───┘ └───┬──┘ └─┬──┘
       │         │      │
       ▼         ▼      ▼
  ┌──────────────────────────┐
  │     STM32F446RE          │
  │  ADC + Timer + DMA       │
  │  Feature Extraction      │
  │  Random Forest Inference │
  │  UART → Diagnostic       │
  └──────────────────────────┘
```

---

## Structure du répertoire

```
.
├── data/                          # Datasets (CSV, MAT)
├── src/                           # Pipeline Python
│   ├── features_config.py         # Configuration harmonisée (12 features)
│   ├── data_loader.py             # Chargement datasets
│   ├── data_processor.py          # Prétraitement et simulation
│   ├── fault_classifier.py        # Entraînement Random Forest
│   ├── model_evaluator.py         # Métriques et rapports
│   ├── code_exporter.py           # Générateur de code C
│   └── main.py                    # Pipeline orchestrateur
├── embedded/                      # Code embarqué STM32
│   ├── src/
│   │   ├── feature_compute.c/h    # Extraction features en C
│   │   ├── rf_inference.c/h       # Inférence Random Forest en C
│   │   ├── random_forest_rules.c  # Modèle exporté (généré)
│   │   └── main_stm32.c           # Application principale STM32
├── notebooks/                     # Jupyter notebooks exploratoires
├── requirements.txt               # Dépendances Python
├── .gitignore                     # Exclusion des fichiers temporaires
└── README.md                      # Ce fichier
```

---

## Installation

### Prérequis

- Python 3.9+
- pip ou conda

### Installation des dépendances

```bash
pip install -r requirements.txt
```

---

## Utilisation

### Exécution avec dataset synthétique (Recommandé pour test)

```bash
python src/main.py --dataset synthetic
```

### Exécution avec dataset AI4I 2020

```bash
python src/main.py --dataset ai4i --data-path data/ai4i_2020.csv
```

### Arguments

| Argument | Description | Défaut |
|---|---|---|
| --dataset | Type de dataset : synthetic, ai4i | synthetic |
| --data-path | Chemin vers le fichier de données | None |
| --output | Chemin du fichier C généré | `embedded/src/random_forest_rules.c` (ou `/kaggle/working/` sur Kaggle) |
| --n-trees | Nombre d'arbres | 10 |
| --max-depth | Profondeur max des arbres | 10 |

### Utilisation sur Kaggle

Le pipeline est optimisé pour un usage sur Kaggle :
1. Importez le fichier `notebooks/kaggle_workflow.ipynb` dans un nouveau notebook Kaggle.
2. Le script détecte automatiquement l'environnement et génère le code C dans `/kaggle/working/random_forest_rules.c`.
3. Vous pouvez ensuite télécharger ce fichier pour l'intégrer dans votre projet STM32CubeIDE.

---

## Implémentation embarquée

### Chaîne de déploiement

- Scikit-learn (Python) → CodeExporter → Code C (if/else) → STM32CubeIDE → Flash STM32

### Cible matérielle

- Microcontrôleur : STM32F446RE (Cortex-M4F, 180 MHz)
- Carte : NUCLEO-F446RE
- Capteurs : ADXL345, PT100, ACS712

---

## Résultats

Le pipeline génère automatiquement :

- Rapport de performance (Précision, Rappel, F1-Score)
- Matrice de confusion
- Fichier `random_forest_rules.c` prêt pour l'intégration STM32

---

## Licence

Projet académique — Travail de Fin d'Études, Master 2 Génie Électrique.

---

## Auteur

Alain Paluku
