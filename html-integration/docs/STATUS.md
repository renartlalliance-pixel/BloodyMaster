# BloodyMaster 0.3.4 — Tape Color

Reprise ici autorisée par l’utilisateur : Codex sur son PC n’a effectué aucune modification. Branche codex/html-webview2, implémentation dans html-integration.

0.3.3 : workflow Windows 42900487704 terminé avec succès, installateur téléchargé et SHA-256 vérifié (652aa4c90b092b1c270f5b8509d9cfcc7cb8a0cc3dcbd7681873bad7799ea7fa), 16 vérifications natives WebView2.

0.3.4 ajoute Tape Color indépendant et sélecteur Discret/Marqué. L’utilisateur a choisi les deux caractères puis délégué le choix de dosage après recherche : voir docs/TAPE_DESIGN_FR.md. 50 paramètres APVTS, 47 actifs ; 38 potards dans la page générée, dont 37 actifs. Huit suites DSP locales, tests ASan/UBSan, navigateur et 24 exports analysés réussis. Vérifier le workflow Windows de ce commit avant d’annoncer sa release validée.

Restent : Stereo Enhance/profils et véritable Mid pan, mesures LUFS/true peak, comparaison à volume compensé, presets enrichis après finalisation des modules, validation FL Studio et installateur final. La mesure LUFS/true peak a été annoncée comme prochaine tâche indépendante ; elle n’est pas encore implémentée. Le choix Tape est maintenant résolu, ne pas renvoyer ce QCM.

Préférences : français court/direct (mode caveman demandé), QCM aux décisions utiles ; si le panneau QCM disparaît, accepter les réponses en texte. Conserver exactement les deux HTML originaux. Pas d’accès distant au PC.
