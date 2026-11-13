# 0.3.9 — version de test Windows vérifiée

IN GAIN indépendant (−18/+18 dB) et LoudnessWar dans le panneau doré final (0/+18 dB) implémentés. Blood Drive/Dynamics.h inchangés. Correction de la protection trop conservatrice : énergie K glissante 400 ms, gains crêtes/loudness séparés. Lire LOUDNESS_GAIN_FR.md. Objectif global −5 LUFS conservé, plafond utilisateur prioritaire avec réserve interne. Compatibilité anciens presets/sessions par gains à zéro ; User v2, 52 paramètres/51 DSP actifs, Delta inclut les gains, latence inchangée. HTML originaux conservés.

Tests DSP locaux, navigateur et mesure indépendante de stress réussis pour audio brut/SRC ; un bruit dense extrême dépasse 0 dBTP après AAC : aucune garantie universelle des encodages. Build et installateur Windows vérifiés ; écoute FL Studio nécessaire.

---

# 0.3.8 — Delta global, version de test Windows vérifiée

DELTA implémenté : entrée avant DSP retardée de la latence complète (848 échantillons à 48 kHz) moins sortie après gain et protection, transitions 20 ms, aucun ajout de latence. État temporaire non sauvegardé, remis à zéro aux rappels Factory/User/session/A-B, à prepare et au rendu hors ligne. Les 50 paramètres de traitement et presets User restent inchangés. Voir DELTA_FR.md.

Différence brute diagnostique après le master protégé : le plafond de protection ne s'applique pas une seconde fois au signal Delta. Indication explicite dans le HUD. Spike Listen est suspendu sans modifier son paramètre pendant Delta. Mesures et scope reflètent l'écoute.

Tests DSP et ASan/UBSan réussis, tests navigateur (commande, état, infobulle, préséance du solo, disposition et non-régression User) réussis. Tests JUCE de soustraction réelle, annulation neutre, blocs surdimensionnés et resets ajoutés. Workflow Windows réussi ; voir preuve 0.3.8 ci-dessous. HTML originaux inchangés.

---

# 0.3.7 — protection globale, version de test Windows vérifiée

Protection finale implémentée, indépendante de LoudnessWar (potard non ajouté). Voir OUTPUT_SAFETY_FR.md et OUTPUT_SAFETY_VALIDATION.json. Active après bypass interne/hôte, delta et compensation ; aucune hausse de gain ; crêtes −2 dBTP avec réserve et régulation prudente d'énergie K pour l'objectif global −5 LUFS. Cette régulation n'est pas une garantie mathématique de tout intégré futur : vérifier le fichier complet.

Latence 848 échantillons à 48 kHz, déclarée au DAW. Le bypass conserve désormais la protection ; signal faible inchangé après délai. Presets User et 50 paramètres inchangés. HTML originaux préservés. Tests dédiés quatre Fs et tests indépendants 16 rendus/SRC/AAC/MP3 réussis. Workflow Windows réussi ; voir preuve 0.3.7 ci-dessous.

La note LOUDNESSWAR_DESIGN_FR.md ci-dessous était la conception initiale ; OUTPUT_SAFETY_FR.md décrit l'implémentation actuelle et ses limites. La 0.3.6 reste la version précédente vérifiée.

---

# Prochaine évolution : LoudnessWar et protection globale

Décision utilisateur : −5 LUFS est une limite globale du VST, indépendante du potard. Lire LOUDNESSWAR_DESIGN_FR.md : gain accessible séparé, surveillance intégré/court terme, protection finale de crêtes à −2 dBTP, respect des anciens presets et traitement explicite du bypass. Cette évolution est une conception documentée ; le DSP livré reste la 0.3.6 et ne garantit pas encore cette limite globale.

---

# BloodyMaster — 0.3.6, version de test Windows vérifiée

Ajoute la sauvegarde nommée dans User, persistante hors installation, avec rappel des 50 paramètres, noms accentués, copies numérotées, actualisation et validation complète avant rappel. Huit presets originaux d'inspiration rap/electro/new rock/trap ajoutés : 17 Factory au total, indices historiques 0..8 inchangés. Lire USER_PRESETS_FR.md et ENGINEER_INSPIRATIONS_2026_FR.md.

Tests locaux : 204 rendus de presets (17 × 4 Fs × 3 OS), 31 tests WAV, navigateur (groupes, dialogue, noms, rappel, erreurs, dimensions), vérification syntaxique JUCE 8.0.4 des tests et de l'éditeur. Tests natifs User et WebView2 réussis ; voir validation Windows 0.3.6 ci-dessous. HTML originaux inchangés.

La demande de potard reste une tâche ultérieure non précisée ; ne pas en inventer le rôle. L'utilisateur a demandé les presets avant de préciser ce potard.

