#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include "pacman_image.h"
#include "constants.h"
#include "../src/common/Constants.h"

using namespace std;

int num_actions = 0;
int max_action = 17;
double total_reward = 0;
action_t action;

pacman_image p_image;

int randInRange(int max){
    double r, x;
    r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
    x = (r * (max+1));
    return (int)x;
}

double euclidean_distance(loc location1, loc location2) {
    return sqrt(pow(abs(location1.first - location2.first), 2) + pow(abs(location1.second - location2.second), 2));
}

double ghost_cost(loc pacman_loc, loc ghost_loc) {
    return GHOST_COST / euclidean_distance(pacman_loc, ghost_loc);
}

double corner_cost(loc pacman_loc) {
    if (pacman_loc.first > SCREEN_HEIGHT / 2) pacman_loc.first = SCREEN_HEIGHT - pacman_loc.first;
    if (pacman_loc.second > SCREEN_WIDTH / 2) pacman_loc.second = SCREEN_WIDTH - pacman_loc.second;
    if (pacman_loc.first < 1 && pacman_loc.second < 1) return CORNER_COST;
    else return CORNER_COST / euclidean_distance(pacman_loc, make_pair(0, 0));
}

Action escape_agent(pacman_image p_image, vector<vector<int> > screen) {
    vector<loc> object_locations = p_image.process_screen(screen);
    loc pacman_loc = object_locations[0];
    if (!isnan(pacman_loc.first) && !isnan(pacman_loc.second)){
        vector<direction> valid_moves = p_image.get_valid_moves(pacman_loc);
        direction least_cost_dir = NULL_DIR;
        double cost = GHOST_COST * 4 + CORNER_COST;
        for (int i = 0; i < valid_moves.size(); ++i) {
            direction dir = valid_moves[i];
            loc next_pacman_loc;
            if (dir == UP_DIR) {
                cout << "up ";
                next_pacman_loc = make_pair(pacman_loc.first - 1, pacman_loc.second);
            }
            else if (dir == DOWN_DIR) {
                cout << "down ";
                next_pacman_loc = make_pair(pacman_loc.first + 1, pacman_loc.second);
            }
            else if (dir == LEFT_DIR) {
                cout << "left ";
                next_pacman_loc = make_pair(pacman_loc.first, pacman_loc.second - 1);
            }
            else if (dir == RIGHT_DIR) {
                cout << "right ";
                next_pacman_loc = make_pair(pacman_loc.first, pacman_loc.second + 1);
            }
            double dir_cost = corner_cost(next_pacman_loc);
            cout << dir_cost << " dir_cost, ";
            for (int ghost = 0; ghost < 4; ++ghost) {
                double c = ghost_cost(next_pacman_loc, object_locations[ghost + 1]);
                dir_cost += (isnan(c) ? 0: c);
            }
            cout << dir_cost << " total_cost, ";
            if (dir_cost < cost) {
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
    }
    else return PLAYER_A_NOOP;
}

void agent_init(const char* task_spec_string) {
    taskspec_t* task_spec = new taskspec_t;
    int decode_status = decode_taskspec(task_spec, task_spec_string);
    if (decode_status != 0) {
        cout << "Not able to parse task specification" << endl;
        cout << task_spec_string << endl;
        exit(1);
    }
    else {
        srand(time(0));
        allocateRLStruct(&action, 1, 0, 0);
    }
}

const action_t* agent_start(const observation_t* observation) {
    int screen_start_index = 128;
    int offset = 0;
    vector<vector<int> > screen;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<int> pixel_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            pixel_row.push_back(observation->intArray[screen_start_index + offset]);
            offset++;
        }
        screen.push_back(pixel_row);
    }
    /* printing maze pattern
    for (int row = 0; row < maze.size(); ++row) {
        for (int column = 0; column < maze[row].size(); ++column) {
            cout << maze[row][column] << " ";
        }
        cout << endl;
    }
    */
    p_image.detect_maze_and_set(screen);
    int action_val = randInRange(max_action);
    action.intArray[0] = action_val;
    return &action;
}

const action_t* agent_step(double reward, const observation_t* observation) {
    int screen_start_index = 128;
    int offset = 0;
    vector<vector<int> > screen;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<int> pixel_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            pixel_row.push_back(observation->intArray[screen_start_index + offset]);
            offset++;
        }
        screen.push_back(pixel_row);
    }
    total_reward += reward;
    int action_val = escape_agent(p_image, screen);
    action.intArray[0] = action_val;
    return &action;
}

void agent_end(double reward) {
    total_reward += reward;
    cout << "Total reward obtained " << total_reward << endl;
    clearRLStruct(&action);
}

void agent_cleanup() {
    clearRLStruct(&action);
}

const char* agent_message(const char* message) {
    cout << "Message received: " << message << endl;
    return "Message received";
}