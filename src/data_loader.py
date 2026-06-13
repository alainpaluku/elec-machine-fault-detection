# -*- coding: utf-8 -*-
"""
Module de chargement et de prétraitement des jeux de données.

Ce module fournit des fonctions pour charger les données de maintenance
prédictive à partir de différentes sources :
  - AI4I 2020 Predictive Maintenance Dataset (données tabulaires)
  - CWRU Bearing Dataset (signaux vibratoires .mat)
  - Données synthétiques générées selon des modèles physiques réalistes

Les défaillances modélisées correspondent aux modes de défaut réels
des machines électriques tournantes : usure d'outil (TWF), dissipation
thermique (HDF), défaillance de puissance (PWF), surcharge (OSF) et
défaillance aléatoire (RNF).

Auteur : Alain Paluku
Projet : TFE Master 2 — Maintenance prédictive des machines électriques
"""

from __future__ import annotations

import logging
from pathlib import Path
from typing import Dict, Optional, Tuple, Union

import numpy as np
import pandas as pd

logger = logging.getLogger(__name__)

# ──────────────────────────────────────────────────────────────────────
# Mapping des colonnes de défaillance vers des classes entières
# 0 = Normal, 1 = TWF, 2 = HDF, 3 = PWF, 4 = OSF, 5 = RNF
# ──────────────────────────────────────────────────────────────────────
FAILURE_COLUMNS: list[str] = ["TWF", "HDF", "PWF", "OSF", "RNF"]
CLASS_NAMES: list[str] = ["Normal", "TWF", "HDF", "PWF", "OSF", "RNF"]

# Mapping du type de produit vers un entier ordinal (qualité croissante)
PRODUCT_TYPE_MAP: dict[str, int] = {"L": 0, "M": 1, "H": 2}


def _map_failure_to_class(row: pd.Series) -> int:
    """
    Convertit les colonnes binaires de défaillance en une classe unique.

    Stratégie : on attribue la première défaillance active trouvée dans
    l'ordre de priorité TWF → HDF → PWF → OSF → RNF. Si aucune
    défaillance n'est active, la classe est 0 (Normal).

    Parameters
    ----------
    row : pd.Series
        Ligne du DataFrame contenant les colonnes de défaillance.

    Returns
    -------
    int
        Identifiant de classe (0–5).
    """
    for idx, col in enumerate(FAILURE_COLUMNS, start=1):
        if row.get(col, 0) == 1:
            return idx
    return 0


