/**
 * @file    feature_compute.h
 * @brief   Module d'extraction de caractéristiques pour la maintenance prédictive
 * @author  Alain Paluku
 * @version 1.1
 */

#ifndef FEATURE_COMPUTE_H
#define FEATURE_COMPUTE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

#define BUFFER_SIZE         1024U
#define NUM_FEATURES        12U
#define SAMPLE_RATE_HZ      4000U
#define ADC_RESOLUTION      4096U
#define ADC_VREF            3.3f
#define MOTOR_POLE_PAIRS    2U

#ifndef M_PI
#define M_PI                3.14159265358979323846f
#endif

/* Paramètres PT100 */
#define PT100_R0            100.0f
#define PT100_ALPHA         0.00385f
#define INA125_GAIN         100.0f
#define BRIDGE_EXCITATION_MA  1.0f

/* Paramètres ACS712-30A */
#define ACS712_SENSITIVITY  0.066f
#define ACS712_OFFSET_V     1.65f
#define ACS712_DIVIDER_RATIO  (3.3f / 5.0f)

typedef struct
{
    float    vibration_buffer[BUFFER_SIZE];
    float    temperature;
    float    current;
    uint16_t buffer_index;
    uint8_t  buffer_full;
} SensorData_t;

typedef struct
{
    float vibration_rms;           /* [0] */
    float vibration_kurtosis;      /* [1] */
    float vibration_crest_factor;  /* [2] */
    float vibration_peak_to_peak;  /* [3] */
    float vibration_skewness;      /* [4] */
    float vibration_std;           /* [5] */
    float vibration_dominant_freq;  /* [6] */
    float vibration_spectral_energy;/* [7] */
    float temperature_celsius;     /* [8] */
    float current_rms;             /* [9] */
    float power_estimate;          /* [10] */
    float speed_rpm;               /* [11] */
} FeatureVector_t;

void Feature_Init(SensorData_t* data);
void Feature_AddSample(SensorData_t* data, float vibration_sample);
void Feature_ExtractAll(const SensorData_t* data, float temperature_adc,
                        float current_adc, FeatureVector_t* features);
float Feature_ComputeRMS(const float* buffer, uint16_t length);
float Feature_ComputeKurtosis(const float* buffer, uint16_t length);
float Feature_ComputeCrestFactor(const float* buffer, uint16_t length);
float Feature_ComputeSkewness(const float* buffer, uint16_t length);
void Feature_ComputeFFT(const float* buffer, uint16_t length,
                        float* magnitude, float sample_rate);
float Feature_ADCToTemperature(uint16_t adc_value);
float Feature_ADCToCurrent(uint16_t adc_value);
void Feature_Normalize(FeatureVector_t* features, const float* min_vals,
                       const float* max_vals);

#ifdef __cplusplus
}
#endif

#endif
