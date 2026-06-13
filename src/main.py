# -*- coding: utf-8 -*-
import os
import sys
import argparse

# Ajout du répertoire src au sys.path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from src.data_processor import DataProcessor
from src.fault_classifier import FaultClassifier
from src.model_evaluator import ModelEvaluator
from src.code_exporter import CodeExporter
from src.features_config import FAULT_CLASSES

def main():
    """
    Point d'entrée principal du pipeline de Maintenance Prédictive.
    """
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    # Détection de l'environnement
    is_kaggle = os.path.exists("/kaggle/working")
    
    # Configuration des arguments
    parser = argparse.ArgumentParser(description="Pipeline de Maintenance Prédictive (TFE)")
    parser.add_argument("--dataset", type=str, default="synthetic", choices=["synthetic", "ai4i"],
                        help="Type de dataset à utiliser")
    parser.add_argument("--data-path", type=str, default=None,
                        help="Chemin vers le fichier CSV (pour ai4i)")
    parser.add_argument("--output", type=str, 
                        default=os.path.join(base_dir, "embedded", "src", "random_forest_rules.c"),
                        help="Chemin de sortie pour le code C généré")
    parser.add_argument("--n-trees", type=int, default=10,
                        help="Nombre d'arbres pour le Random Forest")
    parser.add_argument("--max-depth", type=int, default=10,
                        help="Profondeur maximale des arbres")
    
    args = parser.parse_args()
    
    print("="*60)
    print("   PIPELINE DE MAINTENANCE PRÉDICTIVE - SYSTÈMES TOURNANTS")
    print("="*60)
    print(f"[Config] Dataset : {args.dataset}")
    print(f"[Config] Modèle  : Random Forest (n={args.n_trees}, depth={args.max_depth})")

    # 1. Préparation des données
    processor = DataProcessor(dataset_type=args.dataset, filepath=args.data_path)
    X_train, X_test, y_train, y_test = processor.load_and_preprocess()
    
    if X_train is None:
        return

    # 2. Entraînement
    classifier = FaultClassifier(n_estimators=args.n_trees, max_depth=args.max_depth)
    classifier.fit(X_train, y_train)
    
    # 3. Évaluation
    ModelEvaluator.evaluate(classifier.model, X_test, y_test, FAULT_CLASSES)
    
    # 4. Exportation en C
    CodeExporter.export_rf_to_c(
        rf_model=classifier.model, 
        feature_names=processor.features, 
        output_filename=args.output
    )

    print("="*60)
    print("   PIPELINE TERMINÉ AVEC SUCCÈS")
    print("="*60)

if __name__ == "__main__":
    main()
