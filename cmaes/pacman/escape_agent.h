#ifndef ALE_ESCAPE_AGENT_H
#define ALE_ESCAPE_AGENT_H

#include <common/Constants.h>
#include "pacman_image.h"
#include "constants.h"

class escape_agent {
private:
    double euclidean_distance(loc location1, loc location2);
    double ghost_cost(loc pacman_loc, loc ghost_loc);
    double corner_cost(loc pacman_loc);
    double teleport_cost(loc pacman_loc);
    double pellet_cost(loc pacman_loc, vector<loc> pellet_loc);

public:
    escape_agent();
    Action get_action(pacman_image p_image, vector<loc> object_locations, vector<loc> edible_ghosts);
};

#endif
