# Bloody Master — notice & tutoriel

**Version 0.3.9 · VST3 Windows · Français**

Cette notice décrit les commandes réellement disponibles dans la version HTML 0.3.9. Elle s’adresse aussi aux débutants : les valeurs proposées sont des points de départ à adapter au morceau.

## 1. Réussir son premier master

1. Place Bloody Master sur le master de FL Studio, après les traitements que tu souhaites lui faire recevoir. Pour comprendre son action, commence sans autre limiteur après lui.
2. Charge un preset proche du style de ton morceau. Écoute un passage dense, puis un passage calme. Un preset donne une direction, pas un résultat universel.
3. Laisse **IN GAIN à 0 dB** au départ. Ajuste-le si le signal reçu est trop faible ou trop fort. Il n’est pas nécessaire de pousser tous les potards pour obtenir un master fort.
4. Réduis seulement les duretés et résonances gênantes avec De-Harsh et les Traps. Corrige ensuite l’équilibre grave/médium/aigu avec l’EQ.
5. Ajuste **Blood Drive**, **Squash** et **Recovery** pour obtenir la compression et la couleur voulues. Vérifie que le kick et la caisse claire gardent leur attaque.
6. Dose la largeur et Atmosphere, puis vérifie le résultat en mono avec ton DAW.
7. Dans le panneau doré, règle **Ceiling Out à −3 dB** si c’est le plafond souhaité. Monte **LoudnessWar progressivement**, par exemple vers +3 puis +6 dB. Écoute avant de monter davantage.
8. Clique **RAZ mesures**, lis le morceau entier, puis relève **I** pour le loudness intégré et **TP** pour les crêtes vraies estimées. Une boucle de refrain ne représente pas l’intégré du morceau complet.
9. Compare les réglages A/B sur le même passage. La compensation d’écoute aide à juger le son sans être trompé par une simple hausse de volume.
10. Avant l’export, coupe **DELTA**, **Spike Listen** et **Comp. écoute**, et vérifie que **BYPASS est désactivé**. Sauvegarde ton réglage dans **User**. Contrôle le fichier exporté et, si nécessaire, sa version encodée.

**Point de départ pour le loudness :** vise d’abord un son propre et vivant. −10 à −7 LUFS peut convenir au résultat recherché, mais dépend du mix. Sur un beat synthétique de validation, avec Drive 12, Hammer 6 et plafond −3, LoudnessWar +6 a donné environ −9,8 LUFS et +12 environ −7,7 LUFS. Ce ne sont pas des valeurs promises pour ton morceau.

## 2. Manipuler les commandes

- **Glisser verticalement** sur un potard : monter pour augmenter, descendre pour diminuer.
- **Maintenir Maj** pendant le geste : réglage plus fin.
- **Double-cliquer** : revenir à la valeur par défaut du paramètre. Ce n’est pas forcément zéro ni la valeur du preset chargé.
- **Tab**, puis les **flèches du clavier** : sélectionner et ajuster un potard ; Maj permet également un ajustement plus fin.
- **Survoler** : afficher l’infobulle lorsqu’elle est disponible.

Les commandes sont lissées pour limiter les clics. Les changements importants de gain, de preset ou de traitement restent audibles.

### Quelques mots utiles

| Terme | Signification pratique |
|---|---|
| dB | Écart de niveau ou gain. +6 dB multiplie environ par deux l’amplitude ; cela ne veut pas dire deux fois plus fort à l’oreille. |
| dBFS | Niveau numérique par rapport au maximum d’échantillon. 0 dBFS est la référence maximale. |
| LUFS | Mesure de loudness. −7 LUFS est plus fort que −10 LUFS. |
| dBTP | Crête vraie estimée, incluant les pics qui peuvent apparaître entre les échantillons lors de la reconstruction. |
| Réduction / GR | Atténuation appliquée par un compresseur ou limiteur. Plus elle est élevée, plus l’étage retient le signal. |
| Mid / Side | Mid : contenu commun à gauche et droite ; Side : différence entre les canaux, liée à l’espace stéréo. |

## 3. De-Harsh & Resonance Traps — nettoyer

