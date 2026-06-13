import os
import numpy as np
from sklearn.tree import _tree

class CodeExporter:
    """
    Classe permettant l'extraction des règles de décision du modèle Python
    et leur conversion en code C (if/else) pour l'inférence sur STM32.
    """
    @staticmethod
    def export_rf_to_c(rf_model, feature_names: list, output_filename: str = "random_forest_rules.c"):
        """
        Convertit un modèle Random Forest entraîné en code C pur.
        """
        print(f"\n[Info] Génération du code C pour l'embarqué dans '{output_filename}'...")
        
        # Création du dossier parent s'il n'existe pas
        os.makedirs(os.path.dirname(os.path.abspath(output_filename)), exist_ok=True)
        
        c_code = []
        c_code.append("// Fichier généré automatiquement pour le TFE")
        c_code.append("// Modèle : Random Forest pour la Maintenance Prédictive")
        c_code.append("// Features utilisées : " + ", ".join(feature_names))
        c_code.append("")
        c_code.append("#include <stdint.h>")
        c_code.append("")
        
        def tree_to_c(tree, feature_names, tree_id):
            tree_ = tree.tree_
            feature_name = [
                feature_names[i] if i != _tree.TREE_UNDEFINED else "undefined!"
                for i in tree_.feature
            ]
            
            lines = []
            lines.append(f"static int predict_tree_{tree_id}(float {feature_names[0]}, float {feature_names[1]}, float {feature_names[2]}, float {feature_names[3]}) {{")
            
            def recurse(node, depth):
                indent = "    " * (depth + 1)
                if tree_.feature[node] != _tree.TREE_UNDEFINED:
                    name = feature_name[node]
                    threshold = tree_.threshold[node]
                    lines.append(f"{indent}if ({name} <= {threshold:.5f}f) {{")
                    recurse(tree_.children_left[node], depth + 1)
                    lines.append(f"{indent}}} else {{")
                    recurse(tree_.children_right[node], depth + 1)
                    lines.append(f"{indent}}}")
                else:
                    value = tree_.value[node][0]
                    class_idx = np.argmax(value)
                    lines.append(f"{indent}return {class_idx};")

            recurse(0, 0)
            lines.append("}")
            return "\n".join(lines)

        for i, estimator in enumerate(rf_model.estimators_):
            c_code.append(tree_to_c(estimator, feature_names, i))
            c_code.append("")

        n_trees = len(rf_model.estimators_)
        c_code.append("/**")
        c_code.append(" * @brief Fonction d'inférence principale (Random Forest)")
        c_code.append(" * @return Code de défaut : 0 (Sain), 1 (Balourd), 2 (Désalignement), 3 (Défaut stator)")
        c_code.append(" */")
        c_code.append(f"int predict_machine_state(float {feature_names[0]}, float {feature_names[1]}, float {feature_names[2]}, float {feature_names[3]}) {{")
        c_code.append("    int votes[4] = {0, 0, 0, 0};")
        
        for i in range(n_trees):
            c_code.append(f"    votes[predict_tree_{i}({feature_names[0]}, {feature_names[1]}, {feature_names[2]}, {feature_names[3]})]++;")
            
        c_code.append("")
        c_code.append("    // Recherche du label ayant le maximum de votes")
        c_code.append("    int max_votes = -1;")
        c_code.append("    int best_class = -1;")
        c_code.append("    for (int i = 0; i < 4; i++) {")
        c_code.append("        if (votes[i] > max_votes) {")
        c_code.append("            max_votes = votes[i];")
        c_code.append("            best_class = i;")
        c_code.append("        }")
        c_code.append("    }")
        c_code.append("    return best_class;")
        c_code.append("}")
        
        try:
            with open(output_filename, "w") as f:
                f.write("\n".join(c_code))
            print(f"[Succès] L'exportation du code C est terminée ({output_filename}) !")
        except Exception as e:
            print(f"[Erreur] Échec de la sauvegarde du fichier C: {e}")
