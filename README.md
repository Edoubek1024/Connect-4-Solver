# Connect 4 Solver

Connect 4 is a game in which players alternate placing chips into a grid that traditionally is made up of 7 columns and 6 rows. If a player gets four chips in a row horizontally, vertically, or diagonally, they win. For further information, see: [Connect 4 explained](https://en.wikipedia.org/wiki/Connect_Four).

The files in this repository utilize the SFML 3.0.0 library to display a connect 4 game. The users can play normally by playing alternating turns or they can use the artificial intelligence to make the most optimal move.

<p align="center">
  <img src="https://github.com/user-attachments/assets/a37457b6-2d37-48c3-873d-d2851ecadb69" width="600" />
</p>

## The Connect 4 Game

The connect 4 game starts with an empty grid. The grid is displayed using the `renderGrid` function. This function directly uses SFML circles to display a connect 4 board state for the user. The different players' chips are denoted by red and yellow circles, as opposed to black ones representing holes in the board.
