# Banque d'inspirations — sélection du 13 novembre 2026

Huit propositions originales BloodyMaster, deux par genre, ajoutées aux neuf presets existants. Il s'agit d'une sélection d'ingénieurs reconnus et d'influences encore pertinentes pour ces styles en 2026, pas d'un classement officiel, ni d'une affirmation que leurs chaînes actuelles sont connues. Les sources vont d'entretiens historiques à des publications récentes. Aucun chiffre de réglage ci-dessous ne vient d'un preset officiel ou d'une session privée.

Les noms du menu portent « inspire ». Les choix chiffrés sont nos adaptations de mastering sur un mix stéréo ; ils ne reproduisent pas un travail multipiste, des voix, une console ou un ingénieur. Les différences de drive et de niveau doivent être comparées à volume compensé, puis la compensation coupée avant export temps réel.

| Section | Preset / ingénieur d'inspiration | Intention originale BloodyMaster |
|---|---|---|
| Rap | Jaycen Joshua | Impact des transitoires, grave centré et contrôle modéré des crêtes ; présence légère sans exagérer la largeur. |
| Rap | Derek « MixedByAli » Ali | Texture discrète, coloration Tape/THD légère, image stable et compression moins appuyée. |
| Electro | Luca Pretolesi | Partager le contrôle entre dynamique et clipper, soutenir le punch, grave mono et air latéral léger. |
| Electro | Wez Clarke | Cohésion propre, haut ouvert et coloration réduite. |
| New Rock | Zakk Cervini | Présence et impact d'un rock moderne dense ; de-harsh léger et image contenue. |
| New Rock | Andy Wallace | Cohésion du bus, contraste des transitoires et faible clipping ; influence rock de référence, sans prétendre une nouvelle chaîne 2026. |
| Trap | Alex Tumay | Réserve pour les 808, très peu de clipping ajouté et compression moins serrée. |
| Trap | Mike Dean | Couleur plus dense, sub stabilisé et espace pour les nappes ; aucune reproduction d'un master de Travis Scott. |

Tous ont un plafond −1 dB, ISP actif, suréchantillonnage 8×, pan centré et liaisons stéréo à 100 %. Aucun ne vise automatiquement un niveau LUFS donné. Les fréquences extrêmes restent contraintes par le moteur à la fréquence d'échantillonnage utilisée. Les détails physiques exacts sont exportés depuis la banque C++ dans `Resources/FactoryPresets.json`, source unique commune au plugin et à l'outil WAV.

## Sources consultées et portée

- **Jaycen Joshua** : [MWTM, Critical Listening Workshop](https://mixwiththemasters.com/videos/jaycen-joshua-various-artists-critical-listening-workshop/part/4) présente notamment transitoires, bas du spectre, masquage et M/S. Notre preset interprète ces thèmes, sans reprendre de coefficients propriétaires.
- **MixedByAli** : [entretien Time Out, 21 octobre 2024](https://www.timeout.com/cascais/news/derek-ali-kendrick-lamar-wanted-to-be-the-biggest-artist-in-the-world-and-i-aimed-to-be-the-best-sound-engineer-102124) évoque le développement d'un son et les textures. [Entretien NAMM](https://www.namm.org/trade-show/learn-mixing-mixedbyali-namm-show) décrit son rapprochement entre travail numérique et analogique. Le choix Tape/THD est notre interprétation.
- **Luca Pretolesi** : [entretien Dangerous Music](https://www.dangerousmusic.com/news-press/storming-the-charts-luca-pretolesi) documente son travail de mix/mastering et son approche hybride. [Entretien Modern Mixing](https://modernmixing.com/blog/2015/04/06/psychology-of-a-mix-engineer-luca-pretolesi/) aborde la compression du bus et le clipping. Le preset ne reproduit ni convertisseurs ni routing multibande.
- **Wez Clarke** : [Solid State Logic, entretien du 15 janvier 2018](https://www.solidstatelogic.com/media/wez-clarke-moves-to-ssl-duality) présente son travail avec Clean Bandit et son approche de console flexible. Le preset propre et ouvert est une adaptation esthétique, pas sa chaîne exacte.
- **Zakk Cervini** : [entretien Vintage King, Recording & Mixing Modern Rock & Metal](https://vintageking.com/blogs/interviews/recording-mixing-modern-rock-metal) situe son travail dans le rock contemporain. [Entretien Master Your Mix, 26 juin 2024](https://masteryourmix.com/podcastep204zakkcervini/) décrit sa pratique de mixage et son souci de servir le morceau. Le choix de de-harsh et de dynamique est le nôtre.
- **Andy Wallace** : [MWTM, Mixbus Compression](https://mixwiththemasters.com/videos/andy-wallace-mixbus-compression) et [profil MWTM](https://mixwiththemasters.com/masters/andy-wallace) attestent sa pratique et son influence dans le rock. Aucun réglage privé n'est attribué à cette source.
- **Alex Tumay** : [conférence Red Bull Music Academy](https://www.redbullmusicacademy.com/lectures/alex-tumay-lecture/) documente son travail avec Young Thug et la scène d'Atlanta. [Entretien RBMA](https://daily.redbullmusicacademy.com/2017/01/alex-tumay-interview/) traite notamment des 808 et du clipping. Nous en tirons une proposition prudente pour le sub.
- **Mike Dean** : [profil Soundtoys](https://www.soundtoys.com/artist/mike-dean/) documente ses rôles de production, mixage et mastering, notamment sur les projets de Travis Scott. [Entretien Albumism](https://albumism.com/interviews/sound-and-vision-a-conversation-with-mike-dean) traite de ses synthétiseurs et de son travail de mixage. Les dosages de saturation et d'espace sont nos décisions de développement.

## Validation

Les 17 presets sont rendus à 44,1/48/96/192 kHz et en 2×/4×/8× (204 rendus stéréo), avec contrôle de finitude et du plafond d'échantillons. Les tests plugin vérifient les plages APVTS, le rappel et la sauvegarde de session. Cela ne constitue pas une écoute comparative humaine de ces ingénieurs ni une validation FL Studio.
