# Presets User — 0.3.6

1. Régler BloodyMaster puis cliquer **Sauver**, à côté du choix de preset.
2. Donner un nom et cliquer **Enregistrer**.
3. Retrouver ce preset dans la section **User** du même menu.

Les sections Factory, Rap, Electro, New Rock et Trap restent séparées de User. Le bouton ↻ recharge les presets enregistrés par une autre instance ; la réouverture de l'interface recharge également la liste. Un nom déjà utilisé, sans distinction de casse, crée une copie « (2) », « (3) », etc. Les noms accentués et caractères de chemin sont des libellés : ils ne deviennent jamais des chemins disque.

Les presets sont enregistrés dans le dossier de données utilisateur :

- Windows : `%APPDATA%\BRI Audio Systems\BloodyMaster\User Presets`
- Fichiers `.bmpreset`, JSON versionné, nom et 50 paramètres APVTS en unités physiques.

Les identifiants de fichiers sont des UUID. L'écriture passe par un fichier temporaire dans le même dossier puis son remplacement. Le setup et la désinstallation ne suppriment pas ce dossier. Sauvegarder/copier ce dossier permet de conserver sa banque personnelle. La version présente ne propose pas de suppression ni d'écrasement depuis l'interface.

Les 50 réglages sont rappelés, y compris les choix, bypass et écoute Spike. La compensation d'écoute temporaire A/B et les mesures ne font pas partie des paramètres et ne sont pas enregistrées. Les sessions DAW conservent déjà leur copie des réglages APVTS ; le son d'une session reste rappelable même si le fichier User est ensuite absent. Le nom User peut être reconnu par correspondance des paramètres lorsque son fichier existe encore.

Aucun accès disque dans le callback audio : lecture, validation et écriture sont effectuées sur le thread de l'interface. Les fichiers trop gros (>128 Kio), incomplets, de version inconnue ou aux paramètres invalides sont refusés. Un rappel invalide ne modifie aucun paramètre. L'interface affiche les erreurs d'accès et ignore les fichiers invalides au scan.

Les programmes proposés par le DAW restent la banque Factory de 17 entrées, avec indices historiques 0 à 8 conservés. User est une bibliothèque de l'interface HTML ; sa taille ne modifie pas les indices d'automation des programmes.
