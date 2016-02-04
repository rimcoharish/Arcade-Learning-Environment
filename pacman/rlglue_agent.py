from rlglue.agent.Agent import Agent
from rlglue.types import Action
from rlglue.types import Observation
from rlglue.utils import TaskSpecVRLGLUE3
from rlglue.agent import AgentLoader
import copy
from random import randrange
from pacman_image import pacman_image
import numpy as np
from detect_maze import detect_maze


class pacmanAgent(Agent):
	numActions = 0
	lastAction = Action()
	lastObservation = Observation()
	image = None

	def agent_init(self, task_specification):
		task_spec = TaskSpecVRLGLUE3.TaskSpecParser(task_specification)
		print task_specification
		if task_spec.valid:
			self.numActions = task_spec.getIntActions()[0][1] + 1

		else:
			print "Task Spec could not be parsed: " + task_specification

		self.lastAction = Action()
		self.lastObservation = Observation()

	def agent_start(self, observation):
		screen = observation.intArray[128:]
		screen = np.reshape(screen, (210, -1))
		maze = detect_maze(screen)
		self.image = pacman_image(maze)
		return_action = Action()
		action = randrange(self.numActions)
		return_action.intArray = [action]
		self.lastAction = copy.deepcopy(return_action)
		self.lastObservation = copy.deepcopy(observation)

		return return_action

	def agent_step(self, reward, observation):
		screen = observation.intArray[128:]
		screen = np.reshape(screen, (210, -1))
		self.image.new_image(screen)
		return_action = Action()
		action = randrange(self.numActions)
		return_action.intArray = [action]
		self.lastAction=copy.deepcopy(return_action)
		self.lastObservation=copy.deepcopy(observation)

		return return_action

	def agent_end(self, reward):
		pass

	def agent_cleanup(self):
		pass

if __name__=="__main__":
	AgentLoader.loadAgent(pacmanAgent())