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
    agent_mode amidar_mode;
    loc target_loc;
    set<loc> junctions;
    loc paint_loc;
    vector<loc> junction_neighbors(loc junction);

public:
    target_based_agent();
    Action get_action(amidar_image image, vector<vector<int> > &screen);
    void set_junctions(set<loc> junctions);
};

#endif
