# Édition HTML 0.3.9 — LoudnessWar, IN GAIN et notice complète

[Notice française — PDF](docs/NOTICE_BLOODYMASTER_FR.pdf) · [Version à lire en ligne](docs/NOTICE_BLOODYMASTER_FR.md) · [HTML autonome](docs/NOTICE_BLOODYMASTER_FR.html)

La notice explique les 40 potards, les boutons, les mesures, les presets User, une méthode de mastering et les inspirations Pultec, Pendulum, Bettermaker, bx_masterdesk PRO et Tascam. Elle distingue inspiration fonctionnelle et émulation matérielle.

IN GAIN règle l’entrée indépendamment de Blood Drive ; LoudnessWar pousse l’étage final avant clipper/limiteur. Protection trop conservatrice corrigée ; 52 paramètres, migration des anciens presets avec gains à zéro. [Fonctionnement et limites](docs/LOUDNESS_GAIN_FR.md).

[Télécharger la 0.3.9](https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.9-test-43493392145) : 17/17 CTest Windows, 37 contrôles WebView2, 31 tests WAV et installation réussis, SHA-256 vérifié. Voir [STATUS.md](docs/STATUS.md). Les niveaux après encodage restent à contrôler ; les deux HTML de référence sont inchangés. Les sections suivantes sont historiques.

---

# Édition HTML 0.3.8 — écoute Delta vérifiée sous Windows

Le bouton **DELTA** écoute l'entrée originale alignée moins la sortie de toute la chaîne, gain et protection inclus. Fondu de 20 ms, aucune latence supplémentaire. Aucun réglage DSP modifié ; état temporaire désactivé au rappel des presets/sessions et en rendu hors ligne reconnu par l'hôte. [Détails et vérifications](docs/DELTA_FR.md).

Delta est une différence brute diagnostique : son niveau peut dépasser le plafond du master protégé. Vérifier que DELTA est éteint avant un export en temps réel. Les 50 paramètres et presets User restent compatibles ; HTML originaux conservés.

[Télécharger la 0.3.8](https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.8-test-43482536001). Windows : 16/16 CTest, 35 contrôles WebView2, 31 tests WAV et installateur réussis ; téléchargement et SHA-256 vérifiés. Voir [STATUS.md](docs/STATUS.md) pour les preuves. Les rubriques ci-dessous décrivent les versions précédentes.

---

# Édition HTML 0.3.7 — protection globale vérifiée sous Windows

La sortie est protégée après les traitements, le bypass et la compensation d'écoute. Plafond de crêtes −2 dBTP avec réserve interne, atténuation stéréo liée et régulation prudente de l'énergie pondérée K pour la limite souhaitée de −5 LUFS. Les mesures affichent désormais la sortie réellement délivrée. [Méthode, latence et limites](docs/OUTPUT_SAFETY_FR.md).

Le bypass conserve la protection : un signal faible reste transparent après délai ; un signal trop fort peut être réduit. Latence à 48 kHz : 848 échantillons, déclarée au DAW. Vérifier l'intégré du morceau complet ; la régulation ne garantit pas toutes les fenêtres LUFS ni toutes les conversions des plateformes. Les paramètres, presets User et HTML originaux sont conservés.

[Télécharger la 0.3.7](https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.7-test-43452301612). Windows : 15/15 CTest, 30 vérifications WebView2, 31 tests WAV et installateur réussis ; SHA-256 du téléchargement vérifié. Voir [STATUS.md](docs/STATUS.md) pour les preuves de validation. Les rubriques ci-dessous décrivent les versions précédentes.

---

# Édition HTML 0.3.6 — presets User et inspirations

**Sauver** ouvre la saisie du nom ; les réglages rejoignent la section **User** du sélecteur. Ils persistent après réouverture et mise à jour. Le bouton ↻ actualise les presets d'une autre instance. Un nom existant crée une copie numérotée. [Mode d'emploi et stockage](docs/USER_PRESETS_FR.md).

La banque compte 17 presets, dont huit nouvelles inspirations originales réparties en Rap, Electro, New Rock et Trap. Les ingénieurs cités n'ont ni fourni ni validé ces réglages : [inspirations et sources](docs/ENGINEER_INSPIRATIONS_2026_FR.md).

