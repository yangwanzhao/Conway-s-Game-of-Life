#include <iostream>
#include <fstream>
#include <libgen.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "frame.h"

using namespace std;
using namespace cv;

/** Print some helpful usage information */
void usage(const char *progname) {
  using std::cout;
  cout << "Conway's Game of Life\n";
  cout << "  Usage: " << progname << " [options]\n";
  cout << "    -i <.txt>   : a text file of N lines, each of which contains M columns\n";
  cout << "    -o <.mp4>   : a video file\n";
  cout << "    -f <int>    : The frame rate of the video\n";
  cout << "    -p <int>    : The number of pixels per cell\n";
  cout << "    -r <int>    : The number of rounds for running the simulation\n";
  cout << "    -w <string> : A text watermark included in video\n";
  cout << "    -s <string> : Determine which frames of the game should be output to image files\n";
  cout << "                  For example, \"-s1,4,16,256\" would result in frames 1,4,16,and 256\n";
  cout << "                  being output as images named frame1.png, frame4.png, frame16.png, and\n";
  cout << " 			     frame256.png\n";
  cout << "    -h          : print this message\n";
}

int main(int argc, char *argv[]) {
  // Config vars that we get via getopt
  string input_filename = "born.txt";
  string output_filename = "output.mvi";
  int frame_rate = 8;
  int ppc = 16;
  int rounds = 16;
  string watermark = "Wanzhao";
  string frame_out = "1,4,16";
  bool show_help = false; 

  // Parse the command line options:
  int o;
  while ((o = getopt(argc, argv, "i:o:f:p:r:w:s:h")) != -1) {
    switch (o) {
      case 'i':
      input_filename = optarg;
      break;
      case 'o':
      output_filename = optarg;
      break;
      case 'f':
      frame_rate = atoi(optarg);
      break;
      case 'p':
      ppc = atoi(optarg);
      break;
      case 'r':
      rounds = atoi(optarg);
      break;
      case 'w':
      watermark = optarg;
      break;
      case 's':
      frame_out = optarg;
      break;
      case 'h':
      show_help = true;
      break;
      default:
      show_help = true;
      break;
    }
  }

  // Print help and exit
  if (show_help) {
    usage(basename(argv[0]));
    exit(0);
  }

  // Print the configuration
  cout << "Conway's Game of Life" << endl;
  cout << "(i) input_txt       = " << input_filename << endl;
  cout << "(o) output_mp4      = " << output_filename << endl;
  cout << "(f) frame_rate      = " << frame_rate << endl;
  cout << "(p) pixels_per_cell = " << ppc << endl;
  cout << "(r) rounds          = " << rounds << endl;
  cout << "(w) watermark       = " << watermark << endl;
  cout << "(s) frames_out      = " << frame_out << endl;
  cout << endl;

  // read from born.txt
  vector<vector<int>> matrix;
  vector<int> v;
  int rows,cols;

  ifstream in(input_filename);
  if (in.is_open())
  {
    for (string str; getline(in, str); )
    {
      v = split(str, ",");
      matrix.push_back(v);
    }
    in.close();
  }
  else{
    cout << "Input file name error" << endl;
    return 0;
  }

  rows = matrix.size();
  cols = matrix[0].size();

  show_matrix(matrix);
  cout << "[" << cols << " * " << rows << "]" << endl;

  // Mat grayImg = matrix2Mat(matrix, ppc);
  // cout << grayImg.size() << endl;

  // imwrite("test1.jpg", grayImg);

  // matrix = next_generation(matrix);
  // grayImg = matrix2Mat(matrix, ppc);

  // imwrite("test2.jpg", grayImg);


  //generate video
  VideoWriter out;
  out.open(output_filename, out.fourcc('a', 'v', 'c', '1'), frame_rate, Size(cols*ppc, rows*ppc), false);


  Mat grayImg;

  for (int i = 0; i < frame_rate * 20; ++i)
  {
    grayImg = matrix2Mat(matrix, ppc);
    // imwrite("test" + to_string(i) + ".jpg", grayImg);
    
    out << grayImg;
    
    matrix = next_generation(matrix);
  }

  out.release();

  // dynamic computing

  // tbb

  // smart point

  // use atomic then we can implement a one-consumer-one-producer queue without lock

  // unbounded queue  :  need n atomic variable with <bool>
  // bounded queue  : next_insert and next_remove are atomic





}