USE_SDL := 1

ALE := /home/rimcoharish/Arcade-Learning-Environment-0.5.1

FLAGS := -I$(ALE)/src -I$(ALE)/src/controllers -I$(ALE)/src/os_dependent -I$(ALE)/src/environment -I$(ALE)/src/external -L$(ALE)
CFLAGS := -Wall -pedantic
LDFLAGS := -lale -lz
CMAES_FLAGS := -I/usr/include/eigen3/ -I/home/rimcoharish/CMAES/include/libcmaes/ -L/home/rimcoharish/CMAES/lib
CMAES_CFLAGS := -fopenmp -std=c++11


ifeq ($(strip $(USE_SDL)), 1)
  DEFINES += -D__USE_SDL -DSOUND_SUPPORT
  FLAGS += $(shell sdl-config --cflags)
  LDFLAGS += $(shell sdl-config --libs)
endif

all: pacman

cmaes: pacman_image.o escape_agent.o constants.o pacman.o cmaes.o
	$(CXX) pacman_image.o escape_agent.o constants.o pacman.o cmaes.o -o cmaes $(DEFINES) $(FLAGS) $(CMAES_FLAGS) $(CFLAGS) $(CMAES_CFLAGS) $(LDFLAGS) -lcmaes

cmaes.o: cmaes.cpp
	g++ -fopenmp -std=c++11 -I/usr/include/eigen3/ -I/home/rimcoharish/CMAES/include/libcmaes/ -L/home/rimcoharish/CMAES/lib -lcmaes -c cmaes.cpp -o cmaes.o

pacman: pacman_image.o escape_agent.o constants.o pacman.o
	$(CXX) pacman_image.o escape_agent.o constants.o pacman.o -o pacman $(DEFINES) $(FLAGS) $(CFLAGS) $(LDFLAGS)

pacman.o: pacman.cpp
	$(CXX) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c pacman.cpp -o pacman.o

pacman_image.o: pacman_image.cpp
	$(CXX) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c pacman_image.cpp -o pacman_image.o

escape_agent.o: escape_agent.cpp
	$(CXX) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c escape_agent.cpp -o escape_agent.o

constants.o: constants.cpp
	$(CXX) $(CFLAGS) -c constants.cpp -o constants.o

clean:
	rm -rf cmaes pacman *.o