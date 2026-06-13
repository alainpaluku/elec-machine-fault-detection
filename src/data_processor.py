import pandas as pd
from sklearn.model_selection import train_test_split

class DataProcessor:
    """
    Classe responsable du chargement, du nettoyage et de la préparation des données.
    """
    def __init__(self, filepath: str):
        self.filepath = filepath
        self.features = ['Vibration_RMS', 'Current_A', 'Voltage_V', 'Motor_Temperature_C']
        self.target = 'Label'
        self.label_mapping = {
            'healthy condition': 0,
            'rotor imbalance': 1,
            'shaft misalignment': 2,
            'stator winding faults': 3
        }

    def load_and_preprocess(self) -> tuple:
        """
        Charge les données, gère les valeurs manquantes et divise le jeu de données.
        Retourne (X_train, X_test, y_train, y_test).
        """
        print(f"[Info] Chargement des données depuis {self.filepath}...")
        try:
            df = pd.read_csv(self.filepath)
        except Exception as e:
            print(f"[Erreur] Impossible de lire le fichier: {e}")
            return None, None, None, None

        # Nettoyage des valeurs manquantes
        df = df.dropna(subset=self.features + [self.target])
        
        # Encodage
        df['Label_Encoded'] = df[self.target].map(self.label_mapping)
        df = df.dropna(subset=['Label_Encoded'])

        X = df[self.features].values
        y = df['Label_Encoded'].astype(int).values

        print("[Info] Division stratifiée du dataset (80% Train, 20% Test)...")
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.20, stratify=y, random_state=42
        )
        
        return X_train, X_test, y_train, y_test
