# -*- coding: utf-8 -*-
"""
Module d'extraction de caractéristiques pour la maintenance prédictive.

Ce module implémente les fonctions d'extraction de caractéristiques dans
les domaines temporel et fréquentiel, adaptées à l'analyse de signaux
vibratoires et de données tabulaires de machines électriques tournantes.

**Domaine temporel** :
  RMS, kurtosis, facteur de crête, asymétrie, amplitude crête-à-crête,
  écart-type.

**Domaine fréquentiel** :
  Fréquence dominante, centroïde spectral, largeur de bande spectrale,
  énergie spectrale, amplitude du pic principal.

**Caractéristiques d'interaction** (données tabulaires) :
  Puissance mécanique (P = τ·ω), différence de température process–air.

Auteur : Alain Paluku
Projet : TFE Master 2 — Maintenance prédictive des machines électriques
"""

from __future__ import annotations

import logging
from typing import Dict, List, Optional, Union

import numpy as np
import pandas as pd
from scipy import stats as scipy_stats

logger = logging.getLogger(__name__)


# ======================================================================
#  CARACTÉRISTIQUES TEMPORELLES
# ======================================================================

def compute_rms(signal: np.ndarray) -> float:
    """
    Calcule la valeur efficace (Root Mean Square) du signal.

    Le RMS est un indicateur de l'énergie moyenne du signal vibratoire.
    En maintenance prédictive, une augmentation du RMS indique
    généralement une dégradation mécanique (balourd, désalignement).

    .. math::
        \\text{RMS} = \\sqrt{\\frac{1}{N} \\sum_{i=1}^{N} x_i^2}

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Valeur RMS du signal.
    """
    signal = np.asarray(signal, dtype=np.float64)
    return float(np.sqrt(np.mean(signal ** 2)))


def compute_kurtosis(signal: np.ndarray) -> float:
    """
    Calcule le kurtosis (moment statistique d'ordre 4) du signal.

    Le kurtosis mesure le « poids des queues » de la distribution.
    Un kurtosis élevé (> 3 pour un kurtosis standard) indique la
    présence d'impulsions dans le signal, caractéristiques de
    défauts de roulement ou d'engrenage.

    On utilise le kurtosis de Pearson (fisher=False) :

    .. math::
        K = \\frac{\\frac{1}{N} \\sum_{i=1}^{N} (x_i - \\bar{x})^4}
            {\\left(\\frac{1}{N} \\sum_{i=1}^{N} (x_i - \\bar{x})^2\\right)^2}

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Kurtosis standard (Pearson) du signal.
    """
    signal = np.asarray(signal, dtype=np.float64)
    # fisher=False → kurtosis de Pearson (valeur = 3 pour une gaussienne)
    return float(scipy_stats.kurtosis(signal, fisher=False))


def compute_crest_factor(signal: np.ndarray) -> float:
    """
    Calcule le facteur de crête du signal.

    Le facteur de crête est le rapport entre la valeur crête absolue
    et la valeur efficace (RMS). Il détecte les chocs ponctuels :
    un facteur élevé signale des impacts (écaillage de roulement).

    .. math::
        CF = \\frac{\\max(|x|)}{\\text{RMS}(x)}

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Facteur de crête. Retourne 0.0 si le RMS est nul.
    """
    signal = np.asarray(signal, dtype=np.float64)
    rms = compute_rms(signal)
    if rms == 0.0:
        return 0.0
    return float(np.max(np.abs(signal)) / rms)


def compute_skewness(signal: np.ndarray) -> float:
    """
    Calcule l'asymétrie (skewness, moment d'ordre 3) du signal.

    L'asymétrie caractérise la symétrie de la distribution :
      - Skewness ≈ 0 : distribution symétrique
      - Skewness > 0 : queue droite plus longue
      - Skewness < 0 : queue gauche plus longue

    En vibrations, une asymétrie marquée peut indiquer un
    phénomène de frottement unidirectionnel.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Coefficient d'asymétrie du signal.
    """
    signal = np.asarray(signal, dtype=np.float64)
    return float(scipy_stats.skew(signal))


