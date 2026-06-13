// Fichier généré automatiquement pour le TFE
// Modèle : Random Forest pour la Maintenance Prédictive
// Nombre de features : 12
// [0] vibration_rms
// [1] vibration_kurtosis
// [2] vibration_crest_factor
// [3] vibration_peak_to_peak
// [4] vibration_skewness
// [5] vibration_std
// [6] vibration_dominant_freq
// [7] vibration_spectral_energy
// [8] temperature_celsius
// [9] current_rms
// [10] power_estimate
// [11] speed_rpm

#include <stdint.h>

static int predict_tree_0(const float* features) {
    if (features[9] <= 11.07769f) {
        if (features[5] <= 0.29088f) {
            if (features[8] <= 56.39219f) {
                if (features[6] <= 50.53185f) {
                    if (features[0] <= 0.48058f) {
                        if (features[0] <= 0.33851f) {
                            if (features[10] <= 833.11548f) {
                                if (features[2] <= 1.52058f) {
                                    if (features[1] <= 5.61456f) {
                                        return 0;
                                    } else {
                                        return 5;
                                    }
                                } else {
                                    if (features[3] <= 0.40676f) {
                                        if (features[3] <= 0.38111f) {
                                            return 5;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[9] <= 3.67644f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                }
                            } else {
                                if (features[5] <= 0.14444f) {
                                    if (features[3] <= 0.53211f) {
                                        if (features[9] <= 5.37486f) {
                                            return 5;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[9] <= 5.31645f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                } else {
                                    if (features[6] <= 50.29368f) {
                                        if (features[4] <= 0.03414f) {
                                            return 5;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[6] <= 50.30800f) {
                                            return 5;
                                        } else {
                                            return 5;
                                        }
                                    }
                                }
                            }
                        } else {
                            return 1;
                        }
                    } else {
                        return 4;
                    }
                } else {
                    if (features[9] <= 6.03086f) {
                        if (features[0] <= 0.33494f) {
                            if (features[1] <= 5.60485f) {
                                if (features[4] <= 0.16006f) {
                                    if (features[8] <= 53.96518f) {
                                        return 0;
                                    } else {
                                        return 2;
                                    }
                                } else {
                                    if (features[7] <= 2.81807f) {
                                        if (features[1] <= 3.02814f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        return 0;
                                    }
                                }
                            } else {
                                return 5;
                            }
                        } else {
                            if (features[10] <= 828.16251f) {
                                return 4;
                            } else {
                                return 1;
                            }
                        }
                    } else {
                        if (features[6] <= 52.07565f) {
                            if (features[2] <= 1.26581f) {
                                if (features[9] <= 6.15884f) {
                                    return 5;
                                } else {
                                    if (features[4] <= 0.12645f) {
                                        return 4;
                                    } else {
                                        if (features[7] <= 3.30494f) {
                                            return 4;
                                        } else {
                                            return 5;
                                        }
                                    }
                                }
                            } else {
                                if (features[0] <= 0.39747f) {
                                    return 0;
                                } else {
                                    return 4;
                                }
                            }
                        } else {
                            if (features[10] <= 1187.81012f) {
                                return 4;
                            } else {
                                if (features[8] <= 39.40436f) {
                                    return 5;
                                } else {
                                    return 0;
                                }
                            }
                        }
                    }
                }
            } else {
                if (features[9] <= 6.66754f) {
                    return 2;
                } else {
                    return 4;
                }
            }
        } else {
            if (features[7] <= 10.54293f) {
                if (features[9] <= 6.52756f) {
                    return 1;
                } else {
                    return 4;
                }
            } else {
                if (features[0] <= 0.73379f) {
                    if (features[3] <= 1.22405f) {
                        if (features[11] <= 1478.62500f) {
                            return 2;
                        } else {
                            if (features[8] <= 55.46519f) {
                                if (features[0] <= 0.49593f) {
                                    if (features[5] <= 0.30583f) {
                                        return 5;
                                    } else {
                                        if (features[4] <= 0.00107f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                } else {
                                    return 1;
                                }
                            } else {
                                return 2;
                            }
                        }
                    } else {
                        return 1;
                    }
                } else {
                    return 4;
                }
            }
        }
    } else {
        return 3;
    }
}

static int predict_tree_1(const float* features) {
    if (features[5] <= 0.29079f) {
        if (features[8] <= 56.83952f) {
            if (features[10] <= 1967.64154f) {
                if (features[9] <= 6.13592f) {
                    if (features[1] <= 5.44449f) {
                        if (features[10] <= 805.96021f) {
                            if (features[9] <= 4.82606f) {
                                if (features[2] <= 1.49163f) {
                                    return 0;
                                } else {
                                    if (features[7] <= 1.24174f) {
                                        return 1;
                                    } else {
                                        if (features[6] <= 49.85257f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                }
                            } else {
                                return 4;
                            }
                        } else {
                            if (features[7] <= 0.98256f) {
                                if (features[8] <= 47.47323f) {
                                    if (features[6] <= 50.20226f) {
                                        if (features[0] <= 0.23800f) {
                                            return 0;
                                        } else {
                                            return 1;
                                        }
                                    } else {
                                        if (features[11] <= 1534.18835f) {
                                            return 1;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 1;
                                }
                            } else {
                                if (features[9] <= 5.44971f) {
                                    if (features[8] <= 53.84891f) {
                                        return 0;
                                    } else {
                                        if (features[7] <= 6.62000f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    if (features[10] <= 1066.05353f) {
                                        return 2;
                                    } else {
                                        if (features[2] <= 1.22249f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        return 5;
                    }
                } else {
                    if (features[0] <= 0.45622f) {
                        if (features[7] <= 4.01433f) {
                            if (features[10] <= 1207.64862f) {
                                return 1;
                            } else {
                                if (features[9] <= 6.24302f) {
                                    return 5;
                                } else {
                                    if (features[11] <= 1547.60895f) {
                                        if (features[8] <= 46.65415f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    } else {
                                        return 5;
                                    }
                                }
                            }
                        } else {
                            if (features[3] <= 0.87846f) {
                                if (features[9] <= 6.32999f) {
                                    return 5;
                                } else {
                                    if (features[6] <= 48.80746f) {
                                        return 0;
                                    } else {
                                        return 5;
                                    }
                                }
                            } else {
                                return 0;
                            }
                        }
                    } else {
                        return 4;
                    }
                }
            } else {
                return 3;
            }
        } else {
            if (features[8] <= 58.22294f) {
                if (features[0] <= 0.44728f) {
                    return 2;
                } else {
                    return 4;
                }
            } else {
                return 2;
            }
        }
    } else {
        if (features[0] <= 0.38233f) {
            if (features[10] <= 1963.73480f) {
                if (features[10] <= 1098.79205f) {
                    if (features[0] <= 0.33648f) {
                        if (features[1] <= 5.44307f) {
                            if (features[1] <= 3.29055f) {
                                return 0;
                            } else {
                                return 2;
                            }
                        } else {
                            return 5;
                        }
                    } else {
                        if (features[8] <= 51.84451f) {
                            if (features[11] <= 1526.31689f) {
                                return 5;
                            } else {
                                return 0;
                            }
                        } else {
                            return 2;
                        }
                    }
                } else {
                    if (features[6] <= 48.45913f) {
                        return 2;
                    } else {
                        return 0;
                    }
                }
            } else {
                return 3;
            }
        } else {
            if (features[7] <= 10.48459f) {
                return 1;
            } else {
                if (features[0] <= 0.73344f) {
                    return 1;
                } else {
                    return 4;
                }
            }
        }
    }
}

static int predict_tree_2(const float* features) {
    if (features[5] <= 0.29614f) {
        if (features[1] <= 5.55663f) {
            if (features[6] <= 49.91817f) {
                if (features[5] <= 0.27818f) {
                    if (features[4] <= -0.04658f) {
                        if (features[8] <= 56.09638f) {
                            if (features[1] <= 3.14597f) {
                                if (features[9] <= 5.97296f) {
                                    if (features[5] <= 0.27156f) {
                                        if (features[0] <= 0.32839f) {
                                            return 0;
                                        } else {
                                            return 4;
                                        }
                                    } else {
                                        return 1;
                                    }
                                } else {
                                    if (features[10] <= 2008.65088f) {
                                        if (features[10] <= 1170.44153f) {
                                            return 4;
                                        } else {
                                            return 4;
                                        }
                                    } else {
                                        return 3;
                                    }
                                }
                            } else {
                                if (features[0] <= 0.42997f) {
                                    if (features[1] <= 3.18844f) {
                                        if (features[7] <= 6.82065f) {
                                            return 3;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[11] <= 1473.80652f) {
                                            return 3;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 4;
                                }
                            }
                        } else {
                            return 2;
                        }
                    } else {
                        if (features[7] <= 2.79262f) {
                            if (features[9] <= 6.03679f) {
                                if (features[7] <= 1.30246f) {
                                    if (features[8] <= 51.12363f) {
                                        return 0;
                                    } else {
                                        return 2;
                                    }
                                } else {
                                    if (features[8] <= 51.06182f) {
                                        if (features[2] <= 1.37616f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        return 2;
                                    }
                                }
                            } else {
                                if (features[4] <= 0.12761f) {
                                    if (features[11] <= 1451.04993f) {
                                        if (features[1] <= 2.81509f) {
                                            return 4;
                                        } else {
                                            return 4;
                                        }
                                    } else {
                                        if (features[0] <= 0.38020f) {
                                            return 3;
                                        } else {
                                            return 4;
                                        }
                                    }
                                } else {
                                    if (features[7] <= 1.23634f) {
                                        return 3;
                                    } else {
                                        return 4;
                                    }
                                }
                            }
                        } else {
                            if (features[10] <= 1988.86792f) {
                                if (features[9] <= 6.09922f) {
                                    if (features[8] <= 53.19586f) {
                                        if (features[0] <= 0.49924f) {
                                            return 0;
                                        } else {
                                            return 4;
                                        }
                                    } else {
                                        return 2;
                                    }
                                } else {
                                    if (features[9] <= 6.44899f) {
                                        if (features[8] <= 56.61260f) {
                                            return 4;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[10] <= 1286.09436f) {
                                            return 4;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            } else {
                                return 3;
                            }
                        }
                    }
                } else {
                    if (features[9] <= 6.54803f) {
                        if (features[2] <= 1.24425f) {
                            return 0;
                        } else {
                            if (features[3] <= 0.67497f) {
                                return 1;
                            } else {
                                if (features[3] <= 0.90763f) {
                                    return 0;
                                } else {
                                    return 2;
                                }
                            }
                        }
                    } else {
                        if (features[10] <= 1903.94028f) {
                            return 4;
                        } else {
                            return 3;
                        }
                    }
                }
            } else {
                if (features[4] <= 0.11941f) {
                    if (features[9] <= 10.80217f) {
                        if (features[8] <= 53.25184f) {
                            if (features[0] <= 0.33714f) {
                                return 0;
                            } else {
                                if (features[9] <= 5.69275f) {
                                    if (features[0] <= 0.53694f) {
                                        return 1;
                                    } else {
                                        return 4;
                                    }
                                } else {
                                    return 4;
                                }
                            }
                        } else {
                            if (features[9] <= 6.80566f) {
                                return 2;
                            } else {
                                return 4;
                            }
                        }
                    } else {
                        return 3;
                    }
                } else {
                    if (features[0] <= 0.50568f) {
                        if (features[8] <= 52.11552f) {
                            if (features[7] <= 1.12249f) {
                                if (features[2] <= 1.34909f) {
                                    return 0;
                                } else {
                                    if (features[3] <= 0.34867f) {
                                        return 1;
                                    } else {
                                        if (features[9] <= 4.61524f) {
                                            return 1;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            } else {
                                if (features[9] <= 10.02385f) {
                                    return 0;
                                } else {
                                    return 3;
                                }
                            }
                        } else {
                            return 2;
                        }
                    } else {
                        return 4;
                    }
                }
            }
        } else {
            return 5;
        }
    } else {
        if (features[0] <= 0.37917f) {
            if (features[2] <= 1.45873f) {
                if (features[8] <= 46.51291f) {
                    if (features[1] <= 5.47216f) {
                        if (features[6] <= 50.42857f) {
                            return 3;
                        } else {
                            return 0;
                        }
                    } else {
                        return 5;
                    }
                } else {
                    return 2;
                }
            } else {
                return 0;
            }
        } else {
            if (features[5] <= 0.29988f) {
                if (features[0] <= 0.62021f) {
                    return 1;
                } else {
                    return 4;
                }
            } else {
                if (features[0] <= 0.75883f) {
                    return 1;
                } else {
                    return 4;
                }
            }
        }
    }
}

static int predict_tree_3(const float* features) {
    if (features[5] <= 0.29097f) {
        if (features[7] <= 1.01004f) {
            if (features[8] <= 53.43968f) {
                if (features[0] <= 0.22364f) {
                    if (features[1] <= 5.55909f) {
                        if (features[9] <= 9.69326f) {
                            return 0;
                        } else {
                            return 3;
                        }
                    } else {
                        return 5;
                    }
                } else {
                    if (features[5] <= 0.16535f) {
                        return 4;
                    } else {
                        return 1;
                    }
                }
            } else {
                return 2;
            }
        } else {
            if (features[1] <= 5.55663f) {
                if (features[10] <= 1955.13196f) {
                    if (features[0] <= 0.46258f) {
                        if (features[4] <= -0.12061f) {
                            if (features[10] <= 1043.30164f) {
                                if (features[5] <= 0.26578f) {
                                    if (features[10] <= 947.24081f) {
                                        if (features[3] <= 0.88511f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[8] <= 56.93511f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    return 2;
                                }
                            } else {
                                if (features[2] <= 1.53613f) {
                                    if (features[3] <= 0.56015f) {
                                        return 0;
                                    } else {
                                        if (features[1] <= 2.74163f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 2;
                                }
                            }
                        } else {
                            if (features[8] <= 53.25184f) {
                                return 0;
                            } else {
                                return 2;
                            }
                        }
                    } else {
                        return 4;
                    }
                } else {
                    return 3;
                }
            } else {
                return 5;
            }
        }
    } else {
        if (features[9] <= 6.64246f) {
            if (features[0] <= 0.38243f) {
                if (features[5] <= 0.30245f) {
                    if (features[4] <= 0.07473f) {
                        if (features[5] <= 0.30179f) {
                            return 5;
                        } else {
                            return 0;
                        }
                    } else {
                        if (features[6] <= 50.07202f) {
                            return 0;
                        } else {
                            return 2;
                        }
                    }
                } else {
                    if (features[11] <= 1540.78217f) {
                        if (features[4] <= -0.19375f) {
                            return 0;
                        } else {
                            if (features[4] <= 0.07618f) {
                                return 2;
                            } else {
                                return 5;
                            }
                        }
                    } else {
                        if (features[2] <= 1.27703f) {
                            return 5;
                        } else {
                            return 0;
                        }
                    }
                }
            } else {
                if (features[10] <= 823.73300f) {
                    if (features[0] <= 0.71561f) {
                        return 1;
                    } else {
                        return 4;
                    }
                } else {
                    return 1;
                }
            }
        } else {
            if (features[11] <= 1522.20258f) {
                if (features[9] <= 10.71635f) {
                    return 4;
                } else {
                    return 3;
                }
            } else {
                return 4;
            }
        }
    }
}

static int predict_tree_4(const float* features) {
    if (features[1] <= 5.46153f) {
        if (features[8] <= 56.83952f) {
            if (features[10] <= 1936.45276f) {
                if (features[5] <= 0.29342f) {
                    if (features[0] <= 0.47577f) {
                        if (features[8] <= 53.59981f) {
                            if (features[8] <= 52.02367f) {
                                if (features[3] <= 0.28435f) {
                                    if (features[0] <= 0.25364f) {
                                        return 0;
                                    } else {
                                        return 1;
                                    }
                                } else {
                                    if (features[0] <= 0.35196f) {
                                        return 0;
                                    } else {
                                        return 1;
                                    }
                                }
                            } else {
                                if (features[10] <= 1042.44833f) {
                                    return 0;
                                } else {
                                    if (features[9] <= 5.52252f) {
                                        return 2;
                                    } else {
                                        if (features[6] <= 50.54176f) {
                                            return 1;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            }
                        } else {
                            return 2;
                        }
                    } else {
                        return 4;
                    }
                } else {
                    if (features[9] <= 6.64246f) {
                        if (features[5] <= 0.30231f) {
                            if (features[6] <= 51.46255f) {
                                if (features[4] <= -0.14117f) {
                                    return 4;
                                } else {
                                    if (features[7] <= 6.08245f) {
                                        return 1;
                                    } else {
                                        return 0;
                                    }
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            if (features[3] <= 0.96241f) {
                                return 1;
                            } else {
                                if (features[7] <= 11.35972f) {
                                    return 1;
                                } else {
                                    if (features[9] <= 5.29420f) {
                                        return 1;
                                    } else {
                                        if (features[8] <= 50.39211f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        return 4;
                    }
                }
            } else {
                return 3;
            }
        } else {
            if (features[2] <= 1.54734f) {
                return 2;
            } else {
                if (features[0] <= 0.48941f) {
                    return 2;
                } else {
                    return 4;
                }
            }
        }
    } else {
        return 5;
    }
}

static int predict_tree_5(const float* features) {
    if (features[1] <= 5.60485f) {
        if (features[5] <= 0.29037f) {
            if (features[9] <= 11.07769f) {
                if (features[8] <= 56.60489f) {
                    if (features[0] <= 0.47476f) {
                        if (features[5] <= 0.27273f) {
                            if (features[2] <= 1.34205f) {
                                if (features[1] <= 2.76574f) {
                                    if (features[8] <= 52.19669f) {
                                        return 0;
                                    } else {
                                        return 2;
                                    }
                                } else {
                                    if (features[5] <= 0.26747f) {
                                        if (features[4] <= 0.15971f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        return 1;
                                    }
                                }
                            } else {
                                if (features[8] <= 54.42383f) {
                                    if (features[0] <= 0.32532f) {
                                        return 0;
                                    } else {
                                        return 1;
                                    }
                                } else {
                                    return 2;
                                }
                            }
                        } else {
                            if (features[7] <= 5.11227f) {
                                return 1;
                            } else {
                                return 0;
                            }
                        }
                    } else {
                        return 4;
                    }
                } else {
                    if (features[2] <= 1.54734f) {
                        return 2;
                    } else {
                        if (features[11] <= 1470.95081f) {
                            if (features[4] <= 0.08558f) {
                                return 2;
                            } else {
                                return 4;
                            }
                        } else {
                            return 2;
                        }
                    }
                }
            } else {
                return 3;
            }
        } else {
            if (features[7] <= 10.28715f) {
                return 1;
            } else {
                if (features[5] <= 0.40904f) {
                    if (features[10] <= 1954.81281f) {
                        if (features[9] <= 6.02379f) {
                            if (features[11] <= 1541.72266f) {
                                if (features[9] <= 5.63214f) {
                                    if (features[2] <= 1.27071f) {
                                        return 0;
                                    } else {
                                        return 2;
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 4;
                        }
                    } else {
                        return 3;
                    }
                } else {
                    return 1;
                }
            }
        }
    } else {
        return 5;
    }
}

static int predict_tree_6(const float* features) {
    if (features[4] <= -0.04512f) {
        if (features[1] <= 5.55816f) {
            if (features[5] <= 0.28944f) {
                if (features[11] <= 1513.77295f) {
                    if (features[5] <= 0.22351f) {
                        if (features[10] <= 1967.50677f) {
                            if (features[5] <= 0.15081f) {
                                if (features[8] <= 56.02240f) {
                                    if (features[8] <= 34.42154f) {
                                        if (features[7] <= 1.89204f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[8] <= 35.86466f) {
                                            return 4;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 2;
                                }
                            } else {
                                if (features[0] <= 0.45726f) {
                                    if (features[3] <= 0.67908f) {
                                        if (features[9] <= 4.61243f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[5] <= 0.19472f) {
                                            return 2;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    return 4;
                                }
                            }
                        } else {
                            return 3;
                        }
                    } else {
                        if (features[9] <= 6.17280f) {
                            if (features[8] <= 59.39764f) {
                                if (features[0] <= 0.33803f) {
                                    return 0;
                                } else {
                                    return 1;
                                }
                            } else {
                                return 2;
                            }
                        } else {
                            if (features[4] <= -0.07536f) {
                                if (features[10] <= 1895.38531f) {
                                    return 4;
                                } else {
                                    return 3;
                                }
                            } else {
                                if (features[10] <= 1981.02576f) {
                                    return 4;
                                } else {
                                    return 3;
                                }
                            }
                        }
                    }
                } else {
                    if (features[9] <= 6.01968f) {
                        if (features[2] <= 1.54567f) {
                            if (features[11] <= 1579.16559f) {
                                if (features[0] <= 0.24416f) {
                                    if (features[3] <= 0.62905f) {
                                        if (features[8] <= 55.30966f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[8] <= 58.47337f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    if (features[9] <= 4.71278f) {
                                        return 1;
                                    } else {
                                        if (features[10] <= 1139.72882f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            if (features[2] <= 1.60012f) {
                                return 2;
                            } else {
                                if (features[0] <= 0.24246f) {
                                    return 0;
                                } else {
                                    return 2;
                                }
                            }
                        }
                    } else {
                        if (features[0] <= 0.43039f) {
                            if (features[8] <= 58.58501f) {
                                if (features[2] <= 1.41475f) {
                                    return 3;
                                } else {
                                    if (features[9] <= 10.24329f) {
                                        return 0;
                                    } else {
                                        return 3;
                                    }
                                }
                            } else {
                                return 2;
                            }
                        } else {
                            return 4;
                        }
                    }
                }
            } else {
                if (features[10] <= 2091.25861f) {
                    if (features[8] <= 63.85955f) {
                        if (features[9] <= 5.63793f) {
                            return 1;
                        } else {
                            if (features[4] <= -0.19253f) {
                                return 0;
                            } else {
                                if (features[7] <= 10.03286f) {
                                    return 1;
                                } else {
                                    return 0;
                                }
                            }
                        }
                    } else {
                        return 2;
                    }
                } else {
                    return 3;
                }
            }
        } else {
            return 5;
        }
    } else {
        if (features[8] <= 56.84742f) {
            if (features[0] <= 0.36579f) {
                if (features[5] <= 0.07885f) {
                    if (features[4] <= 0.02551f) {
                        if (features[4] <= -0.00107f) {
                            if (features[9] <= 10.11003f) {
                                return 0;
                            } else {
                                return 3;
                            }
                        } else {
                            if (features[1] <= 5.55135f) {
                                if (features[5] <= 0.05316f) {
                                    return 3;
                                } else {
                                    return 0;
                                }
                            } else {
                                return 5;
                            }
                        }
                    } else {
                        return 0;
                    }
                } else {
                    if (features[3] <= 0.16636f) {
                        if (features[10] <= 1871.68375f) {
                            return 0;
                        } else {
                            return 3;
                        }
                    } else {
                        if (features[10] <= 1955.13196f) {
                            if (features[2] <= 1.21678f) {
                                if (features[3] <= 0.66018f) {
                                    if (features[5] <= 0.16091f) {
                                        if (features[9] <= 5.37747f) {
                                            return 5;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        return 5;
                                    }
                                } else {
                                    if (features[1] <= 5.45146f) {
                                        return 0;
                                    } else {
                                        return 5;
                                    }
                                }
                            } else {
                                if (features[2] <= 1.22278f) {
                                    if (features[9] <= 5.45841f) {
                                        return 0;
                                    } else {
                                        if (features[7] <= 5.68183f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    if (features[1] <= 5.50180f) {
                                        if (features[0] <= 0.34626f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        return 5;
                                    }
                                }
                            }
                        } else {
                            return 3;
                        }
                    }
                }
            } else {
                if (features[5] <= 0.27744f) {
                    if (features[1] <= 3.53054f) {
                        if (features[8] <= 49.29337f) {
                            if (features[9] <= 5.15320f) {
                                return 1;
                            } else {
                                return 4;
                            }
                        } else {
                            if (features[4] <= 0.12083f) {
                                return 4;
                            } else {
                                if (features[7] <= 1.05941f) {
                                    return 1;
                                } else {
                                    return 4;
                                }
                            }
                        }
                    } else {
                        return 1;
                    }
                } else {
                    if (features[5] <= 0.29551f) {
                        if (features[3] <= 0.61929f) {
                            return 1;
                        } else {
                            return 4;
                        }
                    } else {
                        if (features[1] <= 3.43151f) {
                            return 1;
                        } else {
                            if (features[9] <= 6.68126f) {
                                return 1;
                            } else {
                                return 4;
                            }
                        }
                    }
                }
            }
        } else {
            if (features[11] <= 1470.95081f) {
                if (features[2] <= 1.54592f) {
                    return 2;
                } else {
                    if (features[8] <= 62.80058f) {
                        return 4;
                    } else {
                        return 2;
                    }
                }
            } else {
                return 2;
            }
        }
    }
}

static int predict_tree_7(const float* features) {
    if (features[0] <= 0.36230f) {
        if (features[9] <= 10.00068f) {
            if (features[4] <= 0.00618f) {
                if (features[3] <= 0.24123f) {
                    if (features[8] <= 56.42169f) {
                        if (features[9] <= 4.72262f) {
                            if (features[4] <= -0.07166f) {
                                if (features[6] <= 50.99905f) {
                                    return 5;
                                } else {
                                    if (features[10] <= 892.93716f) {
                                        return 5;
                                    } else {
                                        return 1;
                                    }
                                }
                            } else {
                                if (features[8] <= 45.54169f) {
                                    return 0;
                                } else {
                                    return 5;
                                }
                            }
                        } else {
                            if (features[8] <= 46.89024f) {
                                if (features[0] <= 0.26008f) {
                                    if (features[11] <= 1459.20447f) {
                                        return 0;
                                    } else {
                                        if (features[6] <= 51.60316f) {
                                            return 5;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 1;
                                }
                            } else {
                                return 0;
                            }
                        }
                    } else {
                        return 2;
                    }
                } else {
                    if (features[10] <= 984.87576f) {
                        if (features[7] <= 5.00495f) {
                            if (features[1] <= 5.55663f) {
                                if (features[6] <= 51.36999f) {
                                    if (features[5] <= 0.19902f) {
                                        if (features[8] <= 54.07138f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[0] <= 0.22247f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    if (features[5] <= 0.19434f) {
                                        if (features[6] <= 51.98610f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        return 2;
                                    }
                                }
                            } else {
                                return 5;
                            }
                        } else {
                            if (features[8] <= 55.53736f) {
                                if (features[8] <= 45.20862f) {
                                    if (features[4] <= -0.12721f) {
                                        if (features[0] <= 0.25485f) {
                                            return 5;
                                        } else {
                                            return 5;
                                        }
                                    } else {
                                        if (features[1] <= 5.44682f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                } else {
                                    if (features[1] <= 5.46804f) {
                                        return 0;
                                    } else {
                                        return 5;
                                    }
                                }
                            } else {
                                return 2;
                            }
                        }
                    } else {
                        if (features[9] <= 5.04352f) {
                            return 2;
                        } else {
                            if (features[9] <= 6.20996f) {
                                if (features[5] <= 0.14438f) {
                                    if (features[11] <= 1464.68610f) {
                                        if (features[1] <= 5.49365f) {
                                            return 2;
                                        } else {
                                            return 5;
                                        }
                                    } else {
                                        if (features[8] <= 56.92245f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    if (features[0] <= 0.17168f) {
                                        if (features[10] <= 1049.83167f) {
                                            return 5;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[1] <= 5.52600f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                }
                            } else {
                                if (features[9] <= 6.27980f) {
                                    return 5;
                                } else {
                                    if (features[11] <= 1464.22382f) {
                                        return 0;
                                    } else {
                                        if (features[1] <= 5.36352f) {
                                            return 0;
                                        } else {
                                            return 5;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                if (features[7] <= 0.73176f) {
                    if (features[7] <= 0.43337f) {
                        if (features[8] <= 53.10634f) {
                            return 5;
                        } else {
                            return 2;
                        }
                    } else {
                        if (features[3] <= 0.07342f) {
                            return 2;
                        } else {
                            if (features[8] <= 58.17804f) {
                                return 0;
                            } else {
                                return 2;
                            }
                        }
                    }
                } else {
                    if (features[8] <= 54.48453f) {
                        if (features[1] <= 5.45930f) {
                            if (features[9] <= 4.10614f) {
                                if (features[9] <= 4.08533f) {
                                    return 0;
                                } else {
                                    return 2;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 5;
                        }
                    } else {
                        if (features[9] <= 4.08360f) {
                            if (features[5] <= 0.19996f) {
                                return 2;
                            } else {
                                if (features[10] <= 795.88785f) {
                                    return 2;
                                } else {
                                    return 5;
                                }
                            }
                        } else {
                            if (features[0] <= 0.18414f) {
                                if (features[1] <= 5.78293f) {
                                    return 2;
                                } else {
                                    return 5;
                                }
                            } else {
                                return 2;
                            }
                        }
                    }
                }
            }
        } else {
            return 3;
        }
    } else {
        if (features[5] <= 0.27268f) {
            if (features[0] <= 0.46361f) {
                return 1;
            } else {
                return 4;
            }
        } else {
            if (features[7] <= 9.34692f) {
                return 1;
            } else {
                if (features[7] <= 9.52930f) {
                    return 4;
                } else {
                    if (features[11] <= 1493.34094f) {
                        if (features[0] <= 0.72353f) {
                            return 1;
                        } else {
                            return 4;
                        }
                    } else {
                        return 1;
                    }
                }
            }
        }
    }
}

static int predict_tree_8(const float* features) {
    if (features[8] <= 54.48453f) {
        if (features[0] <= 0.36210f) {
            if (features[6] <= 51.42769f) {
                if (features[1] <= 5.55663f) {
                    if (features[9] <= 10.17324f) {
                        if (features[1] <= 3.53063f) {
                            if (features[0] <= 0.34667f) {
                                if (features[9] <= 4.35991f) {
                                    if (features[6] <= 50.40801f) {
                                        return 0;
                                    } else {
                                        if (features[2] <= 1.32937f) {
                                            return 0;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    return 0;
                                }
                            } else {
                                return 1;
                            }
                        } else {
                            if (features[4] <= 0.06855f) {
                                return 0;
                            } else {
                                return 2;
                            }
                        }
                    } else {
                        return 3;
                    }
                } else {
                    return 5;
                }
            } else {
                if (features[9] <= 10.36306f) {
                    if (features[1] <= 5.46601f) {
                        return 0;
                    } else {
                        return 5;
                    }
                } else {
                    return 3;
                }
            }
        } else {
            if (features[5] <= 0.28256f) {
                if (features[7] <= 0.84066f) {
                    if (features[1] <= 2.98037f) {
                        if (features[1] <= 2.74227f) {
                            return 1;
                        } else {
                            if (features[0] <= 0.47537f) {
                                return 1;
                            } else {
                                return 4;
                            }
                        }
                    } else {
                        if (features[5] <= 0.17227f) {
                            return 4;
                        } else {
                            return 1;
                        }
                    }
                } else {
                    return 4;
                }
            } else {
                if (features[9] <= 6.64904f) {
                    if (features[0] <= 0.74249f) {
                        return 1;
                    } else {
                        return 4;
                    }
                } else {
                    return 4;
                }
            }
        }
    } else {
        if (features[4] <= 0.09847f) {
            if (features[1] <= 5.78379f) {
                if (features[0] <= 0.48877f) {
                    if (features[10] <= 1061.01617f) {
                        return 2;
                    } else {
                        if (features[9] <= 5.43058f) {
                            return 0;
                        } else {
                            return 2;
                        }
                    }
                } else {
                    return 4;
                }
            } else {
                return 5;
            }
        } else {
            if (features[6] <= 51.32288f) {
                if (features[0] <= 0.51313f) {
                    return 2;
                } else {
                    return 4;
                }
            } else {
                if (features[4] <= 0.10661f) {
                    return 4;
                } else {
                    return 2;
                }
            }
        }
    }
}

static int predict_tree_9(const float* features) {
    if (features[9] <= 10.97731f) {
        if (features[9] <= 6.15634f) {
            if (features[9] <= 5.99605f) {
                if (features[0] <= 0.35267f) {
                    if (features[2] <= 1.21112f) {
                        if (features[1] <= 5.45072f) {
                            if (features[1] <= 2.64539f) {
                                return 0;
                            } else {
                                if (features[4] <= 0.09439f) {
                                    if (features[4] <= -0.16359f) {
                                        if (features[9] <= 4.62848f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[8] <= 55.93501f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                } else {
                                    if (features[0] <= 0.15749f) {
                                        return 2;
                                    } else {
                                        if (features[5] <= 0.22664f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    }
                                }
                            }
                        } else {
                            return 5;
                        }
                    } else {
                        if (features[11] <= 1486.45905f) {
                            if (features[1] <= 5.54244f) {
                                if (features[8] <= 53.13356f) {
                                    return 0;
                                } else {
                                    return 2;
                                }
                            } else {
                                return 5;
                            }
                        } else {
                            if (features[1] <= 5.50234f) {
                                if (features[3] <= 0.36327f) {
                                    if (features[0] <= 0.16069f) {
                                        if (features[8] <= 55.33059f) {
                                            return 0;
                                        } else {
                                            return 2;
                                        }
                                    } else {
                                        if (features[0] <= 0.17051f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                } else {
                                    if (features[2] <= 1.37885f) {
                                        if (features[7] <= 2.82249f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    } else {
                                        if (features[3] <= 0.99064f) {
                                            return 2;
                                        } else {
                                            return 0;
                                        }
                                    }
                                }
                            } else {
                                return 5;
                            }
                        }
                    }
                } else {
                    if (features[10] <= 760.20749f) {
                        if (features[10] <= 742.01099f) {
                            if (features[2] <= 1.48300f) {
                                if (features[11] <= 1567.75000f) {
                                    if (features[9] <= 4.71229f) {
                                        return 1;
                                    } else {
                                        return 4;
                                    }
                                } else {
                                    return 4;
                                }
                            } else {
                                if (features[5] <= 0.25670f) {
                                    return 4;
                                } else {
                                    return 1;
                                }
                            }
                        } else {
                            if (features[5] <= 0.34054f) {
                                return 4;
                            } else {
                                return 1;
                            }
                        }
                    } else {
                        if (features[5] <= 0.18184f) {
                            return 4;
                        } else {
                            if (features[0] <= 0.36986f) {
                                if (features[8] <= 44.52589f) {
                                    return 1;
                                } else {
                                    return 5;
                                }
                            } else {
                                return 1;
                            }
                        }
                    }
                }
            } else {
                if (features[1] <= 5.47541f) {
                    if (features[8] <= 58.59890f) {
                        if (features[0] <= 0.58962f) {
                            if (features[8] <= 36.91824f) {
                                return 0;
                            } else {
                                if (features[1] <= 2.87620f) {
                                    return 0;
                                } else {
                                    if (features[5] <= 0.23508f) {
                                        return 0;
                                    } else {
                                        return 1;
                                    }
                                }
                            }
                        } else {
                            return 4;
                        }
                    } else {
                        return 2;
                    }
                } else {
                    return 5;
                }
            }
        } else {
            if (features[0] <= 0.54333f) {
                if (features[8] <= 57.52139f) {
                    if (features[0] <= 0.32202f) {
                        if (features[7] <= 4.01433f) {
                            if (features[11] <= 1547.60895f) {
                                if (features[1] <= 5.82562f) {
                                    return 0;
                                } else {
                                    return 5;
                                }
                            } else {
                                return 5;
                            }
                        } else {
                            return 5;
                        }
                    } else {
                        return 1;
                    }
                } else {
                    return 2;
                }
            } else {
                if (features[10] <= 1211.07227f) {
                    return 4;
                } else {
                    if (features[11] <= 1485.67944f) {
                        return 1;
                    } else {
                        return 4;
                    }
                }
            }
        }
    } else {
        return 3;
    }
}

/**
 * @brief Fonction d'inférence principale (Random Forest)
 * @param features Tableau de 12 caractéristiques
 * @return Code de défaut prédit
 */
int predict_machine_state(const float* features) {
    int votes[6];
    for (int i = 0; i < 6; i++) votes[i] = 0;
    votes[predict_tree_0(features)]++;
    votes[predict_tree_1(features)]++;
    votes[predict_tree_2(features)]++;
    votes[predict_tree_3(features)]++;
    votes[predict_tree_4(features)]++;
    votes[predict_tree_5(features)]++;
    votes[predict_tree_6(features)]++;
    votes[predict_tree_7(features)]++;
    votes[predict_tree_8(features)]++;
    votes[predict_tree_9(features)]++;

    // Recherche du label ayant le maximum de votes
    int max_votes = -1;
    int best_class = -1;
    for (int i = 0; i < 6; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            best_class = i;
        }
    }
    return best_class;
}