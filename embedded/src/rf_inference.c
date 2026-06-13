/**
 * @file    rf_inference.c
 * @brief   Implémentation du moteur d'inférence Random Forest embarqué
 * @author  Alain Paluku — Mémoire de Master 2
 * @date    2026
 * @version 1.0
 *
 * @details Ce fichier implémente la prédiction par forêt aléatoire en C pur
 *          pour exécution sur STM32. La forêt est initialisée avec des arbres
 *          de démonstration (3 arbres simples) pour validation du pipeline.
 *
 * =========================================================================
 * COMMENT REMPLACER LES ARBRES DE DÉMONSTRATION PAR LE MODÈLE ENTRAÎNÉ
 * =========================================================================
 *
 *  Le script Python d'export (scripts/export_rf_to_c.py) génère automatiquement
 *  un fichier C header (rf_model_data.h) contenant les tableaux de nœuds
 *  pour chaque arbre entraîné via scikit-learn.
 *
 *  Procédure :
 *  1. Entraîner le modèle Random Forest en Python :
 *       from sklearn.ensemble import RandomForestClassifier
 *       rf = RandomForestClassifier(n_estimators=50, max_depth=12)
 *       rf.fit(X_train, y_train)
 *
 *  2. Exporter avec le script fourni :
 *       python scripts/export_rf_to_c.py --model rf_model.pkl --output rf_model_data.h
 *
 *  3. Le fichier généré contient :
 *       - Un tableau constant par arbre : static const TreeNode_t tree_0_nodes[] = {...};
 *       - Le nombre de nœuds par arbre
 *       - Le nombre total d'arbres
 *
 *  4. Modifier RF_Init() pour charger ces données au lieu des arbres de démo :
 *       #include "rf_model_data.h"
 *       // Puis copier les nœuds dans forest->trees[i].nodes
 *
 *  5. Alternative : stocker le modèle en Flash avec le qualificateur 'const'
 *     pour économiser la RAM :
 *       static const TreeNode_t tree_0[] __attribute__((section(".rodata"))) = {...};
 *
 * =========================================================================
 */

/* ========================================================================= */
/*                              INCLUDES                                     */
/* ========================================================================= */

#include "rf_inference.h"
#include "feature_compute.h"  /* Pour NUM_FEATURES */
#include <string.h>           /* Pour memset, memcpy */

/* ========================================================================= */
/*                      CONSTANTES INTERNES                                  */
/* ========================================================================= */

/** @brief Nombre d'arbres de démonstration */
#define DEMO_NUM_TREES      3U

/** @brief Nombre de caractéristiques utilisées par le modèle de démo */
#define DEMO_NUM_FEATURES   12U

/* ========================================================================= */
/*                   DONNÉES DU MODÈLE DE DÉMONSTRATION                     */
/* ========================================================================= */

/**
 * @brief  Arbres de décision de démonstration
 *
 * @details Ces 3 arbres simples illustrent la structure des données.
 *          Ils utilisent des seuils réalistes basés sur les plages typiques
 *          des caractéristiques normalisées (0-1).
 *
 *          Structure de chaque nœud :
 *          { feature_index, threshold, left_child, right_child, class_label }
 *
 *          Convention :
 *          - feature_index = -1  →  nœud feuille (class_label valide)
 *          - feature_index >= 0  →  nœud interne (tester features[feature_index] < threshold)
 *
 *          Mapping des caractéristiques (normalisées) :
 *          [0] vibration_rms        [6] vibration_dominant_freq
 *          [1] vibration_kurtosis   [7] vibration_spectral_energy
 *          [2] vibration_crest_factor [8] temperature_celsius
 *          [3] vibration_peak_to_peak [9] current_rms
 *          [4] vibration_skewness   [10] power_estimate
 *          [5] vibration_std        [11] speed_rpm
 */