def compute_peak_to_peak(signal: np.ndarray) -> float:
    """
    Calcule l'amplitude crête-à-crête du signal.

    .. math::
        P2P = \\max(x) - \\min(x)

    Indicateur simple de l'amplitude globale des oscillations.
    Utile pour détecter un balourd ou un jeu mécanique excessif.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Amplitude crête-à-crête.
    """
    signal = np.asarray(signal, dtype=np.float64)
    return float(np.max(signal) - np.min(signal))


def compute_std(signal: np.ndarray) -> float:
    """
    Calcule l'écart-type du signal.

    L'écart-type mesure la dispersion du signal autour de sa moyenne.
    En maintenance, il est corrélé au niveau vibratoire global.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    float
        Écart-type du signal.
    """
    signal = np.asarray(signal, dtype=np.float64)
    return float(np.std(signal, ddof=0))


# ======================================================================
#  CARACTÉRISTIQUES FRÉQUENTIELLES
# ======================================================================

def compute_fft_features(
    signal: np.ndarray,
    fs: float,
) -> Dict[str, float]:
    """
    Calcule les caractéristiques spectrales du signal par FFT.

    Le spectre est obtenu par transformée de Fourier rapide (rFFT)
    sur le signal réel. Les caractéristiques extraites sont :

    - **dominant_frequency** : fréquence du pic d'amplitude maximale [Hz].
      Correspond souvent à la fréquence de rotation ou à un harmonique
      caractéristique d'un défaut (BPFO, BPFI, BSF, FTF pour les
      roulements).
    - **spectral_centroid** : barycentre fréquentiel pondéré par
      l'amplitude [Hz]. Indique le « centre de gravité » du spectre.
    - **spectral_bandwidth** : écart-type de la distribution
      spectrale [Hz]. Un spectre large indique un bruit large bande.
    - **spectral_energy** : énergie totale du spectre (théorème de
      Parseval).
    - **peak_amplitude** : amplitude du pic fréquentiel dominant.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.
    fs : float
        Fréquence d'échantillonnage en Hz.

    Returns
    -------
    dict[str, float]
        Dictionnaire des caractéristiques fréquentielles.
    """
    signal = np.asarray(signal, dtype=np.float64)
    n = len(signal)

    # Transformée de Fourier rapide (spectre unilatéral)
    fft_vals: np.ndarray = np.fft.rfft(signal)
    # Magnitude du spectre (normalisation par N)
    magnitude: np.ndarray = np.abs(fft_vals) / n
    # Axe fréquentiel
    freqs: np.ndarray = np.fft.rfftfreq(n, d=1.0 / fs)

    # Ignorer la composante DC (indice 0) pour les calculs spectraux
    magnitude_no_dc = magnitude[1:]
    freqs_no_dc = freqs[1:]

    # Fréquence dominante (pic d'amplitude max)
    peak_idx: int = int(np.argmax(magnitude_no_dc))
    dominant_frequency: float = float(freqs_no_dc[peak_idx])
    peak_amplitude: float = float(magnitude_no_dc[peak_idx])

    # Centroïde spectral : f_c = Σ(f_i × |X_i|) / Σ(|X_i|)
    total_magnitude: float = float(np.sum(magnitude_no_dc))
    if total_magnitude > 0:
        spectral_centroid: float = float(
            np.sum(freqs_no_dc * magnitude_no_dc) / total_magnitude
        )
    else:
        spectral_centroid = 0.0

    # Largeur de bande spectrale (écart-type fréquentiel pondéré)
    if total_magnitude > 0:
        spectral_bandwidth: float = float(
            np.sqrt(
                np.sum(
                    magnitude_no_dc * (freqs_no_dc - spectral_centroid) ** 2
                )
                / total_magnitude
            )
        )
    else:
        spectral_bandwidth = 0.0

    # Énergie spectrale (théorème de Parseval : ∝ Σ|X_i|²)
    spectral_energy: float = float(np.sum(magnitude ** 2))

    return {
        "dominant_frequency": dominant_frequency,
        "spectral_centroid": spectral_centroid,
        "spectral_bandwidth": spectral_bandwidth,
        "spectral_energy": spectral_energy,
        "peak_amplitude": peak_amplitude,
    }


