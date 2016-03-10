#include "game_agent.h"

game_agent::game_agent() {}

double game_agent::euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) + pow(abs(location1.second - location2.second), 2));
}

double game_agent::bank_cost(loc heist_loc, vector<loc> bank_loc) {
    double cost = 0.0;
    for(int i=0; i<bank_loc.size(); i++) {
        cost += BANK_COST / euclidean_distance(heist_loc, bank_loc[i]);
    }
    // cout << "Ghost: " << cost << ", ";
    return cost;
}

Action game_agent::get_action(bank_heist_image img) {
    vector<direction> moves = img.get_valid_moves();
    loc heist_location = img.heist_loc;
    direction least_cost_dir = NULL_DIR;
    double cost = 1000000;
    for (size_t i = 0; i < moves.size(); i++) {
          direction dir = moves[i];
          cout << dir << " ";
          loc next_loc;
          if (dir == UP_DIR) {
              next_loc = make_pair(heist_location.first - 1, heist_location.second);
          }
          else if (dir == DOWN_DIR) {
              next_loc = make_pair(heist_location.first + 1, heist_location.second);
          }
          else if (dir == LEFT_DIR) {
              next_loc = make_pair(heist_location.first, heist_location.second - 1);
          }
          else if (dir == RIGHT_DIR) {
              next_loc = make_pair(heist_location.first, heist_location.second + 1);
          }
          double dir_cost = 0;
          dir_cost += bank_cost(next_loc, img.banks_loc);
          if(dir_cost < cost) {
              cost = dir_cost;
              least_cost_dir = dir;
          }
    }
    cout << endl;
    if (least_cost_dir == NULL_DIR) return PLAYER_A_NOOP;
    else if (least_cost_dir == UP_DIR) return PLAYER_A_UP;
    else if (least_cost_dir == DOWN_DIR) return PLAYER_A_DOWN;
    else if (least_cost_dir == LEFT_DIR) return PLAYER_A_LEFT;
    else if (least_cost_dir == RIGHT_DIR) return PLAYER_A_RIGHT;
    else return PLAYER_A_NOOP;
}
