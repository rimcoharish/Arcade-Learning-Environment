#ifndef ALE_ESCAPE_AGENT_H
#define ALE_ESCAPE_AGENT_H

#include "amidar_image.h"
#include "../src/common/Constants.h"
#include "constants.h"

class escape_agent {
private:
    double ghost_cost(vector<vector<int> > &screen, loc pacman_loc, loc ghost_loc);
    double corner_cost(loc amidar_loc, int screen_height);

public:
    escape_agent();
    Action get_action(amidar_image image, vector<vector<int> > &screen);
};

#endif
