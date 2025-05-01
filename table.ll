# Each line is a state that defines state transitions, ignoring comments. 
# The first non-comment line is state 0, the next non-comment line is 
# state 1, etc. Evaluation starts at `start` uid, and terminates at the
# state with uid `end`.
#
# State format:
#
#   [uid] [transition 1] <transition n>
#
# Where uid and each transition is space separated, and a transition
# consists of:
#
#   [char/char range]:[next state]:[action function index]
#
# Where char/char range, action function index can list multiple values
# with commas.
#
# The below example defines a state with 3 transitions.
#
#   eg_1 a,b-c:eg_2:1,2,3 A:eg_3:1,2 B-C:eg_4:1
#
#   > Transition 1 accepts characters a, b, and c; moves to state eg_2;
#     and performs actions 1, 2, and 3.
#
#   > Transition 2 accepts character A, moves to state eg_3; and performs
#     actions 1 and 2.
#
#   > Transition 3 accepts characters B and C, moves to state eg_4; and
#     performs action 1.

start f,F:foo_1:1,2 b,B:barbaz_1:1 q,Q:qux_1:1,5 *:end:

#foo states
foo_1 o,O:foo_2:1
foo_2  :start:1,0

#bar & baz states
barbaz_1 a,A:barbaz_2:1
barbaz_2 r,R:start:1,3,0 z,Z:start:1,4,0

#qux states
qux_1 u,U:qux_2:1
qux_2 x,X:start

end
