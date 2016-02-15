#ifndef ALE_PACMAN_IMAGE_H
#define ALE_PACMAN_IMAGE_H

#include <vector>
#include <math.h>

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
    void detect_maze_and_set(const vector<vector<int> > &screen);
    vector<vector<int> >& get_maze(void);

    pair<double, double> detect_loc(const vector<vector<int> > &screen, int low, int high);
    pair<double, double> detect_pacman_loc(const vector<vector<int> > &screen);
    loc detect_ghost1_loc(const vector<vector<int> > &screen);
    loc detect_ghost2_loc(const vector<vector<int> > &screen);
    loc detect_ghost3_loc(const vector<vector<int> > &screen);
    loc detect_ghost4_loc(const vector<vector<int> > &screen);
    pair<double, double> detect_ghost_loc(const vector<vector<int> > &screen, int ghost);
    vector<pair<double, double> > process_screen(const vector<vector<int> > &screen);
    pair<bool, bool> can_move_horizontally(const loc pacman_location);
    pair<bool, bool> can_move_vertically(const loc pacman_location);
    vector<direction> get_valid_moves(const loc pacman_location);
    vector<loc> detect_edible_ghosts(const vector<vector<int> > &screen);
    void update_pellet_pos(const vector<vector<int> > &screen);
    vector<loc>& get_pellet_pos(void);
};

vector<vector<int> > detect_maze(const vector<vector<int> > &screen);
vector<loc> detect_pellets(vector<vector<int> > maze);

#endif
