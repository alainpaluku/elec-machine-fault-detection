# 🔧 Système de Maintenance Prédictive pour Machines Électriques

## Conception d'un système de maintenance prédictive pour machines électriques (moteurs/alternateurs) basé sur la fusion de données multi-capteurs et l'algorithme Random Forest

> **Travail de Fin d'Études — Master 2 Génie Électrique, Électroénergétique et Systèmes Embarqués**

---

## 📋 Table des matières

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

Ce projet implémente un système complet de **maintenance prédictive** pour les machines électriques rotatives (moteurs asynchrones, alternateurs synchrones), depuis l'acquisition des données capteurs jusqu'au déploiement embarqué sur microcontrôleur **STM32**.

### Problématique

> *Dans quelle mesure la fusion de données multi-capteurs (vibrations, température, courant statorique) couplée à un algorithme de classification supervisée de type Random Forest, déployé sur une cible embarquée STM32, permet-elle de diagnostiquer de manière fiable et en temps réel les défauts naissants des machines électriques rotatives ?*

### Capteurs et données fusionnées

| Capteur | Grandeur physique | Interface |
|---|---|---|
| Accéléromètre ADXL345 (MEMS) | Vibrations mécaniques | SPI |
| Sonde PT100 + INA125 | Température bobinage/carter | ADC (pont de Wheatstone) |
| Capteur Hall ACS712-30A | Courant statorique | ADC |

### Caractéristiques extraites

**Domaine temporel** : RMS, Kurtosis, Facteur de crête, Skewness, Peak-to-Peak, Écart-type

**Domaine fréquentiel** : FFT → Fréquence dominante, Centroïde spectral, Bande passante spectrale, Énergie spectrale

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
P_TUT/
├── data/                          # Datasets (CSV, MAT)
├── notebooks/                     # Jupyter notebooks exploratoires
├── src/                           # Pipeline Python
│   ├── __init__.py
│   ├── data_loader.py             # Chargement datasets (AI4I, CWRU, synthétique)
│   ├── feature_extraction.py      # Extraction features temporelles/fréquentielles
│   ├── model_training.py          # Random Forest + GridSearchCV
│   ├── model_evaluation.py        # Métriques, visualisations, rapport
│   ├── export_onnx.py             # Export ONNX pour STM32
│   └── main.py                    # Pipeline orchestrateur
├── embedded/                      # Code embarqué STM32
│   ├── src/
│   │   ├── feature_compute.c/h    # Extraction features en C
│   │   ├── rf_inference.c/h       # Inférence Random Forest en C
│   │   └── main_stm32.c           # Application principale STM32
│   ├── stm32_config/              # Fichiers .ioc STM32CubeMX
│   └── README.md                  # Guide d'implémentation embarquée
├── figures/                       # Graphiques et schémas
├── docs/memoire/                  # Chapitres LaTeX du mémoire
├── output/                        # Résultats (modèles, rapports)
├── requirements.txt               # Dépendances Python
└── README.md                      # Ce fichier
```

---

## Installation

### Prérequis

- Python 3.9+
- pip ou conda

### Installation des dépendances

```bash
cd P_TUT
pip install -r requirements.txt
```

---

## Utilisation

### Exécution avec dataset synthétique (par défaut)

```bash
python -m src.main --dataset synthetic --optimize
```

### Exécution avec dataset AI4I 2020

```bash
# Télécharger le dataset depuis Kaggle :
# https://www.kaggle.com/datasets/stephanmatzka/predictive-maintenance-dataset-ai4i-2020

python -m src.main --dataset ai4i --data-path data/ai4i_2020.csv --optimize
```

### Exécution avec dataset CWRU

```bash
# Télécharger les fichiers .mat depuis :
# https://engineering.case.edu/bearingdatacenter

python -m src.main --dataset cwru --data-path data/cwru/ --optimize
```

### Exécution sur Kaggle (Recommandé)

Pour exécuter le pipeline d'IA sans configuration locale et récupérer le code C final :

1. Créez un nouveau Notebook Kaggle.
2. Ajoutez le dataset cible (par ex: *Rotating Machinery Fault Monitoring Data*).
3. Importez directement le fichier [`notebooks/kaggle_workflow.ipynb`](notebooks/kaggle_workflow.ipynb) dans votre environnement Kaggle (File > Import Notebook).

Ou exécutez manuellement les cellules suivantes :
```bash
# 1. Cloner le dépôt GitHub (à adapter avec votre nom d'utilisateur)
!git clone https://github.com/Ton_Nom_Utilisateur/Ton_Projet_TFE.git

# 2. Se placer dans le répertoire
%cd Ton_Projet_TFE

# 3. Lancer le pipeline (les chemins Kaggle sont auto-détectés)
!python src/main.py

# 4. Afficher/Télécharger le code C généré (/kaggle/working/random_forest_rules.c)
with open('/kaggle/working/random_forest_rules.c', 'r') as f:
    print("".join(f.readlines()[:30]))
```

### Arguments

| Argument | Description | Défaut |
|---|---|---|
| `--dataset` | Type de dataset : `synthetic`, `ai4i`, `cwru` | `synthetic` |
| `--data-path` | Chemin vers le fichier/répertoire de données | `None` |
| `--optimize` | Activer l'optimisation hyperparamètres (GridSearchCV) | `False` |
| `--output-dir` | Répertoire de sortie | `./output` |

---

## Datasets

### AI4I 2020 Predictive Maintenance (recommandé)

- **Source** : [Kaggle](https://www.kaggle.com/datasets/stephanmatzka/predictive-maintenance-dataset-ai4i-2020)
- **Taille** : 10 000 échantillons
- **Capteurs** : Température (air, process), Vitesse rotation, Couple, Usure
- **Classes** : Normal + 5 modes de défaillance (TWF, HDF, PWF, OSF, RNF)

### CWRU Bearing Dataset

- **Source** : [Case Western Reserve University](https://engineering.case.edu/bearingdatacenter)
- **Taille** : Signaux vibratoires haute fréquence (12/48 kHz)
- **Capteurs** : Accéléromètres (Drive End, Fan End, Base)
- **Défauts** : Bague interne, bague externe, bille

---

## Implémentation embarquée

Voir le guide complet dans [`embedded/README.md`](embedded/README.md).

### Chaîne de déploiement

```
scikit-learn (Python) → skl2onnx → .onnx → X-CUBE-AI → Code C → STM32
```

### Cible matérielle

- **Microcontrôleur** : STM32F446RE (Cortex-M4F, 180 MHz, 512 KB Flash, 128 KB RAM)
- **Carte** : NUCLEO-F446RE
- **IDE** : STM32CubeIDE + X-CUBE-AI expansion pack

---

## Résultats

Les résultats sont générés automatiquement dans le répertoire `output/` :

- `confusion_matrix.png` — Matrice de confusion
- `feature_importance.png` — Importance des caractéristiques (Gini)
- `roc_curves.png` — Courbes ROC par classe
- `learning_curve.png` — Courbe d'apprentissage
- `evaluation_report.txt` — Rapport textuel complet
- `random_forest_model.onnx` — Modèle exporté pour STM32

---

## Licence

Projet académique — Travail de Fin d'Études, Master 2 Génie Électrique.

---

## Auteur

*À compléter*
