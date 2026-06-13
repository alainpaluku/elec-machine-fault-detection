# -*- coding: utf-8 -*-
"""
Configuration des caractéristiques (features) pour le pipeline de maintenance prédictive.
Ce fichier assure la cohérence entre le pipeline Python et le code C embarqué.
"""

FEATURE_NAMES = [
    "vibration_rms",
    "vibration_kurtosis",
    "vibration_crest_factor",
    "vibration_peak_to_peak",
    "vibration_skewness",
    "vibration_std",
    "vibration_dominant_freq",
    "vibration_spectral_energy",
    "temperature_celsius",
    "current_rms",
    "power_estimate",
    "speed_rpm"
]

# Mapping pour le dataset AI4I 2020 vers nos features
AI4I_MAPPING = {
    "Air temperature [°C]": "temperature_celsius",
    "Rotational speed [rpm]": "speed_rpm",
    "Torque [Nm]": "torque",  # Utilisé pour calculer power_estimate
    "Tool wear [min]": "tool_wear"
}

# Classes de défauts (doit correspondre à FaultClass_t dans rf_inference.h)
FAULT_CLASSES = [
    "Normal",
    "Usure outil",
    "Dissipation thermique",
    "Puissance",
    "Surcharge",
    "Aleatoire"
]
