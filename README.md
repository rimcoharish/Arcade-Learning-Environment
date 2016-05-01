<img align="right" src="doc/manual/figures/ale.gif" width=50>

# Arcade-Learning-Environment - Maze Games
## CS748 Project
  
The agents coded for the project use RL-Glue interface to communicate with the ALE.  

  Ms. Pac-man RL-Glue folder - [pacman_rlglue] (pacman_rlglue)  
  Bank Heist RL-Glue folder - [bank_heist_rlglue] (bank_heist_rlglue)  
  Amidar RL-Glue folder - [amidar] (amidar)
  
##Installation instructions

Install RL-Glue core from [here] (https://code.google.com/archive/p/rl-glue-ext/wikis/RLGlueCore.wiki)  
Install RL-Glue C/C++ codec from [here] (https://code.google.com/archive/p/rl-glue-ext/wikis/CandCPP.wiki)  
  
1) Compilation of Environment (ALE)  
   RL Glue core and C/C++ codec must be installed on the system (links given above)  
   > sudo apt-get install cmake libsdl1.2-dev  
   > cd \<project_root_folder\>  
   > cmake -DUSE_SDL=ON -DUSE_RLGLUE=ON -DBUILD_EXAMPLES=ON  
   > \# ALE needs to be compiled with -DUSE_RLGLUE = ON to use rlglue controller  
   > make -j 4  
   
   For further details, check the [Manual for ALE] (doc/manual/manual.pdf)
  
2) Compiling the Agent and the Experiment  
   Take the example of Ms. Pacman  
   > cd \<project_root_folder\>  
   > cd pacman_rlglue  
   > make  
   
   Now we need to start 4 processes from the Project root folder as follows:  
   > rl_glue  
   > ./pacman_rlglue/pacman_agent  
   > ./pacman_rlglue/pacman_experiment  
   > ./ale -display_screen true -game_controller rlglue roms/ms_pacman.bin  

###Parameter Optimization (CMAES)

   The code for parameter optimization is present in the folder [cmaes/pacman] (cmaes/pacman)    
   Include and Library paths in the Makefile must be changed to suit your installation of ALE and CMAES  

###Gameplay Demo
   Gameplay videos can be found [here] (https://www.youtube.com/playlist?list=PLvAfDcsi0k-8_qRrMhoAluilFuATlKYbd)  

###References
  Refer [RL Glue home page] (http://glue.rl-community.org/wiki/Main_Page) for further details  
  CMAES installation instructions and other details for parameter optimization can be found [here] (https://github.com/beniz/libcmaes)
