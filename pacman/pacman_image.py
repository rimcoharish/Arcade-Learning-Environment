from constants import *

class pacman_image(object):
	"""docstring for pacman_image"""
	def __init__(self, maze):
		self.maze = maze

	def detect_loc(self, screen, low, high):
		row_index = 0
		column_index = 0
		total_matches = 0.0
		row_sum = 0
		column_sum = 0
		for row in screen:
			for color in row:
				if color >= low and color <= high:
					row_sum += row_index
					column_sum += column_index
					total_matches += 1
				column_index += 1
			row_index += 1
		return (row_sum / total_matches, column_sum / total_matches)

	def detect_pacman_loc(self, screen):
		return self.detect_loc(screen, PACMAN_LOW, PACMAN_HIGH)

	def new_image(self, screen):
		print self.detect_pacman_loc(screen)
