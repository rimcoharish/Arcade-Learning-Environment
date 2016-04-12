#ifndef ALE_AMIDAR_IMAGE_H
#define ALE_AMIDAR_IMAGE_H

#include <vector>
#include <math.h>
#include <set>

using namespace std;

typedef pair<int, int> loc;

enum direction {
    UP_DIR,
    DOWN_DIR,
    RIGHT_DIR,
    LEFT_DIR,
    NULL_DIR
};

class amidar_image {
public:
    loc detect_amidar(const vector<vector<int> > &screen);
    vector<loc> detect_ghosts(const vector<vector<int> > &screen);
    pair<bool, bool> can_move_horizontally(const loc amidar_location, vector<vector<int> > &maze);
    pair<bool, bool> can_move_vertically(const loc amidar_location, vector<vector<int> > &maze);
    vector<direction> get_valid_moves(const loc amidar_location, vector<vector<int> > &maze);
    set<loc> detect_junctions(const vector<vector<int> > &screen);
    vector<vector<loc> > construct_squares(set<loc> &junctions);
    bool check_amidar_intact(loc amidar_location, const vector<vector<int> > &screen);
};

#endif
