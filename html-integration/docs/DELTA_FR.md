# Écoute Delta globale — 0.3.8

Bouton DELTA : « Écouter la différence entre le signal original et le signal traité. » État actif ambré, aria-pressed et indication d'écoute diagnostique. Le bouton existe dans l'interface HTML générée et dans l'éditeur natif de repli. Les deux HTML originaux approuvés ne sont pas modifiés.

## Signal

Delta = entrée originale retardée − sortie réelle de toute la chaîne. La référence est capturée avant tout DSP. La sortie soustraite inclut EQ, coloration, dynamique, oversampling, clipper, limiteur, gain/compensation d'écoute éventuelle et protection globale finale. Aucune compensation de volume du signal original, inversion du signe ou normalisation supplémentaire n'est ajoutée.

La référence est retardée exactement de la latence déclarée : 192 + ceil(Fs×0,003) + 32 + ceil(Fs×0,010) échantillons. À 48 kHz : 848 échantillons. La latence de la chaîne reste constante pendant les changements d'oversampling ; Delta n'ajoute aucune latence de sortie. Le retard de transport est compensé, mais la réponse en fréquence et la phase des filtres restent des différences légitimes du traitement.

Transition linéaire de 20 ms entre sortie normale et différence ; après la transition, Delta désactivé restitue exactement les échantillons normaux. Le signal de référence continue de circuler pendant l'écoute normale pour éviter un historique vide lors d'une activation tardive. Entrées non finies remplacées par zéro pour la référence, comme pour la sanitisation d'entrée du moteur.

La protection globale reste dans la chaîne master comparée. **La différence brute est une écoute diagnostique après cette protection : son niveau peut dépasser le plafond du master.** Un second limiteur sur cette différence fausserait l'égalité demandée ; il n'est donc pas appliqué à Delta. L'indication de sortie et son infobulle signalent cette distinction. Crêtes, RMS, LUFS et scope reflètent ce qui est entendu.

Delta global prend priorité sur le solo de nettoyage Spike Listen, suspendu sans modifier son paramètre. La chaîne de mastering continue de fonctionner. À la sortie de Delta, ce solo retrouve son état précédent avec ses transitions habituelles. Les autres paramètres DSP restent strictement identiques.

## État et export

Convention des écoutes temporaires existantes (A/B compensé) : booléen atomique privé au processeur, commande native depuis l'interface et état renvoyé au navigateur. Aucun nouveau paramètre de traitement APVTS ; les 50 IDs, sessions et presets User version 1 restent compatibles. Delta n'est ni enregistré dans les presets ni rétabli depuis une session.

Delta est désactivé à la construction, dans prepareToPlay, au rappel Factory, au rappel User réussi, à la restauration d'état et lors d'un échange A/B. La désactivation ordinaire est fondue sur 20 ms. Lors d'un rendu hors ligne signalé par l'hôte, il est forcé immédiatement sur la sortie normale pour éviter tout début d'export en Delta. Pour un export en temps réel, vérifier que DELTA est éteint : l'hôte ne permet pas de distinguer cet export d'une écoute.

## Temps réel

Retard et deux buffers de référence alloués dans prepareToPlay. Pas de resize, allocation, mutex, accès disque ou appel UI dans le callback. Les blocs plus grands que la taille annoncée sont découpés en vues JUCE mono/stéréo sans allocation : leur tableau de pointeurs est stocké en ligne (JUCE 8.0.4). Référence capturée puis traitée par tranche avant réutilisation du buffer ; compteurs et vumètres agrégés sur tout le bloc. Aucun ajout de latence.

## Vérification

Tests DSP : 44,1/48/96/192 kHz, mono/stéréo, latences 2×/4×/8×, annulation du gain unitaire, signe et différence avec gain 0,5, cas sans latence, transitions, retour normal exact et désactivation hors ligne. Test sous ASan/UBSan.

Tests plugin : deux instances indépendantes (normale/Delta), compensation complète de la latence, null exact en bypass protégé faible, soustraction du vrai signal traité avec gain/phase, blocs de 257 échantillons malgré une préparation de 64, paramètres inchangés, retour bit-exact à l'écoute normale, réinitialisations et rendu hors ligne. Le bypass faible sert de traitement réellement neutre ; des filtres actifs, même avec leurs potards de gain à zéro, ne constituent pas un chemin identité.

Tests WebView2 natifs étendus au bouton, infobulle exacte, paramètres invariants et rappels Factory/User. Workflow Windows 43482536001 réussi : 16/16 CTest, 35 vérifications WebView2, 31 tests WAV et tests installateur. Installateur téléchargé et SHA-256 vérifié ; voir WINDOWS_038_VERIFICATION.md. Aucune écoute humaine ni validation FL Studio n'est revendiquée par ces tests numériques.

Contrôle local d’allocations supplémentaire : remplacement instrumenté des opérateurs new/delete, 1 000 blocs de 257 échantillons avec activations/désactivations répétées après préparation, zéro allocation C++ observée dans capture/update/process de DeltaMonitor. Inspection de JUCE 8.0.4 confirme le tableau de pointeurs en ligne des vues AudioBuffer mono/stéréo.
