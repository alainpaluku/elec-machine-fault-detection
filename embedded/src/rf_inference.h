/**
 * @file    rf_inference.h
 * @brief   Moteur d'inférence Random Forest embarqué
 * @author  Alain Paluku
 * @version 1.1
 */

#ifndef RF_INFERENCE_H
#define RF_INFERENCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_TREES           100U
#define MAX_DEPTH           15U
#define MAX_NODES           500U

typedef enum
{
    FAULT_NORMAL            = 0,
    FAULT_TOOL_WEAR         = 1,
    FAULT_HEAT_DISSIPATION  = 2,
    FAULT_POWER             = 3,
    FAULT_OVERSTRAIN        = 4,
    FAULT_RANDOM            = 5,
    FAULT_NUM_CLASSES       = 6
} FaultClass_t;

typedef struct
{
    int16_t  feature_index;
    float    threshold;
    int16_t  left_child;
    int16_t  right_child;
    uint8_t  class_label;
} TreeNode_t;

typedef struct
{
    TreeNode_t nodes[MAX_NODES];
    uint16_t   num_nodes;
} DecisionTree_t;

typedef struct
{
    DecisionTree_t trees[MAX_TREES];
    uint16_t       num_trees;
    uint16_t       num_features;
} RandomForest_t;

void RF_Init(RandomForest_t* forest);
const char* RF_GetClassName(FaultClass_t fault_class);

#ifdef __cplusplus
}
#endif

#endif
