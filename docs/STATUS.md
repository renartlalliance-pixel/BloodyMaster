# Etat du projet

Bloody Master est un processeur VST3 / standalone JUCE 8, C++20, compatible mono et stereo.

## Fonctionnel

- EQ parametrique 3 bandes, controles tonals, passe-haut/passe-bas et coupe des resonances.
- De-harsh, drive, compression avec detection liee, saturation a valve et clipper.
- Mix parallele, gestion mono sous la frequence choisie, largeur stereo, pan et profils M/S.
- Limiteur de sortie avec release, plafond de sortie et bypass bit-exact.
- Les choix 2x/4x/8x augmentent le nombre de sous-etapes de la saturation/du clipping. Ce sur-echantillonnage leger n'est pas un limiteur true-peak.
- Les presets et l'automation sont sauvegardes via les 42 parametres `bm_*` existants.
- Interface native, redimensionnable, organisee en quatre onglets; le HTML original de reference n'a pas ete modifie.

## Limites connues

- Le choix LUFS ajuste le seuil du compresseur; il ne mesure ni ne normalise la loudness integree conforme a ITU-R BS.1770.
- Le garde ISP conserve une marge de 0,35 dB sur le plafond en plus du limiteur sample-peak; il ne garantit pas une mesure true-peak.
- Les changements des filtres automatises sont appliques par bloc et ne sont pas lisses. Les reglages extremes de frequence sont limites sous Nyquist.

## Compiler et tester (Windows x64)

Prerequis : Visual Studio 2022 avec les outils C++, CMake >= 3.22 et Git.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

Le build telecharge JUCE 8.0.4 si le sous-module `JUCE` est absent. Le plugin est cree dans `build/BloodyMaster_artefacts/Release/VST3/Bloody Master.vst3`.
