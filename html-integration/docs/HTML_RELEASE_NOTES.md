# BloodyMaster 0.3.9 — LoudnessWar et gain d’entrée

Ajoute IN GAIN (−18 à +18 dB), avant la chaîne, et LoudnessWar (0 à +18 dB) dans le panneau doré final. Blood Drive reste inchangé. LoudnessWar pousse après la coloration, avant le clipper suréchantillonné et le limiteur : le plafond reste prioritaire. Les deux nouveaux gains sont à 0 dB par défaut et dans les anciens presets/sessions.

Corrige la suratténuation de la protection finale : mesure d’énergie K sur 400 ms, gains de crêtes et de loudness séparés. Objectif de sécurité global −5 LUFS conservé ; plafond maximal −2 dB avec réserve interne de 0,5 dB. Régler Ceiling Out à −3 dB pour le plafond demandé ; LoudnessWar ne le modifie pas. Atteindre davantage de loudness peut modifier la dynamique : aucune transparence absolue ni cible automatique par morceau promise.

Presets User v1 compatibles, sauvegardes v2 ; anciens réglages et IDs conservés. Delta aligné inclut les nouveaux gains, garde ses transitions et ses remises à zéro. HTML originaux conservés.

La réserve de crêtes réduit le risque après conversion mais ne garantit pas tous les encodeurs : du bruit dense (+0,42 dBTP) et le beat musical fortement poussé (+0,012 dBTP) dépassent 0 après AAC. Contrôler le fichier encodé complet avant diffusion. Voir LOUDNESS_GAIN_FR.md pour les mesures et limites.

Installateur non signé, version de test : fermer FL Studio avant installation puis rescanner les plugins. Validation d’écoute dans FL Studio encore nécessaire.
