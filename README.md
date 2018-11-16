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


# READ ME
################ How to compile the code #################
# Use dockerfile to create the container
# Use . run.sh file to compile the code
# Use ./gamelife to run the program


##################### Prameters ######################
# The parameters are listed below
# Conway's Game of Life
#  Usage: gamelife [options]
#    -i <.txt>   : a text file of N lines, each of which contains M columns
#    -x <int>    : multiply the input matrix by 'x'
#    -o <.mp4>   : a video file
#    -f <int>    : The frame rate of the video
#    -p <int>    : The number of pixels per cell
#    -t <int>    : Time of the video (second)
#    -r <int>    : The number of rounds for running the simulation
#    -w <string> : A text watermark included in video
#    -s <string> : Determine which frames of the game should be output to image files
#                  For example, "-s 1,4,16,256" would result in frames 1,4,16,and 256
#                  being output as images named frame1.png, frame4.png, frame16.png, and
#                  frame256.png
#    -h          : print this message
#    -c          : use circle queue
#    -l          : use pipeline
#    -m          : use circle queue and pipeline