def load_ai4i_dataset(
    filepath: Union[str, Path],
) -> Tuple[pd.DataFrame, pd.Series]:
    """
    Charge le jeu de données AI4I 2020 Predictive Maintenance.

    Le fichier CSV attendu contient 14 colonnes :
      UDI, Product ID, Type, Air temperature [K], Process temperature [K],
      Rotational speed [rpm], Torque [Nm], Tool wear [min],
      Machine failure, TWF, HDF, PWF, OSF, RNF

    Prétraitements appliqués :
      1. Suppression des colonnes identifiantes (UDI, Product ID).
      2. Encodage ordinal du type de produit (L=0, M=1, H=2).
      3. Conversion des températures de Kelvin en degrés Celsius.
      4. Construction de la cible multiclasse à partir des colonnes binaires.

    Parameters
    ----------
    filepath : str | Path
        Chemin vers le fichier CSV du dataset AI4I 2020.

    Returns
    -------
    X : pd.DataFrame
        Matrice de caractéristiques prétraitée.
    y : pd.Series
        Vecteur cible multiclasse (0=Normal … 5=RNF).

    Raises
    ------
    FileNotFoundError
        Si le fichier spécifié n'existe pas.
    ValueError
        Si des colonnes attendues sont manquantes.
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise FileNotFoundError(
            f"Le fichier de données est introuvable : {filepath}"
        )

    logger.info("Chargement du dataset AI4I depuis %s", filepath)
    df: pd.DataFrame = pd.read_csv(filepath)

    # ── Vérification des colonnes requises ──────────────────────────
    required_cols = {
        "Type",
        "Air temperature [K]",
        "Process temperature [K]",
        "Rotational speed [rpm]",
        "Torque [Nm]",
        "Tool wear [min]",
        "Machine failure",
    } | set(FAILURE_COLUMNS)

    missing = required_cols - set(df.columns)
    if missing:
        raise ValueError(
            f"Colonnes manquantes dans le fichier CSV : {missing}"
        )

    # ── Construction de la cible multiclasse ────────────────────────
    y: pd.Series = df.apply(_map_failure_to_class, axis=1).astype(int)
    y.name = "failure_class"

    # ── Suppression des colonnes non prédictives ────────────────────
    cols_to_drop = ["UDI", "Product ID", "Machine failure"] + FAILURE_COLUMNS
    cols_to_drop = [c for c in cols_to_drop if c in df.columns]
    df = df.drop(columns=cols_to_drop)

    # ── Encodage du type de produit ─────────────────────────────────
    if "Type" in df.columns:
        df["Type"] = df["Type"].map(PRODUCT_TYPE_MAP)
        if df["Type"].isna().any():
            logger.warning(
                "Valeurs de 'Type' non reconnues détectées — remplacées par 1 (M)."
            )
            df["Type"] = df["Type"].fillna(1).astype(int)

    # ── Conversion Kelvin → Celsius ─────────────────────────────────
    # T(°C) = T(K) - 273.15
    for col in ["Air temperature [K]", "Process temperature [K]"]:
        if col in df.columns:
            new_col = col.replace("[K]", "[°C]")
            df[new_col] = df[col] - 273.15
            df = df.drop(columns=[col])

    logger.info(
        "Dataset chargé : %d échantillons, %d caractéristiques, %d classes",
        len(df),
        df.shape[1],
        y.nunique(),
    )
    return df, y


def load_cwru_dataset(
    directory: Union[str, Path],
) -> Dict[str, np.ndarray]:
    """
    Charge les fichiers .mat du dataset CWRU Bearing.

    Le Case Western Reserve University Bearing Dataset contient des
    signaux vibratoires acquis par des accéléromètres placés sur le
    palier côté entraînement (Drive End — DE) d'un moteur asynchrone.

    La fréquence d'échantillonnage typique est de 12 kHz ou 48 kHz.
    Seul le canal *DE_time* (accéléromètre Drive End, domaine temporel)
    est extrait.

    Parameters
    ----------
    directory : str | Path
        Répertoire contenant les fichiers .mat du CWRU.

    Returns
    -------
    dict[str, np.ndarray]
        Dictionnaire {label: signal_array} où *label* est dérivé du
        nom du fichier (ex. ``"097_DE"``).

    Raises
    ------
    FileNotFoundError
        Si le répertoire n'existe pas.
    ImportError
        Si scipy n'est pas installé.
    """
    try:
        from scipy.io import loadmat
    except ImportError as exc:
        raise ImportError(
            "scipy est requis pour charger les fichiers .mat. "
            "Installez-le via : pip install scipy"
        ) from exc

    directory = Path(directory)
    if not directory.is_dir():
        raise FileNotFoundError(
            f"Le répertoire CWRU est introuvable : {directory}"
        )

    mat_files = sorted(directory.glob("*.mat"))
    if not mat_files:
        logger.warning("Aucun fichier .mat trouvé dans %s", directory)
        return {}

    data: Dict[str, np.ndarray] = {}

    for mat_path in mat_files:
        try:
            mat_content = loadmat(str(mat_path))
        except Exception as e:
            logger.error("Erreur lors du chargement de %s : %s", mat_path, e)
            continue

        # Recherche de la clé contenant 'DE_time' (accéléromètre Drive End)
        de_key: Optional[str] = None
        for key in mat_content:
            if "DE_time" in key:
                de_key = key
                break

        if de_key is None:
            logger.warning(
                "Aucune clé 'DE_time' trouvée dans %s — fichier ignoré.",
                mat_path.name,
            )
            continue

        # Extraction du signal vibratoire (vecteur 1D)
        signal: np.ndarray = mat_content[de_key].flatten().astype(np.float64)
        label: str = mat_path.stem  # Nom du fichier sans extension
        data[label] = signal
        logger.debug(
            "Fichier %s chargé : %d points (%s)",
            mat_path.name,
            len(signal),
            de_key,
        )

    logger.info(
        "Dataset CWRU chargé : %d fichiers, %d signaux extraits.",
        len(mat_files),
        len(data),
    )
    return data


def generate_synthetic_dataset(
    n_samples: int = 5000,
    random_state: int = 42,
) -> Tuple[pd.DataFrame, pd.Series]:
    """
    Génère un jeu de données synthétique réaliste imitant le AI4I 2020.

    Les paramètres physiques sont générés selon des distributions
    réalistes pour un moteur électrique industriel :
      - Température ambiante : 295–305 K (uniforme)
      - Température process : ambiante + 10 K ± bruit
      - Vitesse de rotation : 1200–2800 tr/min (normale tronquée)
      - Couple : 3–80 N·m (selon le type de produit)
      - Usure de l'outil : 0–250 min (uniforme)

    Les modes de défaillance sont déterminés par des conditions
    physiques réalistes :
      - TWF (Tool Wear Failure) : usure > 200 min + aléa
      - HDF (Heat Dissipation Failure) : ΔT < 8.6 K et vitesse < 1380 rpm
      - PWF (Power Failure) : puissance hors limites [3500, 9000] W
      - OSF (Overstrain Failure) : couple élevé × usure élevée
      - RNF (Random Failure) : probabilité 0.1 %

    Parameters
    ----------
    n_samples : int
        Nombre d'échantillons à générer (défaut : 5000).
    random_state : int
        Graine pour la reproductibilité (défaut : 42).

    Returns
    -------
    X : pd.DataFrame
        Matrice de caractéristiques (même format que ``load_ai4i_dataset``).
    y : pd.Series
        Vecteur cible multiclasse (0=Normal … 5=RNF).
    """
    rng = np.random.default_rng(random_state)
    logger.info(
        "Génération de %d échantillons synthétiques (seed=%d)",
        n_samples,
        random_state,
    )

    # ── Type de produit (L=50%, M=30%, H=20%) ──────────────────────
    product_types: np.ndarray = rng.choice(
        [0, 1, 2], size=n_samples, p=[0.5, 0.3, 0.2]
    )

    # ── Température ambiante [K] → [°C] ────────────────────────────
    # Distribution uniforme entre 295 et 305 K
    air_temp_k: np.ndarray = rng.uniform(295.0, 305.0, size=n_samples)
    air_temp_c: np.ndarray = air_temp_k - 273.15  # Conversion en °C

    # ── Température du process [°C] ─────────────────────────────────
    # Écart nominal ≈ 10 K, avec un bruit gaussien σ=1 K
    process_temp_c: np.ndarray = air_temp_c + 10.0 + rng.normal(0, 1.0, n_samples)

    # ── Vitesse de rotation [tr/min] ────────────────────────────────
    # Distribution normale centrée à 1538 rpm, σ=200 rpm
    # Bornes physiques : [1200, 2860] rpm
    rot_speed: np.ndarray = rng.normal(1538.0, 200.0, size=n_samples)
    rot_speed = np.clip(rot_speed, 1200.0, 2860.0)

    # ── Couple [N·m] ───────────────────────────────────────────────
    # Distribution normale centrée à 40 N·m, σ=10 N·m
    # Bornes : [3, 80] N·m
    torque: np.ndarray = rng.normal(40.0, 10.0, size=n_samples)
    torque = np.clip(torque, 3.0, 80.0)

    # ── Usure de l'outil [min] ──────────────────────────────────────
    # Dépend du type de produit : L→plus d'usure, H→moins
    tool_wear_base: np.ndarray = rng.uniform(0, 250, size=n_samples)
    # Les produits de haute qualité (H=2) s'usent moins vite
    type_wear_factor = np.where(product_types == 0, 1.0,
                                np.where(product_types == 1, 0.8, 0.6))
    tool_wear: np.ndarray = (tool_wear_base * type_wear_factor).astype(float)

    # ── Calcul des grandeurs physiques dérivées ─────────────────────
    # Puissance mécanique P = τ × ω = Couple [N·m] × Vitesse [rad/s]
    omega: np.ndarray = rot_speed * 2.0 * np.pi / 60.0  # rad/s
    power: np.ndarray = torque * omega  # Watts

    # Différence de température process – air
    delta_temp: np.ndarray = process_temp_c - air_temp_c

    # ══════════════════════════════════════════════════════════════════
    # Détermination des modes de défaillance (physique réaliste)
    # ══════════════════════════════════════════════════════════════════
    y_array: np.ndarray = np.zeros(n_samples, dtype=int)

    # 1. TWF — Usure de l'outil excessive
    # L'outil casse quand l'usure dépasse ~200 min (avec une probabilité)
    twf_mask: np.ndarray = (tool_wear > 200.0) & (
        rng.random(n_samples) < 0.3
    )

    # 2. HDF — Défaut de dissipation thermique
    # Se produit quand ΔT < 8.6 K ET que la vitesse est basse (< 1380 rpm)
    # La faible vitesse réduit la ventilation du moteur
    hdf_mask: np.ndarray = (delta_temp < 8.6) & (rot_speed < 1380.0)

    # 3. PWF — Défaillance de puissance
    # La puissance doit rester dans la plage [3500, 9000] W
    # Hors de ces limites, le variateur de fréquence déclenche une alarme
    pwf_mask: np.ndarray = (power < 3500.0) | (power > 9000.0)

    # 4. OSF — Surcharge (Overstrain Failure)
    # Se produit quand le couple est élevé ET l'usure est avancée
    # Pour les produits L : seuil couple > 55 N·m
    # Pour les produits M : seuil couple > 60 N·m
    # Pour les produits H : seuil couple > 65 N·m
    osf_thresholds = np.where(
        product_types == 0, 55.0,
        np.where(product_types == 1, 60.0, 65.0)
    )
    osf_mask: np.ndarray = (torque > osf_thresholds) & (tool_wear > 150.0)

    # 5. RNF — Défaillance aléatoire (Random Failure)
    # Probabilité de 0.1 % (événement rare)
    rnf_mask: np.ndarray = rng.random(n_samples) < 0.001

    # Application des défaillances par ordre de priorité
    # (même logique que _map_failure_to_class)
    y_array[twf_mask] = 1  # TWF
    # HDF n'écrase pas TWF
    y_array[(y_array == 0) & hdf_mask] = 2  # HDF
    y_array[(y_array == 0) & pwf_mask] = 3  # PWF
    y_array[(y_array == 0) & osf_mask] = 4  # OSF
    y_array[(y_array == 0) & rnf_mask] = 5  # RNF

    # ── Construction du DataFrame ───────────────────────────────────
    X = pd.DataFrame(
        {
            "Type": product_types,
            "Air temperature [°C]": air_temp_c,
            "Process temperature [°C]": process_temp_c,
            "Rotational speed [rpm]": rot_speed,
            "Torque [Nm]": torque,
            "Tool wear [min]": tool_wear,
        }
    )

    y = pd.Series(y_array, name="failure_class")

    # ── Statistiques de la génération ───────────────────────────────
    for cls_id, cls_name in enumerate(CLASS_NAMES):
        count = (y == cls_id).sum()
        pct = 100.0 * count / n_samples
        logger.info("  Classe %d (%s) : %d échantillons (%.1f%%)", cls_id, cls_name, count, pct)

    logger.info(
        "Dataset synthétique généré : %d échantillons, %d caractéristiques.",
        n_samples,
        X.shape[1],
    )
    return X, y


# ──────────────────────────────────────────────────────────────────────
# Point d'entrée pour tests rapides
# ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(levelname)s — %(message)s")

    print("=== Test de génération de données synthétiques ===")
    X_syn, y_syn = generate_synthetic_dataset(n_samples=2000)
    print(f"X shape: {X_syn.shape}")
    print(f"y distribution:\n{y_syn.value_counts().sort_index()}")
    print(f"\nAperçu des données :\n{X_syn.head()}")