Windows vérifié : 14/14 CTest, 29 vérifications WebView2 natives, 31 tests WAV et installateur. [Télécharger la 0.3.6](https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.6-test-43442449010). Essai FL Studio et écoute humaine encore à effectuer. Consulter [STATUS.md](docs/STATUS.md) pour les preuves. Les informations 0.3.5 ci-dessous décrivent la version précédente. Les paramètres et les deux HTML originaux restent préservés.

---

# Édition HTML 0.3.5 — état actuel

Lire [docs/STATUS.md](docs/STATUS.md). Atmosphere EQ M/S (quatre profils), Mid Pan centre seul, mesures LUFS intégré/court terme 3 s/true peak estimé et comparaison A/B compensée sont raccordés. Onze suites DSP locales, vérification indépendante SciPy et navigateur testés. Workflow Windows HTML 43437863780 réussi : 14/14 CTest, 24 vérifications WebView2 natives, 31 tests WAV et tests installateur. [Télécharger la version de test](https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.5-test-43437863780). Validation FL Studio et écoute humaine encore à effectuer.

La compensation d'écoute est désactivée par défaut : écouter A puis B au moins 3 s chacun. **La couper avant export temps réel.** Mesures avant compensation ; rendu hors ligne reconnu par l'hôte à gain unité. Détails et limites dans [METERING_FR.md](docs/METERING_FR.md) et [STEREO_DESIGN_FR.md](docs/STEREO_DESIGN_FR.md). TARGET ne normalise pas le fichier.

La version HTML Windows utilise WebView2 ; 50 paramètres APVTS préservés, 49 actifs, 38 potards dans la page générée. Les deux HTML originaux restent intacts. Les commandes ci-dessous incluent aussi l'éditeur natif de repli.

# Installateur VST3 de test — 10 novembre 2026

`BloodyMaster-0.3.5-Test-Setup.exe` installe le vrai VST3 Windows x64 dans
`C:\Program Files\Common Files\VST3\Bloody Master.vst3` et propose l'outil WAV.
Désinstallation dédiée dans les applications Windows. Installateur non signé.

Compilation de cette édition : MinGW-w64 GCC 14 POSIX, JUCE 7.0.12, runtime lié
statiquement ; JUCE 8.0.4/MSVC reste le choix par défaut du projet. JUCE 8 ne
prend plus en charge MinGW. Les IDs de plugin/paramètres sont conservés.

Exécuté : 8/8 suites des binaires Windows sous Wine 10, dont tests JUCE (9 groupes),
hôte VST3 indépendant et DSP. L'installateur a été installé, désinstallé et
réinstallé dans un préfixe Wine isolé ; hash des DLL vérifié, fichier tiers préservé.
Les 31 essais WAV ont aussi passé avec le binaire Windows sous Wine.
Ce sont des tests de compatibilité sous Wine, pas un lancement Windows natif ni FL Studio.

