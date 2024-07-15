# Nine men's morris Protocol

Communication is done through ASCII text messages.

All messages must end with the new line character: *\n*.

Messages from GUI to engine are also called *commands*.

The message string may contain any number of characters (or tokens).

The message string can contain arbitrary whitespace around tokens.

Both the engine and the GUI should try their best to understand and execute a command or parse a message.
Any extra invalid tokens should be ignored.

The engine must process the commands synchronously, in the order that they are received.

The engine should not send messages at inappropriate times.

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

### init

Tells the engine to initialize itself and get ready to play.

Should be sent once at the beginning, after the **ready** message.

### newgame [`start position`]

Tells the engine to prepare for a new game. It is not necessary to send this command right before the first game
(right after **init**). Optionally tells it to start from a
specific position.

### move `move`

Tells the engine to play the move on the internal board.

If the current internal position is a game over, then the engine should do nothing.

It is GUI's responsability to send correct move commands.

### go [noplay]

Tells the engine to think, play and return the best move of its current internal position. Optionally don't play
the move on the internal board, if the second token is *noplay*.

If the current internal position is a game over, then the engine should respond with the best move as *none*.

The GUI is not permitted to send the **go** command while the engine is still thinking. It can only send another
**go** command after it received a **bestmove** message from the engine.

### stop

Tells the engine to stop from thinking and return a valid best move as quickly as possible.

This command should do nothing, if the engine is not in the process of thinking.

The engine must have a valid result even if it was stopped from thinking very early.

### quit

Tells the engine to shut down and exit gracefully.

## Messages From Engine To GUI

### ready

Sent at the beginning, as a signal for the GUI that the engine has started.

### bestmove (`move` | none)

Responds with the best move calculated after a `go` command, or with *none*, if the game is over

### info time `value` [eval `value`] [nodes `value`]

Informs the GUI about its progress in calculating the best move. Can be sent at any time between the **go**
command and the **bestmove** response.

*time* represents the total elapsed time since the thinking algorithm started

*eval* represents how much advantage does the current player have. It is implementation defined and optional.

*nodes* represents the number of leaf nodes processed in the minimax algorithm. This value is optional,
as the engine may not use the minimax algorithm.
