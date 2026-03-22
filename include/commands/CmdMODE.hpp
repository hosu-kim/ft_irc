/* 
MODE: used to change a mode of the given channel for operators
Syntax: MODE <channel> <+/-><i/t/o/k/l> [<parameters>]
Example: 1. i (Inivte-only): MODE #42prague +i
         2. t (Topic Restriction): MODE #42prague +t
            => only operators can change the topic
         3. o (Operator Setup): MODE #42prague +o user12
         4. k (Password Setup): MODE #42prague +k password123
         5. l (Limited number of users) MODE #42prague +l 5
*/