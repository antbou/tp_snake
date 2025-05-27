# Bugs et difficultés rencontrés

Ce fichier recense les principaux problèmes rencontrés pendant le développement du jeu Snake, ainsi que les solutions mises en place ou envisagées.

---

## 1. Nourriture non totalement effacée après avoir été mangée

### Problème :

Lorsque le serpent mange une nourriture, une partie de celle-ci (pixels rouges) restait visible à l'écran.

### Cause :

- La nourriture est dessinée avec un zoom de 4 pixels (4×4).
- Les coordonnées générées pour la nourriture **n'étaient pas toujours alignées sur la grille de 4 pixels**, ce qui causait un décalage visuel.
- Le serpent ne recouvrait pas l’intégralité du fruit lorsqu'il passait dessus.

### Solution :

- Forcer toutes les coordonnées (nourriture et serpent) à être **multiples de 4**.
- Supprimer l’effacement manuel `draw_pixel(..., EMPTY)` avant de dessiner la tête du serpent.
- Corriger les fonctions `generate_food_coord` et `new_position` pour garantir cet alignement.

---

## 2. Le serpent ne grandissait pas après avoir mangé

### Problème :

Après avoir mangé un fruit, le serpent n’augmentait pas de longueur.

### Cause :

- La logique de déplacement du serpent supprimait la queue à chaque frame, même en cas de consommation de nourriture.

### Solution :

- En cas de détection de nourriture, **ajouter la tête sans supprimer la queue**, via `queue_enqueue(...)` seul.
- Appeler `move_snake(...)` uniquement lorsqu’aucun fruit n’a été mangé.

---

## 4. Problèmes de gestion du temps pour l’apparition des fruits

### Problème

La nourriture apparaissait à des intervalles irréguliers :

- parfois **trop rapidement** (à chaque frame),
- parfois **avec beaucoup de retard**,
- parfois **pas du tout**,
- et des logs affichaient même des **valeurs de temps négatives**.

### Causes détaillées

#### a. Mauvaise gestion des unités de temps

- `mu_seconds_elapsed` était mesuré en **millisecondes**, mais comparé à une constante (`FOOD_SPAWN_INTERVAL`) définie en **microsecondes**.
- Cette incohérence entraînait des comparaisons erronées et un comportement imprévisible.

#### b. Incompréhension de `clock_gettime()`

- Le placement incorrect de `clock_gettime(&start)` (au début de la boucle) combiné à `clock_gettime(&finish)` à la fin causait des cas où `finish < start`, ce qui produisait des **valeurs négatives** pour `mu_seconds_elapsed`.
- Cela montrait une mauvaise compréhension du fonctionnement de `CLOCK_MONOTONIC`, qui retourne un temps absolu croissant, mais nécessite une cohérence entre les points de mesure.

#### c. Accumulation du temps via `+=`

- La durée entre deux apparitions de nourriture était gérée en ajoutant à chaque frame la durée estimée d'exécution (`mu_seconds_elapsed`) à un accumulateur (`time_since_last_food`).
- Cette approche semblait fonctionner au départ, mais elle s’est révélée instable :

  - Elle dépendait trop du rythme réel de la boucle (`while`), qui peut varier selon la charge CPU ou le système.
  - Des erreurs d’arrondi ou des frames trop lentes/rapides provoquaient une dérive du temps mesuré.
  - En cas de bug de mesure (ex. `mu_seconds_elapsed` négatif), l'accumulateur pouvait même reculer, empêchant la génération de nouveaux fruits.

- En résumé, cette méthode rendait le comportement imprévisible, avec des nourritures qui apparaissaient trop souvent, ou pas du tout.
- La solution a été d’abandonner l'accumulation au profit d’une comparaison directe entre deux horodatages (`now` et `last_spawn_time`), garantissant un contrôle fiable et indépendant du framerate.

### Solutions apportées

- **Unification des unités** : tout est désormais exprimé en millisecondes.
- **Suppression de l’accumulation** : abandon du `+=`, remplacé par un calcul direct du temps écoulé :
  ```c
  time_since_last_food = (now.tv_sec - last_spawn.tv_sec) * 1000.0 +
                         (now.tv_nsec - last_spawn.tv_nsec) / 1.0e6;
  ```
