# Generic Board Game Protocol

Communication is done through ASCII text messages.

All messages must end with a UNIX new line character.

Messages from GUI to engine are also called *commands*.

Messages may contain arbitrary whitespace around tokens.

Invalid tokens or messages should be ignored.

The engine must process the commands synchronously, in the order that they are received.

## Notation

### Chess

Moves are encoded as long [algebraic notation](https://en.wikipedia.org/wiki/Algebraic_notation_(chess))
and positions as [FEN strings](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation).

### Checkers

Moves are positions are formatted as per the [PDN format](https://en.wikipedia.org/wiki/Portable_Draughts_Notation).

### Nine men's morris

Moves are encoded like so, like in chess algebraic notation:

- place: `a7`
- place + capture: `e4xg7`
- move: `c4-c3`
- move + capture: `d6-f6xa1`

Positions are encoded like in checkers and a bit like in chess, where:

1. The side to move
2. First player pieces separated by comma
3. Second player pieces separated by comma
4. Full move number

Examples:

- initial position: `w:w:b:1`
- after white b4: `b:wb4:b:1`
- after black d2: `w:wb4:bd2:2`
- after white c4: `w:wb4,c4:bd2:2`

## Messages From GUI To Engine

### gbgp

Tell the engine to use the Generic Board Game Protocol. The engine must respond with its identity and its settings.
Lastly, it should send the **gbgpok** message, to confirm that it's up and running and that it speaks the protocol.

Should be sent once at the very beginning.

### debug (on | off)

Tell the engine to switch the debug mode.

### isready

Tell the engine to respond with the **readyok** as soon as it has done processing all commands, if any. It is
used for synchronization.

### setoption name `identifier` [value `x`]

Tell the engine to set the option to the corresponding value.

### newgame

Tell the engine that the next position will come from a new game.

### position (pos `position` | startpos) [moves `move1` `move2` ...]

Tell the engine to set the position and play the corresponding moves.

### go [ponder] [wtime `x`] [btime `x`] [winc `x`] [binc `x`] [movestogo `x`] [maxdepth `x`] [maxtime `x`]

Tells the engine to think and respond with the best move of its current internal position.

### stop

Tell the engine to stop thinking and respond with a valid best move as quickly as possible.

This command should do nothing, if the engine is not thinking.

### ponderhit

Tell the engine that the opponent has played the expected move.

### quit

Tell the engine to shut down and exit gracefully.

## Messages From Engine To GUI

### id (name `x` | author `x`)

Inform the GUI about the engine's identity.

### gbgpok

Confirm the GUI that the engine speaks the Generic Board Game Protocol.

### readyok

Tell the GUI that the engine has done processing commands and is ready to think again.

### bestmove (`move` | none) [ponder `move`]

Tell the GUI that it has done thinking after a **go** command. If, the game is already over, then the move should
be *none*

### info [depth `x`] [time `x`] [nodes `x`] [score (eval `x` | win `x`)] [currmove `move`] [currmovenumber `x`] [hashfull `x`] [nps `x`] [pv `move1` `move2` ...]

Inform the GUI about its progress in calculating the best move. Can be sent at any time between the **go**
command and the **bestmove** response.

### option name `identifier` type `t` default `x` [min `x`] [max `x`] [var `x`]

Tell the GUI about a setting that the engine supports.
