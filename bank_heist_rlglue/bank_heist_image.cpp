#include "bank_heist_image.h"
#include "constants.h"
using namespace std;

int BACKGROUND_COLOR = 117;
int MAZE_COLOR = 0;
bool debug = false;

bank_heist_image::bank_heist_image(){
  banks_visited.push_back(0);
  banks_visited.push_back(0);
  banks_visited.push_back(0);
  time_of_visit.push_back(0);
  time_of_visit.push_back(0);
  time_of_visit.push_back(0);
}

loc bank_heist_image::detect_loc(int low, int high) {
    double row_sum = 0, column_sum = 0;
    vector<double> column_values;
    int total_matches = 0;
    for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            if (low <= screen[row][column] && screen[row][column] <= high) {
                total_matches++;
                row_sum += row;
                column_sum += column;
                column_values.push_back(column);
            }
        }
    }
    double column_mean = column_sum / column_values.size();

    double variance = 0, sd = 0;
    for (size_t i = 0; i < column_values.size(); ++i) {
        variance += pow(column_values[i] - column_mean, 2);
    }
    variance /= column_values.size();
    sd = sqrt(variance);
    //    When the S.D is high => Something wrong
    if(sd > 10) {
        return mp(row_sum / total_matches, MAZE_END_Y-1);
        // assert(false);
    }
    pair<double, double> location = mp(row_sum / total_matches,
                                       column_sum / total_matches);
    return location;
}

void bank_heist_image::observe_colors() {
    BACKGROUND_COLOR = screen[13][0];
    prev_maze_color = MAZE_COLOR;
    MAZE_COLOR = screen[MAZE_START_X][MAZE_START_Y];
}

vector<loc> bank_heist_image::detect_banks() {
    if(BANK_COLOR == BACKGROUND_COLOR) {
        return banks_loc;
    }
    vector<loc> banks;
    vector<vector<int> > tmp_screen = screen;
    for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
        for (size_t column = 0; column < tmp_screen[row].size(); ++column) {
            if(tmp_screen[row][column] == BANK_COLOR) {
                loc bank;
                // modify the 7x7 matrix
                for (size_t i = 0; i < 9; i++) {
                    for (size_t j = 0; j < 9; j++) {
                        if(tmp_screen[row+i-2][column+j-2] == BANK_COLOR &&
                           heist_loc_possible(mp(row+i-2, column+j-2)))
                          bank = mp(row+i-2, column+j-2);
                        tmp_screen[row+i-2][column+j-2] = 0;
                    }
                }
                banks.push_back(bank);
            }
        }
    }
    return banks;
}

