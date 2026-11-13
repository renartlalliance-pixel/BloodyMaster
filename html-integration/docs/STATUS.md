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
