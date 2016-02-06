from constants import *
import numpy as np


class pacman_image(object):
	"""docstring for pacman_image"""
	def __init__(self, maze):
		self.maze = maze

	def detect_loc(self, screen, low, high):
		indices = np.where(np.logical_and(low <= screen, screen <= high))
		mean_pos = []
		for coordinates in indices:
			mean_pos.append(np.mean(coordinates))
		return mean_pos

	def detect_pacman_loc(self, screen):
		return self.detect_loc(screen, PACMAN_LOW, PACMAN_HIGH)

	def new_image(self, screen):
		print self.detect_pacman_loc(screen)