Ces correcteurs retirent dynamiquement certaines zones lorsqu’elles ressortent. Ils ne cherchent pas automatiquement la meilleure fréquence : choisis la zone à corriger à l’oreille.

| Commande | Plage | Fonction et utilisation |
|---|---|---|
| **Harsh Freq** | 4–12 kHz | Centre la zone surveillée par De-Harsh. Cherche la dureté des voix, hi-hats ou cymbales. Ce bouton choisit la fréquence ; De-Harsh règle l’action. |
| **De-Harsh** | 0–100 % | Dose la réduction des agressivités autour de Harsh Freq. À 0 %, cette correction est nulle. Commence doucement ; trop de réduction peut ternir le morceau. |
| **Mud Trap — fréquence** | 80–400 Hz | Choisit la zone de grave/bas-médium à surveiller : accumulation, effet « carton », manque de lisibilité entre basse et kick. |
| **Mud Trap — dosage** | 0–100 % | Petit potard sous Mud Trap : dose sa correction dynamique. Il n’agit que si le bouton Mud est **ON**. |
| **Ring Trap — fréquence** | 2–10 kHz | Choisit une résonance plus aiguë : son métallique, dureté étroite, résonance de voix ou de caisse claire. |
| **Ring Trap — dosage** | 0–100 % | Petit potard sous Ring Trap : dose sa correction dynamique, lorsque Ring est **ON**. |
| **Sub Barrier** | 15–100 Hz | Filtre passe-haut : atténue ce qui se trouve sous sa fréquence. Utile pour les infra-graves inutiles ; trop haut, il retire le poids du kick et de la basse. |
| **Radio Ceil** | 18–45 kHz | Filtre passe-bas : atténue le haut du spectre au-dessus de sa fréquence. Sa fréquence utile dépend de celle du projet ; une valeur au-dessus de la limite numérique peut rendre le filtre neutre. |

**Mud ON/OFF et Ring ON/OFF** activent chaque Trap indépendamment de son dosage. Un réglage de fréquence seul ne suffit pas si sa correction est désactivée.

**Spike Listen / LISTEN** permet d’écouter ce que De-Harsh, Mud et Ring retirent, avant le reste du mastering. Utilise-le brièvement pour vérifier la zone visée, puis coupe-le pour juger le master complet. Il reste une écoute de contrôle, pas le signal à exporter. DELTA prend priorité et suspend cette écoute sans effacer son réglage.

## 4. Console Tone & Matrix Equalizer — équilibrer

### Les quatre grandes couleurs

| Potard | Plage | Ce qu’il change |
|---|---|---|
| **Sub Weight** | −6 à +6 dB | Courbe large du grave. En positif, renforce le bas avec une atténuation couplée plus haut pour éviter une simple bosse uniforme. En négatif, allège le grave. Ce n’est pas le même filtre que Gravity. |
| **Punch Body** | −6 à +6 dB | Courbe large autour de **650 Hz** : corps, matière et bas-médium. Monte si le mix manque de substance ; baisse s’il devient épais ou « boîte ». |
| **Silk Shine** | −6 à +6 dB | Courbe large autour de **8 kHz** : brillance et présence du haut. Trop de gain peut rendre les consonnes et cymbales fatigantes. |
| **Air Diffuse** | −6 à +6 dB | Shelf aigu autour de **12 kHz** : sensation d’air. Il agit sur la tonalité générale ; Atmosphere agit sur le Side stéréo. |

Les dB de Sub Weight sont ceux de sa commande de courbe : la réponse varie selon la fréquence. Ce n’est pas un gain uniforme appliqué à tout le grave.

### Les trois bandes de correction

Chaque bande possède **deux potards** : une fréquence et un gain. Les courbes ont une largeur fixe ; aucun réglage de Q n’est proposé.

| Bande | Potard de fréquence | Potard de gain | Utilité typique |
|---|---|---|---|
| **Band I — Low** | 30–400 Hz | −6 à +6 dB | Ajuster le poids du kick, de la basse ou un excès de bas-médium. |
| **Band II — Mid** | 400 Hz–4 kHz | −6 à +6 dB | Corps, clarté des voix/guitares, médiums trop encombrés ou trop incisifs. |
| **Band III — Air** | 4–18 kHz | −6 à +6 dB | Détail, brillance, haut du spectre trop discret ou trop dur. |

