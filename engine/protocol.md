# Nine men's morris Protocol

## Messages From GUI To Engine

### init

Initialize the engine and prepare it to play the first game.

### newgame [start position]

Prepare the engine for a new game. Optionally set a start position.

### move \<move\>

Play the move on the engine's internal board.

### go [don't play move]

Make the engine think, play and return the best move of its current internal position. Optionally don't play the
move on the internal board, if the second token is *noplay*.

### stop

Stop the engine from thinking and force it to return a valid best move as quickly as possible.

### quit

Shut down the engine.

## Messages From Engine To GUI

### bestmove

The response after a `go` command.

### info [nodes \<value\>] [eval \<value\>] time \<value\>

General information about the thinking algorithm.