vector<loc> bank_heist_image::detect_polices() {
  vector<loc> polices;
  vector<vector<int> > tmp_screen = screen;
  for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
      for (size_t column = 0; column < tmp_screen[row].size(); ++column) {
          if(tmp_screen[row][column] == POLICE_COLOR_1 || tmp_screen[row][column] == POLICE_COLOR_2) {
              loc police=mp(row+2, column+2);
              // modify the 7x7 matrix
              for (size_t i = 0; i < 11; i++) {
                  for (size_t j = 0; j < 11; j++) {
                    if((tmp_screen[row+i-3][column+j-3] == POLICE_COLOR_1 ||
                        tmp_screen[row+i-3][column+j-3] == POLICE_COLOR_2) &&
                       heist_loc_possible(mp(row+i-3, column+j-3)))
                       police = mp(row+i-3, column+j-3);
                      tmp_screen[row+i-3][column+j-3] = 0;
                  }
              }
              polices.push_back(police);
          }
      }
  }
  int num_banks_existing = 0;
  for (size_t i = 0; i < NUM_BANKS; i++)
    if(banks_visited[i] == 0)
      num_banks_existing++;

  int max_index, second_max_index, min_index;
  if(polices.size() + num_banks_existing < NUM_BANKS) {
    int num_add = NUM_BANKS - polices.size() - num_banks_existing;
    if(debug)
      cout << "Adding " << num_add << " police\n";
    if(num_add >= 1) {
      max_index = max_element(banks_visited.begin(), banks_visited.end()) - banks_visited.begin();
      if(step - time_of_visit[max_index] > 70 && step - time_of_visit[max_index] < 160) {
        if(debug) {
          cout << "Difference is " << step - time_of_visit[max_index] << "\n";
          cout << max_index << " " << step << " " << time_of_visit[max_index] << endl;
        }
        polices.push_back(banks_loc[max_index]);
      }
    }
    if(num_add == 2) {
      int tmp = 0;
      swap(tmp, banks_visited[max_index]);
      second_max_index = max_element(banks_visited.begin(), banks_visited.end()) - banks_visited.begin();
      if(debug) {
        cout << "Difference is " << step - time_of_visit[second_max_index] << "\n";
        cout << second_max_index << " " << step << " " << time_of_visit[second_max_index] << endl;
      }
      swap(tmp, banks_visited[max_index]);
      if(step - time_of_visit[second_max_index] > 70 && step - time_of_visit[max_index] < 160)
        polices.push_back(banks_loc[second_max_index]);
    }
    if(num_add == 3) {
      min_index = min_element(banks_visited.begin(), banks_visited.end()) - banks_visited.begin();
      if(step - time_of_visit[min_index] > 70 && step - time_of_visit[max_index] < 160)
        polices.push_back(banks_loc[min_index]);
    }
  }

  return polices;
}

bool bank_heist_image::is_heist(int row, int column) {
  return (screen[row][column] >= HEIST_COLOR - 2 && screen[row][column] <= HEIST_COLOR + 2);
}

// dir:- 1 = right; 0 = left
bool bank_heist_image::can_move_horizontally(loc heist, int dir) {
  int row = round(heist.first);
  int column = round(heist.second);
  int left = 0, right = 0;
  while (left + right < 11) {
      if (screen[row][column - left] == MAZE_COLOR && screen[row][column + right] == MAZE_COLOR) break;
      else if (screen[row][column - left] == MAZE_COLOR) right++;
      else if (screen[row][column + right] == MAZE_COLOR) left++;
      else {
          if ((left + right) % 2 == 0) left++;
          else right++;
      }
  }
  if(dir == 1)
    return screen[row][column + right] != MAZE_COLOR;
  else
    return screen[row][column - left] != MAZE_COLOR;
}

// dir:- 1 = up; 0 = down
bool bank_heist_image::can_move_vertically(loc heist, int dir) {
  int row = round(heist.first);
  int column = round(heist.second);
  int up = 0, down = 0;
  while (up + down < 11) {
      if (screen[row - up][column] == MAZE_COLOR && screen[row + down][column] == MAZE_COLOR) break;
      else if (screen[row - up][column] == MAZE_COLOR) down++;
      else if (screen[row + down][column] == MAZE_COLOR) up++;
      else {
          if ((down + up) % 2 == 0) up++;
          else down++;
      }
  }
  if(dir == 0)
    return screen[row + down][column] != MAZE_COLOR;
  else
    return screen[row - up][column] != MAZE_COLOR;
}

vector<direction> bank_heist_image::get_valid_moves() {
    vector<direction> valid_moves;
    if (can_move_horizontally(heist_loc, 0)) valid_moves.push_back(LEFT_DIR);
    if (can_move_horizontally(heist_loc, 1)) valid_moves.push_back(RIGHT_DIR);
    if (can_move_vertically(heist_loc, 1)) valid_moves.push_back(UP_DIR);
    if (can_move_vertically(heist_loc, 0)) valid_moves.push_back(DOWN_DIR);
    return valid_moves;
}

bool bank_heist_image::heist_loc_possible(loc heist) {
  // check in 7x7 square with the heist location as center that there is no maze
  for(int i=heist.first-3; i<=heist.first+3; i++)
    for(int j=heist.second-3; j<=heist.second+3; j++)
      if(screen[i][j] == MAZE_COLOR)
        return false;
  return true;
}

