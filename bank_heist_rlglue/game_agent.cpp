#include "game_agent.h"

game_agent::game_agent() {}

double game_agent::euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) +
                pow(abs(location1.second - location2.second), 2));
}

Action game_agent::get_action(bank_heist_image img) {
    vector<direction> moves = img.get_valid_moves();
    loc heist_location = img.heist_loc;
    vector<loc> banks = img.banks_loc;

    double tmp_cost = 1000000;
    loc target = mp(MAZE_EXIT_X, MAZE_EXIT_Y);
    for (size_t i = 0; i < banks.size(); i++) {
        double tmp = euclidean_distance(heist_location, banks[i]);
        if(tmp_cost > tmp) {
            tmp_cost = tmp;
            target = banks[i];
        }
    }
    direction least_dist_dir = move_to_target(heist_location, target, moves);
    if (least_dist_dir == NULL_DIR) return PLAYER_A_NOOP;
    else if (least_dist_dir == UP_DIR) return PLAYER_A_UP;
    else if (least_dist_dir == DOWN_DIR) return PLAYER_A_DOWN;
    else if (least_dist_dir == LEFT_DIR) return PLAYER_A_LEFT;
    else if (least_dist_dir == RIGHT_DIR) return PLAYER_A_RIGHT;
    else return PLAYER_A_NOOP;
}

direction game_agent::move_to_target(loc heist_location, loc target, vector<direction> moves) {
  double dist = 1000000;
  direction least_dist_dir = NULL_DIR;
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
        double dir_dist = euclidean_distance(next_loc, target);
        if(dir_dist < dist) {
            dist = dir_dist;
            least_dist_dir = dir;
        }
  }
  cout << endl;
  return least_dist_dir;
}
