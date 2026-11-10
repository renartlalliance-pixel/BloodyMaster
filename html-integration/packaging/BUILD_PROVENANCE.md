# Provenance de cette edition de test

Le fichier installe est un vrai VST3 Windows x64 construit depuis les sources de
ce paquet. JUCE 7.0.12 est utilise pour la voie MinGW, JUCE 8 ayant retire ce
support. Les transferts DSP et les IDs APVTS/du plugin restent ceux du projet.
Le chemin MSVC/JUCE 8 par defaut reste disponible, mais pas execute ici.

Les dependances de compilation ont ete extraites localement depuis les depots
Debian declares, sans installation systeme. Un juceaide Linux JUCE 7 a ete compile
comme outil hote ; les exports CMake du paquet hote ont ete corriges pour pointer
sur ce binaire. Les manifests VST3 automatiques sont desactives en cross-compilation
pour ne pas executer un outil Windows pendant la configuration Linux. Le bundle
standard contient Contents/x86_64-win/Bloody Master.vst3 et est chargeable sans
manifest moduleinfo.json, comme verifie par l'hote de test.

Runtimes MinGW lies statiquement. Aucun DLL libgcc/libstdc++/winpthread externe.
Installateur NSIS 3.11, cible amd64-unicode, demande de droits administrateur,
dossier VST3 commun 64 bits, outil WAV optionnel et desinstallation dediee.
Aucune signature numerique ni certificat fournis.

Le code JUCE de reference utilise est celui du tag officiel 7.0.12 ; sa licence
et ses sources sont fournies avec l'archive de sources de cette edition de test.
Les essais Windows ont ete executes sous Wine 10, pas sur Windows natif.
