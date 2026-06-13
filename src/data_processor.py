# -*- coding: utf-8 -*-
"""
Classe de traitement des données pour le pipeline de maintenance prédictive.
"""

import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from src.features_config import FEATURE_NAMES, FAULT_CLASSES
from src.data_loader import load_ai4i_dataset, generate_synthetic_dataset

class DataProcessor:
    """
    Responsable du chargement et de la préparation des données pour l'entraînement.
    """
    def __init__(self, dataset_type="synthetic", filepath=None):
        self.dataset_type = dataset_type
        self.filepath = filepath
        self.features = FEATURE_NAMES
        self.label_mapping = {name: i for i, name in enumerate(FAULT_CLASSES)}

    def load_and_preprocess(self) -> tuple:
        """
        Charge les données et les prépare pour l'entraînement.
        Si on utilise le dataset synthétique, on génère directement les 12 features.
        Si on utilise AI4I, on simule/calcule les features manquantes.
        """
        if self.dataset_type == "synthetic":
            # Pour simplifier et assurer la cohérence avec le C, on génère
            # directement un dataset avec les 12 features cibles.
            X, y = self._generate_full_synthetic_data(n_samples=10000)
        elif self.dataset_type == "ai4i":
            if not self.filepath:
                print("[Erreur] Chemin du fichier AI4I manquant.")
                return None, None, None, None
            X_raw, y = load_ai4i_dataset(self.filepath)
            X = self._map_ai4i_to_12_features(X_raw)
        else:
            print(f"[Erreur] Type de dataset inconnu : {self.dataset_type}")
            return None, None, None, None

        print(f"[Info] Dataset prêt : {X.shape[0]} échantillons, {X.shape[1]} caractéristiques.")
        
        # Division en train/test
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.20, stratify=y, random_state=42
        )
        
        return X_train, X_test, y_train, y_test

    def _generate_full_synthetic_data(self, n_samples=10000):
        """
        Génère un dataset synthétique contenant exactement les 12 features.
        """
        rng = np.random.default_rng(42)

        # Initialisation de la matrice X
        X = pd.DataFrame(index=range(n_samples), columns=FEATURE_NAMES)
        y = np.zeros(n_samples, dtype=int)

        # Génération de base (Normal)
        X["vibration_rms"] = rng.normal(0.2, 0.05, n_samples)
        X["vibration_kurtosis"] = rng.normal(3.0, 0.2, n_samples)
        X["vibration_crest_factor"] = rng.normal(1.4, 0.1, n_samples)
        X["vibration_peak_to_peak"] = X["vibration_rms"] * 3.0 + rng.normal(0, 0.1, n_samples)
        X["vibration_skewness"] = rng.normal(0, 0.1, n_samples)
        X["vibration_std"] = X["vibration_rms"] * 0.9
        X["vibration_dominant_freq"] = rng.normal(50.0, 1.0, n_samples) # 50Hz nominal
        X["vibration_spectral_energy"] = X["vibration_rms"]**2 * 100
        X["temperature_celsius"] = rng.normal(40.0, 5.0, n_samples)
        X["current_rms"] = rng.normal(5.0, 0.5, n_samples)
        X["power_estimate"] = 230 * X["current_rms"] * 0.85
        X["speed_rpm"] = X["vibration_dominant_freq"] * 30 # 1500 RPM pour 50Hz (2 paires poles)

        # Ajout de défauts (on modifie certaines lignes)
        n_faults = n_samples // 6

        # 1. Usure outil (Tool Wear)
        idx = range(n_faults, 2*n_faults)
        X.loc[idx, "vibration_rms"] += 0.3
        X.loc[idx, "vibration_std"] += 0.2
        y[idx] = 1

        # 2. Dissipation thermique (Heat)
        idx = range(2*n_faults, 3*n_faults)
        X.loc[idx, "temperature_celsius"] += 30.0
        y[idx] = 2

        # 3. Puissance (Power)
        idx = range(3*n_faults, 4*n_faults)
        X.loc[idx, "current_rms"] += 10.0
        X.loc[idx, "power_estimate"] = 230 * X.loc[idx, "current_rms"] * 0.85
        y[idx] = 3

        # 4. Surcharge (Overstrain)
        idx = range(4*n_faults, 5*n_faults)
        X.loc[idx, "vibration_rms"] += 0.5
        X.loc[idx, "current_rms"] += 2.0
        y[idx] = 4

        # 5. Aléatoire (Random)
        idx = range(5*n_faults, n_samples)
        X.loc[idx, "vibration_kurtosis"] += 5.0
        y[idx] = 5

        return X.astype(float).values, y

    def _map_ai4i_to_12_features(self, df_raw):
        """
        Mappe le dataset AI4I vers nos 12 features.
        Puisque AI4I n'a pas de vibrations brutes, on simule ces features
        à partir des données disponibles (Speed, Torque).
        """
        n = len(df_raw)
        X = pd.DataFrame(index=range(n), columns=FEATURE_NAMES)

        # Mappage direct
        X["temperature_celsius"] = df_raw["Process temperature [°C]"]
        X["speed_rpm"] = df_raw["Rotational speed [rpm]"]

        # Calculs
        # P = Torque * Omega
        torque = df_raw["Torque [Nm]"]
        omega = X["speed_rpm"] * 2 * np.pi / 60
        X["power_estimate"] = torque * omega
        X["current_rms"] = X["power_estimate"] / (230 * 0.85)

        # Simulation des vibrations (car non présentes dans AI4I)
        # On crée une corrélation avec Speed et Torque pour que le modèle apprenne quelque chose
        X["vibration_rms"] = 0.1 + 0.001 * torque + 0.0001 * X["speed_rpm"] + np.random.normal(0, 0.02, n)
        X["vibration_std"] = X["vibration_rms"] * 0.95
        X["vibration_kurtosis"] = 3.0 + np.random.normal(0, 0.1, n)
        X["vibration_crest_factor"] = 1.4 + np.random.normal(0, 0.05, n)
        X["vibration_peak_to_peak"] = X["vibration_rms"] * 3.1
        X["vibration_skewness"] = np.random.normal(0, 0.05, n)
        X["vibration_dominant_freq"] = X["speed_rpm"] / 30.0
        X["vibration_spectral_energy"] = X["vibration_rms"]**2 * 100

        return X.astype(float).values
