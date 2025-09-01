# Masacre

Silly top-view 2D shooting game.

![Screenshot](./misc/screenshot.png)

Bugs:

- global scope is getting fishy - try to replace it with something injectable

Todo:

- hurt particle generation is controlled by the particle group - so if the player gets hurt in each frame there will be a ton created - make it globally sourced (with repeated task)
- new enemy spawned should be at a minimum distance from the player
- player died message
- player deploying landmines
- enemy has health + rapid fire not being instant kill (but have smoke)
- enemy spawner zapping when close

Idea:

- destroying map (like in worms)
- enemy spawning from factories on a time interval
- enemy driver flying out of the tank
- laser sight (temporarily)