#include <iostream>
#include <fstream>
#include <libgen.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <chrono>
#include <stack>
#include <thread>
#include <time.h>
#include <opencv2/opencv.hpp>
#include "tbb/pipeline.h"
#include "frame.h"
#include "circle_queue.h"
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

using namespace std;
using namespace cv;
using namespace tbb;


/** Print some helpful usage information */
void usage(const char *progname) {
  using std::cout;
  cout << "Conway's Game of Life\n";
  cout << "  Usage: " << progname << " [options]\n";
  cout << "    -i <.txt>   : a text file of N lines, each of which contains M columns\n";
  cout << "    -x <int>    : multiply the input matrix by 'x'\n";
  cout << "    -o <.mp4>   : a video file\n";
  cout << "    -f <int>    : The frame rate of the video\n";
  cout << "    -p <int>    : The number of pixels per cell\n";
  cout << "    -t <int>    : Time of the video (second)\n";
  cout << "    -r <int>    : The number of rounds for running the simulation\n";
  cout << "    -w <string> : A text watermark included in video\n";
  cout << "    -s <string> : Determine which frames of the game should be output to image files\n";
  cout << "                  For example, \"-s 1,4,16,256\" would result in frames 1,4,16,and 256\n";
  cout << "                  being output as images named frame1.png, frame4.png, frame16.png, and\n";
  cout << "                  frame256.png\n";
  cout << "    -h          : print this message\n";
  cout << "    -c          : use circle queue\n";
  cout << "    -l          : use pipeline\n";
  cout << "    -m          : use circle queue and pipeline\n";
}

