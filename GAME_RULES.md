# Game Rules

## Standard Game

- Each player has 9 pieces, black or white
- The board consists of 24 nodes and 16 possible mills
- In phase I, players take turns to place pieces on the board, on any empty node; white begins
- When all pieces are placed, phase II begins, where pieces are moved to any empty neighbor nodes
- Any time a mill is formed (3 pieces in line), an opponent piece must be taken
- Opponent pieces in mills cannot be taken, except when all their pieces are in mills; in that case
  any piece can be taken
- When a player remains with 3 pieces, they can "jump", i.e. they can move the pieces to any
  empty node, instead of only moving to neighbor nodes
- The game is over when either a player remains with only 2 pieces, or they cannot make any legal
  move; in that case, that player loses
- When a double mill is formed, still only one opponent piece can be taken
- If a board position repeats for the third time, the game ends in a tie, i.e. threefold repetition
- Positions are considered the same, if
  - pieces of the same color occupy the same nodes and
  - the same player has the move
- When 50 plies pass without forming any mill, the game ends in a tie
- A player can resign at any time, which means they lose the game
- A player's turn (ply) ends only after they either
  - put a piece on the board,
  - move a piece,
  - or any of the previous two and they take an opponent's piece
- Only after a player ends their turn (ply) and before the next turn, the board's position can be
  analyzed for game over or other conditions
