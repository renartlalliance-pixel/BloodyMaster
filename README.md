# Bloody Master — B.R.I Audio Systems

Processeur de mastering VST3 (JUCE 8, C++20, CMake). **Etat : M1 (squelette). Aucun traitement DSP n'est encore implemente.**
Voir `docs/STATUS.md` pour l'etat reel, fonction par fonction.

## Compiler (Windows x64, Visual Studio 2022 + CMake >= 3.22 + Git)

    cmake -S . -B build -G "Visual Studio 17 2022" -A x64
    cmake --build build --config Release --target BloodyMaster_VST3 BloodyMaster_Standalone
    ctest --test-dir build -C Release --output-on-failure

JUCE est telecharge (tag 8.0.4) au premier configure ; pour un sous-module : `git submodule add https://github.com/juce-framework/JUCE JUCE`.
Compilation locale : double-clic sur `build.bat`. CI : `.github/workflows/build.yml` (copie visible : `ci/build-windows.yml`).
Sorties : `build/BloodyMaster_artefacts/Release/VST3/Bloody Master.vst3` et `.../Standalone/Bloody Master.exe`.
Copier le .vst3 dans `C:\Program Files\Common Files\VST3\` puis rescanner dans FL Studio.

## Arborescence
- `Resources/Reference/index.html` : interface HTML d'origine, **intacte** (md5 208fb6f8da2506c7d98dde18997779d4).
- `Source/Parameters/` : IDs (`bm_*`) et plages generes depuis les attributs `data-*` du HTML.
- `Tests/` : tests sans JUCE, executes par CTest.
