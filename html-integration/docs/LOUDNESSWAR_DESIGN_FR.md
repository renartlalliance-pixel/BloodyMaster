# LoudnessWar — décision de conception, non encore implémentée

Demande utilisateur : −5 LUFS doit être une protection globale du VST, indépendante du potard LoudnessWar. Le choix technique est délégué à Codex.

## Décision

Séparer le gain d'accessibilité LoudnessWar d'une protection finale commune. LoudnessWar exploite la marge mesurée ; il ne change pas les EQ, colorations ou réglages de dynamique et ne force pas l'atteinte de −5 LUFS. La protection finale doit pouvoir atténuer le signal de façon stéréo liée, même quand LoudnessWar vaut zéro. Afficher la réduction de protection et la raison du plafonnement.

Retenir −5 LUFS comme borne supérieure du programme intégré. La mesure intégrée dépend du programme complet et du gating : ce n'est pas un plafond instantané. Ajouter une surveillance court terme 3 s pour éviter de masquer un passage durablement trop fort dans une moyenne globale plus faible. Le comportement temps réel devra être décrit comme une régulation mesurée, avec son temps de réaction, et non comme une garantie absolue sur tout programme futur. Une validation de l'intégré sur un export complet est nécessaire pour annoncer son respect sur ce fichier.

Plafond final de crêtes : −2 dBTP comme choix prudent de distribution, ou plus bas si l'utilisateur a choisi un plafond inférieur. Le limiteur actuel et le mesureur 4× ne suffisent pas à revendiquer cette garantie après toutes conversions. Renforcer et vérifier la limitation finale avant de présenter cette fonction comme une protection true peak complète.

Privilégier une réduction de gain stable ou lente, sans remontées rapides de volume. Une protection qui intervient modifie nécessairement le niveau, et peut modifier l'enveloppe ; ne pas promettre simultanément une sortie toujours plafonnée et un signal inchangé en toutes circonstances. Une source déjà au-dessus du seuil doit être atténuée, pas seulement empêchée de monter davantage.

Une protection globale active dans le plugin implique de distinguer le bypass des traitements (protection conservée et indiquée) d'un vrai bypass externe du DAW. Le plugin ne peut protéger aucune sortie lorsque l'hôte cesse de l'exécuter. Le bypass bit-exact actuel ne doit pas être modifié silencieusement : documenter et tester ce changement fonctionnel avant livraison.

## Vérifications nécessaires à l'implémentation

- Mesurer les sorties effectives après gain, protection, transitions de bypass et auditions ; les contrôles ne doivent pas pouvoir contourner la protection.
- Tester silence, démarrage fort, variations brusques, programme long et gating avec une mesure indépendante BS.1770 ; publier les limites de réaction.
- Vérifier crêtes reconstruites indépendamment, SRC 44,1/48 kHz dans les deux sens et encodages/décodages AAC/MP3 disponibles. −2 dBTP réduit le risque, sans garantir tous les encodeurs et traitements des plateformes.
- Conserver les 50 IDs existants. Si le potard ajoute un paramètre APVTS, migrer explicitement les presets User version 1 (qui exigent actuellement 50 valeurs) et les anciennes sessions, avec LoudnessWar à zéro.
- Conserver exactement les HTML originaux et déclarer toute latence ajoutée ; réaligner le bypass.

Sources déjà consultées : recommandation Spotify de rester sous −2 dBTP pour les masters dépassant −14 LUFS (https://support.spotify.com/us/artists/article/loudness-normalization/) ; Apple Digital Masters, conversion de fréquence et contrôle de l'encodage (https://www.apple.com/apple-music/apple-digital-masters/docs/apple-digital-masters.pdf).
