import rlglue.RLGlue as RLGlue

print 'Environment started'
RLGlue.RL_init()
RLGlue.RL_episode(0)
RLGlue.RL_cleanup()
print 'Program finished'
