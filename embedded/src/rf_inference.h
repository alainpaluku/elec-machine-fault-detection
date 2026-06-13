/**
 * @file    rf_inference.h
 * @brief   Moteur d'inférence Random Forest embarqué pour diagnostic de pannes
 * @author  Alain Paluku — Mémoire de Master 2
 * @date    2026
 * @version 1.0
 *
 * @details Ce module implémente l'inférence d'un classificateur Random Forest
 *          directement en C pour exécution sur microcontrôleur STM32.
 *          Le modèle est représenté comme un tableau de structures d'arbres
 *          de décision, chaque nœud contenant :
 *            - L'index de la caractéristique à tester
 *            - Le seuil de décision
 *            - Les indices des enfants gauche/droite
 *            - La classe prédite (pour les feuilles)
 *
 *          La prédiction se fait par vote majoritaire sur l'ensemble des arbres.
 *
 * @note    Les données du modèle entraîné sont intégrées en dur dans rf_inference.c.
 *          Voir le script Python d'export pour générer automatiquement les
 *          structures C à partir de scikit-learn.
 */

#ifndef RF_INFERENCE_H
#define RF_INFERENCE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/*                              INCLUDES                                     */
/* ========================================================================= */

#include <stdint.h>

/* ========================================================================= */
/*                          CONSTANTES & MACROS                              */
/* ========================================================================= */

/** @brief Nombre maximal d'arbres dans la forêt */
#define MAX_TREES           100U

/** @brief Profondeur maximale d'un arbre de décision */
#define MAX_DEPTH           15U

/** @brief Nombre maximal de nœuds par arbre (2^(MAX_DEPTH+1) - 1 théorique) */
#define MAX_NODES           500U

/* ========================================================================= */
/*                          TYPES ÉNUMÉRÉS                                   */
/* ========================================================================= */

/**
 * @brief Énumération des classes de défauts diagnostiqués
 *
 * @details Correspondance avec les étiquettes du jeu de données AI4I 2020 :
 *          - FAULT_NORMAL          : Fonctionnement normal (pas de défaut)
 *          - FAULT_TOOL_WEAR       : Usure de l'outil (TWF)
 *          - FAULT_HEAT_DISSIPATION: Défaut de dissipation thermique (HDF)
 *          - FAULT_POWER           : Défaut de puissance (PWF)
 *          - FAULT_OVERSTRAIN      : Surcharge mécanique (OSF)
 *          - FAULT_RANDOM          : Défaut aléatoire (RNF)
 */
typedef enum
{
    FAULT_NORMAL            = 0, /**< Fonctionnement normal                     */
    FAULT_TOOL_WEAR         = 1, /**< Usure de l'outil                          */
    FAULT_HEAT_DISSIPATION  = 2, /**< Défaut de dissipation thermique            */
    FAULT_POWER             = 3, /**< Défaut de puissance                        */
    FAULT_OVERSTRAIN        = 4, /**< Surcharge mécanique                        */
    FAULT_RANDOM            = 5, /**< Défaut aléatoire                           */
    FAULT_NUM_CLASSES       = 6  /**< Nombre total de classes (sentinelle)        */
} FaultClass_t;

/* ========================================================================= */
/*                        STRUCTURES DE DONNÉES                              */
/* ========================================================================= */

/**
 * @brief Nœud d'un arbre de décision
 *
 * @details Chaque nœud est soit un nœud interne (test feature < seuil),
 *          soit une feuille (prédiction de classe).
 *          Convention : feature_index == -1 indique une feuille.
 */
typedef struct
{
    int16_t  feature_index;  /**< Index de la caractéristique à tester (-1 si feuille)  */
    float    threshold;      /**< Seuil de décision (ignoré si feuille)                 */
    int16_t  left_child;     /**< Index du fils gauche (si feature < threshold)          */
    int16_t  right_child;    /**< Index du fils droit  (si feature >= threshold)         */
    uint8_t  class_label;    /**< Classe prédite (valide uniquement si feuille)          */
} TreeNode_t;

/**
 * @brief Arbre de décision complet
 *
 * @details Représentation aplatie de l'arbre en tableau de nœuds.
 *          Le nœud racine est à l'index 0.
 */
typedef struct
{
    TreeNode_t nodes[MAX_NODES]; /**< Tableau de nœuds de l'arbre                       */
    uint16_t   num_nodes;        /**< Nombre effectif de nœuds dans l'arbre              */
} DecisionTree_t;

/**
 * @brief Forêt aléatoire complète
 *
 * @details Ensemble d'arbres de décision avec vote majoritaire.
 */
typedef struct
{
    DecisionTree_t trees[MAX_TREES]; /**< Tableau d'arbres de décision                   */
    uint16_t       num_trees;        /**< Nombre effectif d'arbres dans la forêt          */
    uint16_t       num_features;     /**< Nombre de caractéristiques attendues en entrée  */
} RandomForest_t;

/* ========================================================================= */
/*                        PROTOTYPES DE FONCTIONS                            */
/* ========================================================================= */

/**
 * @brief  Initialise la forêt aléatoire avec les données du modèle entraîné
 * @param  forest  Pointeur vers la structure RandomForest_t à initialiser
 * @retval Aucun
 * @note   Cette fonction charge les arbres de démonstration. En production,
 *         remplacer par les données exportées depuis Python (voir documentation).
 */
void RF_Init(RandomForest_t* forest);

/**
 * @brief  Prédit la classe avec un seul arbre de décision
 * @param  tree      Pointeur vers l'arbre de décision
 * @param  features  Tableau de NUM_FEATURES valeurs normalisées
 * @retval Classe prédite (FaultClass_t)
 */
FaultClass_t RF_PredictTree(const DecisionTree_t* tree, const float* features);

/**
 * @brief  Prédit la classe par vote majoritaire de tous les arbres
 * @param  forest    Pointeur vers la forêt aléatoire
 * @param  features  Tableau de NUM_FEATURES valeurs normalisées
 * @retval Classe prédite (FaultClass_t) par vote majoritaire
 */
FaultClass_t RF_Predict(const RandomForest_t* forest, const float* features);

/**
 * @brief  Obtient le décompte des votes de chaque classe
 * @param  forest    Pointeur vers la forêt aléatoire
 * @param  features  Tableau de NUM_FEATURES valeurs normalisées
 * @param  votes     Tableau de sortie de taille FAULT_NUM_CLASSES (votes par classe)
 * @retval Aucun
 */
void RF_GetVotes(const RandomForest_t* forest, const float* features,
                 uint16_t* votes);

/**
 * @brief  Calcule le niveau de confiance de la prédiction
 * @param  votes      Tableau de votes par classe (FAULT_NUM_CLASSES éléments)
 * @param  num_trees  Nombre total d'arbres dans la forêt
 * @retval Confiance entre 0.0 et 1.0 (max_votes / num_trees)
 */
float RF_GetConfidence(const uint16_t* votes, uint16_t num_trees);

/**
 * @brief  Retourne le nom lisible d'une classe de défaut
 * @param  fault_class  Classe de défaut (FaultClass_t)
 * @retval Chaîne de caractères statique décrivant le défaut (en français)
 */
const char* RF_GetClassName(FaultClass_t fault_class);

#ifdef __cplusplus
}
#endif

#endif /* RF_INFERENCE_H */
