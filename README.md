rconteam
========

Team balancing over rcon for OpenArena

# Building

type: make -C src

# Running

./src/rconteam <host> <port> <rconpassword>

eg.

./src/rconteam 192.168.0.1 27960 noonecanguessme

# cvars

Various cvars affect the running of the program.

There are:

rconteam_policy    = "LIFO" | "SKILL" // SKILL is not yet implemented
rconteam_enforcing = 1 | 0            // 1 -> force team change using !putteam

