#ifndef ALE_CONSTANTS_H
#define ALE_CONSTANTS_H
const int MAZE_COLOR = 74;
const int SCREEN_HEIGHT = 210;
const int SCREEN_WIDTH = 160;
const int PACMAN_LOW = 41;
const int PACMAN_HIGH = 43;
const int GHOST1_LOW = 38;
const int GHOST1_HIGH = 38;
const int GHOST2_LOW = 69;
const int GHOST2_HIGH = 73;
const int GHOST3_LOW = 102;
const int GHOST3_HIGH = 102;
const int GHOST4_LOW = 181;
const int GHOST4_HIGH = 185;
const int TEL1 = 103;
const int ghost_low[] = {GHOST1_LOW, GHOST2_LOW, GHOST3_LOW, GHOST4_LOW};
const int ghost_high[] = {GHOST1_HIGH, GHOST2_HIGH, GHOST3_HIGH, GHOST4_HIGH};
const int GHOST_COST = 5000;
const int CORNER_COST = 5000;
const int TELE_COST = 1000;
#endif