**EQ ON/OFF** active ou neutralise **ces trois bandes**. Les grandes couleurs Sub Weight/Punch Body/Silk Shine/Air Diffuse, les filtres de nettoyage et Gravity restent indépendants.

**Méthode :** trouve d’abord la fréquence, puis dose le gain. Pour une correction de mastering, commence par de petits écarts, par exemple 0,5 à 1 dB, puis écoute l’ensemble.

## 5. Blood Drive & Dynamics — densité et couleur

| Commande | Plage | Fonction et utilisation |
|---|---|---|
| **IN GAIN** | −18 à +18 dB | Gain indépendant placé avant tous les traitements. Augmenter fait travailler davantage la chaîne qui suit. Le bouton ne change pas la valeur de Blood Drive, mais peut changer la compression et la saturation obtenues. |
| **Blood Drive** | 0 à +12 dB | Gain d’excitation après le nettoyage dynamique et avant EQ/compresseur. Il augmente l’engagement des traitements suivants. À utiliser pour le caractère et la densité de la chaîne, plutôt que comme simple volume final. |
| **Dual Link** | 0–100 % | Liaison des détecteurs du compresseur gauche/droite. À 100 %, une crête d’un côté influence les deux côtés : image plus stable. À 0 %, chaque côté réagit indépendamment, avec davantage de risque de déplacement stéréo. |
| **Density Blend** | 0–100 % | Mélange de la compression principale. À 100 %, sa réduction est entièrement appliquée ; en baissant, on réintroduit le signal non compressé à cet étage. À 0 %, Drive, Glue et les autres traitements restent actifs. |
| **Squash** | 1,2:1–8:1 | Ratio de la compression principale. Plus il monte, plus les niveaux au-dessus du seuil sont retenus. Un ratio élevé peut donner une densité plus ferme et réduire le relief. |
| **Recovery** | 10–1 200 ms | Temps de récupération du compresseur principal. Court : retour plus rapide, pouvant respirer ou pomper. Long : maintien plus doux, pouvant écraser une succession de coups. En Valve, la récupération dépend aussi du signal. |
| **Tape Glue** | 0–6 dB | Dose un deuxième étage de compression de cohésion : plus on monte, plus il s’engage. L’échelle ne correspond pas à un simple gain de sortie ni à une réduction garantie. La coloration de bande se règle séparément avec Tape Color. |
| **Crush Clip** | 0–10 | Excitation du clipper à coude doux. Arrondit les crêtes avant limitation pour gagner en densité ; à 0, son action d’écrêtage est neutralisée. Trop de Clip peut produire du grain, réduire l’attaque et durcir le grave. |
| **Tape Color** | 0–100 % | Dosage de coloration de bande : saturation douce et arrondi des aigus dépendant du signal. À 0 %, cet effet est neutre. Ne modifie pas les réglages de Tape Glue. |

**PURE / VALVE** : Pure désactive le comportement optical de la compression principale et la petite coloration Valve associée. Valve active une récupération dépendante du programme et une légère coloration harmonique. **Pure ne coupe pas** Tape Color, THD, XL, Glue ou le clipper : ce n’est pas un bypass général des couleurs.

**Tape — Discret / Marqué** : Discret donne une excitation et un mélange plus modérés ; Marqué pousse une coloration plus dense. Choisis d’abord le caractère, puis ajuste Tape Color. Il n’y a pas de bruit de bande, de wow ou de flutter ajouté.

### THD et XL, sous l’écran central

| Commande | Plage / choix | Fonction et utilisation |
|---|---|---|
| **THD** | −60 à −20 dB | Dose une coloration harmonique. **−60 dB = OFF** pour cet effet ; en allant vers −20, le dosage augmente. L’échelle n’est pas une mesure du taux de distorsion réel en sortie. |
| **THD : Warm / Dense** | Deux caractères | Warm favorise une génération asymétrique, associée aux harmoniques paires ; Dense une génération symétrique, associée aux impaires. Le résultat dépend du niveau du signal. |
| **XL** | 0–100 % | Dose un modelage non linéaire complémentaire. À 0 %, la contribution XL est nulle. Il peut changer la texture et l’arrondi des crêtes ; ce n’est pas un élargisseur stéréo. |
| **XL MODE** | Densité / Présence / Impact | Densité travaille surtout au-dessus du grave ; Présence cible davantage le détail supérieur ; Impact agit sur une portion plus large du signal et arrondit les grandes excursions. Compare à dosage faible avant de pousser. |

