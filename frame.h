#include <iostream>
#include <fstream>
#include <libgen.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


vector<int> my_split(string str, string pattern)
{
  vector<int> ret;
  if (pattern.empty()) 
    return ret;
  int start = 0, index = str.find_first_of(pattern, 0);
  while (index != str.npos)
  {
    if (start != index)
      ret.push_back(stoi(str.substr(start, index - start)));
    start = index + 1;
    index = str.find_first_of(pattern, start);
  }
  if (!str.substr(start).empty())
    ret.push_back(stoi(str.substr(start)));
  return ret;
}

vector<vector<int>> load_matrix(string input_name, int multiply){
  vector<vector<int>> matrix, temp_m;
  vector<int> v, temp_v;
  ifstream in(input_name);
  if (in.is_open())
  {
    for (string str; getline(in, str); )
    {
      v = my_split(str, ",");
      temp_v = v;
      for (int i = 1; i < multiply; ++i)
      {
        v.insert(v.end(), temp_v.begin(), temp_v.end());
      }
      matrix.push_back(v);
    }
    temp_m = matrix;
    for (int i = 1; i < multiply; ++i)
    {
      matrix.insert(matrix.end(), temp_m.begin(), temp_m.end());
    }
    
    in.close();
    return matrix;
  }
  else{
    cout << "Input file name error" << endl;
    exit(1);
  }

}

stack<int> frame_out_2_stack(string frames){
  vector<int> frame_out_array;
  stack<int> s;
  
  frame_out_array = my_split(frames, ",");
  sort(frame_out_array.begin(), frame_out_array.end());
  for (int i = frame_out_array.size()-1; i >= 0; i--)
  {
    s.push(frame_out_array[i]);
  }
  return s;
}

void show_stack(stack<int> s){
  while (!s.empty()) 
  { 
    cout << '\t' << s.top(); 
    s.pop(); 
  } 
  cout << '\n'; 
}


void show_matrix(vector<vector<int>> matrix){
  int rows = matrix.size();
  int cols = matrix[0].size();
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }
}


Mat matrix2Mat(vector<vector<int>> matrix, int ppc){
  int rows = matrix.size();
  int cols = matrix[0].size();
  // cout << rows << " * " << cols << endl;
  Mat grayImg = Mat(rows*ppc, cols*ppc, CV_8U);
  // cout << grayImg.rows << " ^ " << grayImg.cols << endl;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      grayImg(Rect(j*ppc,i*ppc,ppc,ppc)).setTo(255-255*matrix[i][j]); 

    }
  }
  return grayImg;
}

/**
 * 0 : from 0 to 0
 * 1 : from 1 to 1
 * 2 : from 1 to 0
 * 3 : from 0 to 1
 */
vector<vector<int>> next_generation(vector<vector<int>> matrix){
  int rows = matrix.size();
  int cols = matrix[0].size();
  vector<vector<int>> matrix_out = matrix;
  vector<vector<int>> matrix_temp = matrix;

  // four corners
  matrix_temp[0][0] = matrix[0][1]+matrix[1][0]+matrix[1][1];
  matrix_temp[0][cols-1] = matrix[0][cols-2]+matrix[1][cols-1]+matrix[1][cols-2];
  matrix_temp[rows-1][0] = matrix[rows-1][1]+matrix[rows-2][0]+matrix[rows-2][1];
  matrix_temp[rows-1][cols-1] = matrix[rows-1][cols-2]+matrix[rows-2][cols-1]+matrix[rows-2][cols-2];

  // top edge and bottom edge
  for (int j = 1; j <= cols-2; ++j)
  {
    matrix_temp[0][j] = matrix[0][j-1]+matrix[0][j+1]+matrix[1][j-1]+matrix[1][j]+matrix[1][j+1];
    matrix_temp[rows-1][j] = matrix[rows-1][j-1] + matrix[rows-1][j+1] + matrix[rows-2][j-1] + matrix[rows-2][j] + matrix[rows-2][j+1];
  }

  // right and left edge
  for (int i = 1; i <= rows-2; ++i)
  {
    matrix_temp[i][cols-1] = matrix[i-1][cols-1]+matrix[i+1][cols-1]+matrix[i-1][cols-2]+matrix[i][cols-2]+matrix[i+1][cols-2];
    matrix_temp[i][0] = matrix[i-1][0]+matrix[i+1][0]+matrix[i-1][1]+matrix[i][1]+matrix[i+1][1];
  }



  for (int i = 1; i < rows-1; ++i)
  {
    for (int j = 1; j < cols-1; ++j)
    {
      matrix_temp[i][j] = matrix[i-1][j-1] + matrix[i-1][j] + matrix[i-1][j+1] +
      matrix[i][j-1] + matrix[i][j+1] +
      matrix[i+1][j-1] + matrix[i+1][j] + matrix[i+1][j+1];
    }
  }

  for (int i = 0; i < rows-1; ++i)
  {
    for (int j = 0; j < cols-1; ++j)
    {
      if (matrix[i][j] == 0 && matrix_temp[i][j] == 3)
      {
        matrix_out[i][j] = 1;
      }
      if (matrix[i][j] == 1)
      {
        if ( matrix_temp[i][j] < 2 || matrix_temp[i][j] > 3 )
          matrix_out[i][j] = 0;
      }
    }
  }


  return matrix_out;
}





















