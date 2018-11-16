# Conways_Game_of_Life
Conway's Game of Life
/**
 * There are two goals to this assignment.  The first is to implement an
 * iterative simulation.  The second is to practice visualization of a
 * simulation.  To that end, we will write a program that generates videos that
 * depict the execution of Conway's Game of Life.
 *
 * Conway's Game of Life is a simulation, which takes as its initial state a
 * two-dimensional grid of zeroes and ones.  The state of a cell in step k+1 is
 * determined exclusively by the state of that cell and its neighbors in step k.
 *
 * The rules are as follows:
 * - If a "1" cell has fewer than two live neighbors, then it becomes "0" in
 *   the next round.  This is "underpopulation"
 * - If a "1" cell has two or three live neighbors, then it remains "1" in the
 *   next round.  This is "healthy population"
 * - If a "1" cell has more than three live neighbors, then it becomes "0" in
 *   the next round.  This is "overpopulation".
 * - If a "0" cell has exactly three live neighbors, then it becomes "1" in the
 *   next round.  This is "reproduction".
 */
