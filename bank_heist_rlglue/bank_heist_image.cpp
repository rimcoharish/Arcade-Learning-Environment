#include "bank_heist_image.h"
#include "constants.h"
using namespace std;

int BACKGROUND_COLOR = 117;
int MAZE_COLOR = 22;

bank_heist_image::bank_heist_image(){}

loc bank_heist_image::detect_loc(int low, int high) {
    double row_sum = 0, column_sum = 0;
    vector<double> column_values;
    int total_matches = 0;
    double tmp;
    for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            if (low <= screen[row][column] && screen[row][column] <= high) {
                tmp = min(tmp, double(column));
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
        assert(false);
    }
    pair<double, double> location = mp(row_sum / total_matches,
                                       column_sum / total_matches);
    return location;
}

void bank_heist_image::observe_colors() {
    BACKGROUND_COLOR = screen[13][0];
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
                banks.push_back(mp(row, column));
                // modify the 7x7 matrix
                for (size_t i = 0; i < 9; i++) {
                    for (size_t j = 0; j < 9; j++) {
                        tmp_screen[row+i-2][column+j-2] = 0;
                    }
                }
            }
        }
    }
    return banks;
}

// dir:- 1 = right; 0 = left
bool bank_heist_image::can_move_horizontally(int dir) {
  bool ret = true;
  int var = -1;
  if(dir == 1)
    var = 1;
  for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
      for (size_t column = 0; column < screen[row].size(); ++column) {
          if(screen[row][column] >= HEIST_COLOR - 2 && screen[row][column] <= HEIST_COLOR + 2) {
              if(screen[row][column+var] == MAZE_COLOR || screen[row][column+2*var] == MAZE_COLOR)
                  ret = false;
          }
      }
  }
  return ret;
}

// dir:- 1 = up; 0 = down
bool bank_heist_image::can_move_vertically(int dir) {
  bool ret = true;
  int var = 1;
  if(dir == 1)
    var = -1;
  for (int row = MAZE_START_X; row < MAZE_END_X; ++row) {
      for (size_t column = 0; column < screen[row].size(); ++column) {
        if(screen[row][column] >= HEIST_COLOR - 2 && screen[row][column] <= HEIST_COLOR + 2) {
              if(screen[row+var][column] == MAZE_COLOR || screen[row+2*var][column] == MAZE_COLOR) {
                ret = false;
              }
          }
      }
  }
  return ret;
}

vector<direction> bank_heist_image::get_valid_moves() {
    vector<direction> valid_moves;
    if (can_move_horizontally(0)) valid_moves.push_back(LEFT_DIR);
    if (can_move_horizontally(1)) valid_moves.push_back(RIGHT_DIR);
    if (can_move_vertically(1)) valid_moves.push_back(UP_DIR);
    if (can_move_vertically(0)) valid_moves.push_back(DOWN_DIR);
    return valid_moves;
}

void bank_heist_image::process_screen(vector<vector<int> > &screen) {
    this->screen = screen;
    observe_colors();
    print_image();
    this->heist_loc = detect_loc(HEIST_COLOR-1, HEIST_COLOR+1);
    // vector<loc> banks;
    banks_loc = detect_banks();
    cout << "HEIST: " << heist_loc.first << "," << heist_loc.second << endl;
    cout << "Banks: ";
    for (size_t i = 0; i < banks_loc.size(); i++) {
        cout << banks_loc[i].first << "," << banks_loc[i].second << " ";
    }
    cout << endl;
    cout << "MAZE_COLOR: " << MAZE_COLOR << "\n";
    cout << "BACKGROUND_COLOR: " << BACKGROUND_COLOR << "\n";
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
