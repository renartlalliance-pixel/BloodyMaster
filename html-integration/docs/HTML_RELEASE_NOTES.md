# BloodyMaster 0.3.5 — version de test HTML

Ajoute Atmosphere, EQ sur le Side avec quatre profils et dosage, centre préservé, grave très peu modifié, sans latence supplémentaire. Mid Pan déplace désormais le centre en gardant les côtés fixes ; les anciens réglages de pan non nuls peuvent changer de rendu. Voir STEREO_DESIGN_FR.md.

Conserve Tape Color indépendant (Discret/Marqué), THD/XL, nettoyages, compression et limiteur. HTML originaux conservés.

Ajoute les mesures réelles LUFS intégré/court terme 3 s et true peak estimé 4×, RAZ manuelle et comparaison A/B compensée. Écouter les deux états au moins 3 s ; atténuation seule jusqu'à 12 dB. **Couper la compensation avant export temps réel.** Elle est désactivée par défaut et ignorée au rendu hors ligne signalé par l'hôte. Mesures avant compensation. Presets de genre enrichis légèrement. Voir METERING_FR.md pour limites, gates et protocole de vérification. TARGET ne normalise pas le fichier.

Reste la validation FL Studio et l'écoute comparative humaine. Installateur non signé, version de test. Fermer FL Studio avant installation et rescanner les plugins. WebView2 installé si nécessaire avec une connexion dans ce cas.
