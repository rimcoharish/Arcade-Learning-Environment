#include "game_agent.h"

game_agent::game_agent() {}
bool debug1 = false;

Action game_agent::get_action(bank_heist_image &img) {
  loc heist_location = img.heist_loc;
  vector<loc> banks = img.banks_loc;
  if(isnan(heist_location.first) || isnan(heist_location.second)) {
    if(debug1)
      cout << "Heist not present\n";
    return PLAYER_A_NOOP;
  }
  direction least_dist_dir;
  double tmp_cost = nearest_police(img, heist_location);
  // Escape agent
  if(tmp_cost < 20) {
    if(debug1)
      cout << "Escape oriented agent " << tmp_cost << "\n";
    least_dist_dir = escape(img);
  }
  else {
    if(debug1)
      cout << "Target oriented agent " << tmp_cost << "\n";
    double tmp_cost = 1000000;
    loc target = mp(MAZE_EXIT_X, MAZE_END_Y-1);
    for (size_t i = 0; i < banks.size(); i++) {
        if(img.banks_visited[i] != 0)
          continue;
        double tmp = img.euclidean_distance(heist_location, banks[i]);
        if(tmp <= 3) {
          cout << "Reached bank: " << i << "\n";
          int tmp = 0;
          for (size_t j = 0; j < NUM_BANKS; j++)
            if(tmp < img.banks_visited[j])
              tmp = img.banks_visited[j];
          img.banks_visited[i] = tmp+1;
          img.time_of_visit[i] = img.step;
          continue;
        }
        if(tmp_cost > tmp) {
            tmp_cost = tmp;
            target = banks[i];
        }
      }
    if(debug1)
      cout << "target: " << target.first << "," << target.second << "\n";
    least_dist_dir = move_to_target(img, target);
  }
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

  double dist = 1000000;
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
                next_loc.second = MAZE_START_Y;
            }
        }
        double dir_dist;
        if(target == img.banks_loc[0])
          dir_dist = img.dist1[round(next_loc.first)][round(next_loc.second)];
        else if(target == img.banks_loc[1])
          dir_dist = img.dist2[round(next_loc.first)][round(next_loc.second)];
        else if(target == img.banks_loc[2])
          dir_dist = img.dist3[round(next_loc.first)][round(next_loc.second)];
        else
          dir_dist = img.distance(next_loc, target);
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

double game_agent::nearest_police(bank_heist_image& img, loc location) {
  vector<loc> polices = img.police_loc;
  double least = 10000;
  for(size_t i=0; i<polices.size(); i++) {
    double tmp = img.euclidean_distance(location, polices[i]);
    if(tmp < 50)
      tmp = img.distance(location, polices[i]);
    if(tmp < least)
      least = tmp;
  }
  return least;
}

direction game_agent::escape(bank_heist_image& img) {
  loc heist_location = img.heist_loc;
  vector<direction> moves = img.get_valid_moves();

  double dist = -1;
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
        if(next_loc.first < 100)
          return RIGHT_DIR;
        else
          next_loc.second = MAZE_START_Y;
      }
    }
    double dir_dist = nearest_police(img, next_loc);
    if(debug1)
      cout << dir_dist << endl;
    if(dir_dist > dist) {
        dist = dir_dist;
        least_dist_dir = dir;
    }
  }
  if(debug1)
    cout << "direction: " << least_dist_dir << endl;
  return least_dist_dir;
}