# ======================================================================
#  EXTRACTEURS COMBINÉS
# ======================================================================

def extract_time_domain_features(signal: np.ndarray) -> Dict[str, float]:
    """
    Extrait l'ensemble des caractéristiques du domaine temporel.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.

    Returns
    -------
    dict[str, float]
        Dictionnaire contenant : rms, kurtosis, crest_factor,
        skewness, peak_to_peak, std, mean, max, min.
    """
    signal = np.asarray(signal, dtype=np.float64)
    return {
        "rms": compute_rms(signal),
        "kurtosis": compute_kurtosis(signal),
        "crest_factor": compute_crest_factor(signal),
        "skewness": compute_skewness(signal),
        "peak_to_peak": compute_peak_to_peak(signal),
        "std": compute_std(signal),
        "mean": float(np.mean(signal)),
        "max": float(np.max(signal)),
        "min": float(np.min(signal)),
    }


def extract_frequency_domain_features(
    signal: np.ndarray,
    fs: float,
) -> Dict[str, float]:
    """
    Extrait l'ensemble des caractéristiques du domaine fréquentiel.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.
    fs : float
        Fréquence d'échantillonnage en Hz.

    Returns
    -------
    dict[str, float]
        Dictionnaire des caractéristiques fréquentielles.
    """
    return compute_fft_features(signal, fs)


def extract_all_features(
    signal: np.ndarray,
    fs: float = 4000.0,
) -> Dict[str, float]:
    """
    Extrait toutes les caractéristiques (temporelles + fréquentielles).

    Combine :
      - 9 caractéristiques temporelles (RMS, kurtosis, etc.)
      - 5 caractéristiques fréquentielles (fréquence dominante, etc.)

    soit un vecteur de 14 caractéristiques au total.

    Parameters
    ----------
    signal : np.ndarray
        Signal temporel monodimensionnel.
    fs : float
        Fréquence d'échantillonnage en Hz (défaut : 4000 Hz).

    Returns
    -------
    dict[str, float]
        Dictionnaire complet de 14 caractéristiques.
    """
    features: Dict[str, float] = {}
    features.update(extract_time_domain_features(signal))
    features.update(extract_frequency_domain_features(signal, fs))
    return features


