# Gestion du temps dans le jeu Snake

Lors du développement du jeu Snake en C, une problématique de synchronisation est apparue : le déplacement du serpent ne devait pas dépendre du taux de rafraîchissement de l’écran. Initialement, la fréquence d’affichage était fixée à 24 FPS, mais cela ne suffisait pas à garantir un comportement constant du serpent.

Pour y remédier, une constante `SNAKE_MOVE_INTERVAL` a été introduite afin de réguler sa vitesse de manière indépendante.

## Objectif de `SNAKE_MOVE_INTERVAL`

Cette constante fixe un intervalle minimal (en millisecondes) entre deux déplacements du serpent. Elle permet de stabiliser sa vitesse, quelles que soient les performances de la machine ou la fluidité de l’affichage.

L’ajout de ce mécanisme permet d’éviter deux situations fréquentes :

- un déplacement trop rapide du serpent (liés à un taux de rafraîchissement élevé), rendant le jeu injouable,
- un comportement irrégulier si le nombre de FPS varie pendant l’exécution.

## Différence avec `time_between_frames`

Le tableau suivant met en évidence les rôles distincts de chaque temporisation :

`time_between_frames` limite la fréquence d’affichage en microsecondes et est lié au FPS, sans impact direct sur le gameplay, tandis que `SNAKE_MOVE_INTERVAL` définit la cadence de déplacement du serpent en millisecondes, influençant directement le gameplay.

## Mise en œuvre

Le système repose sur l’utilisation de `clock_gettime` et de structures `timespec`. À chaque boucle de jeu, le temps écoulé depuis le dernier déplacement est calculé. Si l’intervalle est dépassé, le serpent avance.

```c
double time_since_last_move = elapsed_ms(&last_move_time, &current_time);
if (time_since_last_move >= SNAKE_MOVE_INTERVAL) {
    // Déplacement du serpent
}
```
