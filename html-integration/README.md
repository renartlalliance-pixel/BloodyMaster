# Installateur VST3 de test — 10 novembre 2026

`BloodyMaster-0.3.0-Test-Setup.exe` installe le vrai VST3 Windows x64 dans
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

L'éditeur de test présente 32 contrôles réellement raccordés, neuf presets et des
mesures audio réelles (crêtes d'échantillons par bloc et réduction de gain).
Le HTML approuvé reste inchangé et n'est pas intégré. Les autres DSP décrits dans
l'audit restent à développer ; cette édition n'est pas une version finale complète.

Compilation standard Windows : installer NSIS 3.11 ou plus, compiler normalement
le projet, puis `cmake --build build --config Release --target BloodyMasterInstaller`.
La cible produit `build/installer/BloodyMaster-0.3.0-Test-Setup.exe`.
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
