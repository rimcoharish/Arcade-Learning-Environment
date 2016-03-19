#include "game_agent.h"

game_agent::game_agent() {}
bool debug1 = false;

Action game_agent::get_action(bank_heist_image &img) {
    loc heist_location = img.heist_loc;
    vector<loc> banks = img.banks_loc;
    if(isnan(heist_location.first) || isnan(heist_location.second)) {
      cout << "Heist not present\n";
      return PLAYER_A_NOOP;
    }

    double tmp_cost = 1000000;
    loc target = mp(MAZE_EXIT_X, MAZE_END_Y-1);
    for (size_t i = 0; i < banks.size(); i++) {
        if(img.banks_visited[i])
          continue;
        double tmp = img.euclidean_distance(heist_location, banks[i]);
        if(tmp <= 5) {
          cout << "Reached bank: " << i << "\n";
          img.banks_visited[i]=true;
          continue;
        }
        if(tmp_cost > tmp) {
            tmp_cost = tmp;
            target = banks[i];
        }
    }
    if(debug1)
      cout << "target: " << target.first << "," << target.second << "\n";
    direction least_dist_dir = move_to_target(img, target);
    if (least_dist_dir == NULL_DIR) return PLAYER_A_NOOP;
    else if (least_dist_dir == UP_DIR) return PLAYER_A_UP;
    else if (least_dist_dir == DOWN_DIR) return PLAYER_A_DOWN;
    else if (least_dist_dir == LEFT_DIR) return PLAYER_A_LEFT;
    else if (least_dist_dir == RIGHT_DIR) return PLAYER_A_RIGHT;
    else return PLAYER_A_NOOP;
}

direction game_agent::move_to_target(bank_heist_image &img, loc target) {
  loc heist_location = img.heist_loc;
  vector<direction> moves = img.get_valid_moves();

  int dist = 1000000;
  direction least_dist_dir = NULL_DIR;
  for (size_t i = 0; i < moves.size(); i++) {
        direction dir = moves[i];
        if(debug1)
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
            if(next_loc.second < MAZE_START_Y)
              next_loc.second = MAZE_END_Y - 1;
        }
        else if (dir == RIGHT_DIR) {
            next_loc = make_pair(heist_location.first, heist_location.second + 1);
            if(next_loc.second >= MAZE_END_Y) {
              if(target == mp(double(MAZE_EXIT_X), double(MAZE_END_Y-1)))
                return RIGHT_DIR;
              else
                continue;
            }
        }
        int dir_dist = img.distance(next_loc, target);
        if(debug1)
          cout << dir_dist << endl;
        if(dir_dist - dist <= 0) {
            dist = dir_dist;
            least_dist_dir = dir;
        }
  }
  if(debug1)
    cout << "direction: " << least_dist_dir << endl;
  return least_dist_dir;
}