/* --- Arbre 0 : Détection primaire basée sur vibration et température --- */
static const TreeNode_t demo_tree_0_nodes[] =
{
    /*  idx   feat  threshold  left  right  class */
    /* [0] */ {  0,   0.45f,     1,     4,    0  },  /* vibration_rms < 0.45 ? */
    /* [1] */ {  8,   0.60f,     2,     3,    0  },  /* temperature < 0.60 ? */
    /* [2] */ { -1,   0.00f,     0,     0,    FAULT_NORMAL           },  /* Feuille : Normal */
    /* [3] */ { -1,   0.00f,     0,     0,    FAULT_HEAT_DISSIPATION },  /* Feuille : Dissipation thermique */
    /* [4] */ {  1,   0.70f,     5,     6,    0  },  /* kurtosis < 0.70 ? */
    /* [5] */ { -1,   0.00f,     0,     0,    FAULT_OVERSTRAIN       },  /* Feuille : Surcharge */
    /* [6] */ { -1,   0.00f,     0,     0,    FAULT_TOOL_WEAR        },  /* Feuille : Usure outil */
};

/* --- Arbre 1 : Détection basée sur courant et puissance --- */
static const TreeNode_t demo_tree_1_nodes[] =
{
    /* [0] */ {  9,   0.55f,     1,     4,    0  },  /* current_rms < 0.55 ? */
    /* [1] */ { 10,   0.40f,     2,     3,    0  },  /* power_estimate < 0.40 ? */
    /* [2] */ { -1,   0.00f,     0,     0,    FAULT_NORMAL           },  /* Feuille : Normal */
    /* [3] */ { -1,   0.00f,     0,     0,    FAULT_POWER            },  /* Feuille : Puissance */
    /* [4] */ {  2,   0.65f,     5,     6,    0  },  /* crest_factor < 0.65 ? */
    /* [5] */ { -1,   0.00f,     0,     0,    FAULT_OVERSTRAIN       },  /* Feuille : Surcharge */
    /* [6] */ { -1,   0.00f,     0,     0,    FAULT_RANDOM           },  /* Feuille : Aléatoire */
};

/* --- Arbre 2 : Détection basée sur spectre et vitesse --- */
static const TreeNode_t demo_tree_2_nodes[] =
{
    /* [0] */ {  7,   0.50f,     1,     4,    0  },  /* spectral_energy < 0.50 ? */
    /* [1] */ { 11,   0.35f,     2,     3,    0  },  /* speed_rpm < 0.35 ? */
    /* [2] */ { -1,   0.00f,     0,     0,    FAULT_NORMAL           },  /* Feuille : Normal */
    /* [3] */ { -1,   0.00f,     0,     0,    FAULT_POWER            },  /* Feuille : Puissance */
    /* [4] */ {  5,   0.60f,     5,     8,    0  },  /* vibration_std < 0.60 ? */
    /* [5] */ {  8,   0.70f,     6,     7,    0  },  /* temperature < 0.70 ? */
    /* [6] */ { -1,   0.00f,     0,     0,    FAULT_TOOL_WEAR        },  /* Feuille : Usure outil */
    /* [7] */ { -1,   0.00f,     0,     0,    FAULT_HEAT_DISSIPATION },  /* Feuille : Dissipation */
    /* [8] */ { -1,   0.00f,     0,     0,    FAULT_RANDOM           },  /* Feuille : Aléatoire */
};

/* ========================================================================= */
/*                      IMPLÉMENTATION DES FONCTIONS                         */
/* ========================================================================= */

/**
 * @brief  Initialise la forêt aléatoire avec les arbres de démonstration
 *
 * @details Cette fonction charge 3 arbres de décision simples pour
 *          valider le pipeline d'inférence. En production, ces arbres
 *          doivent être remplacés par le modèle exporté depuis Python.
 *
 * @param  forest  Pointeur vers la structure RandomForest_t
 */
