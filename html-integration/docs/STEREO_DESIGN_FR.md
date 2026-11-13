# Atmosphere et Mid Pan — 0.3.5

Choix utilisateur : Atmosphere en EQ Mid/Side, quatre profils, centre et grave préservés, aucun délai ajouté. Mid Pan : déplacer le centre et garder les côtés fixes (réponse A).

## Atmosphere

Décodage M=(L+R)/2, S=(L−R)/2, après largeur et grave mono existants. Le Mid est inchangé par Atmosphere. Correction uniquement sur le Side, avant dynamique et limitation ; ces étages peuvent ensuite réagir au niveau modifié.

Quatre profils à dosage maximal :

1. Ouvert : shelf +2 dB à 2,5 kHz.
2. Présence : cloche +2,5 dB à 3,5 kHz, Q 0,7.
3. Air : shelf +3 dB à 7 kHz.
4. Chaleur : cloche +1,5 dB à 1,2 kHz, Q 0,7.

La correction est la différence filtré−original appliquée au Side passé par un passe-haut 350 Hz. Ce n'est pas une séparation brickwall : les tests mesurent moins de 0,05 dB de modification jusqu'à 120 Hz aux quatre Fs testées. 0 % donne une correction nulle. Mono et contenu strictement centré ne sont pas élargis artificiellement.

Filtres RBJ fixes, états continuellement entretenus, gains de correction lissés sur 20 ms pendant changement de dosage/profil. Aucun buffer de délai, aucune allocation dans le traitement. Les IIR changent la phase du Side : « sans délai ajouté » désigne l'absence de latence de transport supplémentaire, pas une phase linéaire.

IDs existants bm_enhance et bm_profile raccordés, sans changer plages/défauts APVTS. Les presets Clean/Glue/Punch restent à 0 %, profil 2 ; les six presets de genre reçoivent des dosages légers, voir PRESETS_FR.md. Le défaut APVTS historique de dosage reste 15 %.

## Mid Pan

bm_pan conserve −50 à +50. Avec p=pan/50, angle=(p+1)π/4 : L=√2 cos(angle)M+S et R=√2 sin(angle)M−S. Puissance du Mid constante ; centre inchangé à zéro ; extrêmes entièrement gauche/droite. Side inchangé. Le paramètre agit désormais sur le Mid au lieu de l'ancienne balance L/R ; les anciens presets avec pan non nul changent donc de rendu conformément au choix A. En mono la commande ne déplace pas le signal.

## Vérification locale

Onze suites DSP sans JUCE réussies, dont test_stereo : quatre Fs 44,1/48/96/192 kHz, centre, grave, effet des profils, panoramique, automation bornée et latence inchangée. Test stéréo également réussi sous ASan/UBSan (détection des fuites désactivée). Workflow Windows HTML 43437863780 réussi : 14/14 CTest, dont 24 vérifications WebView2 natives. Aucun test FL Studio effectué ici.

HTML UI et Reference originaux inchangés, SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.
