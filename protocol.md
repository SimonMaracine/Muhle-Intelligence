# Nine men's morris Protocol

## Messages From GUI To Engine

### init

Initialize the engine and prepare it to play the first game.

### newgame [`start position`]

<!-- TODO how positions look -->

Prepare the engine for a new game. Optionally set a start position.

### move `move`

Play the move on the engine's internal board.

If the current internal position is a game over, then the engine should do nothing.

### go [noplay]

Make the engine think, play and return the best move of its current internal position. Optionally don't play the
move on the internal board, if the second token is *noplay*.

If the current internal position is a game over, then the engine should respond with the best move as *none*.

### stop

Stop the engine from thinking and force it to return a valid best move as quickly as possible.

### quit

Shut down the engine.

## Messages From Engine To GUI

### ready

Sent at the beginning, as a signal for the GUI that the engine has started.

### bestmove (`move` | none)

The response after a `go` command.

### info time `value` [eval `value`] [nodes `value`]

General information about the thinking algorithm.
