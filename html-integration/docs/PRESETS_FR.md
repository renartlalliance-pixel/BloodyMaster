# Presets rap / pop / électro — 10 novembre 2026

Six propositions originales sont ajoutées à Clean, Glue et Punch. Les principes
ci-dessous s'inspirent de publications accessibles ; aucun réglage chiffré de cette
banque n'est attribué à un ingénieur ni présenté comme son preset officiel.
La banque utilise seulement les DSP raccordés. Elle ne remplace pas un mixage par
pistes et n'ajoute pas de de-esser, de compresseur multibande ou de modèle de lampe.

## Inspirations vérifiées et adaptation

- Rap : la présentation de la session Jaycen Joshua / Lady London documente le
  travail du grave, de l'EQ, de la compression et du clipping. Notre adaptation
  consiste à soutenir le sub modérément, stabiliser son image et répartir le
  contrôle entre compression et crêtes. Aucune chaîne de voix de cette session
  n'est transposée telle quelle sur le master.
  Source : https://mixwiththemasters.com/videos/jaycen-joshua-lady-london-d-r-u-n-k
- Pop : John Hanes explique que leur master bus n'est pas piloté par un sidechain
  externe destiné à faire pomper le mix. Nos propositions gardent la compression
  stéréo liée, une correction large et peu de clipping. C'est une interprétation
  de principes, sans reproduction de la chaîne de Serban Ghenea / John Hanes.
  Source : https://gearspace.com/threads/interview-with-john-hanes.1362525/
- Électro : Luca Pretolesi enseigne la gestion de niveau des drums avec clipping
  et plafond ; il décrit aussi l'usage créatif du clipping de convertisseur dans
  son travail hybride. Notre adaptation emploie un clipper numérique avant le
  limiteur, sans prétendre reproduire son convertisseur ni son routage multibande.
  Sources : https://new.mymixlab.com/Courses/Course/9062
  et https://www.dangerousmusic.com/news-press/storming-the-charts-luca-pretolesi

## Réglages principaux

Tous : plafond −1 dB, ISP activé, suréchantillonnage 8×, liaison compresseur et
limiteur 100 %, Mid Pan centré. Les valeurs Clip sont les commandes de drive du
clipper ; elles ne sont pas une quantité de crêtes effectivement coupées.

| Preset / identifiant CLI | Intention | Drive dB | Ratio | Release ms | Mix % | Clip | Push dB | Mono Hz | Largeur % |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Rap Sub Focus / rap-sub-focus | Sub stable, bas-médium allégé | 1 | 1,6 | 160 | 85 | 1 | 0,8 | 100 | 100 |
| Rap Forward / rap-forward | Présence et densité plus affirmées | 2 | 2 | 100 | 90 | 1,8 | 1,2 | 110 | 103 |
| Pop Open / pop-open | Ouverture, corrections légères | 0,5 | 1,5 | 240 | 100 | 0,4 | 0,4 | 90 | 104 |
| Pop Dense / pop-dense | Cohésion et brillance mesurées | 1,5 | 1,8 | 180 | 100 | 1 | 0,8 | 100 | 105 |
| Electro Punch / electro-punch | Partager les crêtes sans trop comprimer | 1 | 1,8 | 100 | 80 | 2 | 1 | 120 | 106 |
| Electro Club / electro-club | Variante plus appuyée | 2 | 2 | 80 | 85 | 3 | 1,8 | 130 | 108 |

Pop Open/Dense utilisent le relâchement optical ; Rap et Electro utilisent le
relâchement non optical. L'attaque principale reste fixée à 8 ms dans ce moteur.
L'optical rallonge la récupération selon le programme : la valeur affichée n'est
pas une durée totale de récupération garantie. L'intégralité des 49 paramètres
actifs est exportée dans `Resources/FactoryPresets.json` depuis la banque C++.

## Choisir et ajuster

1. Commencer avec un mix non limité ; si déjà masterisé, essayer d'abord Clean.
2. Choisir le preset puis régler le niveau entrant dans le DAW. Pour une première
   écoute, viser environ 0,5 à 2 dB de compression et des interventions ponctuelles
   du limiteur. Ces valeurs sont des suggestions, pas une mesure déjà réalisée sur
   votre musique. Si le groove s'écrase, réduire Drive, Clip et Push.
3. Comparer à volume perçu égal en compensant extérieurement au plugin : l'A/B
   compensé automatique n'est pas encore développé. Vérifier kick/808, voix,
   sifflantes, cymbales et compatibilité mono. Réduire la largeur si nécessaire.
4. Éviter une cible de volume universelle : les presets ne mesurent ni ne règlent
   automatiquement les LUFS. Pour plus de volume, écouter et mesurer chaque étape.

## Accès

Outil WAV : les neuf presets figurent dans le menu Windows au double-clic.
CLI : `BloodyMasterTest.exe mix.wav --preset rap-sub-focus`.
`--list-presets` liste les identifiants, `--presets-json` exporte les paramètres.

Plugin : banque déclarée comme programmes JUCE, rappelable par l'hôte après
compilation (selon l'interface de programmes exposée par le DAW). Le HTML n'a pas
été modifié et son menu n'est pas encore relié. Les tests de rappel JUCE passent avec les binaires Windows sous Wine ;
leur validation sous Windows natif et FL Studio reste à faire. La sauvegarde d'état conserve l'indice du programme et les
valeurs de paramètres ; une automation ultérieure peut personnaliser le preset.


## Mise à jour 0.3.5

Les presets de genre utilisent les modules finalisés avec des dosages prudents.
THD/XL et les pièges restent au repos ; aucun nettoyage automatique imposé.

| Preset | Atmosphere | Profil | Tape Color Discret |
|---|---:|---|---:|
| Rap Sub Focus | 0 % | 2 Présence | 8 % |
| Rap Forward | 8 % | 2 Présence | 10 % |
| Pop Open | 15 % | 1 Ouvert | 5 % |
| Pop Dense | 10 % | 4 Chaleur | 12 % |
| Electro Punch | 8 % | 3 Air | 6 % |
| Electro Club | 10 % | 1 Ouvert | 10 % |

Clean, Glue et Punch conservent Atmosphere et Tape Color à zéro. Ce sont des
points de départ numériques à écouter, pas des réglages validés sur tout master.