void RF_Init(RandomForest_t* forest)
{
    if (forest == NULL)
    {
        return;
    }

    /* Remise à zéro complète de la structure */
    (void)memset(forest, 0, sizeof(RandomForest_t));

    forest->num_trees    = DEMO_NUM_TREES;
    forest->num_features = DEMO_NUM_FEATURES;

    /* --------------------------------------------------------------- */
    /* Chargement de l'arbre 0                                         */
    /* --------------------------------------------------------------- */
    forest->trees[0].num_nodes = (uint16_t)(sizeof(demo_tree_0_nodes) /
                                            sizeof(TreeNode_t));
    (void)memcpy(forest->trees[0].nodes, demo_tree_0_nodes,
                 sizeof(demo_tree_0_nodes));

    /* --------------------------------------------------------------- */
    /* Chargement de l'arbre 1                                         */
    /* --------------------------------------------------------------- */
    forest->trees[1].num_nodes = (uint16_t)(sizeof(demo_tree_1_nodes) /
                                            sizeof(TreeNode_t));
    (void)memcpy(forest->trees[1].nodes, demo_tree_1_nodes,
                 sizeof(demo_tree_1_nodes));

    /* --------------------------------------------------------------- */
    /* Chargement de l'arbre 2                                         */
    /* --------------------------------------------------------------- */
    forest->trees[2].num_nodes = (uint16_t)(sizeof(demo_tree_2_nodes) /
                                            sizeof(TreeNode_t));
    (void)memcpy(forest->trees[2].nodes, demo_tree_2_nodes,
                 sizeof(demo_tree_2_nodes));
}

/**
 * @brief  Prédit la classe de défaut avec un seul arbre de décision
 *
 * @details Parcours de l'arbre depuis la racine (index 0) :
 *          - Si le nœud est une feuille (feature_index == -1), retourner class_label
 *          - Sinon, comparer features[feature_index] avec le seuil
 *            et descendre à gauche (< seuil) ou à droite (>= seuil)
 *
 *          Protection contre les boucles infinies via MAX_DEPTH.
 *
 * @param  tree      Pointeur vers l'arbre de décision
 * @param  features  Tableau de caractéristiques normalisées
 * @retval Classe prédite (FaultClass_t)
 */
FaultClass_t RF_PredictTree(const DecisionTree_t* tree, const float* features)
{
    uint16_t node_idx = 0U;
    uint16_t depth    = 0U;
    const TreeNode_t* node;

    if ((tree == NULL) || (features == NULL) || (tree->num_nodes == 0U))
    {
        return FAULT_NORMAL;  /* Valeur par défaut en cas d'erreur */
    }

    while (depth < MAX_DEPTH)
    {
        node = &tree->nodes[node_idx];

        /* Vérification : est-ce une feuille ? */
        if (node->feature_index < 0)
        {
            return (FaultClass_t)node->class_label;
        }

        /* Vérification de validité de l'index de caractéristique */
        if ((uint16_t)node->feature_index >= NUM_FEATURES)
        {
            return FAULT_NORMAL;  /* Index invalide → valeur par défaut */
        }

        /* Décision : aller à gauche ou à droite */
        if (features[node->feature_index] < node->threshold)
        {
            node_idx = (uint16_t)node->left_child;
        }
        else
        {
            node_idx = (uint16_t)node->right_child;
        }

        /* Vérification de validité de l'index du nœud enfant */
        if (node_idx >= tree->num_nodes)
        {
            return FAULT_NORMAL;  /* Index hors limites → valeur par défaut */
        }

        depth++;
    }

    /* Profondeur maximale atteinte sans trouver de feuille */
    return FAULT_NORMAL;
}

/**
 * @brief  Prédit la classe par vote majoritaire de tous les arbres
 *
 * @details Procédure :
 *          1. Chaque arbre vote pour une classe
 *          2. On compte les votes par classe
 *          3. La classe avec le plus de votes l'emporte
 *
 *          En cas d'égalité, la classe avec l'index le plus petit est choisie
 *          (convention déterministe).
 *
 * @param  forest    Pointeur vers la forêt aléatoire
 * @param  features  Tableau de caractéristiques normalisées
 * @retval Classe prédite par vote majoritaire
 */
