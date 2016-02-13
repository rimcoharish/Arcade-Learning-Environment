from constants import *
import numpy as np
from detect_maze import print_maze

extra_colors = []


class pacman_image(object):
	"""docstring for pacman_image"""
	def __init__(self, maze):
		self.maze = maze

	def detect_loc(self, screen, low, high):
		indices = np.where(np.logical_and(low <= screen, screen <= high))
		return np.mean(indices, axis=1)

	def detect_pacman_loc(self, screen):
		# detecting colors other than mentioned
		other_colors = np.array([4.0, 24.0, 38.0, 40.0, 42.0, 68.0, 70.0, 72.0, 74.0, 82.0, 88.0, 93.0, 102.0, 144.0, 182.0, 184.0])
		bool_vals = np.in1d(screen, other_colors, invert=True)
		colors = screen[np.reshape(bool_vals, (SCREEN_WIDTH, -1))]
		colors = np.sort(colors)
		pos, = np.where(np.diff(colors) != 0)
		pos = np.concatenate(([0], pos+1, [len(colors)]))
		rle = [(a, b, colors[a]) for (a, b) in zip(pos[:-1], pos[1:])]
		for x in rle:
			if x[2] not in extra_colors:
				extra_colors.append(x[2])

		return self.detect_loc(screen, PACMAN_LOW, PACMAN_HIGH)

	def detect_ghost1_loc(self, screen):
		return self.detect_loc(screen, GHOST1_LOW, GHOST1_HIGH)

	def detect_ghost2_loc(self, screen):
		return self.detect_loc(screen, GHOST2_LOW, GHOST2_HIGH)

	def detect_ghost3_loc(self, screen):
		return self.detect_loc(screen, GHOST3_LOW, GHOST3_HIGH)

	def detect_ghost4_loc(self, screen):
		return self.detect_loc(screen, GHOST4_LOW, GHOST4_HIGH)

	def new_image(self, screen):
		location_details = dict()
		location_details['pacman'] = self.detect_pacman_loc(screen)
		location_details['ghost1'] = self.detect_ghost1_loc(screen)
		location_details['ghost2'] = self.detect_ghost2_loc(screen)
		location_details['ghost3'] = self.detect_ghost3_loc(screen)
		location_details['ghost4'] = self.detect_ghost4_loc(screen)
		print location_details
		return location_details