def extract_features_from_dataframe(
    df: pd.DataFrame,
    signal_columns: Optional[List[str]] = None,
) -> pd.DataFrame:
    """
    Enrichit un DataFrame tabulaire avec des caractéristiques statistiques.

    Pour chaque colonne numérique sélectionnée, cette fonction ajoute
    des statistiques descriptives par colonne (écart-type, min, max)
    et des caractéristiques d'interaction physiquement pertinentes :

    - **power** : puissance mécanique P = τ × ω = Torque × Speed × 2π/60
      La puissance mécanique est un indicateur clé de la charge du moteur.
      Un dépassement de la plage nominale indique une surcharge.

    - **temp_diff** : ΔT = T_process – T_air
      La différence de température est liée à l'efficacité de la
      dissipation thermique. Un ΔT trop faible à basse vitesse
      signale un risque de surchauffe (ventilation insuffisante).

    Parameters
    ----------
    df : pd.DataFrame
        DataFrame d'entrée (ex. issu de ``load_ai4i_dataset``).
    signal_columns : list[str] | None
        Colonnes sur lesquelles calculer les statistiques. Si ``None``,
        toutes les colonnes numériques sont utilisées.

    Returns
    -------
    pd.DataFrame
        DataFrame enrichi avec les caractéristiques supplémentaires.
    """
    df_enriched: pd.DataFrame = df.copy()

    # Sélection des colonnes numériques
    if signal_columns is None:
        numeric_cols: List[str] = df_enriched.select_dtypes(
            include=[np.number]
        ).columns.tolist()
    else:
        numeric_cols = [c for c in signal_columns if c in df_enriched.columns]

    logger.info(
        "Extraction de caractéristiques sur %d colonnes numériques : %s",
        len(numeric_cols),
        numeric_cols,
    )

    # ── Statistiques par colonne ────────────────────────────────────
    # Ajout de caractéristiques dérivées pour chaque colonne numérique
    for col in numeric_cols:
        col_values: np.ndarray = df_enriched[col].values.astype(np.float64)
        col_mean: float = float(np.mean(col_values))
        col_std: float = float(np.std(col_values, ddof=0))

        # Z-score normalisé : mesure l'écart de chaque observation
        # par rapport à la distribution globale (détection d'anomalies)
        if col_std > 0:
            df_enriched[f"{col}_zscore"] = (col_values - col_mean) / col_std
        else:
            df_enriched[f"{col}_zscore"] = 0.0

    # ── Caractéristiques d'interaction physique ─────────────────────

    # Identification des colonnes de vitesse et de couple
    speed_col: Optional[str] = None
    torque_col: Optional[str] = None
    air_temp_col: Optional[str] = None
    proc_temp_col: Optional[str] = None

    for col in df_enriched.columns:
        col_lower = col.lower()
        if "rotational" in col_lower or "speed" in col_lower:
            speed_col = col
        elif "torque" in col_lower:
            torque_col = col
        elif "air" in col_lower and "temp" in col_lower:
            air_temp_col = col
        elif "process" in col_lower and "temp" in col_lower:
            proc_temp_col = col

    # Puissance mécanique : P = τ × ω = Torque [N·m] × Speed [rad/s]
    if speed_col is not None and torque_col is not None:
        omega = df_enriched[speed_col].values * 2.0 * np.pi / 60.0
        df_enriched["power"] = df_enriched[torque_col].values * omega
        logger.info(
            "Caractéristique 'power' ajoutée (P = %s × %s × 2π/60)",
            torque_col,
            speed_col,
        )

    # Différence de température process – air
    if air_temp_col is not None and proc_temp_col is not None:
        df_enriched["temp_diff"] = (
            df_enriched[proc_temp_col].values - df_enriched[air_temp_col].values
        )
        logger.info(
            "Caractéristique 'temp_diff' ajoutée (ΔT = %s − %s)",
            proc_temp_col,
            air_temp_col,
        )

    # Produit couple × usure (indicateur de surcharge cumulée)
    tool_wear_col: Optional[str] = None
    for col in df_enriched.columns:
        if "tool" in col.lower() and "wear" in col.lower():
            tool_wear_col = col
            break

    if torque_col is not None and tool_wear_col is not None:
        df_enriched["torque_wear_product"] = (
            df_enriched[torque_col].values * df_enriched[tool_wear_col].values
        )
        logger.info("Caractéristique 'torque_wear_product' ajoutée.")

    n_new = df_enriched.shape[1] - df.shape[1]
    logger.info(
        "Extraction terminée : %d nouvelles caractéristiques ajoutées "
        "(total : %d).",
        n_new,
        df_enriched.shape[1],
    )
    return df_enriched


# ──────────────────────────────────────────────────────────────────────
# Point d'entrée pour tests rapides
# ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(levelname)s — %(message)s")

    # Test sur un signal sinusoïdal + bruit
    fs_test = 4000.0  # Hz
    t = np.linspace(0, 1, int(fs_test), endpoint=False)
    # Signal : sinusoïde 50 Hz + harmonique 3 à 150 Hz + bruit
    signal_test = (
        np.sin(2 * np.pi * 50 * t)
        + 0.5 * np.sin(2 * np.pi * 150 * t)
        + 0.1 * np.random.randn(len(t))
    )

    print("=== Test d'extraction de caractéristiques (signal) ===")
    features = extract_all_features(signal_test, fs=fs_test)
    for name, value in features.items():
        print(f"  {name:25s} : {value:.6f}")

    # Test sur un DataFrame tabulaire
    print("\n=== Test d'extraction de caractéristiques (DataFrame) ===")
    df_test = pd.DataFrame({
        "Air temperature [°C]": [22.0, 25.0, 23.5],
        "Process temperature [°C]": [32.0, 33.0, 31.5],
        "Rotational speed [rpm]": [1500.0, 1800.0, 1350.0],
        "Torque [Nm]": [40.0, 35.0, 55.0],
        "Tool wear [min]": [100, 200, 50],
    })
    df_enriched = extract_features_from_dataframe(df_test)
    print(f"Colonnes enrichies : {df_enriched.columns.tolist()}")
    print(df_enriched.head())
