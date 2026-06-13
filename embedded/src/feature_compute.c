/**
 * @file    feature_compute.c
 * @brief   Implémentation du module d'extraction de caractéristiques
 * @author  Alain Paluku — Mémoire de Master 2
 * @date    2026
 * @version 1.0
 *
 * @details Ce fichier implémente :
 *          1. Statistiques temporelles : RMS, kurtosis, facteur de crête,
 *             peak-to-peak, skewness, écart-type
 *          2. Analyse spectrale : FFT radix-2 Cooley-Tukey itérative,
 *             fréquence dominante, énergie spectrale
 *          3. Conversion ADC : PT100 (Wheatstone + INA125) → °C,
 *             ACS712-30A → Ampères
 *          4. Normalisation min-max pour l'entrée du classificateur
 *
 * @note    Implémentation en C pur sans dépendance CMSIS-DSP.
 *          Optimisée pour STM32 Cortex-M4 avec FPU simple précision.
 */

/* ========================================================================= */
/*                              INCLUDES                                     */
/* ========================================================================= */

#include "feature_compute.h"
#include <string.h>  /* Pour memset */
#include <float.h>   /* Pour FLT_EPSILON */

/* ========================================================================= */
/*                      VARIABLES STATIQUES INTERNES                         */
/* ========================================================================= */

/**
 * @brief Tampons de travail pour la FFT (parties réelle et imaginaire)
 * @note  Déclarés en statique pour éviter l'allocation sur la pile
 *        (2 × BUFFER_SIZE × sizeof(float) = 8 Ko)
 */
static float fft_real[BUFFER_SIZE];
static float fft_imag[BUFFER_SIZE];

/* ========================================================================= */
/*                    FONCTIONS UTILITAIRES INTERNES                         */
/* ========================================================================= */

/**
 * @brief  Calcule la moyenne arithmétique d'un tableau
 * @param  buffer  Tableau de valeurs flottantes
 * @param  length  Nombre d'éléments
 * @retval Moyenne arithmétique
 */
static float compute_mean(const float* buffer, uint16_t length)
{
    float sum = 0.0f;
    uint16_t i;

    for (i = 0U; i < length; i++)
    {
        sum += buffer[i];
    }

    return sum / (float)length;
}

/**
 * @brief  Trouve la valeur absolue maximale d'un tableau
 * @param  buffer  Tableau de valeurs flottantes
 * @param  length  Nombre d'éléments
 * @retval Valeur absolue maximale
 */
static float compute_abs_max(const float* buffer, uint16_t length)
{
    float max_val = 0.0f;
    float abs_val;
    uint16_t i;

    for (i = 0U; i < length; i++)
    {
        abs_val = fabsf(buffer[i]);
        if (abs_val > max_val)
        {
            max_val = abs_val;
        }
    }

    return max_val;
}

/**
 * @brief  Trouve les valeurs minimale et maximale d'un tableau
 * @param  buffer  Tableau de valeurs flottantes
 * @param  length  Nombre d'éléments
 * @param  min_out Pointeur pour la valeur minimale
 * @param  max_out Pointeur pour la valeur maximale
 */
static void compute_min_max(const float* buffer, uint16_t length,
                            float* min_out, float* max_out)
{
    float min_val = buffer[0];
    float max_val = buffer[0];
    uint16_t i;

    for (i = 1U; i < length; i++)
    {
        if (buffer[i] < min_val)
        {
            min_val = buffer[i];
        }
        if (buffer[i] > max_val)
        {
            max_val = buffer[i];
        }
    }

    *min_out = min_val;
    *max_out = max_val;
}

/**
 * @brief  Inversement de bits pour la permutation de la FFT
 * @param  x     Valeur à inverser
 * @param  log2n Nombre de bits significatifs
 * @retval Valeur avec bits inversés
 */
static uint16_t bit_reverse(uint16_t x, uint16_t log2n)
{
    uint16_t result = 0U;
    uint16_t i;

    for (i = 0U; i < log2n; i++)
    {
        result = (uint16_t)((result << 1U) | (x & 1U));
        x >>= 1U;
    }

    return result;
}

/**
 * @brief  Calcule log2 d'un entier (puissance de 2 uniquement)
 * @param  n  Entier positif, doit être une puissance de 2
 * @retval log2(n)
 */
