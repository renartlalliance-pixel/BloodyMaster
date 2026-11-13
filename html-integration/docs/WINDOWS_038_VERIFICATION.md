# Vérification 0.3.8 — 13 novembre 2026

## Validation Windows 0.3.8

Workflow HTML 43482536001, commit f61a88c : succès, 16/16 CTest, 35 vérifications WebView2 natives, 31 tests WAV et tests installation/réinstallation/désinstallation réussis.

Release : https://github.com/renartlalliance-pixel/BloodyMaster/releases/tag/bloodymaster-html-0.3.8-test-43482536001

Installateur téléchargé et SHA-256 comparé au fichier publié : df7d5131538afb604e7341b68142cf6e762e5f517adb5f94c4ff7ab6f2c32ccb. Rapport WebView2 téléchargé : passed=35, backend=WebView2 native Windows.

Les tests plugin passent à quatre Fs, en mono/stéréo, 2×/4×/8×, avec blocs de 257 malgré une préparation de 64. Annulation exacte en chemin neutre, comparaison à une instance traitée indépendante (tolérance 1e-7), retour normal bit-exact après fondu et réinitialisations vérifiés. Tests DSP séparés, ASan/UBSan, navigateur, vérification syntaxique de l'éditeur natif et compteur d'allocation Delta (0 après préparation) réussis.

Le premier passage Windows passait l'audio mais échouait à une assertion d'infobulle : le constructeur juce::String(const char*) interprète les octets comme ASCII. L'assertion a été séparée du contrôle d'activation et utilise maintenant des séquences Unicode ASCII explicites. Le texte réel de l'interface et les sources audio sont inchangés par cette correction de test.

Delta est une différence brute après comparaison au master protégé ; son niveau peut dépasser le plafond du master. Il est forcé OFF au rendu hors ligne reconnu par l'hôte ; vérifier OFF avant export en temps réel. Aucun test d'écoute humaine ni FL Studio revendiqué. Les commits de documentation après f61a88c ne changent pas le code compilé de cet installateur.

HTML originaux inchangés : SHA-256 c48de97f75130c18aa8aef1f766e5be5652254fbf0da1a96fcf91d37761a83ff.
