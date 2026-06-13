from sklearn.metrics import classification_report, confusion_matrix

class ModelEvaluator:
    """
    Classe dédiée à l'évaluation des performances du modèle.
    """
    @staticmethod
    def evaluate(model, X_test, y_test, target_names: list):
        """
        Évalue le modèle avec un rapport de classification et une matrice de confusion.
        """
        print("\n" + "="*40)
        print("--- Évaluation du Modèle sur le jeu de Test ---")
        print("="*40)
        
        y_pred = model.predict(X_test)
        
        print("\n> Rapport de Classification (Précision, Rappel, F1-Score) :")
        print(classification_report(y_test, y_pred, target_names=target_names))
        
        print("\n> Matrice de Confusion :")
        cm = confusion_matrix(y_test, y_pred)
        print(cm)
