# BloodyMaster 0.3.7 — protection globale de sortie

Ajoute une protection finale indépendante des presets et du futur potard LoudnessWar : crêtes liées en stéréo, plafond global −2 dBTP avec réserve, régulation prudente d'énergie pondérée K pour rester sous la limite souhaitée de −5 LUFS. Aucun gain ajouté. Les mesures indiquent la sortie réellement délivrée.

Important : le bypass désactive les traitements mais conserve cette protection. Un signal faible reste inchangé après compensation du délai ; un signal trop fort peut être atténué en bypass. Latence totale à 48 kHz : 848 échantillons, déclarée au DAW. Le plugin ne peut agir si l'hôte cesse de l'exécuter.

Les LUFS ne sont pas un plafond instantané ; vérifier l'intégré de l'export complet. Les limites et vérifications de crêtes, SRC et AAC/MP3 sont documentées dans OUTPUT_SAFETY_FR.md. Aucun chiffre ne garantit toutes les conversions des plateformes.

Presets User, 17 presets Factory, paramètres et HTML originaux conservés. Installateur non signé, version de test ; fermer FL Studio puis rescanner après installation. Écoute humaine et validation FL Studio encore à effectuer.
