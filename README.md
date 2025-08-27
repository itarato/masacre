# Masacre

Silly top-view 2D shooting game.

Bugs:

- enemy spawning outside the last cell grid can be rounded to the next number - making path detection fail and cause
  immobility
- (similar) player close to edge/hit can get its cell-coords rounded to the dead zone - same result
- enemies can overlap each other
- player can get spawn (center) on a hit zone
- enemy/collectible can be spawned on a surrounded zone

Missing:

- moving smoke
- enemy kill particles
- player dying smoke
- player hit flash
- player died message
- minimap
- new enemy spawned should be at a minimum distance from the player
- player deploying landmines
- enemy should stop at a distance from the player