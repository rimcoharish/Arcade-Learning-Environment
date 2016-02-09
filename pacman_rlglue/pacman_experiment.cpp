#include <rlglue/RL_glue.h>
#include <iostream>
#include <stdio.h>

using namespace std;

int main() {
    const char* task_spec_string = RL_init();
    cout << task_spec_string << endl;
    int terminal=RL_episode(0);
    printf("%d Steps \t%f total reward\t %d natural end \n", RL_num_steps(), RL_return(), terminal);
    RL_cleanup();
    return 0;
}