void bank_heist_image::process_screen(vector<vector<int> > &scr) {
    step++;
    screen = scr;
    for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            if(is_heist(row, column+1) && is_heist(row, column-1)) {
                screen[row][column] = HEIST_COLOR;
            }
        }
    }
    observe_colors();
    if(prev_maze_color != MAZE_COLOR)
      analyze_maze();
    police_loc = detect_polices();
    this->heist_loc = detect_loc(HEIST_COLOR-2, HEIST_COLOR+2);
    if(debug) {
      print_image();
      cout << "HEIST: " << heist_loc.first << "," << heist_loc.second << endl;
      cout << "Banks: ";
      for (size_t i = 0; i < banks_loc.size(); i++)
        if(banks_visited[i]==0)
          cout << banks_loc[i].first << "," << banks_loc[i].second << " ";
      cout << endl;
      cout << "Polices: ";
      for (size_t i = 0; i < police_loc.size(); i++)
          cout << police_loc[i].first << "," << police_loc[i].second << " ";
      cout << endl;
      cout << "MAZE_COLOR: " << MAZE_COLOR << "\n";
      cout << "BACKGROUND_COLOR: " << BACKGROUND_COLOR << "\n";
    }
}

// The first time a new maze is encountered
void bank_heist_image::analyze_maze() {
    cout << "New maze\n";
    step = 0;
    banks_loc = detect_banks();
    if(banks_loc.size() == 3) {
      dist1 = complete_dfs(banks_loc[0]);
      dist2 = complete_dfs(banks_loc[1]);
      dist3 = complete_dfs(banks_loc[2]);
      dist4 = complete_dfs(mp(MAZE_EXIT_X, MAZE_END_Y-1));
    }
    banks_visited[0] = 0;
    banks_visited[1] = 0;
    banks_visited[2] = 0;
    time_of_visit[0] = 0;
    time_of_visit[1] = 0;
    time_of_visit[2] = 0;
}

void bank_heist_image::print_image() {
    ofstream my_file;
    my_file.open("image.txt", ofstream::out);
    for (size_t row = 0; row < screen.size(); ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            int tmp = screen[row][column];
            if(tmp == MAZE_COLOR)
                tmp = 1;
            else if (tmp == BACKGROUND_COLOR) tmp = 0;
            my_file << tmp << " ";
        }
        my_file << endl;
    }
    my_file.close();
}

// Distance b/w source and destination using bfs on the game image
double bank_heist_image::distance(loc source, loc destination) {
  source.first = round(source.first);
  source.second = round(source.second);
  destination.first = round(destination.first);
  destination.second = round(destination.second);

  vector<vector<bool> > visited(SCREEN_HEIGHT, vector<bool>(SCREEN_WIDTH, false));
  vector<vector<double> > dist(SCREEN_HEIGHT, vector<double>(SCREEN_WIDTH, 1000000));
  dist[source.first][source.second] = 0;

  queue<loc> locations;
  locations.push(source);
  while(!locations.empty()) {
    loc current_loc = locations.front();
    locations.pop();
    // cout << "current_loc " << current_loc.first << "," << current_loc.second << "\n";
    // cout << screen[current_loc.first][current_loc.second] << "\n";
    if(current_loc.first < MAZE_START_X || current_loc.first >= MAZE_END_X ||
       current_loc.second < MAZE_START_Y || current_loc.second >= MAZE_END_Y)
       exit(-1);
    if(visited[current_loc.first][current_loc.second])
      continue;
    visited[current_loc.first][current_loc.second] = true;
    if(current_loc == destination) {
      if(false) {
        ofstream my_file;
        my_file.open("distance.txt", ofstream::out);
        for (size_t row = 0; row < screen.size(); ++row) {
            for (size_t column = 0; column < screen[row].size(); ++column) {
                double tmp = dist[row][column];
                if(tmp == 1000000)
                    tmp = -1;
                my_file << tmp << " ";
            }
            my_file << endl;
        }
        my_file.close();
      }
      return dist[current_loc.first][current_loc.second];
    }
    vector<loc> adjacent_locations = next_loc(current_loc);
    for(size_t i=0; i<adjacent_locations.size(); i++) {
      if(!visited[adjacent_locations[i].first][adjacent_locations[i].second]) {
        double new_dist = dist[current_loc.first][current_loc.second] + 1;
        if(new_dist < dist[adjacent_locations[i].first][adjacent_locations[i].second]) {
          dist[adjacent_locations[i].first][adjacent_locations[i].second] = new_dist;
          locations.push(adjacent_locations[i]);
        }
      }
    }
  }
  if(debug) {
    ofstream my_file;
    my_file.open("distance.txt", ofstream::out);
    for (size_t row = 0; row < screen.size(); ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            double tmp = dist[row][column];
            if(tmp == 1000000 || tmp ==0)
                tmp = 0;
            my_file << tmp << " ";
        }
        my_file << endl;
    }
    my_file.close();
    cout << "Destination " << destination.first << "," << destination.second << " unreachable\n";
  }
  return euclidean_distance(source, destination);
}

