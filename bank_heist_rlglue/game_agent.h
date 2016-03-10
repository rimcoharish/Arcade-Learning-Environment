#ifndef ALE__AGENT_H
#define ALE__AGENT_H

#include "../src/common/Constants.h"
#include "bank_heist_image.h"
#include "constants.h"

class game_agent {
private:
    double euclidean_distance(loc location1, loc location2);

public:
    game_agent();
    double bank_cost(loc heist_loc, vector<loc> bank_loc);
    Action get_action(bank_heist_image img);
};

#endif
