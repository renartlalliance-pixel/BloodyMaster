# BloodyMaster — consignes persistantes de l'utilisateur

Lire REPRISE_CODEX.md avant toute modification. Le développement est repris depuis une conversation ChatGPT ; les sources contiennent M2a puis une M3 alpha en cours.

- C'est Codex qui réalise directement le développement. Ne pas renvoyer l'utilisateur vers OpenCode et ne pas se limiter à rédiger des prompts.
- Conserver exactement `Resources/UI/index.html` et sa copie `Resources/Reference/index.html`. L'utilisateur a explicitement demandé de garder son HTML tel quel : pas de redesign, de déplacement de boutons, de nouvelle maquette ou de changement des couleurs. Prévoir la liaison JUCE séparément, sans écraser la référence.
- Développer un plugin de mastering original : couverture fonctionnelle inspirée de bx_masterdesk PRO, EQ musicale à courbes inspirées du Pultec ; dynamique/clipper/limiteur inspirés du comportement documenté de Bettermaker et du Pendulum OCL-2. Ne pas prétendre émuler fidèlement leurs circuits ou disposer de leurs DSP propriétaires.
- Cible : VST3 Windows, notamment FL Studio. Le Standalone et un outil WAV de test sont également présents.
- L'objectif immédiat est une vraie version de test utilisable, avec des indications honnêtes sur la compilation, les mesures et les tests Windows réellement exécutés.
- Préserver les archives originales. Travailler uniquement dans BloodyMaster, pas dans Prisme, PyraVintageSkin ou un autre projet.
- Conserver les IDs APVTS et les réglages existants tant qu'une migration explicite n'est pas nécessaire.
- Pas d'allocations, d'accès disque, de verrous ou d'appels UI dans la boucle audio. Déclarer la latence au DAW et aligner le bypass.
- Valider les DSP avec des tests numériques pertinents et une mesure indépendante des crêtes. Ne pas transformer quelques tests réussis en garantie universelle de qualité, de true peak ou de conformité.
- Répondre en français simple, avec des étapes courtes et des résultats concrets. L'utilisateur a déjà autorisé le développement : avancer sans redemander les mêmes choix.
