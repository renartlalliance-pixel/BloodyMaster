# 0.3.10 — réduire la charge de traitement

Retour utilisateur : FL Studio consomme environ la moitié du budget de traitement disponible, même au buffer maximal. Cela décrit une charge audio élevée, à distinguer de la latence déclarée par le plugin (848 échantillons à 48 kHz). L'optimisation ci-dessous vise le temps de calcul ; elle ne réduit pas le lookahead ni la latence.

## Changement

Le profilage local attribue environ 72 % du temps échantillonné au chemin PeakChain (incluant ses FIR et du code incorporé). Les convolutions de l'interpolation, de la décimation et des détecteurs de crêtes faisaient un test de bouclage du tampon pour chaque coefficient. Le chemin d'interpolation utilisait aussi des coefficients espacés en mémoire.

Les historiques sont désormais des anneaux dupliqués : chaque somme lit une plage contiguë sans branche de bouclage par coefficient. Les coefficients sont regroupés par phase à la préparation. Le bouclage et la duplication se font à l'écriture. Coefficients, ordre des termes de chaque somme, facteurs de suréchantillonnage, lissage, filtres, dynamique, couleurs, protections et latence conservés. Aucun nouvel accès disque, verrou ou allocation dans process. Compromis : un peu plus de mémoire pour réduire les accès et branchements dans les calculs FIR.

Pas de changement des paramètres, presets, Drive, LoudnessWar, Delta ou des deux HTML originaux. La notice 0.3.9 reste valable pour les commandes.

## Mesures locales

Sonde reproductible : tools/benchmark_dsp.cpp. G++ C++20 -O3 -DNDEBUG, Linux, stéréo 48 kHz, blocs de 512, trois secondes audio, cinq passages alternés avant/après par mode. Elle inclut le moteur, la protection finale et les deux mesureurs ; exclut JUCE, l'interface, Delta et le pilote. Les pourcentages représentent le budget d'un cœur sur cette machine, pas le compteur FL Studio.

| Mode | Médiane avant | Médiane après | Baisse du temps de calcul |
|---|---:|---:|---:|
| 2× | 0,795730 s | 0,532797 s | 33,0 % |
| 4× | 0,986531 s | 0,686471 s | 30,4 % |
| 8× | 1,324792 s | 0,948505 s | 28,4 % |

Les mesures fluctuent avec l'environnement ; données brutes dans PERFORMANCE_0310_TIMING.json. Aucun gain exact promis sur le PC de l'utilisateur. Mesurer le même passage du même projet, au même Fs, buffer et facteur OS, avant/après ; noter si la fenêtre du plugin est ouverte. Tester les passages denses et le plus petit buffer réellement utilisé.

## Équivalence et vérification

36 WAV comparés octet par octet entre les outils avant/après compilés avec les mêmes options : quatre Fs, trois OS, Clean/Trap/Bypass, entrée stéréo avec bruit et crête, IN GAIN +3 et LoudnessWar +6. Tous identiques ; voir PERFORMANCE_0310_EQUIVALENCE.json. Ceci prouve ces cas sous ce compilateur, pas tous les signaux et compilateurs futurs.

Nouveau test FIR contre une convolution de référence à registre décalé indépendant : 2×/4×/8×, stéréo, plusieurs tours de tampon, queue silencieuse et nouvelle préparation. Tests existants de latence, transitions OS, protections, loudness, Delta et presets conservés. Validation Windows 43625462716 réussie : 18/18 CTest, 37 vérifications WebView2, 31 tests WAV et tests d’installation. Voir STATUS.md pour le téléchargement et le SHA-256.

Contrôle ASan/UBSan du nouveau test FIR réussi. Les 31 tests WAV passent ; 60 vecteurs indépendants passent également (pire crête reconstruite 32× −2,42461 dBTP). Aucun essai FL Studio réalisé ici.


Complément à la fréquence du projet utilisateur, 44,1 kHz : même sonde avec fs=44100 et cinq passages alternés par mode. Médianes 4× : 0,919157 → 0,661926 s (−28,0 %) ; 8× : 1,313623 → 0,937150 s (−28,7 %). Voir PERFORMANCE_0310_44100.json pour les fluctuations ; ces chiffres Linux ne sont pas une mesure du compteur FL Studio.

ASan/UBSan réussit aussi les suites du mesureur AudioMeter et de la protection OutputSafety après modification de leurs anneaux.