static uint16_t compute_log2(uint16_t n)
{
    uint16_t result = 0U;
    uint16_t val = n;

    while (val > 1U)
    {
        val >>= 1U;
        result++;
    }

    return result;
}

/* ========================================================================= */
/*                      IMPLÉMENTATION DES FONCTIONS                         */
/* ========================================================================= */

/**
 * @brief  Initialise la structure de données des capteurs
 */
void Feature_Init(SensorData_t* data)
{
    if (data == NULL)
    {
        return;
    }

    /* Remise à zéro de tout le tampon de vibration */
    (void)memset(data->vibration_buffer, 0, sizeof(data->vibration_buffer));

    data->temperature  = 0.0f;
    data->current      = 0.0f;
    data->buffer_index = 0U;
    data->buffer_full  = 0U;
}

/**
 * @brief  Ajoute un échantillon de vibration au tampon circulaire
 */
void Feature_AddSample(SensorData_t* data, float vibration_sample)
{
    if (data == NULL)
    {
        return;
    }

    /* Stockage de l'échantillon dans le tampon */
    data->vibration_buffer[data->buffer_index] = vibration_sample;
    data->buffer_index++;

    /* Vérification de remplissage du tampon */
    if (data->buffer_index >= BUFFER_SIZE)
    {
        data->buffer_full  = 1U;
        data->buffer_index = 0U;  /* Réinitialisation pour le prochain cycle */
    }
}

/* ------------------------------------------------------------------------- */
/*                    CARACTÉRISTIQUES TEMPORELLES                           */
/* ------------------------------------------------------------------------- */

/**
 * @brief  Calcule la valeur efficace (RMS) d'un signal
 *
 * @details Formule : RMS = sqrt( (1/N) × Σ x[i]² )
 *          Boucle déroulée par paires pour réduire la surcharge de boucle.
 */
float Feature_ComputeRMS(const float* buffer, uint16_t length)
{
    float sum_sq = 0.0f;
    uint16_t i;

    if ((buffer == NULL) || (length == 0U))
    {
        return 0.0f;
    }

    /* Accumulation des carrés — déroulage par 4 pour performance */
    i = 0U;
    while ((i + 3U) < length)
    {
        sum_sq += buffer[i]     * buffer[i];
        sum_sq += buffer[i + 1U] * buffer[i + 1U];
        sum_sq += buffer[i + 2U] * buffer[i + 2U];
        sum_sq += buffer[i + 3U] * buffer[i + 3U];
        i += 4U;
    }

    /* Traitement des échantillons restants */
    while (i < length)
    {
        sum_sq += buffer[i] * buffer[i];
        i++;
    }

    return sqrtf(sum_sq / (float)length);
}

/**
 * @brief  Calcule le kurtosis (aplatissement) d'un signal
 *
 * @details Algorithme en deux passes pour la stabilité numérique :
 *          1ère passe : calcul de la moyenne
 *          2ème passe : calcul des moments centraux d'ordre 2 et 4
 *
 *          Kurtosis = μ₄ / μ₂² = E[(X-μ)⁴] / (E[(X-μ)²])²
 *
 *          Un kurtosis de 3.0 correspond à une distribution normale (mesokurtique).
 *          Kurtosis > 3 : signal impulsionnel (défaut de roulement).
 *          Kurtosis < 3 : signal uniforme.
 */
float Feature_ComputeKurtosis(const float* buffer, uint16_t length)
{
    float mean;
    float m2 = 0.0f;  /* Moment central d'ordre 2 (variance) */
    float m4 = 0.0f;  /* Moment central d'ordre 4 */
    float diff;
    float diff2;
    float variance;
    uint16_t i;

    if ((buffer == NULL) || (length < 4U))
    {
        return 0.0f;  /* Pas assez d'échantillons pour un kurtosis significatif */
    }

    /* 1ère passe : calcul de la moyenne */
    mean = compute_mean(buffer, length);

    /* 2ème passe : moments centraux */
    for (i = 0U; i < length; i++)
    {
        diff  = buffer[i] - mean;
        diff2 = diff * diff;
        m2   += diff2;
        m4   += diff2 * diff2;
    }

    m2 /= (float)length;
    m4 /= (float)length;

    /* Protection contre la division par zéro */
    variance = m2 * m2;
    if (variance < FLT_EPSILON)
    {
        return 0.0f;
    }

    return m4 / variance;
}

