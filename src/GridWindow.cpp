/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/labgen
 *
 * This file is part of LaBGen.
 *
 * LaBGen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaBGen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LaBGen.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <opencv2/imgproc/imgproc.hpp>

#include <labgen/GridWindow.hpp>

using namespace std;
using namespace boost;
using namespace cv;
using namespace ns_labgen;

/* ========================================================================== *
 * GridWindow                                                                 *
 * ========================================================================== */

unordered_set<std::string> GridWindow::available_windows;

/******************************************************************************/

GridWindow::GridWindow(
  string window_name,
  int32_t height,
  int32_t width,
  int32_t rows,
  int32_t cols
) :
window_name(window_name),
height(height),
width(width),
rows(rows),
cols(cols),
buffer() {
  if (height <= 0)
    throw logic_error("The height must be larger than 0");

  if (width <= 0)
    throw logic_error("The width must be larger than 0");

  if (rows <= 0)
    throw logic_error("The number of rows must be larger than 0");

  if (cols <= 0)
    throw logic_error("The number of columns must be larger than 0");

  buffer = Mat(height * rows, width * cols, CV_8UC3);

  if (available_windows.find(window_name) != available_windows.end())
    throw logic_error("The window " + window_name + " already exists");

  available_windows.insert(window_name);

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col)
      rois.push_back(Rect(width * col, height * row, width, height));
  }
}

/******************************************************************************/

GridWindow::~GridWindow() {
  available_windows.erase(window_name);
}

/******************************************************************************/

void GridWindow::display(const Mat& mat, int32_t index) {
  if ((index < 0) || (index >= rois.size())) {
    throw logic_error(
      "The index " + lexical_cast<string>(index) + " is out of bounds"
    );
  }

  Mat roi = buffer(rois[index]);

  if (mat.type() == CV_8UC3)
    mat.copyTo(roi);
  else
    cvtColor(mat, roi, CV_GRAY2BGR);

  imshow(window_name, buffer);
}

/******************************************************************************/

void GridWindow::display(const Mat& mat, int32_t row, int32_t col) {
  display(mat, width * row + col);
}
