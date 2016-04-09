#ifndef ALE_TARGET_BASED_AGENT_H
#define ALE_TARGET_BASED_AGENT_H

#include "amidar_image.h"
#include "../src/common/Constants.h"
#include <queue>

enum agent_mode {
    ROAM,
    ESCAPE
};

class target_based_agent {
private:

    set<loc> ghost_locations;
    loc amidar_location;

    agent_mode amidar_mode;

    Action action_taken;

    set<loc> junctions;
    vector<vector<loc> > squares;

    queue<loc> targets;

    vector<loc> junction_neighbors(loc junction);
    loc nearest_unpainted_loc(const vector<vector<int> > &screen, loc amidar_loc);
    bool corner_section(loc location);
    pair<loc, loc> get_edge(loc location);
    vector<loc> get_square(pair<loc, loc> edge);

public:
    target_based_agent();
    Action get_action(amidar_image image, vector<vector<int> > &screen);
    void set_junctions(set<loc> junctions);
    void set_squares(vector<vector<loc> > squares);
    void update_amidar_location(loc amidar_loc);
    void update_ghost_locations(vector<loc> current_ghost_locations);
};

#endif
