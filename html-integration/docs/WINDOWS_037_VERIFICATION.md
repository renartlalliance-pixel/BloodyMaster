# Vérification 0.3.7 — 13 novembre 2026

## Validation Windows 0.3.7

Workflow HTML 43452301612, commit 90eeb18 : succès. 15/15 CTest, 30 vérifications WebView2 natives, 31 tests WAV, installation/réinstallation/désinstallation et conservation des fichiers tiers réussies.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.7-test-43452301612

Installateur téléchargé et SHA-256 comparé au fichier publié : 0abaf2dfe73ccaaf896c01296ec5985232cb0911f3c837cec76d2f62148ab135. Rapport WebView2 téléchargé : passed=30, backend=WebView2 native Windows.

Un premier test de bypass démarrait avec une transition des traitements encore active ; son initialisation a été corrigée. Une exécution suivante a échoué au démarrage de WebView2 avant chargement de la page ; la nouvelle exécution complète ci-dessus passe sans contourner les tests.

Vérifications indépendantes : 60 vecteurs à quatre Fs et trois modes OS (pire crête 32× −2,42463 dBTP), 16 rendus de protection avec mesure LUFS à 48 kHz, SRC 44,1/48 kHz et AAC/MP3, puis 32 décodages Opus/Vorbis. Les plafonds et limites sont décrits dans OUTPUT_SAFETY_FR.md ; aucune garantie universelle sur les LUFS d'un programme futur ni tous les traitements des plateformes. Vérifier le fichier complet et effectuer l'essai FL Studio/écoute humaine.

Les commits de documentation après 90eeb18 ne modifient pas les sources compilées de cet installateur. Deux HTML originaux conservés, SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.
