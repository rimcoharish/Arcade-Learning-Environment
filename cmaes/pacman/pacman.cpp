/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare,
 *  Matthew Hausknecht, and the Reinforcement Learning and Artificial Intelligence
 *  Laboratory
 * Released under the GNU General Public License; see License.txt for details.
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 *  sharedLibraryInterfaceExample.cpp
 *
 *  Sample code for running an agent with the shared library interface.
 **************************************************************************** */

#include <iostream>
#include <ale_interface.hpp>

#include "pacman.h"
#include "constants.h"
#include "pacman_image.h"
#include "escape_agent.h"

#include <SDL.h>

using namespace std;

void update_object_locations(vector<loc> &object_locations, vector<loc> &prev_object_locations) {
    for (size_t i = 0; i < object_locations.size(); ++i) {
        if (std::isnan(object_locations[i].first) || std::isnan(object_locations[i].second)) {
            object_locations[i] = prev_object_locations[i];
        }
    }
}

void update_previous_locations(const vector<loc> &object_locations, vector<loc> &prev_object_locations) {
    for (size_t i = 0; i < object_locations.size(); ++i) {
        prev_object_locations[i] = object_locations[i];
    }
}

double simulate_episode(ALEInterface &ale) {
    double episode_reward = 0;
    pacman_image p_image;
    escape_agent esc_agent;
    vector<loc> prev_object_locations;
    while (!ale.game_over()) {
        if (ale.getEpisodeFrameNumber() == 0) {
            prev_object_locations.clear();
            for (int i = 0; i < 5; ++i) {
                loc tmp_loc = make_pair(NAN, NAN);
                prev_object_locations.push_back(tmp_loc);
            }
            p_image.detect_maze_and_set(ale.getScreen());
            episode_reward += ale.act(PLAYER_A_NOOP);
            continue;
        }
        vector<loc> object_locations = p_image.process_screen(ale.getScreen());
        p_image.update_pellet_pos(ale.getScreen());
        update_object_locations(object_locations, prev_object_locations);
        vector<loc> edible_ghosts = p_image.detect_edible_ghosts(ale.getScreen());
        Action action_val = esc_agent.get_action(p_image, object_locations, edible_ghosts);
        update_previous_locations(object_locations, prev_object_locations);
        episode_reward += ale.act(action_val);
    }
    return episode_reward;
}

double evaluate(string rom_file, int total_episodes, const double *params, int N) {
    assert(N == 5);
    GHOST_COST = params[0];
    CORNER_COST = params[1];
    TELE_COST = params[2];
    EDIBLE_GHOST_COST = params[3];
    PELLET_COST = params[4];

    ALEInterface ale;

    // Load the ROM file. (Also resets the system for new settings to
    // take effect.)
    ale.loadROM(rom_file);

    vector<double> rewards;
    double total_reward = 0;
    for (int episode = 0; episode < total_episodes; ++episode) {
        cout << "Episode: " << episode + 1 << " Reward: ";
        double episode_reward = simulate_episode(ale);
        rewards.push_back(episode_reward);
        total_reward += episode_reward;
        cout << episode_reward << " Steps: " << ale.getEpisodeFrameNumber() << endl;
        ale.reset_game();
    }
    double mean_reward = total_reward / total_episodes;
    cout << "Average reward for " << total_episodes << " episodes is " << mean_reward << endl;
    double sd = 0;
    for (int i = 0; i < rewards.size(); ++i) {
        sd += pow(rewards[i] - mean_reward, 2);
    }
    sd = sqrt(sd / (rewards.size() - 1));
    cout << "Standard deviation " << sd << endl;

    return mean_reward - (sd / sqrt(total_episodes));
}

/*int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
        return 1;
    }

    ALEInterface ale;

    // Load the ROM file. (Also resets the system for new settings to
    // take effect.)
    ale.loadROM(argv[1]);

    vector<double> rewards;
    double total_reward = 0;
    int total_episodes = 100;
    for (int episode = 0; episode < total_episodes; ++episode) {
        cout << "Episode: " << episode + 1 << " Reward: ";
        double episode_reward = simulate_episode(ale);
        rewards.push_back(episode_reward);
        total_reward += episode_reward;
        cout << episode_reward << " Steps: " << ale.getEpisodeFrameNumber() << endl;
        ale.reset_game();
    }
    double mean_reward = total_reward / total_episodes;
    cout << "Average reward for " << total_episodes << " episodes is " << mean_reward << endl;
    double sd = 0;
    for (int i = 0; i < rewards.size(); ++i) {
        sd += pow(rewards[i] - mean_reward, 2);
    }
    sd = sqrt(sd / (rewards.size() - 1));
    cout << "Standard deviation " << sd << endl;

    return 0;
}*/
