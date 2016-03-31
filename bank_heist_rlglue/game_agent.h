#ifndef ALE__AGENT_H
#define ALE__AGENT_H

#include "../src/common/Constants.h"
#include "bank_heist_image.h"
#include "constants.h"

class game_agent {
public:
    game_agent();
    Action get_action(bank_heist_image &img);
    direction move_to_target(bank_heist_image &img, loc target);
    double nearest_police(bank_heist_image& img, loc location);
    direction escape(bank_heist_image& img);
};

#endif