int main(int argc, char *argv[]) {
  // Config vars that we get via getopt
  string input_filename = "born.txt";
  int multiply_x = 1;
  string output_filename = "life.mp4";
  int frame_rate = 10;
  int ppc = 8;
  int time_video = 10;
  int rounds = time_video * frame_rate;
  string watermark = "Wanzhao";
  string frame_out = "44,4,16";
  bool show_help = false; 
  bool use_circle = false;
  bool use_circle_pipeline = false;
  bool use_pipeline = false;

  // Parse the command line options:
  int o;
  while ((o = getopt(argc, argv, "i:x:o:f:p:r:w:s:t:hclm")) != -1) {
    switch (o) {
      case 'i':
      input_filename = optarg;
      break;
      case 'x':
      multiply_x = atoi(optarg);
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
      case 't':
      time_video = atoi(optarg);
      break;
      case 'c':
      use_circle = true;
      break;
      case 'm':
      use_circle_pipeline = true;
      break;
      case 'l':
      use_pipeline = true;
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
  cout << "(x) multiply factor = " << input_filename << endl;
  cout << "(o) output_mp4      = " << output_filename << endl;
  cout << "(f) frame_rate      = " << frame_rate << endl;
  cout << "(p) pixels_per_cell = " << ppc << endl;
  cout << "(t) time_of_video   = " << time_video << endl;
  cout << "(r) rounds          = " << rounds << " (should be \"f * t\")" << endl; 
  cout << "(w) watermark       = " << watermark << endl;
  cout << "(s) frames_out      = " << frame_out << endl;
  cout << endl;

  // parse the string frame_out
  stack<int> s = frame_out_2_stack(frame_out);

  // read from born.txt
  vector<vector<int>> matrix;
  matrix = load_matrix(input_filename, multiply_x);
  
  int rows,cols;
  rows = matrix.size();
  cols = matrix[0].size();
  // show_matrix(matrix);
  cout << "[" << rows << " * " << cols << "]\n" << endl;

  
  // generate video
  VideoWriter out;
  out.open(output_filename, out.fourcc('a', 'v', 'c', '1'), frame_rate, Size(cols*ppc, rows*ppc), false);

  Mat grayImg, logo, mask, imageROI;
  // get text and ROI 
  int baseline; // Output parameter : y - coordinate of the baseline relative to the bottom-most text point
  int font_face = FONT_HERSHEY_DUPLEX; 
  double font_scale = (double)ppc/10;
  int thickness = (double)ppc/15;
  Size text_size = getTextSize(watermark, font_face, font_scale, thickness, &baseline);
  logo = Mat::zeros(text_size.height+5, text_size.width+5, CV_8U);
  mask = Mat::zeros(text_size.height+5, text_size.width+5, CV_8U);;
  putText(logo, watermark, Point(5, text_size.height), font_face, font_scale, cv::Scalar(255), thickness, CV_AA, 0);

  for (int i = 0; i < logo.rows; ++i)
  {
    for (int j = 0; j < logo.cols; ++j)
    {
      if (logo.at<uchar>(i,j) != 0)
      {
        mask.at<uchar>(i,j) = 255;
        logo.at<uchar>(i,j) = 0; 
      }
    }
  }
  clock_t startTime, endTime, time1, time2, time3, time4;
  clock_t MatTime=0, OutTime=0, GenTime=0;
  // start time 
  startTime = clock();
  auto starttime = high_resolution_clock::now();
  /*************************************************************************************************************************
   ******************************************** Implemented in four ways **************************************************
   *************************************************************************************************************************/

  /********************************************* Using Circle Queue **************************************************/
  if (use_circle == true)
  { 
    cout << "Using Circle Queue !!!" << endl;
    int p_rounds = 0, c_rounds = 0, num_frame = 1;
    
    CircularFifo<vector<vector<int>>, 128> queue;

  /* Consumer thread */
    thread Consumer{[&](){
      vector<vector<int>> temp_matrix;
      while(1){
        if ( false == queue.pop(temp_matrix)) {
          continue;
        }
      // cout << "pop" << endl;
        grayImg = matrix2Mat(temp_matrix, ppc);
        imageROI = grayImg(Rect(10,20,logo.cols,logo.rows));
        logo.copyTo(imageROI,mask);
        if ( !s.empty() && num_frame == s.top() )
        {
          imwrite("frame" + to_string(num_frame) + ".png", grayImg);
          s.pop();
        }
        num_frame++;
        out << grayImg;
        c_rounds++;
        if (c_rounds==rounds)
        {
          break;
        }
      }
      out.release();
    }};


  /* Producer thread */
    thread Producer{[&](){
      while(1){
        while( false == queue.push(matrix) ){}
          p_rounds++;
        matrix = next_generation(matrix);
        if (p_rounds==rounds)
        {
          break;
        }
      }
    }};
    Producer.join();
    Consumer.join();
  }
  /************************************************** End of Method 1 **********************************************************/
  
  /********************************************* Using Circle Queue and Pipeline **************************************************/
  else if (use_circle_pipeline == true)
  { 
    cout << "Using Circle and Pipeline !!!" << endl;
    int p_rounds = 0, out_rounds = 0, num_frame = 1;

    CircularFifo<vector<vector<int>>, 128> queue;

  /* Consumer thread */
    thread Consumer{[&](){
      vector<vector<int>> temp_matrix;
      parallel_pipeline( 3,
        make_filter<void, vector<vector<int>>>(
          filter::serial,
          [&](flow_control& fc)-> vector<vector<int>> {
            while(queue.pop(temp_matrix) == false){
              if (out_rounds >= rounds)
              {
                fc.stop();
                break;
              }
            }
            return temp_matrix;
          }    
          ) &
        make_filter<vector<vector<int>>, Mat>(
          filter::serial,
          [&](vector<vector<int>> temp)-> Mat {
            grayImg = matrix2Mat(temp, ppc);
            return grayImg;
          } 
          ) &
        make_filter<Mat, void>(
          filter::serial,
          [&](Mat out_Mat){
            imageROI = out_Mat(Rect(10,20,logo.cols,logo.rows));
            logo.copyTo(imageROI,mask);
            if ( !s.empty() && num_frame == s.top() )
            {
              imwrite("frame" + to_string(num_frame) + ".png", out_Mat);
              s.pop();
            }
            num_frame++;
            out << out_Mat;
            out_rounds++;
          }
          )
        );
      out.release();
    }};


  /* Producer thread */
    thread Producer{[&](){
      while(1){
        while( false == queue.push(matrix) ){}
          p_rounds++;
        matrix = next_generation(matrix);
        if (p_rounds==rounds)
        {
          break;
        }
      }
    }};
    Producer.join();
    Consumer.join();
  }
  /************************************************** End of Method 2 **********************************************************/
  
  /*********************************************************** Using Pipeline **************************************************/
  else if(use_pipeline == true)
  { 
    cout << "Using Pipeline !!!" << endl;
    int p_rounds = 0, out_rounds = 0, num_frame = 1;
    
    parallel_pipeline( 3,
      make_filter<void, vector<vector<int>>>(
        filter::serial,
        [&](flow_control& fc)-> vector<vector<int>> {
          if (p_rounds < rounds && p_rounds > 0)
          {
            matrix = next_generation(matrix);
          }
          if (out_rounds >= rounds)
          {
            fc.stop();
          }
          p_rounds++;
          return matrix;
        }    
        ) &
      make_filter<vector<vector<int>>, Mat>(
        filter::serial,
        [&](vector<vector<int>> temp)-> Mat {
          grayImg = matrix2Mat(temp, ppc);
          return grayImg;
        } 
        ) &
      make_filter<Mat, void>(
        filter::serial,
        [&](Mat out_Mat){
          imageROI = out_Mat(Rect(10,20,logo.cols,logo.rows));
          logo.copyTo(imageROI,mask);
          if ( !s.empty() && num_frame == s.top() )
          {
            imwrite("frame" + to_string(num_frame) + ".png", out_Mat);
            s.pop();
          }
          num_frame++;
          out << out_Mat;
          out_rounds++;
        }
        )
      );
    out.release();

  }
  /************************************************** End of Method 3 **********************************************************/

  /**************************************************** Using nothing special **************************************************/
  else{ 
    for (int i = 0; i < rounds; ++i)
    {
      time1 = clock();
      grayImg = matrix2Mat(matrix, ppc);
      imageROI = grayImg(Rect(10,20,logo.cols,logo.rows));
      logo.copyTo(imageROI,mask);
      if ( !s.empty() && i+1 == s.top() )
      {
        imwrite("frame" + to_string(i+1) + ".png", grayImg);
        s.pop();
      }

      time2 = clock();
      out << grayImg;
      time3 = clock();
      matrix = next_generation(matrix);
      time4 = clock();

      MatTime += time2 - time1; 
      OutTime += time3 - time2;
      GenTime += time4 - time3;
    }

    out.release();

    cout << "Mat time : " << MatTime << endl;
    cout << "Out time : " << OutTime << endl;
    cout << "Gen time : " << GenTime << endl;
    cout << endl;
  }
  /************************************************** End of Method 4 **********************************************************/
  
  /*************************************************************************************************************************
   ******************************************** End of Implementation ******************************************************
   *************************************************************************************************************************/
  
  // end time
  auto endtime = high_resolution_clock::now();
  endTime = clock();
  // cout << "Total execution time (clock) : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds\n" << endl;
  cout << "Total execution clock : " << endTime - startTime<< endl;
  duration<double> time_span = duration_cast<duration<double>>(endtime - starttime);
  std::cout << "Total execution time: " << time_span.count() << " seconds\n" << std::endl;


}