/**
 * @brief  Calcule le facteur de crête d'un signal
 *
 * @details Facteur de crête = max(|x|) / RMS
 *          Indicateur de l'impulsivité du signal.
 *          Valeur typique pour bruit gaussien : ~√2 ≈ 1.414
 *          Valeur élevée : présence de chocs (défaut mécanique).
 */
float Feature_ComputeCrestFactor(const float* buffer, uint16_t length)
{
    float rms;
    float peak;

    if ((buffer == NULL) || (length == 0U))
    {
        return 0.0f;
    }

    rms  = Feature_ComputeRMS(buffer, length);
    peak = compute_abs_max(buffer, length);

    /* Protection contre la division par zéro */
    if (rms < FLT_EPSILON)
    {
        return 0.0f;
    }

    return peak / rms;
}

/**
 * @brief  Calcule l'asymétrie (skewness) d'un signal
 *
 * @details Formule : skewness = μ₃ / σ³ = E[(X-μ)³] / (E[(X-μ)²])^(3/2)
 *
 *          Skewness = 0 : distribution symétrique
 *          Skewness > 0 : queue droite plus longue
 *          Skewness < 0 : queue gauche plus longue
 *
 *          Algorithme en deux passes comme pour le kurtosis.
 */
float Feature_ComputeSkewness(const float* buffer, uint16_t length)
{
    float mean;
    float m2 = 0.0f;  /* Moment central d'ordre 2 */
    float m3 = 0.0f;  /* Moment central d'ordre 3 */
    float diff;
    float diff2;
    float std_cubed;
    uint16_t i;

    if ((buffer == NULL) || (length < 3U))
    {
        return 0.0f;
    }

    /* 1ère passe : moyenne */
    mean = compute_mean(buffer, length);

    /* 2ème passe : moments centraux d'ordre 2 et 3 */
    for (i = 0U; i < length; i++)
    {
        diff  = buffer[i] - mean;
        diff2 = diff * diff;
        m2   += diff2;
        m3   += diff2 * diff;
    }

    m2 /= (float)length;
    m3 /= (float)length;

    /* std³ = (m2)^(3/2) */
    std_cubed = m2 * sqrtf(m2);

    /* Protection contre la division par zéro */
    if (std_cubed < FLT_EPSILON)
    {
        return 0.0f;
    }

    return m3 / std_cubed;
}

/* ------------------------------------------------------------------------- */
/*                    ANALYSE SPECTRALE (FFT)                                */
/* ------------------------------------------------------------------------- */

/**
 * @brief  Calcule la FFT et le spectre de magnitude
 *
 * @details Implémentation de l'algorithme de Cooley-Tukey DIT (Decimation In Time)
 *          radix-2, version itérative.
 *
 *          Étapes :
 *          1. Copie du signal d'entrée avec permutation bit-reversal
 *          2. Papillons (butterfly) pour chaque étage de la FFT
 *          3. Calcul du spectre de magnitude : |X(k)| = sqrt(Re² + Im²)
 *
 *          Complexité : O(N log N) avec N = length
 *
 * @warning Le paramètre length DOIT être une puissance de 2.
 *          Le tableau magnitude doit avoir une taille >= length/2.
 */
