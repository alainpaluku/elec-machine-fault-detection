#include "rf_inference.h"
#include <string.h>

const char* RF_GetClassName(FaultClass_t fault_class)
{
    static const char* const class_names[FAULT_NUM_CLASSES] =
    {
        "Normal",
        "Usure outil",
        "Dissipation thermique",
        "Puissance",
        "Surcharge",
        "Aleatoire"
    };

    if ((uint16_t)fault_class >= (uint16_t)FAULT_NUM_CLASSES)
    {
        return "Inconnu";
    }

    return class_names[(uint16_t)fault_class];
}
