#ifndef ALE_CONSTANTS_H
#define ALE_CONSTANTS_H
// Maze related
const int MAZE_COLOR = 74;
const int SCREEN_HEIGHT = 210;
const int SCREEN_WIDTH = 160;
const int MAZE_HEIGHT = 210 - 38;

// Colours of pacman, ghosts
const int PACMAN_LOW = 41;
const int PACMAN_HIGH = 43;
const int GHOST1_LOW = 38;
const int GHOST1_HIGH = 38;
const int GHOST1_BLINK_LOW = 4;
const int GHOST1_BLINK_HIGH = 4;
const int GHOST2_LOW = 70;
const int GHOST2_HIGH = 70;
const int GHOST2_BLINK_LOW = 82;
const int GHOST2_BLINK_HIGH = 82;
const int GHOST3_LOW = 102;
const int GHOST3_HIGH = 102;
const int GHOST4_LOW = 181;
const int GHOST4_HIGH = 185;
const int EDIBLE_GHOST_LOW = 148;
const int EDIBLE_GHOST_HIGH = 150;
const int ghost_low[] = {GHOST1_LOW, GHOST2_LOW, GHOST3_LOW, GHOST4_LOW};
const int ghost_high[] = {GHOST1_HIGH, GHOST2_HIGH, GHOST3_HIGH, GHOST4_HIGH};

// Position of teleportation row
const int UPPER_TELEPORTATION_ROW = 56;
const int LOWER_TELEPORTATION_ROW = 103;

// Grape colour
const int GRAPE_LOW = 68;
const int GRAPE_HIGH = 68;

// Costs
const int GHOST_COST = 5000;
const int CORNER_COST = 1000;
const int TELE_COST = 1000;
const int EDIBLE_GHOST_COST = -5000;
const int PELLET_COST = -100;
#endif
