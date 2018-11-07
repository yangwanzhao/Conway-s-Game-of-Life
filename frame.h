#include <iostream>
#include <fstream>
#include <libgen.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


vector<int> split(string str, string pattern)
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
  Mat grayImg = Mat(rows*ppc, cols*ppc, CV_8U);

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

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      int lives=0;
      // top left corner
      if (i == 0 && j == 0)
      {
        lives = matrix[0][1]+matrix[1][0]+matrix[1][1];
      }
      // top right corner
      else if (i == 0 && j == cols-1)
      {
        lives = matrix[0][cols-2]+matrix[1][cols-1]+matrix[1][cols-2];
      }
      // bottom left corner
      else if (i == rows-1 && j == 0)
      {
        lives = matrix[rows-1][1]+matrix[rows-2][0]+matrix[rows-2][1];
      }
      // bottom right corner
      else if (i == rows-1 && j == cols-1)
      {
        lives = matrix[rows-1][cols-2]+matrix[rows-2][cols-1]+matrix[rows-2][cols-2];
      }
      // top edge
      else if (i == 0)
      {
        lives = matrix[i][j-1]+matrix[i][j+1]+matrix[i+1][j-1]+matrix[i+1][j]+matrix[i+1][j+1];
      }
      // right edge
      else if (j == cols-1)
      {
        lives = matrix[i-1][j]+matrix[i+1][j]+matrix[i-1][j-1]+matrix[i][j-1]+matrix[i+1][j-1];
      }
      // bottom edge
      else if (i == rows-1)
      {
        lives = matrix[i][j-1] + matrix[i][j+1] + matrix[i-1][j-1] + matrix[i-1][j] + matrix[i-1][j+1];
      }
      // left edge
      else if (j == 0)
      {
        lives = matrix[i-1][j]+matrix[i+1][j]+matrix[i-1][j+1]+matrix[i][j+1]+matrix[i+1][j+1];
      }
      else{
        lives = matrix[i-1][j-1] + matrix[i-1][j] + matrix[i-1][j+1] +
                matrix[i][j-1] + matrix[i][j+1] +
                matrix[i+1][j-1] + matrix[i+1][j] + matrix[i+1][j+1];
      }

      if (matrix[i][j] == 0 && lives == 3)
      {
        matrix_out[i][j] = 1;
      }
      if (matrix[i][j] == 1)
      {
        if ( lives < 2 || lives > 3 )
          matrix_out[i][j] = 0;
      }

    }
  }

  return matrix_out;
}





















