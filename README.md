# Connect 4 Solver

Connect 4 is a game in which players alternate placing chips into a grid that traditionally is made up of 7 columns and 6 rows. If a player gets four chips in a row horizontally, vertically, or diagonally, they win. For further information, see: [Connect 4 explained](https://en.wikipedia.org/wiki/Connect_Four).

The files in this repository utilize the SFML 3.0.0 library to display a connect 4 game. The users can play normally by playing alternating turns or they can use the artificial intelligence to make the most optimal move.

<p align="center">
  <img src="https://github.com/user-attachments/assets/a37457b6-2d37-48c3-873d-d2851ecadb69" width="600" />
</p>

## The Connect 4 Game

The connect 4 game starts with an empty grid. The grid is displayed using the `renderGrid` function. This function directly uses SFML circles to display a connect 4 board state for the user. The different players' chips are denoted by red and yellow circles, as opposed to black ones representing holes in the board.

## Interaction

The players can interact with the board in two ways: clicking on the column in which they wish to place a chip, or pressing the 'p' key in order to play the most optimal move using an AI solve.

Mouse clicking is listened for thanks to the `eventHandler` function. This function uses the SFML `Event` class to listen for any events from the user. 

A player's interaction only occurs upon a click event. Once a click event is found, the mouse position is checked in order to place the player's chip in the column they click. If the column where the player clicked is available, their corresponding chip is placed and the next turn is initiated.

An AI solution only occurs upon a 'p' key press event. Once a 'p' key press is found, the program uses the `findMove` function to find the optimal move assuming a perfect game. Then, the piece corresponding to the current player is placed in that optimal spot.

## The Negamax Algorithm

This program uses the negamax variant of the minmax algorithm to find the most optimal move.

### What actually is negamax?

Negamax is a search algorithm used in two-player, zero-sum games (such as chess, tic-tac-toe, and checkers) to determine the optimal move for a player. It's essentially a simplified version of the Minimax algorithm, where both players are assumed to play optimally and the goal is to minimize the opponent's possible outcome while maximizing your own. In most cases, including in this program, this is done by using depth-first-search through a decision tree.

### The Implementation

Optimal moves are chosen through the `bestMove` function of the `State` class. This function initializes a negamax procedure and then uses the `negamax` function to search through the decisions tree. Upon finding a leaf node, the negamax function returns 0 upon a cat's game (no one wins) result, and returns a weighted result based on if the winner of that outcome corresponds to the current player. The sooner the position, the stronger the weight.

Both the `bestMove` and `negamax` functions also include alpha-beta pruning. Alpha-beta pruning can be described in-depth [here](https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-4-alpha-beta-pruning/), but is a way to avoid considering many nodes in the decision tree in order to make the functions far more efficient.

### Optimization

Based on how Connect 4 works as a game, there are many opportunities for optimization. One of the most effective is checking for immediate wins or immediate threats. If there is an immediate win opportunity, the algorithm will prune all other branches since the most optimal win is the winning move that the player can make. If there is an immediate threat, the algorithm will make sure to block the opposing player's opportunity to win.

Based on how leaf nodes are evaluated, the algorithm is also able to calculate if there won't be any nodes past a certain point that will be a more effective score than the most optimal move already found.