---

# BloodyMaster — 0.3.5, version de test Windows vérifiée

Branche codex/html-webview2, implémentation html-integration. Lire AGENTS.md, docs/STEREO_DESIGN_FR.md et docs/METERING_FR.md.

## 0.3.4 vérifiée

Workflow Windows 43433424498, commit 07b840d386841ed5aa12b239571adede6e45150f : job build-html réussi, 11/11 CTest, 31 tests WAV, installation/réinstallation/désinstallation réussies. Release de test publiée : bloodymaster-html-0.3.4-test-43433424498. Installateur téléchargé et SHA-256 comparé au fichier publié : 39342135e4f704823a436096004b1bc0e765fbd72f6328eb2e50823c640ca116. Cela ne remplace pas un essai dans FL Studio.

## Ajouts 0.3.5

Atmosphere EQ M/S : quatre profils, Side seulement, grave préservé à moins de 0,05 dB jusqu'à 120 Hz dans les tests, aucun délai ajouté. Vrai Mid Pan, choix utilisateur A : centre déplacé, côtés fixes. IDs APVTS préservés, 50 paramètres dont 49 actifs. HTML originaux inchangés. Voir STEREO_DESIGN_FR.md pour courbes et comportement des anciens réglages de pan.

Onze suites DSP locales réussies ; stéréo et mesure sous ASan/UBSan. Mesure indépendante SciPy et navigateur avec backend simulé réussis. Tests natifs Windows réussis ; voir validation ci-dessous.

## Suite

LUFS intégré et court terme 3 s + true peak estimé 4× et RAZ ajoutés (choix A). Comparaison d'écoute A/B temporaire, désactivée par défaut, atténuation seule jusqu'à 12 dB ; couper avant export temps réel, gain unité pour rendu hors ligne signalé par l'hôte (choix A). Mesures avant compensation. Presets de genre enrichis avec Atmosphere et Tape Color légers. Voir METERING_FR.md et PRESETS_FR.md.

Restent écoute comparative humaine et validation FL Studio. Le sélecteur TARGET reste inactif : aucune normalisation automatique LUFS promise.

Préférences : français court/direct, QCM aux décisions utiles ; accepter réponses dans le chat si le panneau disparaît. Atmosphere, Mid Pan A, mesures A et compensation A acceptés. L’utilisateur a indiqué « A à chaque fois » : A par défaut, nouvelle question seulement pour un arbitrage important. Tape Color Discret/Marqué déjà résolu. Conserver exactement les deux HTML originaux. Pas d'accès distant au PC.

Demande suivante de l’utilisateur : ajouter un potard après la fin de cette validation. Son rôle n’a pas encore été précisé ; ne pas l’inventer et terminer d’abord la version courante.

## Validation Windows 0.3.5

Workflow HTML 43437863780, commit e96d2a7 : succès, 14/14 CTest, 24 vérifications WebView2 natives, 31 tests WAV, installation/réinstallation/désinstallation réussies. Release de test : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.5-test-43437863780

Installateur téléchargé et SHA-256 comparé au fichier publié : 172892efd9f4aa385dbc3b22c827b30e4542aca41a209962e1b7697318d97eee. Rapport WebView2 téléchargé : passed=24, backend=WebView2 native Windows. Le workflow Windows de la racine 43437863633 est également réussi, mais porte sur l'édition historique ; la référence pour les ajouts est le workflow HTML ci-dessus.

Restent écoute humaine et validation FL Studio. Les commits de documentation après e96d2a7 ne changent pas les sources compilées de cet installateur.

## Validation Windows 0.3.6

Workflow HTML 43442449010 réussi, commit 0fbeadf : 14/14 CTest, dont les tests User (persistance, noms accentués, doublons, fichiers invalides, droits et rappel de session) ; 29 vérifications WebView2 natives, 31 tests WAV et tests installation/réinstallation/désinstallation réussis.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.6-test-43442449010

Installateur téléchargé et SHA-256 comparé au fichier publié : ddd2b17e7765edded6b99e3afdfa88ab043e1ae59f27658409aecee7ac2d9266. Rapport WebView2 téléchargé : passed=29, backend=WebView2 native Windows.

Les 24 exports indépendants supplémentaires des nouveaux presets passent la reconstruction de crêtes SciPy 32× ; voir INSPIRATION_PRESET_VALIDATION.json. Les commits de documentation suivant 0fbeadf ne changent pas le code compilé de l'installateur. Restent écoute comparative humaine et validation FL Studio.

## Validation Windows 0.3.7

Workflow HTML 43452301612, commit 90eeb18 : succès. 15/15 CTest, 30 vérifications WebView2 natives, 31 tests WAV, installation/réinstallation/désinstallation et conservation des fichiers tiers réussies.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.7-test-43452301612

