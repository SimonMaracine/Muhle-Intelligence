# Standard Rules

- Each player has 9 pieces, black or white
- The board consists of 24 nodes and 16 possible mills
- In phase I, players take turns to place pieces on the board, on any empty node
- When all pieces were placed, phase II begins, where pieces are moved to any empty neighbor nodes
- Any time a mill is formed (3 pieces in line), an opponent piece must be taken
- Opponent pieces in mills cannot be taken, except when all their pieces are in mills; in that case
  any piece can be taken
- When an opponent remains with 3 pieces, they can "jump", i.e. they can move the pieces to any
  empty node, instead of only moving to neighbor nodes
- The game is over when either a player remains with only 2 pieces, or they cannot make any legal
  move; in that case, that player loses
- When a double mill is formed, stil only one opponent piece can be taken
- If a board configuration, position, state repeats for the third time, the game ends in a tie,
  i.e. threefold repetition
- When 40 moves are made without forming any mill, the game ends in a tie
- Players can resign at any time, which means they lose the game
- A player's turn ends only after they
  - put a piece on the board,
  - moved a piece,
  - or any of the previous two and they took an opponent's piece;
- Only after a player ends their turn, the board's state can be analyzed for game over or other
  conditions
