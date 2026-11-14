# BloodyMaster 0.3.10 — calcul audio allégé

Optimise les convolutions FIR du suréchantillonnage et de la détection des crêtes : historiques contigus et coefficients regroupés par phase. Les courbes, DSP, Drive, LoudnessWar, paramètres, presets et protections restent inchangés. Aucun compromis ajouté sur le lookahead : latence identique, 848 échantillons à 48 kHz. HTML originaux conservés.

Sur le banc Linux : environ 28 % de temps de calcul en moins en 8×, 30 % en 4× et 33 % en 2×. Ce ne sont pas des mesures dans FL Studio ; résultat réel à vérifier sur votre projet. 36 WAV locaux avant/après sont identiques octet par octet. Voir PERFORMANCE_FR.md et STATUS.md pour les méthodes et validations.

Fermer FL Studio avant installation puis rescanner le VST3. La notice 0.3.9 reste valable. Les limites de protection et de reconstruction après codecs restent celles documentées en 0.3.9 ; contrôler le fichier livré.