void Feature_ComputeFFT(const float* buffer, uint16_t length,
                        float* magnitude, float sample_rate)
{
    uint16_t log2n;
    uint16_t stage;
    uint16_t half_size;
    uint16_t full_size;
    uint16_t j, k;
    uint16_t idx_a, idx_b;
    float angle;
    float w_real, w_imag;
    float t_real, t_imag;
    uint16_t reversed;
    uint16_t half_len;

    (void)sample_rate;  /* Utilisé implicitement pour la résolution fréquentielle */

    if ((buffer == NULL) || (magnitude == NULL) || (length == 0U))
    {
        return;
    }

    log2n = compute_log2(length);

    /* --------------------------------------------------------------- */
    /* Étape 1 : Permutation bit-reversal et initialisation            */
    /* --------------------------------------------------------------- */
    for (j = 0U; j < length; j++)
    {
        reversed = bit_reverse(j, log2n);
        fft_real[j] = buffer[reversed];
        fft_imag[j] = 0.0f;
    }

    /* --------------------------------------------------------------- */
    /* Étape 2 : Papillons (butterfly) — itération sur les étages      */
    /* --------------------------------------------------------------- */
    for (stage = 1U; stage <= log2n; stage++)
    {
        half_size = (uint16_t)(1U << (stage - 1U));
        full_size = (uint16_t)(1U << stage);

        for (j = 0U; j < length; j += full_size)
        {
            for (k = 0U; k < half_size; k++)
            {
                /* Calcul du facteur de rotation (twiddle factor) */
                angle = -2.0f * (float)M_PI * (float)k / (float)full_size;
                w_real = cosf(angle);
                w_imag = sinf(angle);

                idx_a = j + k;
                idx_b = j + k + half_size;

                /* Multiplication complexe : T = W × X[idx_b] */
                t_real = (w_real * fft_real[idx_b]) - (w_imag * fft_imag[idx_b]);
                t_imag = (w_real * fft_imag[idx_b]) + (w_imag * fft_real[idx_b]);

                /* Papillon : combinaison des résultats */
                fft_real[idx_b] = fft_real[idx_a] - t_real;
                fft_imag[idx_b] = fft_imag[idx_a] - t_imag;
                fft_real[idx_a] = fft_real[idx_a] + t_real;
                fft_imag[idx_a] = fft_imag[idx_a] + t_imag;
            }
        }
    }

    /* --------------------------------------------------------------- */
    /* Étape 3 : Calcul du spectre de magnitude (moitié utile)         */
    /* --------------------------------------------------------------- */
    half_len = length / 2U;
    for (j = 0U; j < half_len; j++)
    {
        magnitude[j] = sqrtf((fft_real[j] * fft_real[j]) +
                             (fft_imag[j] * fft_imag[j]));
    }
}

/* ------------------------------------------------------------------------- */
/*                    CONVERSIONS ADC                                        */
/* ------------------------------------------------------------------------- */

/**
 * @brief  Convertit une valeur ADC 12 bits en température via PT100
 *
 * @details Chaîne de mesure :
 *          PT100 → Pont de Wheatstone → INA125 (gain=100) → ADC 12 bits
 *
 *          Formule inverse :
 *          1. V_adc = adc_value × (Vref / 4096)
 *          2. V_bridge = V_adc / Gain_INA125
 *          3. R_PT100 ≈ R0 × (1 + V_bridge / V_excitation)
 *             (approximation linéaire pour ponts légèrement déséquilibrés)
 *          4. T = (R_PT100 / R0 - 1) / alpha
 *
 *          Plage typique : -50°C à +500°C
 */
float Feature_ADCToTemperature(uint16_t adc_value)
{
    float v_adc;
    float v_bridge;
    float delta_r;
    float temperature;

    /* Conversion ADC → tension */
    v_adc = ((float)adc_value / (float)ADC_RESOLUTION) * ADC_VREF;

    /* Tension en sortie du pont avant amplification */
    v_bridge = v_adc / INA125_GAIN;

    /*
     * Pour un pont de Wheatstone avec une seule résistance variable (PT100) :
     * V_bridge ≈ V_excitation × ΔR / (4 × R0)  (approximation linéaire)
     * Avec V_excitation = I_excitation × R0 ≈ 1mA × 100Ω = 0.1V
     *
     * Donc : ΔR = V_bridge × 4 × R0 / V_excitation
     */
    float v_excitation = (BRIDGE_EXCITATION_MA / 1000.0f) * PT100_R0;

    if (v_excitation < FLT_EPSILON)
    {
        return 0.0f;
    }

    delta_r = (v_bridge * 4.0f * PT100_R0) / v_excitation;

    /* Conversion résistance → température (relation linéaire du PT100) */
    /* R = R0 × (1 + alpha × T)  →  T = ΔR / (R0 × alpha) */
    temperature = delta_r / (PT100_R0 * PT100_ALPHA);

    return temperature;
}

