#!/usr/bin/env python
# python_example.py
# Author: Ben Goodrich
#
# This is a direct port to python of the shared library example from
# ALE provided in doc/examples/sharedLibraryInterfaceExample.cpp
import sys
from random import randrange
from ale_python_interface import ALEInterface
import numpy as np
from detect_maze import detect_maze, print_maze

if len(sys.argv) < 2:
  print 'Usage:', sys.argv[0], 'rom_file'
  sys.exit()

ale = ALEInterface()

# Get & Set the desired settings
ale.setInt('random_seed', 123)

# Set USE_SDL to true to display the screen. ALE must be compilied
# with SDL enabled for this to work. On OSX, pygame init is used to
# proxy-call SDL_main.
USE_SDL = True
if USE_SDL:
  if sys.platform == 'darwin':
    import pygame
    pygame.init()
    ale.setBool('sound', False) # Sound doesn't work on OSX
  elif sys.platform.startswith('linux'):
    ale.setBool('sound', True)
  ale.setBool('display_screen', True)

# Load the ROM file
ale.loadROM(sys.argv[1])

# Get the list of legal actions
legal_actions = ale.getLegalActionSet()

# Play 10 episodes
screen = np.reshape(ale.getScreen(), (210, -1))
maze = detect_maze(screen)
# print_maze(maze)

for episode in xrange(1):
  total_reward = 0
  step = 1
  while not ale.game_over():
    if step == 500:
      screen = np.reshape(ale.getScreen(), (210, -1))
      print_maze(screen)
      break
    step += 1
    # print step
    a = legal_actions[randrange(len(legal_actions))]
    # Apply an action and get the resulting reward
    reward = ale.act(a);
    total_reward += reward
  print 'Episode', episode, 'ended with score:', total_reward
  ale.reset_game()
