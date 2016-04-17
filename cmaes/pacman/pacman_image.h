#ifndef ALE_PACMAN_IMAGE_H
#define ALE_PACMAN_IMAGE_H

#include <vector>
#include <math.h>
#include <ale_interface.hpp>

using namespace std;

#define loc pair<double, double>
#define mp make_pair

enum direction {
    UP_DIR,
    DOWN_DIR,
    RIGHT_DIR,
    LEFT_DIR,
    NULL_DIR
};

class pacman_image {
private:
    vector<vector<int> > maze;
    vector<loc> orig_pellet_pos, pellet_pos;

public:
    // Constructors
    pacman_image();
    pacman_image(const vector<vector<int> > &maze);

    // Maze detect and getter functions
    void detect_maze_and_set(const ALEScreen &screen);
    vector<vector<int> >& get_maze(void);

    pair<double, double> detect_loc(const ALEScreen &screen, int low, int high);
    pair<double, double> detect_pacman_loc(const ALEScreen &screen);
    loc detect_ghost1_loc(const ALEScreen &screen);
    loc detect_ghost2_loc(const ALEScreen &screen);
    loc detect_ghost3_loc(const ALEScreen &screen);
    loc detect_ghost4_loc(const ALEScreen &screen);
    pair<double, double> detect_ghost_loc(const ALEScreen &screen, int ghost);
    vector<pair<double, double> > process_screen(const ALEScreen &screen);
    pair<bool, bool> can_move_horizontally(const loc pacman_location);
    pair<bool, bool> can_move_vertically(const loc pacman_location);
    vector<direction> get_valid_moves(const loc pacman_location);
    vector<loc> detect_edible_ghosts(const ALEScreen &screen);
    void update_pellet_pos(const ALEScreen &screen);
    vector<loc>& get_pellet_pos(void);
};

vector<vector<int> > detect_maze(const ALEScreen &screen);
vector<loc> detect_pellets(vector<vector<int> > maze);

#endif
