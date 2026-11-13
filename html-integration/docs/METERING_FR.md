# Mesures et comparaison d'écoute — 0.3.5

Choix utilisateur A : LUFS intégré + court terme (3 s) + true peak, remise à zéro manuelle. Comparaison A/B compensée désactivée par défaut. Réponses A données dans le chat ; à l'avenir A par défaut, question seulement si un arbitrage important le nécessite.

## Mesures

Mesure de la sortie DSP avant la compensation temporaire d'écoute, y compris pendant le bypass. Pondération K BS.1770, énergie mono comptée une fois et somme des deux canaux en stéréo. Intégré : fenêtres de 400 ms espacées de 100 ms, gate absolu −70 LUFS et relatif −10 LU. Stockage borné en histogramme de 0,01 LU ; la position du gate relatif est quantifiée à un bin. Court terme : 3 s, sans gate. Les fenêtres incomplètes ne sont pas affichées ; silence/mesure indisponible = « — ». Mesure continue pendant les callbacks audio, sans suivi du transport ni remise à zéro automatique au changement de morceau. Cliquer RAZ mesures pour commencer une nouvelle mesure et réinitialiser l'apprentissage A/B. Changer la fréquence d'échantillonnage réinitialise également les mesures.

Crête vraie estimée : interpolation sinc Blackman 4×, support 64 échantillons, maximum cumulé incluant les crêtes d'échantillons. Observation décalée de 32 échantillons ; laisser le moteur traiter la fin du signal et du silence pour observer les derniers pics interpolés. Cette analyse passive ne retarde pas le signal. Ce n'est pas une certification BS.1770/EBU ni une nouvelle garantie du limiteur. Le filtre fini et la grille 4× peuvent sous-estimer certaines crêtes, notamment près de Nyquist.

Vérification indépendante : coefficients K 48 kHz publiés, SciPy, gates recalculés exactement et reconstruction 32× pour les crêtes. Rapport METER_VALIDATION.json. Cas mono/stéréo, 60 Hz/1 kHz/10 kHz, changements de niveau, bruit avec gate ; sinusoïdes 6/12/18 kHz pour le pic. Les tests C++ couvrent aussi quatre Fs, silence, temps de disponibilité, reset, différence mono/stéréo et crête inter-échantillons. La réussite de ces cas ne garantit pas tous les signaux.

## Compensation temporaire A/B

Activer « Comp. écoute », écouter A et B pendant au moins 3 s chacun. Leur LUFS court terme non compensé est mémorisé ; seul le plus fort est atténué (maximum 12 dB), gain stéréo lié et transition 100 ms. Aucun boost après le limiteur. Modifier un réglage du snapshot invalide son niveau appris et relance les 3 s d'apprentissage. Les valeurs évoluent avec le contenu : comparer le même passage et laisser chaque état se stabiliser. Ce n'est pas une normalisation de fichier.

Les LUFS et vumètres DSP restent avant cette compensation, pour mesurer le traitement exportable. Le scope de sortie montre l'écoute réelle, donc peut refléter l'atténuation. Le bouton affiche le gain demandé ; la transition prend 100 ms. Le bypass désactive immédiatement la compensation. Un rendu hors ligne reconnu par l'hôte applique immédiatement un gain unité.

**Couper Comp. écoute avant tout export en temps réel.** Un plugin ne peut pas distinguer ce rendu de l'écoute. L'option est temporaire, désactivée à la création du processeur, et n'est pas sauvegardée dans les presets ni les sessions. Aucun nouveau paramètre APVTS ; IDs historiques conservés. Pas de normalisation automatique : le sélecteur TARGET reste inactif et ne change pas le son.

## Temps réel

Mémoire des mesures fixe, coefficients pré-calculés dans prepare ; aucune allocation, aucun verrou, aucune UI dans les boucles audio. Reset demandé par flag atomique et exécuté dans le callback audio. Mesures publiées atomiquement vers l'interface. La mesure true peak ajoute du coût CPU, pas de délai audio.
