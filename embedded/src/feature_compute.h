/**
 * @file    feature_compute.h
 * @brief   Module d'extraction de caractéristiques pour la maintenance prédictive
 * @author  Alain Paluku — Mémoire de Master 2
 * @date    2026
 * @version 1.0
 *
 * @details Ce module implémente l'extraction de 12 caractéristiques à partir
 *          des signaux de vibration (accéléromètre ADXL345), température (PT100)
 *          et courant statorique (ACS712-30A) pour l'alimentation d'un
 *          classificateur Random Forest embarqué sur STM32.
 *
 *          Caractéristiques extraites :
 *          - Domaine temporel : RMS, kurtosis, facteur de crête, peak-to-peak,
 *            asymétrie (skewness), écart-type
 *          - Domaine fréquentiel : fréquence dominante, énergie spectrale
 *          - Thermique : température en °C
 *          - Électrique : courant RMS, puissance estimée, vitesse (RPM)
 *
 * @note    Compatible STM32 HAL. Arithmétique flottante simple précision (float).
 *          Pas de dépendance CMSIS-DSP pour portabilité maximale.
 */

#ifndef FEATURE_COMPUTE_H
#define FEATURE_COMPUTE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/*                              INCLUDES                                     */
/* ========================================================================= */

#include <stdint.h>
#include <math.h>

/* ========================================================================= */
/*                          CONSTANTES & MACROS                              */
/* ========================================================================= */

/** @brief Taille du tampon circulaire d'échantillons vibration (doit être puissance de 2 pour FFT) */
#define BUFFER_SIZE         1024U

/** @brief Nombre total de caractéristiques extraites */
#define NUM_FEATURES        12U

/** @brief Fréquence d'échantillonnage en Hz (Timer2 déclenche l'ADC à cette cadence) */
#define SAMPLE_RATE_HZ      4000U

/** @brief Résolution de l'ADC STM32 (12 bits) */
#define ADC_RESOLUTION      4096U

/** @brief Tension de référence de l'ADC en volts */
#define ADC_VREF            3.3f

/** @brief Nombre de paires de pôles du moteur (pour conversion freq → RPM) */
#define MOTOR_POLE_PAIRS    2U

/** @brief Valeur PI pour les calculs trigonométriques */
#ifndef M_PI
#define M_PI                3.14159265358979323846f
#endif

/* --- Paramètres du capteur PT100 avec pont de Wheatstone + INA125 --- */

/** @brief Résistance nominale du PT100 à 0°C (ohms) */
#define PT100_R0            100.0f

/** @brief Coefficient de température du platine (°C⁻¹) */
#define PT100_ALPHA         0.00385f

/** @brief Gain de l'amplificateur d'instrumentation INA125 */
#define INA125_GAIN         100.0f

/** @brief Courant d'excitation du pont de Wheatstone (A) */
#define BRIDGE_EXCITATION_MA  1.0f

/* --- Paramètres du capteur de courant ACS712-30A --- */

/** @brief Sensibilité du ACS712-30A (V/A) */
#define ACS712_SENSITIVITY  0.066f

/** @brief Tension de sortie à courant nul (VCC/2) adaptée au diviseur 5V→3.3V */
#define ACS712_OFFSET_V     1.65f

/** @brief Rapport du diviseur de tension pour adapter 5V → 3.3V */
#define ACS712_DIVIDER_RATIO  (3.3f / 5.0f)

/* ========================================================================= */
/*                        STRUCTURES DE DONNÉES                              */
/* ========================================================================= */

/**
 * @brief Structure de données brutes des capteurs
 *
 * @details Contient le tampon circulaire des échantillons de vibration
 *          ainsi que les dernières valeurs de température et courant.
 *          Le tampon est rempli par l'ISR DMA de l'ADC.
 */
typedef struct
{
    float    vibration_buffer[BUFFER_SIZE]; /**< Tampon circulaire des échantillons de vibration (g)   */
    float    temperature;                   /**< Dernière valeur brute de température (ADC counts)     */
    float    current;                       /**< Dernière valeur brute de courant (ADC counts)         */
    uint16_t buffer_index;                  /**< Index courant dans le tampon circulaire               */
    uint8_t  buffer_full;                   /**< Drapeau : 1 si le tampon est plein et prêt à traiter  */
} SensorData_t;

/**
 * @brief Vecteur de 12 caractéristiques extraites
 *
 * @details Ce vecteur constitue l'entrée du classificateur Random Forest.
 *          L'ordre des champs correspond à l'ordre d'entraînement du modèle Python.
 */
typedef struct
{
    /* --- Caractéristiques temporelles de vibration --- */
    float vibration_rms;           /**< Valeur efficace (RMS) du signal de vibration        */
    float vibration_kurtosis;      /**< Kurtosis : mesure de l'aplatissement du signal      */
    float vibration_crest_factor;  /**< Facteur de crête : max(|x|) / RMS                  */
    float vibration_peak_to_peak;  /**< Étendue crête-à-crête du signal                    */
    float vibration_skewness;      /**< Asymétrie (skewness) du signal                     */
    float vibration_std;           /**< Écart-type du signal de vibration                  */

    /* --- Caractéristiques fréquentielles de vibration --- */
    float vibration_dominant_freq;  /**< Fréquence dominante du spectre (Hz)               */
    float vibration_spectral_energy;/**< Énergie spectrale totale (somme |X(k)|²)          */

    /* --- Caractéristiques thermiques --- */
    float temperature_celsius;     /**< Température mesurée par PT100 (°C)                 */

    /* --- Caractéristiques électriques --- */
    float current_rms;             /**< Courant statorique RMS (A)                         */
    float power_estimate;          /**< Estimation de puissance (W) = V_nominal × I × cos(φ) */
    float speed_rpm;               /**< Vitesse de rotation estimée (RPM)                  */
} FeatureVector_t;

