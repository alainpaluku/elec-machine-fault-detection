import os
import sys
import argparse

# Ajout du répertoire src au sys.path si nécessaire pour les imports
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from data_processor import DataProcessor
from fault_classifier import FaultClassifier
from model_evaluator import ModelEvaluator
from code_exporter import CodeExporter

def main():
    """
    Point d'entrée principal du pipeline de Maintenance Prédictive.
    """
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    # Détection automatique de l'environnement Kaggle
    kaggle_data_path = "/kaggle/input/rotating-machinery-fault-monitoring-data/rotating_machinery_data.csv"
    kaggle_output_path = "/kaggle/working/random_forest_rules.c"
    is_kaggle = os.path.exists("/kaggle/working")
    
    default_data = kaggle_data_path if is_kaggle else os.path.join(base_dir, "data", "rotating_machinery_data.csv")
    default_output = kaggle_output_path if is_kaggle else os.path.join(base_dir, "embedded", "random_forest_rules.c")
    
    if is_kaggle:
        print("[Info] Environnement Kaggle détecté. Configuration automatique des chemins de données et de sortie.")
    
    # Configuration des arguments de ligne de commande
    parser = argparse.ArgumentParser(description="Pipeline de Maintenance Prédictive (TFE)")
    parser.add_argument("--data", type=str, 
                        default=default_data,
                        help="Chemin vers le fichier CSV de données")
    parser.add_argument("--output", type=str, 
                        default=default_output,
                        help="Chemin de sortie pour le code C généré")
    
    args = parser.parse_args()
    
    csv_file = args.data
    output_c_file = args.output
    
    print("="*50)
    print(" PIPELINE DE MAINTENANCE PRÉDICTIVE (TFE)")
    print("="*50)

    # 1. Préparation des données
    processor = DataProcessor(filepath=csv_file)
    X_train, X_test, y_train, y_test = processor.load_and_preprocess()
    
    if X_train is None:
        print("\n[!] Vérifiez que le fichier de données existe à l'emplacement indiqué :", csv_file)
        return

    # 2. Entraînement du modèle (Hyperparamètres contraints)
    classifier = FaultClassifier(n_estimators=10, max_depth=5)
    classifier.fit(X_train, y_train)
    
    # 3. Évaluation
    # On trie les clés du dictionnaire pour avoir [Sain, Balourd, Désalignement, Défaut stator]
    target_names = [k for k, v in sorted(processor.label_mapping.items(), key=lambda item: item[1])]
    ModelEvaluator.evaluate(classifier.model, X_test, y_test, target_names)
    
    # 4. Exportation en C
    CodeExporter.export_rf_to_c(
        rf_model=classifier.model, 
        feature_names=processor.features, 
        output_filename=output_c_file
    )

if __name__ == "__main__":
    main()
