# Etat reel — M1 (corrige)

Verifie reellement (execute) lors de la redaction :
- `Tests/DSP/test_dbutils.cpp` : compile (g++ 13, C++20, -Wall -Wextra) et passe.
- `.github/workflows/build.yml` : YAML syntaxiquement valide (parse Python).
- `Resources/Reference/index.html` : identique a l'original (md5 208fb6f8da2506c7d98dde18997779d4).

Ecrit mais NON compile / NON execute (JUCE, CMake, MSVC absents de l'environnement de redaction) :
- CMakeLists.txt, PluginProcessor/Editor, ParameterLayout, build.bat.
- Tests/Plugin/PluginTests.cpp (layouts mono/stereo, passthrough bit-exact, 42 parametres automatisables, etat).
- Le workflow GitHub Actions n'a jamais tourne : il est "operationnel" seulement apres un premier run vert chez vous.

Contenu fonctionnel de M1 : passthrough, 34 parametres float + 8 globaux, bypass hote, etat XML. Aucun DSP. UI = liste generique (HTML non integre : prevu en M7, fichier non modifie).
Latence : 0.

## Correctif chemins longs Windows (post-M1)
- JUCE est maintenant place dans `C:/bm_deps` (variable CMake `BM_DEPS_DIR`), `build.bat` retombe sur `%LOCALAPPDATA%\bm_deps` si C:\ n'est pas inscriptible.
- Avertissement CMake/`build.bat` si le chemin du projet est long.
- Ecrit, **non execute** par moi : le correctif repose sur le retour de l'utilisateur (le build a reussi apres deplacement vers `C:\BM_M1`, selon un compte rendu non verifie par moi).
- Apres mise a jour : supprimer l'ancien dossier `build` avant de relancer.
