# Generic Board Game Protocol

Communication is done through ASCII text messages.

All messages must end with a UNIX new line character.

Messages from GUI to engine are also called *commands*.

Messages may contain arbitrary whitespace around tokens.

Invalid tokens or messages should be ignored.

The engine must process the commands synchronously, in the order that they are received.

## Notation

The board is represented by 24 nodes, each having an index from 0 to 23 from top to bottom and left to right.

### Moves

Moves are encoded like so:

```txt
P INDEX [T INDEX]
M INDEX-INDEX [T INDEX]
```

Where INDEX is a number between 0 and 23.

*P* means *place*, *M* means *move* and *T* means *take*.

Examples:

- `P0`
- `P4T15`
- `M0-1`
- `M1-4T9`

### Positions

Positions are encoded like so:

```txt
PLAYER_1:PLAYER_1_PIECE_POSITIONS;PLAYER_2:PLAYER_2_PIECE_POSITIONS;PLAYER_HAVING_THE_TURN;PLIES;PLIES_WITHOUT_ADVANCEMENT
```

Where:

- PLAYER_X: either *w* or *b*
- PLAYER_X_PIECE_POSITIONS: a comma separated list of positions, where a position is a number between 0 and 23
- PLAYER_HAVING_THE_TURN: either *w* or *b*
- PLIES + PLIES_WITHOUT_ADVANCEMENT: a number in the range [0, 9999]

Examples:

- Initial position: `w:;b:;w;0;0`
- Pieces one after the other: `w:0,2,4;b:1,3;b;4;0`

## Messages From GUI To Engine

### gbgp

Tell the engine to use the Generic Board Game Protocol. The engine must respond with its identity and its settings.
Lastly, it should send the **gbgpok** message, to confirm that it's up and running and that it speaks the protocol.

Should be sent once at the very beginning.

### debug (on | off)

Tell the engine to switch the debug mode.

### isready

Tell the engine to respond with the **readyok** as soon as it has done processing all commnads, if any. It is
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
