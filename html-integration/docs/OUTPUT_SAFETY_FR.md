# Protection globale — 0.3.7

Protection active sur toute sortie traitée par le plugin : après les DSP, écoute Spike/delta, bypass des traitements, bypass traité par l'hôte et compensation d'écoute A/B. Dans le plugin elle est le dernier traitement ; mesures et scope sont ensuite relevés sur le signal réellement délivré. Dans l'outil WAV elle est activée au dernier étage du moteur, compensation de latence incluse. Un hôte qui ne fait plus exécuter le plugin peut contourner toute protection du plugin.

Aucun nouveau paramètre : les 50 IDs et presets User sont conservés. Aucun potard LoudnessWar ajouté à cette étape. La protection n'est pas désactivable par un preset, ISP off ou le choix de liaison stéréo du limiteur précédent. Elle ne monte jamais le volume. Affichage « Protection sortie : −x dB » ; le bouton bypass indique que la protection reste active.

## Méthode et latence

Détection de crêtes pleine bande sinc Blackman à 8×, support 64 échantillons, liaison stéréo complète, anticipation 10 ms. Détection de l'énergie pondérée K, attaque 1 ms, récupération 400 ms ; seuil prudent équivalent à −6 LUFS pour garder une réserve avant la limite souhaitée de −5 LUFS. Demandes combinées dans une fenêtre d'anticipation, attaque du gain 0,2 ms et récupération 2 s. Une demande de protection peut réduire la dynamique et le volume : elle ne promet pas un signal inchangé lorsqu'elle intervient.

Plafond demandé retenu : min(plafond utilisateur, −2 dB), avec réserve interne supplémentaire de 0,5 dB pour la reconstruction et les variations du gain. La crête d'échantillons est bornée par une atténuation stéréo liée, sans écrêtage canal par canal. Ce traitement ne rééchantillonne pas l'audio ; l'interpolation sert uniquement à la détection.

Latence ajoutée = 32 + ceil(Fs × 0,010) échantillons ; à 48 kHz, total du plugin 848 échantillons au lieu de 336. Déclaration au DAW et tests d'alignement mis à jour. Le bypass faible reste bit-exact après ce délai ; un bypass trop fort est atténué. Les changements de plafond vers le bas peuvent imposer une réduction immédiate. Aucun accès disque, allocation ou verrou dans la boucle audio ; buffers alloués dans prepare.

RAZ mesures ne réinitialise pas la protection. L'apprentissage A/B utilise un mesureur distinct avant compensation/protection ; l'affichage final utilise la sortie protégée. Une différence de réduction de protection entre A et B peut limiter la précision de la compensation d'écoute.

## Limites explicites

−5 LUFS est une limite de programme, pas un seuil instantané. Le détecteur rapide d'énergie est une protection conservatrice, pas un limiteur mathématique de toutes les fenêtres BS.1770 possibles. L'intégré exact d'un programme inconnu doit être contrôlé sur l'export complet. La réserve −6 LUFS et les tests indépendants visent cet objectif sans prétendre garantir tout signal futur, toute phase de démarrage ou toute automation.

La crête vraie reste une estimation FIR finie ; les conversions et encodeurs peuvent recréer des crêtes. −2 dBTP avec réserve réduit le risque mais ne garantit pas toutes les plateformes. Voir OUTPUT_SAFETY_VALIDATION.json : 16 rendus stéréo (44,1/48 kHz, dense, bruit, crêtes inter-échantillons, marche DC ; bypass et preset Trap), crêtes indépendantes SciPy 32×, conversions 44,1↔48 kHz, encodage/décodage AAC 256 kbit/s et MP3 320 kbit/s. LUFS indépendant à 48 kHz avec coefficients BS.1770 publiés, gating et fenêtres 400 ms. Les sorties initiales passent −2 dBTP sur ces cas ; les sorties converties/décodées restent sous 0 dBTP. Ce sont des observations, pas une certification universelle.

Les tests DSP de composants utilisent le mode interne brut de Engine pour préserver leurs comparaisons de référence. Les deux chemins livrés (PluginProcessor et outil WAV) activent explicitement la protection ; tests dédiés vérifient le produit en bypass et son niveau réel.

Campagne de régression supplémentaire : 60 vecteurs (44,1/48/96/192 kHz × 2×/4×/8×), crêtes reconstruites indépendamment à 32×. Pire crête observée −2,42463 dBTP. Bypass faible bit-exact et sources intactes ; comparaison d’alias normalisée par le fondamental pour ne pas confondre atténuation de protection et réduction d’alias. Rapport OUTPUT_SAFETY_60_VECTORS.json.

Complément de diffusion : 32 encodages/décodages Opus 160 kbit/s et Vorbis qualité 6, incluant la conversion automatique de fréquence nécessaire à Opus. Crêtes décodées reconstruites à 32× ; pire crête observée −2,53459 dBTP. Voir OUTPUT_SAFETY_OPUS_VORBIS.json. Ce contrôle ne simule pas tous les traitements propres à chaque plateforme.
