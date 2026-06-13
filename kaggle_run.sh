#!/bin/bash
# Script d'automatisation pour Kaggle

echo "------------------------------------------------------------"
echo "  CONFIGURATION ET EXÉCUTION DU PIPELINE (KAGGE)"
echo "------------------------------------------------------------"

# 1. Installation des dépendances
echo "[1/2] Installation des dépendances Python..."
pip install -r requirements.txt --quiet

# 2. Exécution du pipeline
echo "[2/2] Lancement de l'entraînement et de l'exportation..."
# On passe les arguments éventuels au script python (ex: --dataset ai4i)
python src/main.py "$@"

echo ""
echo "------------------------------------------------------------"
echo "  TERMINÉ !"
echo "  Le code C est généré dans : /kaggle/working/random_forest_rules.c"
echo "------------------------------------------------------------"
