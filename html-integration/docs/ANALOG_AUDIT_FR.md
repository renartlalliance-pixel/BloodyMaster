# Mise à jour 0.3.5

Cet audit décrit un état antérieur. THD/XL, Valve Color, Tape Color, nettoyages, Atmosphere, profils et Mid Pan sont maintenant raccordés ; les mesures réelles et la compensation d'écoute sont ajoutées. Consulter STATUS.md, STEREO_DESIGN_FR.md et METERING_FR.md pour les fonctions et limites actuelles. Les réserves historiques sur l'absence d'émulation de circuits propriétaires restent applicables.

---

# Audit des inspirations analogiques

Les photos et manuels servent à définir des comportements. Aucune capture des
machines, mesure de leur réponse impulsionnelle/non linéaire, calibration dBu/dBFS
ou écoute comparative avec elles n'a été fournie. Il est donc impossible de
mesurer une correspondance sonore à partir de ce paquet. Les tests ajoutés
vérifient des propriétés de notre DSP, sans tolérance de correspondance matérielle.

| Référence | Comportement retenu / retesté | Écart restant |
|---|---|---|
| Pultec | Courbe grave large, boost/cut couplés ; réponses fréquentielles aux quatre Fs dans les tests EQ | Deux shelves RBJ originaux, sans circuit passif, lampe, transformateur ni commandes boost/cut indépendantes |
| Pendulum OCL-2 | Coude doux continu, liaison stéréo, récupération dépendante du programme après sustain | Détecteur numérique, attaque principale fixe 8 ms, pas de cellule optique ni d'étage tube ; modes FAST/PRESET/MANUAL matériels non reproduits |
| Bettermaker Mastering Compressor | Compression, sidechain filtré à 80 Hz, liaison et mélange parallèle | Pas des trois modes de compression du matériel, pas de sidechain paramétrique complet, pas de THD réglable ni mode de compression M/S |
| Bettermaker Mastering Limiter | Contrôle des crêtes et plafond en fin de chaîne | Limiteur FIR/lookahead original ; aucune courbe de limiteur matériel mesurée, ni garantie d'émulation |
| Tascam Model 12 | Référence visuelle préservée à l'identique | Aucune modélisation de préampli, EQ ou sommation Tascam |
| bx_masterdesk PRO | Objectif de couverture fonctionnelle mastering | Fonctionnalités encore partielles ; aucun DSP propriétaire connu |

Le terme Valve dans l'interface actuelle ne crée pas de distorsion de lampe.
Glue est un compresseur séparé, sans modèle de bande malgré le libellé Tape Glue.
XL, THD, de-harsh, pièges de résonances, Enhance, profils, LUFS et écoute Spike
restent sans traitement sonore raccordé. Ils ne sont pas validés comme DSP actifs.

Sources constructeurs consultées le 10 novembre 2026 :

- https://www.pendulumaudio.com/ocl-2 : genou doux, liaison, temps dépendants du programme et modes matériels.
- https://www.bettermaker.com/mastering-compressor : VCA analogique, modes, sidechain et THD (description indexée ; ouverture directe redirigée et inaccessible lors de cet audit).
- https://help.uaudio.com/hc/en-us/articles/7183176266260-Pultec-Passive-EQ-Collection-Manual : interaction grave boost/atténuation ; ce manuel décrit les produits UA et leur référence Pultec, pas notre algorithme.

Nouveaux tests : `test_analog_behaviors.cpp` vérifie monotonie et continuité du
coude, comportement du sidechain, récupération optical/non optical, ratio 1,
liaison stéréo et symétrie/bornage du clipper. Les tests existants couvrent EQ,
filtres, stéréo/mono, silence, automation, bypass, délais, crêtes et changements OS.
Les validations WAV SciPy reconstruisent indépendamment les crêtes à 32×.
Les tests de fichiers vérifient aussi que les entrées restent intactes.
