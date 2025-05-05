# Each line is a state that defines state transitions, ignoring comments. 
# The first non-comment line is state 0, the next non-comment line is 
# state 1, etc. Evaluation starts at `start` uid, and terminates at the
# state with uid `end`.
#
# [State format]:
#
#   [uid]~[transition 1]~<transition n>
#
# Where uid and each transition is space separated, and a transition
# consists of:
#
#   [char/char range]$[next state]$[action function index]
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
#
# [Special characters]:
#
# ~ - section separator 
# $ - transition section separator
# @ - EOF
#
#
# [Current actions]:
#
#  SAVE_TOKEN   0
#  APPEND_CHAR  1
#
#  SET TYPE     2
#  SET VAR      3
#  SET IMM      4
#  SET ARITH =  5
#  SET OP_BRACK 6
#  SET CL_BRACK 7
#  SET IF       8
#  SET ELSE     9
#  SET GOTO    10
#  SET LABEL   11
#

#0
start~i$if_0$1~e$else_0$1~g$goto_0$1~0_9$imm_0$1~+,-,=,/,*,>,<$arith_0$1~($start$6,1,0~)$start$7,1,0~v$type_0$1~A_Z,a_d,f,h_u,w_z$var_0$1~ $start$~\n$start$~@$end$

#1
type_0~a$type_1$1~A_Z,b_z$var_0$1~ $start$3,0~:$start$11,0
#2
type_1~r$type_2$1~A_Z,a_q,s_z$var_0$1~ $start$3,0~:$start$11,0
#3
type_2~ $start$2,0~A_Z,a_z$var_0$1

#4
var_0~A_Z,a_z$var_0$1~ $start$3,0~:$start$11,0
#5
imm_0~0_9$imm_0$1~ $start$4,0

#6
arith_0~=,<,>$arith_1$1~ $start$5,0
#7
arith_1~ $start$5,0

#8
if_0~f$if_1$1~a_Z,a_e,g_z$var_0$1~ $start$3,0~:$start$11,0
#9
if_1~ $start$8,0~A_Z,a_z$var_0$1

#10
else_0~l$else_1$1~A_Z,a_k,m_z$var_0$1~ $start$3,0~:$start$11,0
#11
else_1~s$else_2$1~A_Z,a_r,t_z$var_0$1~ $start$3,0~:$start$11,0
#12
else_2~e$else_3$1~A_Z,a_d,f_z$var_0$1~ $start$3,0~:$start$11,0
#13
else_3~ $start$9,0~A_Z,a_z$var_0$1

#14
goto_0~o$goto_1$1~A_Z,a_n,p_z$var_0$1~ $start$3,0~:$start$11,0
#15
goto_1~t$goto_2$1~A_Z,a_s,u_z$var_0$1~ $start$3,0~:$start$11,0
#16
goto_2~o$goto_3$1~A_Z,a_n,p_z$var_0$1~ $start$3,0~:$start$11,0
#17
goto_3~ $start$10,0~A_Z,a_z$var_0$1

#18
end
