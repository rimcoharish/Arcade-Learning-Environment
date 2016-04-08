#ifndef ALE_TARGET_BASED_AGENT_H
#define ALE_TARGET_BASED_AGENT_H

#include "amidar_image.h"
#include "../src/common/Constants.h"

enum agent_mode {
    ROAM,
    PAINT,
    ESCAPE
};

class target_based_agent {
private:

    set<loc> ghost_locations;
    loc amidar_location;

    agent_mode amidar_mode;

    loc target_loc;
    loc paint_loc;

    Action action_taken;

    set<loc> junctions;
    vector<loc> junction_neighbors(loc junction);

public:
    target_based_agent();
    Action get_action(amidar_image image, vector<vector<int> > &screen);
    void set_junctions(set<loc> junctions);
    void update_amidar_location(loc amidar_loc);
    void update_ghost_locations(vector<loc> current_ghost_locations);
};

#endif