/**
 * @brief  Convertit une valeur ADC 12 bits en courant via ACS712-30A
 *
 * @details Chaîne de mesure :
 *          Courant statorique → ACS712-30A → Diviseur de tension (5V→3.3V) → ADC
 *
 *          Formule :
 *          1. V_adc = adc_value × (3.3V / 4096)
 *          2. V_sensor = V_adc / ratio_diviseur  (restauration tension 5V)
 *          3. I = (V_sensor - VCC/2) / Sensibilité
 *
 *          ACS712-30A : Sensibilité = 66 mV/A, Plage = ±30A
 *          VCC/2 = 2.5V (sortie à courant nul)
 */
float Feature_ADCToCurrent(uint16_t adc_value)
{
    float v_adc;
    float v_sensor;
    float current;

    /* Conversion ADC → tension */
    v_adc = ((float)adc_value / (float)ADC_RESOLUTION) * ADC_VREF;

    /* Restauration de la tension du capteur avant le diviseur */
    v_sensor = v_adc / ACS712_DIVIDER_RATIO;

    /* Conversion tension → courant */
    /* Offset à VCC/2 = 2.5V (5V / 2), pas ACS712_OFFSET_V qui est post-diviseur */
    current = (v_sensor - 2.5f) / ACS712_SENSITIVITY;

    return current;
}

/* ------------------------------------------------------------------------- */
/*                    EXTRACTION COMPLÈTE                                    */
/* ------------------------------------------------------------------------- */

/**
 * @brief  Extrait l'ensemble des 12 caractéristiques
 *
 * @details Orchestration de tous les calculs :
 *          1. Caractéristiques temporelles du signal de vibration
 *          2. FFT et caractéristiques fréquentielles
 *          3. Conversion des mesures ADC (température, courant)
 *          4. Estimations dérivées (puissance, vitesse RPM)
 */
void Feature_ExtractAll(const SensorData_t* data, float temperature_adc,
                        float current_adc, FeatureVector_t* features)
{
    float magnitude_spectrum[BUFFER_SIZE / 2U];
    float min_val, max_val;
    float mean;
    float std_dev;
    float max_magnitude;
    uint16_t dominant_bin;
    float freq_resolution;
    float spectral_energy;
    uint16_t half_len;
    uint16_t i;

    if ((data == NULL) || (features == NULL))
    {
        return;
    }

    /* ================================================================= */
    /* 1. CARACTÉRISTIQUES TEMPORELLES DU SIGNAL DE VIBRATION            */
    /* ================================================================= */

    /* RMS — Valeur efficace */
    features->vibration_rms = Feature_ComputeRMS(data->vibration_buffer,
                                                  BUFFER_SIZE);

    /* Kurtosis — Aplatissement */
    features->vibration_kurtosis = Feature_ComputeKurtosis(data->vibration_buffer,
                                                            BUFFER_SIZE);

    /* Facteur de crête */
    features->vibration_crest_factor = Feature_ComputeCrestFactor(
                                          data->vibration_buffer, BUFFER_SIZE);

    /* Peak-to-Peak — Étendue crête-à-crête */
    compute_min_max(data->vibration_buffer, BUFFER_SIZE, &min_val, &max_val);
    features->vibration_peak_to_peak = max_val - min_val;

    /* Skewness — Asymétrie */
    features->vibration_skewness = Feature_ComputeSkewness(data->vibration_buffer,
                                                            BUFFER_SIZE);

    /* Écart-type */
    mean = compute_mean(data->vibration_buffer, BUFFER_SIZE);
    std_dev = 0.0f;
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        float diff = data->vibration_buffer[i] - mean;
        std_dev += diff * diff;
    }
    std_dev = sqrtf(std_dev / (float)BUFFER_SIZE);
    features->vibration_std = std_dev;

    /* ================================================================= */
    /* 2. CARACTÉRISTIQUES FRÉQUENTIELLES (FFT)                          */
    /* ================================================================= */

    /* Calcul de la FFT et du spectre de magnitude */
    Feature_ComputeFFT(data->vibration_buffer, BUFFER_SIZE,
                       magnitude_spectrum, (float)SAMPLE_RATE_HZ);

    half_len = BUFFER_SIZE / 2U;
    freq_resolution = (float)SAMPLE_RATE_HZ / (float)BUFFER_SIZE;

    /* Recherche de la fréquence dominante (on ignore le bin DC, index 0) */
    max_magnitude = 0.0f;
    dominant_bin   = 1U;
    for (i = 1U; i < half_len; i++)
    {
        if (magnitude_spectrum[i] > max_magnitude)
        {
            max_magnitude = magnitude_spectrum[i];
            dominant_bin  = i;
        }
    }
    features->vibration_dominant_freq = (float)dominant_bin * freq_resolution;

    /* Énergie spectrale : somme de |X(k)|² */
    spectral_energy = 0.0f;
    for (i = 0U; i < half_len; i++)
    {
        spectral_energy += magnitude_spectrum[i] * magnitude_spectrum[i];
    }
    features->vibration_spectral_energy = spectral_energy;

    /* ================================================================= */
    /* 3. CONVERSION DES MESURES ADC                                     */
    /* ================================================================= */

    /* Température : PT100 via pont de Wheatstone + INA125 */
    features->temperature_celsius = Feature_ADCToTemperature((uint16_t)temperature_adc);

    /* Courant statorique : ACS712-30A */
    features->current_rms = Feature_ADCToCurrent((uint16_t)current_adc);
    /* Note : pour un vrai RMS du courant AC, il faudrait un tampon dédié */
    /* Ici on utilise la valeur instantanée comme approximation */
    features->current_rms = fabsf(features->current_rms);

    /* ================================================================= */
    /* 4. CARACTÉRISTIQUES DÉRIVÉES                                      */
    /* ================================================================= */

    /*
     * Vitesse RPM estimée à partir de la fréquence fondamentale de vibration
     * Pour un moteur à p paires de pôles :
     *   RPM = (f_fondamentale × 60) / p
     */
    features->speed_rpm = (features->vibration_dominant_freq * 60.0f) /
                          (float)MOTOR_POLE_PAIRS;

    /*
     * Estimation de puissance mécanique approximative
     * P ≈ V_nominal × I_rms × cos(φ) × η
     * Hypothèse : V_nominal = 230V (monophasé), cos(φ) ≈ 0.85, η ≈ 0.9
     * Simplification : P ≈ 230 × I × 0.85 × 0.9 ≈ 175.95 × I
     */
    features->power_estimate = 175.95f * features->current_rms;
}

