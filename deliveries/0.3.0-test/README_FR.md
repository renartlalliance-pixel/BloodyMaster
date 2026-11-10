# BloodyMaster — installateur de test Windows x64

Fichier : **BloodyMaster-0.3.0-Test-Setup.exe** (3 552 025 octets).

## Installer et essayer
1. Fermer FL Studio et les autres logiciels audio.
2. Lancer l'installateur. Il place le VST3 dans `C:\Program Files\Common Files\VST3\Bloody Master.vst3`.
3. Dans FL Studio : Options → Manage plugins → Find installed plugins, puis chercher **Bloody Master**.
4. Le programme peut être désinstallé depuis les applications Windows.

Neuf presets, 32 contrôles actifs et mesures audio réelles dans l'éditeur de test.
L'HTML de référence est conservé, mais son intégration et les DSP listés comme inactifs dans l'audit restent à développer. Ce n'est pas la version complète finale.

## Vérifications exécutées
Compilation Windows x64 MinGW-w64 / JUCE 7.0.12, runtimes statiques.
8/8 suites des binaires Windows passent sous Wine 10, dont tests JUCE et petit hôte VST3 indépendant. Installation, réinstallation, désinstallation, protection d'un fichier tiers et refus d'installation lorsque le plugin est verrouillé ont été vérifiés.
**Pas de test sous Windows natif ou dans FL Studio. Installateur non signé.**

## Sources correspondantes
[Sources de cette édition](BloodyMaster-0.3.0-Test-Project-Sources.zip).
[Sources JUCE 7.0.12 utilisées](https://github.com/juce-framework/JUCE/archive/refs/tags/7.0.12.zip).

Cette édition provient du travail de cette conversation. Elle est distincte de l'implémentation actuellement sur `main` ; le dépôt existant n'a pas été remplacé.
Voir `INSTALLER_VALIDATION.json` et `SHA256SUMS.txt`.