/* ========================================================================= */
/*                        PROTOTYPES DE FONCTIONS                            */
/* ========================================================================= */

/**
 * @brief  Initialise la structure de données des capteurs
 * @param  data  Pointeur vers la structure SensorData_t à initialiser
 * @retval Aucun
 */
void Feature_Init(SensorData_t* data);

/**
 * @brief  Ajoute un échantillon de vibration au tampon circulaire
 * @param  data              Pointeur vers la structure SensorData_t
 * @param  vibration_sample  Échantillon de vibration en unités g (accélération)
 * @retval Aucun
 * @note   Positionne le drapeau buffer_full lorsque BUFFER_SIZE échantillons
 *         ont été collectés
 */
void Feature_AddSample(SensorData_t* data, float vibration_sample);

/**
 * @brief  Extrait l'ensemble des 12 caractéristiques à partir des données brutes
 * @param  data            Pointeur vers les données capteurs (tampon vibration rempli)
 * @param  temperature_adc Valeur brute ADC 12 bits du canal température
 * @param  current_adc     Valeur brute ADC 12 bits du canal courant
 * @param  features        Pointeur vers le vecteur de caractéristiques à remplir
 * @retval Aucun
 */
void Feature_ExtractAll(const SensorData_t* data, float temperature_adc,
                        float current_adc, FeatureVector_t* features);

/**
 * @brief  Calcule la valeur efficace (RMS) d'un signal
 * @param  buffer  Tableau d'échantillons flottants
 * @param  length  Nombre d'échantillons
 * @retval Valeur RMS du signal (>= 0)
 */
float Feature_ComputeRMS(const float* buffer, uint16_t length);

/**
 * @brief  Calcule le kurtosis (aplatissement) d'un signal
 * @param  buffer  Tableau d'échantillons flottants
 * @param  length  Nombre d'échantillons
 * @retval Kurtosis du signal (3.0 pour une distribution normale)
 * @note   Algorithme en deux passes pour la stabilité numérique
 */
float Feature_ComputeKurtosis(const float* buffer, uint16_t length);

/**
 * @brief  Calcule le facteur de crête d'un signal
 * @param  buffer  Tableau d'échantillons flottants
 * @param  length  Nombre d'échantillons
 * @retval Facteur de crête = max(|x|) / RMS
 */
float Feature_ComputeCrestFactor(const float* buffer, uint16_t length);

/**
 * @brief  Calcule l'asymétrie (skewness) d'un signal
 * @param  buffer  Tableau d'échantillons flottants
 * @param  length  Nombre d'échantillons
 * @retval Skewness du signal (0.0 pour une distribution symétrique)
 */
float Feature_ComputeSkewness(const float* buffer, uint16_t length);

/**
 * @brief  Calcule la FFT et le spectre de magnitude d'un signal
 * @param  buffer      Tableau d'échantillons d'entrée (longueur = puissance de 2)
 * @param  length      Nombre d'échantillons (doit être puissance de 2)
 * @param  magnitude   Tableau de sortie pour le spectre de magnitude (longueur/2)
 * @param  sample_rate Fréquence d'échantillonnage en Hz
 * @retval Aucun
 * @note   Implémentation Cooley-Tukey DIT radix-2 itérative en C pur
 */
void Feature_ComputeFFT(const float* buffer, uint16_t length,
                        float* magnitude, float sample_rate);

/**
 * @brief  Convertit une valeur ADC 12 bits en température (°C) via PT100
 * @param  adc_value  Valeur brute ADC (0-4095)
 * @retval Température en degrés Celsius
 * @note   Hypothèse : pont de Wheatstone + INA125, Vref=3.3V
 */
float Feature_ADCToTemperature(uint16_t adc_value);

/**
 * @brief  Convertit une valeur ADC 12 bits en courant (A) via ACS712-30A
 * @param  adc_value  Valeur brute ADC (0-4095)
 * @retval Courant en ampères (signé, positif = sens direct)
 * @note   Capteur à effet Hall, sensibilité 66mV/A, offset VCC/2
 */
float Feature_ADCToCurrent(uint16_t adc_value);

/**
 * @brief  Normalise le vecteur de caractéristiques par min-max scaling
 * @param  features  Pointeur vers le vecteur de caractéristiques à normaliser
 * @param  min_vals  Tableau de NUM_FEATURES valeurs minimales (du jeu d'entraînement)
 * @param  max_vals  Tableau de NUM_FEATURES valeurs maximales (du jeu d'entraînement)
 * @retval Aucun
 * @note   Formule : x_norm = (x - min) / (max - min)
 *         Les min/max proviennent du prétraitement Python lors de l'entraînement
 */
void Feature_Normalize(FeatureVector_t* features, const float* min_vals,
                       const float* max_vals);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_COMPUTE_H */