/* ------------------------------------------------------------------------- */
/*                    NORMALISATION MIN-MAX                                  */
/* ------------------------------------------------------------------------- */

/**
 * @brief  Normalise le vecteur de caractéristiques par min-max scaling
 *
 * @details Formule : x_norm = (x - x_min) / (x_max - x_min)
 *
 *          Les valeurs min et max doivent provenir du jeu de données
 *          d'entraînement (calculées dans le script Python).
 *          Si max == min (écart nul), la caractéristique est mise à 0.
 *
 *          L'accès au vecteur de caractéristiques se fait par pointeur
 *          reinterprété en tableau de float, car la structure FeatureVector_t
 *          est composée de NUM_FEATURES floats contigus.
 */
void Feature_Normalize(FeatureVector_t* features, const float* min_vals,
                       const float* max_vals)
{
    float* feature_array;
    float range;
    uint16_t i;

    if ((features == NULL) || (min_vals == NULL) || (max_vals == NULL))
    {
        return;
    }

    /*
     * Réinterprétation de la structure comme tableau de float.
     * Ceci est valide car FeatureVector_t ne contient que des float
     * et est définie sans padding (12 × float = 48 octets).
     */
    feature_array = (float*)features;

    for (i = 0U; i < NUM_FEATURES; i++)
    {
        range = max_vals[i] - min_vals[i];

        if (range > FLT_EPSILON)
        {
            feature_array[i] = (feature_array[i] - min_vals[i]) / range;
        }
        else
        {
            /* Écart nul : caractéristique constante → normaliser à 0 */
            feature_array[i] = 0.0f;
        }

        /* Clamp à [0, 1] pour protéger contre les valeurs hors distribution */
        if (feature_array[i] < 0.0f)
        {
            feature_array[i] = 0.0f;
        }
        else if (feature_array[i] > 1.0f)
        {
            feature_array[i] = 1.0f;
        }
        else
        {
            /* Valeur dans la plage — pas de correction nécessaire */
        }
    }
}