**Repère :** Tape Glue = compression ; Tape Color = coloration ; THD = harmonique ; XL = modelage de texture ; Blood Drive = excitation en amont ; LoudnessWar = poussée finale. Leurs effets peuvent se cumuler.

## 6. Spatial Horizon & M/S — placer la stéréo

| Commande | Plage | Fonction et utilisation |
|---|---|---|
| **Mono Base** | 0–400 Hz | Atténue progressivement le Side dans le grave sous la fréquence choisie. Le bas devient plus centré. À 0, cette réduction du Side est neutralisée. Ce n’est pas une séparation brutale à la fréquence affichée. |
| **Horizon Width** | 0–200 % | Niveau du Side : 100 % conserve sa largeur de référence ; 0 % réduit le Side à zéro ; au-dessus de 100 %, élargit ce qui possède déjà du Side. Vérifie la compatibilité mono. |
| **Atmosphere** | 0–100 % | Dose une EQ sur le Side selon le profil choisi. À 0 %, cette correction est nulle. Le centre est préservé par cet effet et sa correction vise surtout au-dessus du grave. Aucun délai de transport supplémentaire n’est ajouté. |
| **Mid Bias** | −50 à +50 | Déplace le **Mid** vers la gauche ou la droite en gardant le Side fixe. C = centre. Ce n’est pas une balance de tout le mix ; laisse-le au centre sauf intention précise. |

### Warp Profile — les quatre profils d’Atmosphere

| Profil | Caractère | Correction à dosage maximal |
|---|---|---|
| **1 — Ouvert** | Ouvre le haut des côtés | Shelf d’environ +2 dB à 2,5 kHz. |
| **2 — Présence** | Fait ressortir le détail latéral | Cloche d’environ +2,5 dB à 3,5 kHz. |
| **3 — Air** | Ajoute de l’air sur les côtés | Shelf d’environ +3 dB à 7 kHz. |
| **4 — Chaleur** | Donne de la matière latérale | Cloche d’environ +1,5 dB à 1,2 kHz. |

Les valeurs décrivent les filtres du profil ; la correction finale dépend du dosage et du filtrage de protection du grave. Atmosphere ne crée pas artificiellement de stéréo à partir d’un signal parfaitement mono. La dynamique située après peut réagir au niveau modifié.

## 7. Final Wall & Master — monter le niveau final

| Commande | Plage | Fonction et utilisation |
|---|---|---|
| **Gravity** | −10 à +10 | Renforce ou allège une courbe large du grave autour de **160 Hz**. L’échelle correspond à environ −3/+3 dB de shelf. Malgré sa position dans le panneau final, son EQ est traitée en amont de la compression. Ce n’est pas un contrôle de loudness. |
| **LoudnessWar** | 0 à +18 dB | Pousse le signal **après compression et coloration, avant clipper et limiteur**. Garde les réglages de Drive et de couleur, tout en faisant travailler davantage la gestion finale des crêtes. À 0 dB, il n’ajoute pas de gain. |
| **Ceiling Out** | −6 à 0 dB | Plafond demandé au limiteur. **Ce n’est pas un fader de volume ordinaire** : il fixe la limite supérieure, sans normaliser automatiquement un signal faible. La protection finale peut retenir un plafond plus bas. |
| **Hammer** | 0 à +6 dB | Gain de poussée **après le clipper**, avant le limiteur. Permet de charger le limiteur sans augmenter directement l’excitation du clipper. À distinguer de LoudnessWar, situé avant le clipper. |
| **Release** | 1–500 ms | Récupération du limiteur. Le moteur utilise un minimum effectif de 8 ms puis un allongement dépendant du signal. Une valeur plus longue peut lisser davantage ; une valeur courte peut ramener du mouvement ou de la distorsion. |
| **LR Unify** | 0–100 % | Liaison gauche/droite du limiteur principal. 100 % retient les canaux ensemble pour stabiliser l’image ; plus bas, ils peuvent réagir différemment. La protection globale finale reste entièrement liée. |

