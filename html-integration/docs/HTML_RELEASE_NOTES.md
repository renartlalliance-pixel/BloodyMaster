Votre interface HTML approuvée est intégrée au VST3 Windows via Microsoft WebView2.

- Disposition et style de référence conservés ; CSS embarqué, sans dépendance réseau pour afficher l'interface.
- Potentiomètres actifs, gestes d'automation, retour des paramètres hôte, neuf presets, bypass et comparaison A/B.
- Oscilloscope et crêtes issus des échantillons réels ; courbe EQ calculée avec les coefficients du moteur.
- Fréquence d'échantillonnage et latence réelles. Les LUFS et true peak non mesurés ne sont pas simulés.
- Les fonctions DSP encore absentes sont indiquées comme inactives ; l'intégration graphique ne les implémente pas.
- WebView2 est installé si nécessaire par le setup (connexion Internet nécessaire uniquement dans ce cas).

Fermer FL Studio, lancer le setup, puis scanner les plugins et ouvrir Bloody Master.
Cette édition est compilée avec MSVC et JUCE 8.0.4. Les sources correspondantes sont dans l'archive jointe et dans le dossier `html-integration` du commit ; JUCE est récupéré depuis son tag officiel 8.0.4 : https://github.com/juce-framework/JUCE/archive/refs/tags/8.0.4.zip
Installateur non signé, édition de test. La validation FL Studio sur la machine de l'utilisateur reste à effectuer.