FaultClass_t RF_Predict(const RandomForest_t* forest, const float* features)
{
    uint16_t votes[FAULT_NUM_CLASSES];
    FaultClass_t best_class = FAULT_NORMAL;
    uint16_t max_votes = 0U;
    uint16_t i;

    if ((forest == NULL) || (features == NULL))
    {
        return FAULT_NORMAL;
    }

    /* Obtenir les votes de tous les arbres */
    RF_GetVotes(forest, features, votes);

    /* Trouver la classe majoritaire */
    for (i = 0U; i < (uint16_t)FAULT_NUM_CLASSES; i++)
    {
        if (votes[i] > max_votes)
        {
            max_votes  = votes[i];
            best_class = (FaultClass_t)i;
        }
    }

    return best_class;
}

/**
 * @brief  Obtient le décompte des votes de chaque classe
 *
 * @details Chaque arbre de la forêt prédit une classe.
 *          Les votes sont accumulés dans le tableau de sortie.
 *
 * @param  forest    Pointeur vers la forêt aléatoire
 * @param  features  Tableau de caractéristiques normalisées
 * @param  votes     Tableau de sortie (FAULT_NUM_CLASSES éléments)
 */
void RF_GetVotes(const RandomForest_t* forest, const float* features,
                 uint16_t* votes)
{
    FaultClass_t prediction;
    uint16_t i;

    if ((forest == NULL) || (features == NULL) || (votes == NULL))
    {
        return;
    }

    /* Initialisation des compteurs de votes */
    for (i = 0U; i < (uint16_t)FAULT_NUM_CLASSES; i++)
    {
        votes[i] = 0U;
    }

    /* Vote de chaque arbre */
    for (i = 0U; i < forest->num_trees; i++)
    {
        prediction = RF_PredictTree(&forest->trees[i], features);

        /* Vérification de validité de la prédiction */
        if ((uint16_t)prediction < (uint16_t)FAULT_NUM_CLASSES)
        {
            votes[(uint16_t)prediction]++;
        }
    }
}

/**
 * @brief  Calcule le niveau de confiance de la prédiction
 *
 * @details Confiance = max(votes) / num_trees
 *          Plage : [1/num_trees, 1.0]
 *
 *          Interprétation :
 *          - > 0.8 : haute confiance (tous les arbres convergent)
 *          - 0.5-0.8 : confiance modérée
 *          - < 0.5 : faible confiance (diagnostic incertain)
 *
 * @param  votes      Tableau de votes par classe
 * @param  num_trees  Nombre total d'arbres
 * @retval Confiance entre 0.0 et 1.0
 */
float RF_GetConfidence(const uint16_t* votes, uint16_t num_trees)
{
    uint16_t max_votes = 0U;
    uint16_t i;

    if ((votes == NULL) || (num_trees == 0U))
    {
        return 0.0f;
    }

    /* Trouver le nombre maximal de votes */
    for (i = 0U; i < (uint16_t)FAULT_NUM_CLASSES; i++)
    {
        if (votes[i] > max_votes)
        {
            max_votes = votes[i];
        }
    }

    return (float)max_votes / (float)num_trees;
}

/**
 * @brief  Retourne le nom lisible d'une classe de défaut (en français)
 *
 * @details Table de correspondance statique pour affichage sur terminal UART
 *          ou interface homme-machine (IHM).
 *
 * @param  fault_class  Classe de défaut
 * @retval Chaîne de caractères statique (ne pas libérer)
 */
const char* RF_GetClassName(FaultClass_t fault_class)
{
    /* Table de noms de classes — stockée en Flash (const) */
    static const char* const class_names[FAULT_NUM_CLASSES] =
    {
        "Normal",                   /* FAULT_NORMAL            */
        "Usure outil",              /* FAULT_TOOL_WEAR         */
        "Dissipation thermique",    /* FAULT_HEAT_DISSIPATION  */
        "Puissance",                /* FAULT_POWER             */
        "Surcharge",                /* FAULT_OVERSTRAIN        */
        "Aleatoire"                 /* FAULT_RANDOM            */
    };

    if ((uint16_t)fault_class >= (uint16_t)FAULT_NUM_CLASSES)
    {
        return "Inconnu";
    }

    return class_names[(uint16_t)fault_class];
}
