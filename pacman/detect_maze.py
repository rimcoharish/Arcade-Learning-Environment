import numpy as np
from constants import *

def print_maze(screen):
	for row in screen:
		for color in row:
			if color == 74:
				print 1,
			elif color == 144:
				print 0,
			else:
				print color,
		print

def detect_maze(screen):
	"""
	Given a screen of the game returns the position of maze(1), ghost pellet(2) and normal pellet(3)
	"""
	row_index = 0
	column_index = 0
	maze_pattern = np.empty((SCREEN_WIDTH, SCREEN_HEIGHT), dtype=np.int)
	maze_pattern.fill(-1)
	for row in screen:
		for color in row:
			if color == MAZE_COLOR:
				if row_index == 0 or column_index == 0: # Corner is always a maze pattern
					maze_pattern[row_index][column_index] = 1
				else:
					if maze_pattern[row_index - 1][column_index] == 1 or \
						maze_pattern[row_index][column_index - 1] == 1: # Continuation of maze
						maze_pattern[row_index][column_index] = 1
					elif maze_pattern[row_index - 1][column_index] == 2 or \
						maze_pattern[row_index][column_index - 1] == 2: # Continuation of ghost pellet
						maze_pattern[row_index][column_index] = 2
					elif maze_pattern[row_index - 1][column_index] == 3 or \
						maze_pattern[row_index][column_index - 1] == 3: # Continuation of normal pellet
						maze_pattern[row_index][column_index] = 3
					else:
						if screen[row_index][column_index + 4] == MAZE_COLOR: # Not any of the pellet
							maze_pattern[row_index][column_index] = 1
						elif screen[row_index + 2][column_index] == MAZE_COLOR: # Not normal pellet
							if screen[row_index + 7][column_index] == MAZE_COLOR: # Not ghost pellet
								maze_pattern[row_index][column_index] = 1
							else: # Ghost pellet top left corner encountered
								maze_pattern[row_index][column_index] = 2
						else: # Normal pellete top left corner encountered
							maze_pattern[row_index][column_index] = 3
			else:
				maze_pattern[row_index][column_index] = 0
			column_index += 1
		row_index += 1
		column_index = 0
	return maze_pattern
