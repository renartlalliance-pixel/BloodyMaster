# 0.3.9 — gains et correction du master final

IN GAIN : −18 à +18 dB, rampe de 20 ms, avant nettoyage/EQ/dynamique. Référence Delta et mesures d’entrée prises avant ce gain. Blood Drive et Dynamics.h inchangés. LoudnessWar : 0 à +18 dB, rampe de 20 ms, après la coloration, avant le clipper suréchantillonné et le limiteur existants. Hammer conserve sa place et son rôle. Les paramètres précédents ne bougent pas ; deux IDs ajoutés à la fin (52 paramètres, 51 DSP actifs).

LoudnessWar est un gain de poussée protégé, pas un algorithme d’optimisation automatique de transparence. Quand le plafond/limiteur intervient déjà, tourner davantage agit surtout sur la densité et peut déformer. Une cible −10 à −7 LUFS dépend du morceau et ne peut pas être garantie sans modification de dynamique. Les réglages de coloration ne sont pas changés automatiquement. Pour écouter la hausse réelle, désactiver la compensation A/B.

## Protection corrigée

La 0.3.8 estimait l’énergie par enveloppe très rapide, avec récupération du gain de crêtes de 2 s : les transitoires maintenaient une atténuation excessive. Comparaison sur le même beat synthétique à −22 LUFS, Drive 12, Hammer 6, Crush Clip 1,8, plafond −3 : ancien master protégé −16,01 LUFS ; même chaîne sans protection finale −11,73 LUFS ; nouvelle protection environ −11,80 LUFS avant LoudnessWar.

La 0.3.9 sépare les demandes : crêtes 8× et récupération 80 ms ; énergie K moyenne glissante 400 ms et récupération 2 s. Gain appliqué = minimum des deux protections, pas leur produit. Seuil énergie prudent équivalent −5,5 LUFS, objectif global −5 LUFS ; startup calculé sur le nombre d’échantillons disponibles. Détection, anticipation 10 ms, réserve de crêtes 0,5 dB et latence totale 848 échantillons à 48 kHz conservées. Pas de gain ascendant dans la protection, pas d’allocation dans process.

Ce plafond est une protection de programme, pas la preuve d’une borne sur tout intégré futur. Vérifier l’export complet. Le plafond utilisateur est plafonné à −2 dB, réserve additionnelle de 0,5 dB. Avec Ceiling Out −3, la borne d’échantillons est −3,5 dB ; le true peak reconstruit est mesuré séparément. Delta reste une différence brute diagnostique après protection et peut dépasser ce plafond.

## Migration et tests

Deux gains à 0 dB dans les 17 presets Factory. Anciennes sessions et User v1 sans ces champs : rappel explicite à zéro ; leurs autres valeurs restent identiques. Sauvegarde User v2 avec les 52 paramètres. Les fichiers v1 ne sont pas réécrits. Delta inclut les deux nouveaux gains ; annulation neutre et retour normal conservés.

Tests de gains à quatre fréquences, trois facteurs OS, bypass ; tests plugin de migration, sauvegarde v2 et indépendance de Drive ; tests natifs WebView2 de commande et automation. Le script Tests/DSP/validate_loudness_gain.py construit un beat déterministe, mesure le loudness avec coefficients BS.1770 publiés et gating indépendant, reconstruit les crêtes SciPy 32× et relève les dépassements AAC/MP3 si ffmpeg est installé. Voir LOUDNESS_GAIN_VALIDATION.json pour les résultats. Ce beat ne représente pas le fichier de l’utilisateur.

Stress additionnel : 16 rendus 44,1/48 kHz, crêtes 32×, SRC et AAC/MP3 ; voir OUTPUT_SAFETY_039_STRESS.json. Crêtes initiales sous −2 dBTP et conversions sous 0 sur ces vecteurs ; LUFS intégré et fenêtres 400 ms sous −5 sur les cas mesurés à 48 kHz. Un bruit dense traité à 44,1 kHz dépasse 0 après AAC (environ +0,42 dBTP). À LoudnessWar +12 sur le beat musical, AAC atteint aussi environ +0,012 dBTP malgré un master brut sous −3 dBTP. Ces dépassements sont enregistrés comme limites, pas présentés comme une réussite de protection codec. La réserve ne garantit donc pas toutes les plateformes ni tous les signaux. Les rapports 0.3.7 restent historiques et ne prouvent pas le comportement 0.3.9.

HTML originaux inchangés : SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff. Écoute humaine/FL Studio à faire.