**ISP Guard** active la prise en compte des crêtes inter-échantillons dans le limiteur principal, avec une réserve supplémentaire. Le couper ne désactive pas la protection globale de sortie.

### Ce que LoudnessWar peut faire

Si le master manque de niveau, il pousse les étages finaux. Si ceux-ci retiennent déjà fortement le signal, monter encore peut surtout augmenter l’écrêtage et la réduction, avec peu de gain de loudness. **Il ne s’immobilise pas automatiquement sur le réglage « parfait »** : surveille le son, les LUFS et la réduction. Garder les valeurs des traitements ne signifie pas garder exactement la même dynamique après une forte poussée.

### La protection globale

Elle reste active après les traitements, le bypass interne et la compensation d’écoute. Elle ne monte jamais le gain. Elle vise l’objectif de sécurité global de **−5 LUFS**, avec une régulation d’énergie prudente. −5 LUFS est une limite haute recherchée, pas une cible à atteindre à chaque master ; vérifie l’intégré du fichier complet.

Son plafond retient le plus bas entre Ceiling Out et −2 dB, avec **0,5 dB de réserve interne**. Exemple : Ceiling Out −3 donne une borne d’échantillons à −3,5 dB ; un réglage −0,2 ne permet pas de contourner la protection. La crête vraie est mesurée séparément. L’indication **Protection sortie : −x dB** montre son intervention.

Le rééchantillonnage et les codecs peuvent recréer des pics. Les tests ont notamment observé un dépassement AAC sur un master fortement poussé, malgré un master brut sous −3 dBTP. Contrôle la version encodée ; baisse le plafond ou la poussée si nécessaire. Une plateforme peut aussi réduire le volume de lecture : un master plus dense n’y sera pas forcément plus fort.

## 8. Écouter et comparer

### BYPASS

Écoute l’entrée alignée en temps, avec les traitements contournés. **La protection finale reste active**, donc un signal très fort peut encore être atténué. IN GAIN et LoudnessWar sont contournés avec la chaîne. Un hôte qui arrête complètement le plugin peut contourner aussi sa protection.

### A / B

Deux mémoires temporaires de réglages pour comparer deux versions. Au premier basculement, l’autre mémoire est une copie : passe sur B, modifie tes réglages, puis reviens sur A pour comparer. A/B n’est pas une comparaison automatique « original contre master » ; utilise BYPASS pour cela. Sauvegarde les versions utiles dans User.

### Comp. écoute

Compensation temporaire pour comparer A et B à des niveaux plus proches. Active-la et écoute **le même passage au moins 3 secondes dans chaque état**. Le plus fort est atténué, jusqu’à 12 dB ; le plus faible n’est pas amplifié. Après une modification, laisse l’apprentissage se refaire. Une protection différente entre A et B peut limiter la correspondance obtenue.

Pour régler le loudness réel et avant un export temps réel, **coupe Comp. écoute**. Le rendu hors ligne reconnu par l’hôte l’ignore, mais un export temps réel peut conserver cette atténuation. Les mesures de sortie reflètent l’écoute après compensation et protection.

### DELTA

**Delta = entrée originale alignée − sortie traitée complète.** Le bouton fait entendre uniquement cette différence ; le master normal est remplacé dans l’écoute, sans désactiver les calculs DSP nécessaires à la comparaison.

La référence est prise **avant IN GAIN**. La sortie comparée inclut les nouveaux gains, les traitements, la compensation éventuelle et la protection. L’alignement tient compte du suréchantillonnage et du lookahead. Une différence de niveau seule peut donc être audible dans Delta ; ce n’est pas exclusivement un détecteur de distorsion.

Utilise-le pour repérer ce que le traitement change. Coupe-le pour retrouver le master normal, avec transition douce. Un chemin réellement neutre à gain unitaire s’annule à la précision numérique près ; un preset nommé « Clean » peut néanmoins contenir des traitements.

Delta est désactivé par défaut, au rappel des presets/sessions et au rendu hors ligne reconnu. Il est aussi coupé au changement A/B. **Vérifie OFF avant export temps réel.** C’est une différence brute après comparaison au master protégé : son niveau peut dépasser le plafond du master.