vector<vector<double> > bank_heist_image::complete_dfs(loc source) {
  source.first = round(source.first);
  source.second = round(source.second);

  vector<vector<bool> > visited(SCREEN_HEIGHT, vector<bool>(SCREEN_WIDTH, false));
  vector<vector<double> > dist(SCREEN_HEIGHT, vector<double>(SCREEN_WIDTH, 1000000));
  dist[source.first][source.second] = 0;

  queue<loc> locations;
  locations.push(source);
  while(!locations.empty()) {
    loc current_loc = locations.front();
    locations.pop();
    // cout << "current_loc " << current_loc.first << "," << current_loc.second << "\n";
    // cout << screen[current_loc.first][current_loc.second] << "\n";
    if(current_loc.first < MAZE_START_X || current_loc.first >= MAZE_END_X ||
       current_loc.second < MAZE_START_Y || current_loc.second >= MAZE_END_Y)
       exit(-1);
    if(visited[current_loc.first][current_loc.second])
      continue;
    visited[current_loc.first][current_loc.second] = true;
    vector<loc> adjacent_locations = next_loc(current_loc);
    for(size_t i=0; i<adjacent_locations.size(); i++) {
      if(!visited[adjacent_locations[i].first][adjacent_locations[i].second]) {
        double new_dist = dist[current_loc.first][current_loc.second] + 1;
        if(new_dist < dist[adjacent_locations[i].first][adjacent_locations[i].second]) {
          dist[adjacent_locations[i].first][adjacent_locations[i].second] = new_dist;
          locations.push(adjacent_locations[i]);
        }
      }
    }
  }
  return dist;
}

// Adjacent locations for the given point
// Teleportation considered.
vector<loc> bank_heist_image::next_loc(loc location) {
  vector<loc> ret;
  loc down=mp(location.first+1, location.second);
  if(heist_loc_possible(down)) {
    ret.push_back(down);
  }

  loc up=mp(location.first-1, location.second);
  if(heist_loc_possible(up)) {
    ret.push_back(up);
  }

  loc left=mp(location.first, location.second-1);
  if(left.second < MAZE_START_Y)
    left.second = MAZE_END_Y-1;
  if(heist_loc_possible(left)) {
    ret.push_back(left);
  }

  loc right=mp(location.first, location.second+1);
  if(right.second >= MAZE_END_Y) {
    if(right.first < 100)
      return ret;
    else
      right.second = MAZE_START_Y;
  }
  if(heist_loc_possible(right)) {
    ret.push_back(right);
  }
  return ret;
}

// double bank_heist_image::police_cost(loc location) {
//   double cost = 0.0;
//   for (size_t i = 0; i < police_loc.size(); i++) {
//     if(location == police_loc[i])
//       cost += 100;
//     else
//       cost += 10 / euclidean_distance(location, police_loc[i]);
//   }
//   return cost;
// }

double bank_heist_image::euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) +
           pow(abs(location1.second - location2.second), 2));
}
