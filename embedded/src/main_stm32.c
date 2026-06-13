/**
 * @file main_stm32.c
 * @brief Point d'entrée principal pour le diagnostic embarqué (STM32)
 * @author Alain Paluku
 * 
 * Ce fichier montre comment intégrer les règles générées par Python (random_forest_rules.c)
 * ainsi que le module d'extraction de caractéristiques.
 */

#include "stm32f4xx_hal.h"
#include "feature_compute.h"
#include "rf_inference.h"
#include <stdio.h>
#include <string.h>

// Déclaration externe de la fonction d'inférence générée par le script Python
extern int predict_machine_state(const float* features);

// Handles pour les périphériques
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;

// Structures de données
SensorData_t sensor_data;
FeatureVector_t features;

// Prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);

void UART_Print(const char* str) {
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART2_UART_Init();

    Feature_Init(&sensor_data);

    UART_Print("=== SYSTÈME DE MAINTENANCE PRÉDICTIVE INITIALISÉ ===\r\n");

    while (1) {
        // 1. Acquisition (simulation simplifiée du remplissage du tampon)
        // Normalement, ceci est fait sous interruption DMA
        for(int i=0; i<BUFFER_SIZE; i++) {
            Feature_AddSample(&sensor_data, 0.2f); // Exemple de valeur
        }

        if (sensor_data.buffer_full) {
            // 2. Extraction des caractéristiques (12 features)
            // On récupère aussi les dernières valeurs ADC pour Temp et Courant
            uint16_t temp_adc = 2048; // Valeur fictive
            uint16_t curr_adc = 2048; // Valeur fictive

            Feature_ExtractAll(&sensor_data, (float)temp_adc, (float)curr_adc, &features);

            // 3. Inférence Random Forest
            // On passe le pointeur de la structure casté en float* (car 12 floats contigus)
            int machine_state = predict_machine_state((float*)&features);

            // 4. Affichage du Diagnostic
            char buffer[128];
            const char* state_name = RF_GetClassName((FaultClass_t)machine_state);

            sprintf(buffer, "[DIAGNOSTIC] Etat: %s | RMS: %.2f | Temp: %.1f C\r\n",
                    state_name, features.vibration_rms, features.temperature_celsius);
            UART_Print(buffer);

            sensor_data.buffer_full = 0;
        }

        HAL_Delay(1000);
    }
}