## 9. Precision HUD — comprendre les mesures

| Commande / affichage | Fonction |
|---|---|
| **SCOPE** | Forme d’onde réelle du signal sélectionné. Observe la forme et les grandes crêtes ; ce n’est pas un analyseur de spectre. |
| **EQ CURVE** | Réponse calculée des filtres de tonalité. Ne représente pas toute la coloration non linéaire, la compression ni tous les effets stéréo. |
| **METERS** | Crêtes L/R en dBFS et réductions COMP/LIM. COMP correspond à la compression principale ; LIM au limiteur principal. La protection finale possède son indication séparée. |
| **IN / OUT** | Change la source des crêtes et du scope. IN est avant les traitements, donc avant IN GAIN. Ce bouton ne transforme pas l’indicateur LUFS en mesure d’entrée. |
| **I — LUFS intégré** | Loudness de l’audio mesuré depuis la remise à zéro, avec gating. Lis le morceau entier pour juger son niveau global. |
| **S — LUFS court terme** | Loudness sur environ 3 secondes. Utile pour suivre couplet, refrain et passages denses. |
| **TP — dBTP** | Maximum cumulé de crête vraie estimée 4× depuis la remise à zéro. À vérifier avec un mesureur indépendant sur l’export. |
| **RAZ mesures** | Recommence les mesures et l’apprentissage de compensation A/B. Ne change pas les potards et ne désactive pas la protection. |
| **TARGET** | Repère visuel inactif en 0.3.9. Aucune normalisation automatique vers la valeur affichée. |
| **Sample Rate / Latency** | Fréquence du projet et délai déclaré au DAW. À 48 kHz, le total est de 848 échantillons, environ 17,7 ms. |

Les mesures suivent l’audio reçu : elles ne se remettent pas automatiquement à zéro au début du morceau. « — » peut indiquer une fenêtre encore incomplète ou du silence. En Delta, les mesures de sortie reflètent la différence entendue.

**Suréchantillonnage 2× / 4× / 8×** : paramètre accessible par la liste des paramètres du plugin dans le DAW ; il n’a pas de potard dédié dans l’interface HTML. Concerne les étages non linéaires, avec un coût CPU croissant. Il ne change pas la fréquence du fichier exporté et n’est pas un réglage de volume. La latence de la chaîne reste compensée.

## 10. Presets et sauvegarde

Le menu regroupe les presets **Factory**, **Rap**, **Electro**, **New Rock**, **Trap** et **User**. Les presets d’inspiration sont des réglages originaux, sans approbation ni reproduction garantie des ingénieurs cités.

1. Règle le plugin et coupe les écoutes de contrôle.
2. Clique **Sauver**, donne un nom, puis **Enregistrer**.
3. Retrouve le réglage dans **User**.
4. Le bouton **↻** actualise la liste, notamment après une sauvegarde depuis une autre instance.

Un nom déjà utilisé crée une copie numérotée. Le menu ne propose pas d’écrasement ou de suppression directe. Les fichiers sont dans `%APPDATA%\BRI Audio Systems\BloodyMaster\User Presets` : copie ce dossier pour sauvegarder ta banque.

Les presets actuels enregistrent les 52 paramètres, dont les deux gains, le bypass et Spike Listen. **Delta et Comp. écoute ne sont pas sauvegardés.** Les anciens presets compatibles rappellent IN GAIN et LoudnessWar à zéro. La session du DAW conserve aussi ses réglages propres. La désinstallation conserve la banque User.

## 11. Résoudre les problèmes courants

