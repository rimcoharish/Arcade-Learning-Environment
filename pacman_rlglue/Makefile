# Modified from the C/C++ RL-Glue codec RLGlueAgent's makefile.

AGENTLDFLAGS := -lm -lrlutils -lrlagent -lrlgluenetdev
EXPLDFLAGS := -lrlutils -lrlexperiment -lrlgluenetdev
CFLAGS := -Wall -pedantic -std=c++11

all: cmaes
	AGENTLDFLAGS="$(AGENTLDFLAGS)" CFLAGS="$(CFLAGS)" make -f Makefile.agent
	EXPLDFLAGS="$(EXPLDFLAGS)" CFLAGS="$(CFLAGS)"  make -f Makefile.experiment

cmaes: cmaes.cpp
	g++ -fopenmp -std=gnu++11 -I/usr/include/eigen3/ -I/home/rimcoharish/CMAES/include/libcmaes/ -L/home/rimcoharish/CMAES/lib -o cmaes cmaes.cpp -lcmaes

clean:
	make -f Makefile.agent  clean
	make -f Makefile.experiment clean
	rm -f cmaes
