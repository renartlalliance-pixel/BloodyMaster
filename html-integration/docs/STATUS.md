# Nettoyage dynamique — étape en cours, 0.3.2

Choix utilisateur : De-Harsh ciblé et transparent ; Mud/Ring dynamiques ; profondeur jusqu’à 12 dB ; dosages/activations séparés ; Spike Listen écoute le signal retiré par les trois bandes.

Implémentation : filtres TPT, détection de prépondérance spectrale liée en stéréo, gate à −70 dBFS RMS, gains et fréquences lissés, budget de profondeur partagé. Le nettoyage précède Drive/EQ. Le delta est prélevé avant ces étages et retardé à la latence déclarée ; le bypass global conserve la priorité. Ce n’est pas une recherche automatique de résonances.

Validation locale observée : sept suites CTest réussies ; tests ciblés à 44,1/48/96/192 kHz, activations et dosages indépendants, fréquences automatisées, signal faible, silence, reconstruction du delta, latence et bypass. ASan/UBSan réussis (détection des fuites désactivée). Tests navigateur : 36 boutons rotatifs, 33 actifs, contrôles Traps et Listen, automation et presets ; aucune requête externe ni erreur JavaScript. Les couleurs et dimensions des boutons d’origine sont conservées, les contrôles ajoutés occupent davantage de hauteur.

Les tests natifs Windows de cette nouvelle étape sont préparés dans le workflow ; leur réussite reste à constater. La version 0.3.1 précédente a passé la compilation MSVC/JUCE 8.0.4, neuf suites Windows, 31 essais WAV et les tests d’installation/réinstallation/désinstallation.

Restent : THD/XL, coloration Valve/Tape, Stereo Enhance et profils/Mid pan, mesures LUFS/true peak, comparaison compensée. Les décisions de couleur THD/XL/Valve sont demandées par QCM. Pas d’écoute comparative sur les machines analogiques ni de validation FL Studio observée.