| Ce que tu entends / constates | Quoi vérifier en premier |
|---|---|
| Pas assez de loudness | Comp. écoute OFF, BYPASS OFF, niveau du mix entrant. Monte LoudnessWar par étapes ; utilise IN GAIN si l’entrée est faible. TARGET n’effectue aucune normalisation. |
| Le volume augmente peu mais le son se dégrade | Clipper, limiteur ou protection travaillent déjà beaucoup. Réduis la poussée ; corrige le mix ou le grave plutôt que d’insister. |
| Kick aplati / grave qui distord | Réduis LoudnessWar, Hammer ou Crush Clip. Vérifie Sub Weight/Gravity, puis la compression et ses récupérations. |
| Voix ou cymbales trop ternes | Réduis De-Harsh ou Ring ; vérifie Radio Ceil et la coloration Tape. |
| Mix trop épais | Vérifie Sub Weight, Gravity et Punch Body. Cherche ensuite la bonne fréquence de Mud plutôt que de tout couper. |
| Image qui bouge | Remonte Dual Link et LR Unify ; vérifie Mid Bias. |
| Stéréo impressionnante mais disparition en mono | Réduis Horizon Width/Atmosphere et contrôle les effets stéréo déjà présents dans le mix. |
| Son étrange, très faible ou différence seule | Vérifie DELTA et Spike Listen, puis Comp. écoute et BYPASS. |
| Le plafond semble plus bas que sa valeur | Protection globale, réserve interne, ISP Guard et éventuelle compensation d’écoute : le plafond affiché n’est pas un gain de rattrapage. |
| Le rappel d’un preset change peu le niveau | Compare le même passage avec Comp. écoute OFF ; les étages de protection peuvent retenir les hausses. |

## 12. Repère de chaîne audio

Pour comprendre les interactions, le trajet principal est :

**Entrée originale → IN GAIN → De-Harsh/Traps → Blood Drive → filtres/EQ/Gravity et M/S → compression principale → Glue → Tape Color/THD/XL/Valve → LoudnessWar → Crush Clip → Hammer → limiteur → compensation d’écoute éventuelle → protection finale → sortie.**

Les écoutes Spike, BYPASS et DELTA utilisent des chemins de contrôle spécifiques. Les positions graphiques sont des regroupements d’usage : notamment Gravity reste une EQ avant compression, même si son bouton est dans le panneau final.

**À retenir :** règle d’abord le son, puis le niveau final. Compare à volume proche, préserve les attaques utiles et contrôle le fichier réellement livré.


## 13. Les machines et outils qui ont inspiré Bloody Master

Bloody Master rassemble des idées de mastering issues de plusieurs références. **Le DSP est original : il reprend des principes de travail et des comportements, pas les circuits ni les algorithmes propriétaires de ces appareils.** Voici précisément ce qui a été retenu.

### Pultec — le grave musical et les courbes larges

La famille Pultec, notamment le principe de grave rendu célèbre par l’EQP-1A, est la référence pour une EQ musicale : des corrections larges et l’interaction entre renforcement et atténuation du bas.

Dans Bloody Master, cette inspiration se retrouve surtout dans **Sub Weight**. En montant ce potard, la courbe renforce le grave et combine une atténuation située plus haut. Cela permet de rechercher du poids sans ajouter uniformément la même épaisseur à tout le bas du spectre.

**Notre réalisation :** deux courbes numériques couplées, commandées par un seul potard. Ce n’est pas la reproduction du réseau passif, des transformateurs ou des lampes d’un Pultec, ni de toutes ses positions de fréquence. Les autres bandes d’EQ sont aussi des filtres numériques originaux.

### Pendulum Audio OCL-2 — la souplesse de la compression optical

L’OCL-2 est une référence de compression optique à lampes. Le projet en retient l’idée d’une **compression à genou doux**, d’une liaison stéréo et d’une récupération qui dépend du programme musical.

Dans Bloody Master, cela concerne le comportement du compresseur, notamment **VALVE**, **Recovery** et **Dual Link**. Après un passage qui sollicite davantage la compression, la récupération peut s’allonger : la valeur du potard n’est donc pas une durée totale rigide.

**Notre réalisation :** détecteur et enveloppes numériques, attaque principale fixée à 8 ms et coloration Valve originale. Aucune cellule optique ou lampe physique modélisée ; les modes matériels FAST/PRESET/MANUAL de l’OCL-2 ne sont pas reproduits. **L’OCL-2 inspire la compression, pas un limiteur brickwall.**

### Bettermaker Mastering Compressor — le contrôle du bus stéréo

Le Mastering Compressor sert de référence fonctionnelle pour le travail de dynamique en mastering : contrôle de la compression, liaison stéréo, dosage parallèle et détecteur moins dominé par les très basses fréquences.