Installateur téléchargé et SHA-256 comparé au fichier publié : 0abaf2dfe73ccaaf896c01296ec5985232cb0911f3c837cec76d2f62148ab135. Rapport WebView2 téléchargé : passed=30, backend=WebView2 native Windows.

Un premier test de bypass démarrait avec une transition des traitements encore active ; son initialisation a été corrigée. Une exécution suivante a échoué au démarrage de WebView2 avant chargement de la page ; la nouvelle exécution complète ci-dessus passe sans contourner les tests.

Vérifications indépendantes : 60 vecteurs à quatre Fs et trois modes OS (pire crête 32× −2,42463 dBTP), 16 rendus de protection avec mesure LUFS à 48 kHz, SRC 44,1/48 kHz et AAC/MP3, puis 32 décodages Opus/Vorbis. Les plafonds et limites sont décrits dans OUTPUT_SAFETY_FR.md ; aucune garantie universelle sur les LUFS d'un programme futur ni tous les traitements des plateformes. Vérifier le fichier complet et effectuer l'essai FL Studio/écoute humaine.

Les commits de documentation après 90eeb18 ne modifient pas les sources compilées de cet installateur. Deux HTML originaux conservés, SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.

## Validation Windows 0.3.8

Workflow HTML 43482536001, commit f61a88c : succès, 16/16 CTest, 35 vérifications WebView2 natives, 31 tests WAV et tests installation/réinstallation/désinstallation réussis.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.8-test-43482536001

Installateur téléchargé et SHA-256 comparé au fichier publié : df7d5131538afb604e7341b68142cf6e762e5f517adb5f94c4ff7ab6f2c32ccb. Rapport WebView2 téléchargé : passed=35, backend=WebView2 native Windows.

Les tests plugin passent à quatre Fs, en mono/stéréo, 2×/4×/8×, avec blocs de 257 malgré une préparation de 64. Annulation exacte en chemin neutre, comparaison à une instance traitée indépendante (tolérance 1e-7), retour normal bit-exact après fondu et réinitialisations vérifiés. Tests DSP séparés, ASan/UBSan, navigateur, vérification syntaxique de l'éditeur natif et compteur d'allocation Delta (0 après préparation) réussis.

Le premier passage Windows passait l'audio mais échouait à une assertion d'infobulle : le constructeur juce::String(const char*) interprète les octets comme ASCII. L'assertion a été séparée du contrôle d'activation et utilise maintenant des séquences Unicode ASCII explicites. Le texte réel de l'interface et les sources audio sont inchangés par cette correction de test.

Delta est une différence brute après comparaison au master protégé ; son niveau peut dépasser le plafond du master. Il est forcé OFF au rendu hors ligne reconnu par l'hôte ; vérifier OFF avant export en temps réel. Aucun test d'écoute humaine ni FL Studio revendiqué. Les commits de documentation après f61a88c ne changent pas le code compilé de cet installateur.

HTML originaux inchangés : SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.


## Validation Windows 0.3.9

Workflow HTML 43493392145, commit 6ec0e12274e35305ba03fceeeeb8005259c39d32 : succès. 17/17 CTest (audio, migration des sessions/User v1, gains indépendants, Delta avec nouveaux gains, chargement VST3), 37 vérifications WebView2 natives, 31 tests WAV. Installation répétée, désinstallation, conservation des fichiers tiers et réinstallation réussies.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.9-test-43493392145

Installateur téléchargé (4 333 125 octets) ; SHA-256 local comparé au fichier publié : 7810f26f44adae643e771f2080cc647b83f2fa313331ecd04b35973e3572b92f. Rapport natif téléchargé : passed=37, backend=WebView2 native Windows.

Mesure indépendante du beat synthétique (entrée −22 LUFS, Drive 12/Hammer 6 inchangés, plafond −3) : LoudnessWar 0/6/12/18 → −11,80/−9,83/−7,68/−6,41 LUFS. Crêtes brutes reconstruites 32× sous −3 dBTP sur ces quatre sorties. AAC du cas +12 remonte à +0,012 dBTP ; un bruit dense de stress à +0,42 : limites explicitement enregistrées, aucune garantie codec universelle. Voir LOUDNESS_GAIN_VALIDATION.json et OUTPUT_SAFETY_039_STRESS.json.

Régression indépendante de 60 vecteurs, quatre Fs/trois OS : pire crête 32× −2,42461 dBTP ; voir OUTPUT_SAFETY_039_60_VECTORS.json. Tests DSP locaux, vérification syntaxique JUCE, navigateur et ASan/UBSan de protection réussis. HTML originaux inchangés, SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff. Les commits de documentation après 6ec0e12 ne modifient pas les sources de cet installateur ; écoute humaine/FL Studio à faire.