L'éditeur de test présente 49 contrôles réellement raccordés, neuf presets et des
mesures audio réelles (crêtes d'échantillons par bloc et réduction de gain).
Le HTML approuvé reste inchangé et n'est pas intégré. Les autres DSP décrits dans
l'audit restent à développer ; cette édition n'est pas une version finale complète.

Compilation standard Windows : installer NSIS 3.11 ou plus, compiler normalement
le projet, puis `cmake --build build --config Release --target BloodyMasterInstaller`.
La cible produit `build/installer/BloodyMaster-0.3.5-Test-Setup.exe`.
Les workflows ont été préparés pour produire l'artefact ; pas exécutés sur GitHub.

Voir `packaging/TEST_WINDOWS_FR.txt` pour le premier essai FL Studio,
`docs/INSTALLER_VALIDATION.json` pour les preuves de cette compilation et
`docs/ANALOG_AUDIT_FR.md` pour les écarts aux machines.

---

Historique de la reprise avant cette compilation :

# BloodyMaster — reprise M3, 10 novembre 2026

Plugin de mastering original C++20/JUCE : EQ musicale, compression RMS et Glue,
clipper suréchantillonné et limiteur avec anticipation. L'interface HTML approuvée
est conservée octet pour octet. L'éditeur JUCE reste générique ; le HTML n'est pas
encore relié au son et ses vumètres sont simulés.

## État de validation

Tests Linux exécutés : EQ (76 272 assertions), dynamique/crêtes (9 665), utilitaires
dB, ASan/UBSan dynamique et transitions, campagne indépendante 60 vecteurs et 31 exécutions WAV.
Le suréchantillonnage 2×/4×/8× peut maintenant changer pendant la lecture, avec
préchauffage et fondu de 20 ms à latence constante (417 100 vérifications DSP).
Pire crête reconstruite à 32× : −1,227148 dBTP pour plafond −1 dB.
Ces résultats portent sur les signaux testés, sans certification true peak.

L'outil WAV Windows est une compilation croisée x64 à partir des sources de cette
reprise. Il n'a pas été exécuté sous Windows. Aucun VST3 Windows validé ni test FL
Studio n'est fourni. Voir `docs/STATUS.md` pour les limites.

## Essai WAV Windows

Dans `TestVersion`, lancer `BloodyMasterTest.exe`, sélectionner une copie WAV puis
choisir Clean, Glue ou Punch. La sortie float32 garde la durée, la fréquence et les
canaux ; son nom contient `_BloodyMaster_`. L'entrée est conservée. Ce programme
traite des fichiers ; ce n'est pas le plugin VST3.

CLI : `BloodyMasterTest.exe fichier.wav --preset clean --ceiling -1 --os 8`.

## Compilation du plugin sous Windows

Visual Studio 2022 (outils C++), CMake ≥3.22, Git. JUCE 8.0.4 est téléchargé si absent.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
python Tests/DSP/test_wav_io.py build/Release/BloodyMasterTest.exe
```

Les workflows Windows incluent maintenant les tests WAV et l'artefact de l'outil.
Ils ont été modifiés localement, sans exécution GitHub Actions.

## Tests sans JUCE

```bash
cmake -S Tests/DSP -B build-core -DCMAKE_BUILD_TYPE=Release
cmake --build build-core
ctest --test-dir build-core --output-on-failure
g++ -std=c++20 -O3 -I Source tools/wav_test.cpp -o BloodyMasterTest
python Tests/DSP/test_wav_io.py ./BloodyMasterTest
python Tests/DSP/validate_audio.py ./BloodyMasterTest ./audio-validation
```

La dernière commande nécessite NumPy/SciPy et crée ses propres signaux de test.

## Banque de presets rap / pop / électro

Six créations originales ajoutées à Clean/Glue/Punch : Rap Sub Focus, Rap Forward,
Pop Open, Pop Dense, Electro Punch, Electro Club. Banque C++ partagée entre le
menu de l'outil WAV et les programmes JUCE du plugin (ces derniers à valider après
compilation). Aucun changement du HTML. Guide et sources : `docs/PRESETS_FR.md`.
Paramètres exportés : `Resources/FactoryPresets.json`.

```text
BloodyMasterTest.exe mix.wav --preset electro-punch
BloodyMasterTest.exe --list-presets
BloodyMasterTest.exe --presets-json
```

Les DSP actifs ont été retestés ; les contrôles encore inactifs ne constituent
pas des fonctions validées. `docs/ANALOG_AUDIT_FR.md` détaille les inspirations et
les écarts matériels. Aucune équivalence sonore avec les machines n'a été mesurée.
Les valeurs des presets sont originales, inspirées de principes publics ; aucun
preset n'est fourni, approuvé ou présenté comme utilisé par un ingénieur cité.

La banque emploie HPF ≥15 Hz pour respecter les plages du plugin ; Clean/Glue/Punch
ont donc maintenant un HPF à 15 Hz. L'outil WAV conserve aussi le LPF de la banque
à 45 kHz au lieu de le désactiver systématiquement : accord des réglages avec le
plugin, sous réserve des fréquences effectives imposées par Fs. Les relances de
validation après ces changements figurent dans `docs/RETEST_PRESETS.json`.
