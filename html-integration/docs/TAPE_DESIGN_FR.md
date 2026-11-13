# Tape Color — décision et réalisation

L’utilisateur a retenu deux caractères avec sélecteur, puis délégué le choix du dosage après recherche des usages en mastering.

Décision : dosage Tape Color indépendant du compresseur Tape Glue. Cela permet de conserver la dynamique choisie en ajustant la saturation. Ce choix est une déduction de conception, pas une règle universelle du mastering.

Sources primaires consultées :
- FabFilter Saturn 2, Band Controls : https://www.fabfilter.com/help/saturn/using/bandcontrols — contrôles de Drive, Dynamics et Level distincts, styles tape doux à extrêmes.
- u-he Satin, manuel : https://u-he.com/downloads/manuals/plugins/satin/Satin-user-guide.pdf — contrôle Input/Output et makeup ; types de bande aux comportements de distorsion et de réponse différents.

Algorithme original : saturation symétrique tanh à gain de petit signal normalisé ; arrondi des aigus dépendant d’une enveloppe stéréo liée, attaques 0,7 ms et récupération 80 ms ; deux caractères interpolés sur 20 ms, dosage parallèle. Discret utilise une excitation plus modérée et un mélange maximum de 50 %, Marqué une excitation plus forte et un mélange maximum de 85 %. Ce dosage change la coloration et son arrondi naturel des crêtes, sans modifier les paramètres ou détecteurs de Glue. Ce n’est pas une modélisation d’hystérésis ni une émulation précise de magnétophone. Pas de bruit, wow ou flutter ajouté.

Tape Color=0 % est neutre. Valeur par défaut et presets existants : 0 %. Le traitement s’exécute dans le suréchantillonnage 2/4/8x existant, avant THD/XL/Valve puis clipper/limiteur. Il n’ajoute pas de latence. L’atténuation naturelle des crêtes n’est pas une compensation automatique de sonie ; ne pas revendiquer une égalité perceptuelle de niveau.

Deux nouveaux paramètres sont ajoutés après les 48 précédents : bm_tape_color (0..100 %) et bm_tape_mode (Discret/Marqué). Les IDs/index précédents restent inchangés. Les ressources HTML approuvées sont conservées à l’octet près ; les contrôles sont ajoutés à la page générée.

Validation locale : huit suites DSP ; harmoniques, dosage, niveau faible, neutralité, absence de DC sur sinus, stéréo, automation, plafond et latence ; ASan/UBSan sans détection des fuites ; 24 exports de huit configurations à trois facteurs OS analysés avec NumPy/SciPy ; navigateur et gestes d’automation. La validation Windows propre à ce commit doit être constatée avant de déclarer l’installateur validé.