Dans Bloody Master, ces idées correspondent à **Squash**, **Recovery**, **Dual Link** et **Density Blend**. Le détecteur du compresseur possède un filtrage à 80 Hz pour limiter sa sensibilité au bas : ce filtre concerne la détection, pas une coupure à 80 Hz du signal écouté.

**Notre réalisation :** un compresseur numérique original. Les différents modes et circuits du Bettermaker, son sidechain complet et sa calibration analogique ne sont pas reproduits. Le THD de Bloody Master est un étage de couleur propre au projet.

### Bettermaker Mastering Limiter — maîtriser le dernier étage

Le Mastering Limiter inspire la fonction de fin de chaîne : pousser le niveau tout en maîtrisant les crêtes et le plafond.

Dans Bloody Master, on retrouve cette logique avec **Hammer**, **Ceiling Out**, **Release**, **LR Unify** et **ISP Guard**, associés à un clipper placé avant le limiteur. **LoudnessWar** a ensuite été ajouté pour rendre la poussée finale plus accessible.

**Notre réalisation :** clipper doux, suréchantillonnage, anticipation et limitation numériques originaux. La courbe du clipper n’est pas une capture du Bettermaker. LoudnessWar et la protection globale sont des fonctions propres à Bloody Master, sans équivalent matériel exact revendiqué.

### bx_masterdesk PRO — l’approche « chaîne de master dans un seul outil »

Cette référence est **un plugin**, pas une machine analogique. Elle a guidé l’objectif fonctionnel : réunir nettoyage, tonalité, dynamique, couleur, stéréo, contrôle du niveau et mesures dans une interface de mastering accessible.

Bloody Master suit cette idée d’un parcours complet, avec ses propres choix de commandes et de DSP. Il n’utilise pas le code ni les algorithmes propriétaires de bx_masterdesk PRO.

### Tascam Model 12 — l’inspiration de l’interface

La Model 12 a inspiré **l’aspect visuel**, les commandes et l’esprit de la façade. Le son de ses préamplis, son EQ et sa sommation ne sont pas modélisés. L’habillage Tascam ne signifie donc pas que le plugin reproduit une console Model 12.

### Et les autres étages ?

**Tape Color, THD, XL, De-Harsh, les Traps, Atmosphere, Gravity, LoudnessWar et la protection finale** sont des réalisations originales du projet, nourries de principes généraux de traitement audio. Tape Color ne revendique pas une émulation Studer ou Ampex ; Tape Glue est un compresseur de cohésion, pas une machine à bande. Les presets inspirés d’ingénieurs sont des propositions originales, pas leurs chaînes officielles.

### Sources et traçabilité

Les références techniques du développement sont conservées dans le dépôt, notamment l’audit des inspirations et les documents de conception. Pour retrouver les principes d’origine :

- [Manuel Pultec Passive EQ Collection — Universal Audio](https://help.uaudio.com/hc/en-us/articles/7183176266260-Pultec-Passive-EQ-Collection-Manual) : explication des commandes et de l’interaction boost/atténuation ; ce manuel décrit les produits UA et leurs références, pas notre DSP.
- [Pendulum Audio — OCL-2](https://www.pendulumaudio.com/ocl-2) : présentation du compresseur et de ses comportements.
- [Bettermaker — Mastering Compressor](https://www.bettermaker.com/mastering-compressor) : référence constructeur pour l’architecture fonctionnelle du compresseur.
- [Bettermaker — Mastering Limiter FAQ](https://bettermaker.eu/en_GB/i/Bettermaker-Mastering-Limiter-FAQ/14) : principes et usages de la limitation de mastering.

Ces références documentent les inspirations. Elles ne constituent pas une validation de correspondance sonore entre Bloody Master et les appareils cités.

---

Notice vérifiée contre les sources de Bloody Master 0.3.9, commit audio `6ec0e12`. Les traitements sont des algorithmes originaux ; leurs noms n’impliquent pas une émulation exacte d’appareils analogiques. Les précisions de validation sont disponibles dans `docs/STATUS.md`, `LOUDNESS_GAIN_FR.md`, `DELTA_FR.md` et les rapports de mesure du dépôt.
