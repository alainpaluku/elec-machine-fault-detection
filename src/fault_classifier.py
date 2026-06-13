from sklearn.ensemble import RandomForestClassifier

class FaultClassifier:
    """
    Classe gérant le modèle de classification (Random Forest).
    Les hyperparamètres sont limités pour respecter les contraintes mémoire du STM32.
    """
    def __init__(self, n_estimators: int = 10, max_depth: int = 5):
        print(f"[Info] Initialisation du Random Forest (n_estimators={n_estimators}, max_depth={max_depth})...")
        self.model = RandomForestClassifier(
            n_estimators=n_estimators, 
            max_depth=max_depth, 
            random_state=42
        )

    def fit(self, X_train, y_train):
        """
        Entraîne le modèle Random Forest sur les données d'apprentissage.
        """
        print("[Info] Entraînement du modèle...")
        self.model.fit(X_train, y_train)

    def predict(self, X):
        """
        Prédit l'état de la machine sur de nouvelles données.
        """
        return self.model.predict(X)
