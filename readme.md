# Snake

## Table des matières

- [Résumé](#résumé)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Arguments](#arguments)
- [Fonctionnalités](#fonctionnalités)
- [Fonctionnalités personnalisées](#fonctionnalités-personnalisées)
- [Bugs rencontrés](#bugs-rencontrés)
- [Licence](#licence)

---

## Résumé

Snake est une version moderne du jeu classique, développée en C avec la bibliothèque SDL2. Le but est simple : diriger un serpent pour qu’il mange de la nourriture, sans heurter les murs ni se mordre lui-même.

---

## Prérequis

Avant de compiler et exécuter le jeu, assurez-vous d’avoir :

- `gcc` (compilateur C)
- `make`
- SDL2 : `libsdl2-dev`
- SDL2_ttf : `libsdl2-ttf-dev`

### Installation des dépendances (Ubuntu/Debian)

```sh
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

---

## Installation

Clonez le dépôt et compilez le projet avec `make` :

```sh
git clone https://gitedu.hesge.ch/tonrepo/snake
cd snake
make
```

---

## Utilisation

Lancez simplement le jeu :

```sh
./main
```

ou avec make :

```sh
make run
```

Ou avec des paramètres personnalisés :

```sh
./main [food_spawn_interval_seconds] [max_food_count]
```

### Exemple

```sh
./main 3 25
```

Cela configure une nourriture générée toutes les 3 secondes, avec un maximum de 25 sur l’écran.

---

## Arguments

| Argument                      | Type     | Description                               |
| ----------------------------- | -------- | ----------------------------------------- |
| `food_spawn_interval_seconds` | `double` | Temps entre les apparitions de nourriture |
| `max_food_count`              | `int`    | Nombre maximum de nourritures simultanées |

Les paramètres sont **optionnels**. Si non spécifiés ou invalides, des valeurs par défaut sont utilisées.

---

## Fonctionnalités

- Contrôles : touches fléchées ou `W`, `A`, `S`, `D`
- Menu interactif de démarrage
- 3 niveaux de difficulté
- Détection des collisions :
  - Murs
  - Nourriture
  - Soi-même
  - Demi-tour interdit (reverse turn = game over)
- Écran de fin avec score et option rejouer/quitter

---

## Fonctionnalités aditionnelles

3 "fonctionnalités" en plus ont été ajoutées :

- **Menus dynamiques** au démarrage et à la fin du jeu, permettant de choisir la difficulté et de rejouer.
- **choix de la vitesse du serpent** qui est lié à la difficulté choisie, plus la difficulté est élevée, plus le serpent est rapide. (voir [docs/snake_move_interval](docs/snake_move_interval))
- **Affichage du score** avec un écran de fin affichant le score final.

---

## Bugs rencontrés

Une liste des bugs rencontrés et résolus se trouve dans le fichier [docs/bug](docs/bug).

## Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.
