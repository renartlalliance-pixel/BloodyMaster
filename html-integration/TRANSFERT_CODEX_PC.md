# Mise à jour de reprise — 0.3.4

L’utilisateur reprend dans cette conversation cloud : aucun travail n’a été fait sur le PC. Lire docs/STATUS.md et docs/TAPE_DESIGN_FR.md pour l’état le plus récent. Les sections historiques ci-dessous décrivent la 0.3.3 et restent utiles pour les contraintes, mais leurs compteurs sont dépassés.

Tape : sélecteur Discret/Marqué choisi par l’utilisateur, dosage séparé décidé par l’assistant après recherche expressément déléguée. Implémenté et testé localement ; workflow Windows à vérifier pour ce commit. Pas de nouveau QCM Tape nécessaire.

# BloodyMaster — reprise dans Codex sur le PC

## Dossier à ouvrir

Ouvrir ce dossier `html-integration` comme projet dans l’application Codex. Il contient la version développée dans cette conversation. La racine du dépôt contient une autre implémentation : ne pas la fusionner ni l’écraser automatiquement. Branche de travail : `codex/html-webview2` du dépôt https://github.com/renartlalliance-pixel/BloodyMaster.

Lire AGENTS.md et REPRISE_CODEX.md. Le second document décrit aussi un historique ancien ; pour l’état actuel, lire ce fichier et docs/STATUS.md.

## But utilisateur

Terminer tous les modules DSP, conserver l’interface HTML approuvée en ajoutant les nouveaux contrôles expressément choisis, créer des presets rap/pop/electro inspirés de méthodes documentées d’ingénieurs, et fournir un .exe installant le VST3 Windows x64 pour FL Studio. Travailler directement, ne pas renvoyer l’utilisateur vers un autre outil. Il veut un QCM à chaque étape de choix. Ne pas redemander les décisions déjà prises ci-dessous.

Dernière demande : transfert du projet vers l’application ChatGPT Codex installée sur son PC. Aucun accès distant au PC ni ouverture locale n’a été effectué par la session cloud.

## Décisions validées

- De-Harsh ciblé/transparent autour de Harsh Freq.
- Mud et Ring dynamiques, correction seulement quand la zone ressort, profondeur maximale visée 12 dB.
- Chaque Trap a son dosage et activation séparés ; contrôles supplémentaires autorisés. Ils sont désactivés par défaut.
- Spike Listen écoute le signal retiré par les trois nettoyages, aligné en latence ; bypass global prioritaire.
- THD polyvalent : deux couleurs sélectionnables Warm (asymétrique/paires) et Dense (symétrique/impaires).
- XL : sélecteur rotatif à trois positions Densité/Présence/Impact + potard XL existant pour le dosage. Pas trois dosages simultanés.
- Valve : légère coloration à lampes ajoutée à sa récupération optical existante.
- Préserver byte pour byte Resources/UI/index.html et Resources/Reference/index.html : SHA256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.

## Réalisé et vérifié

La version 0.3.2 a réussi le workflow Windows natif 42861013470 : https://github.com/renartlalliance-pixel/BloodyMaster/actions/runs/42861013470. Elle inclut la véritable UI HTML/WebView2 et le nettoyage dynamique. Installer à chercher dans la release `bloodymaster-html-0.3.2-test-42861013470`.

Les sources de cette reprise sont plus récentes (0.3.3). Coloration.h est exécuté dans le chemin suréchantillonné existant de PeakChain avant clipper/limiteur ; aucune latence supplémentaire. THD −60 dB = off ; son échelle est un dosage, pas une mesure certifiée du THD. XL traite différemment la bande selon le mode. Les transitions sont lissées et le DC est retiré uniquement du résidu généré. Valve est raccordé au même paramètre que la récupération optical. Les presets font correspondre cette coloration au mode optical.

48 paramètres APVTS : les 42 initiaux gardent leurs index, puis quatre contrôles Traps, puis deux choix THD/XL. 45 paramètres actifs exposés à l’UI. Deux paramètres restent sans effet DSP : enhance et profile ; lufs_target reste un repère désactivé dans cette version. Les ressources originales ont 34 potards ; la page générée en a 37, dont 36 actifs. Les contrôles harmoniques sont repliés dans le HUD pour éviter le débordement.

Tests locaux : huit suites CTest DSP, test Coloration (harmoniques paires/impaires, dosage, DC, grave, modes, stéréo, automation, plafond et latence), 18 exports analysés par NumPy/SciPy, tests navigateur avec backend simulé. Consulter docs/COLOR_VALIDATION.json et docs/HTML_BROWSER_VALIDATION.json. Ce ne sont pas des écoutes comparatives de machines analogiques. Pour 0.3.3, vérifier le nouveau workflow Windows avant d’annoncer son installateur validé.

## Reprendre concrètement

1. Vérifier le workflow `.github/workflows/html-ui.yml` à la racine du dépôt pour le dernier commit de la branche. Il compile avec MSVC/JUCE 8.0.4, exécute les tests DSP/plugin/WebView2/VST3, puis construit et teste le setup NSIS ; la release n’est publiée que si tout passe. Corriger toute erreur réelle. La dernière 0.3.2 est déjà testée.
2. Pour tests DSP portables : `cmake -S Tests/DSP -B build/core -DCMAKE_BUILD_TYPE=Release`, puis `cmake --build build/core --config Release`, puis `ctest --test-dir build/core -C Release --output-on-failure`. Les chemins des exécutables varient avec le générateur.
3. `Tests/DSP/validate_color.py <bm_test_color> <nouveau-dossier>` utilise NumPy/SciPy et écrit des exports dédiés. Ne pas l’exécuter dans les masters de l’utilisateur.
4. Passer ensuite au QCM suivant : rôle et intensité de Tape Glue (compression déjà présente, coloration bande absente), puis Stereo Enhance/profils/Mid pan. Demander les choix avant d’implémenter les comportements concernés. Les LUFS doivent utiliser une mesure réelle, pas les anciennes simulations de l’HTML.
5. Finir mesures LUFS/true peak, comparaison A/B compensée, presets cohérents avec tous les modules. Recompiler l’installateur final, valider sauvegarde/reprise et automation dans FL Studio et fournir un lien .exe.

## Contraintes importantes

Pas d’allocation, verrou, fichier ou appel UI dans la boucle audio. Déclarer et aligner la latence. Préserver IDs/index APVTS, masters et archives d’origine. Les algorithmes sont originaux, inspirés de principes documentés Pultec/Bettermaker/Pendulum et de la couverture bx_masterdesk PRO : aucune promesse d’émulation fidèle ou de coefficients propriétaires. Le HTML exact reste la référence ; travailler les extensions dans tools/build_web_ui.py et Resources/Web/bridge.js.

Le dossier source n’embarque pas JUCE ni Visual Studio. Le CMake télécharge JUCE ; le workflow fournit WebView2/NSIS. Un PC qui lance les builds localement doit disposer des outils correspondants. Aucun skill personnel « appris hier » n’était fourni ; ne pas inventer son contenu.
