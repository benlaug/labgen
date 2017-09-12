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
#include <algorithm>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <labgen/GridWindow.hpp>

using namespace std;
using namespace boost;
using namespace cv;
using namespace ns_labgen;

/* ========================================================================== *
 * GridWindow                                                                 *
 * ========================================================================== */

const double GridWindow::ADAPTIVE_SCALE_TERM = 0.05;

/******************************************************************************/

unordered_set<std::string> GridWindow::available_windows;

/******************************************************************************/

GridWindow::GridWindow(
  string window_name,
  int32_t height,
  int32_t width,
  int32_t rows,
  int32_t cols,
  TextProperties::TextPropertiesPtr titles_properties
) :
window_name(window_name),
height(height),
width(width),
rows(rows),
cols(cols),
interpolation(Interpolation::LINEAR),
k_ratio(false),
title_properties(titles_properties) {
  if (height <= 0)
    throw logic_error("The height must be larger than 0");

  if (width <= 0)
    throw logic_error("The width must be larger than 0");

  if (rows <= 0)
    throw logic_error("The number of rows must be larger than 0");

  if (cols <= 0)
    throw logic_error("The number of columns must be larger than 0");

  size_t text_h =
    are_titles_enabled() ? titles_properties->get_text_height() : 0;

  buffer = Mat((height + text_h) * rows, width * cols, CV_8UC3);

  if (available_windows.find(window_name) != available_windows.end())
    throw logic_error("The window " + window_name + " already exists");

  available_windows.insert(window_name);

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      rois.push_back(
        Rect(
          width * col,
          (height + text_h) * row, width,
          height
        )
      );

      if (are_titles_enabled()) {
        title_rois.push_back(
          Rect(
            width * col,
            (height + text_h) * row + height,
            width,
            text_h
          )
        );

        title_cache.push_back("");
      }
    }
  }
}

/******************************************************************************/

GridWindow::~GridWindow() {
  available_windows.erase(window_name);
}

/******************************************************************************/
#include <iostream>
void GridWindow::display(const Mat& mat, int32_t index) {
  if ((index < 0) || (index >= rois.size())) {
    throw logic_error(
      "The index " + lexical_cast<string>(index) + " is out of bounds"
    );
  }

  Mat roi = buffer(rois[index]);

  /* Rendering. */
  if ((mat.rows != roi.rows) || (mat.cols != roi.cols)) {
    /* Adapt roi to keep aspect ratio. */
    if (k_ratio) {
      double ratio = min(
        static_cast<double>(roi.rows) / mat.rows,
        static_cast<double>(roi.cols) / mat.cols
      );

      int32_t resize_height = mat.rows * ratio;
      int32_t resize_width = mat.cols * ratio;

      Rect rect = rois[index];
      rect.y = rect.y + (rect.height - resize_height) / 2;
      rect.x = rect.x + (rect.width - resize_width) / 2;
      rect.height = resize_height;
      rect.width = resize_width;

      roi = buffer(rect);
    }

    if (index == 2) { //////////////////////////////////////////////////////////////////////////////////////
      Rect r = rois[2];
      r.x = r.x + r.width / 2;
      roi = buffer(r);
    }
    /* Resize mat directly in roi. */
    if (mat.type() == CV_8UC3)
      resize(mat, roi, Size(roi.cols, roi.rows), 0, 0, interpolation);
    else { // Convert to color.
      Mat resized;
      resize(mat, resized, Size(roi.cols, roi.rows), 0, 0, interpolation);
      cvtColor(resized, roi, CV_GRAY2BGR);
    }
  }
  else {
    if (mat.type() == CV_8UC3)
      mat.copyTo(roi);
    else // Convert to color.
      cvtColor(mat, roi, CV_GRAY2BGR);
  }

  imshow(window_name, buffer);
}

/******************************************************************************/

void GridWindow::display(const Mat& mat, int32_t row, int32_t col) {
  display(mat, width * row + col);
}

/******************************************************************************/

void GridWindow::put_title(const string& title, int32_t index) {
  if (!are_titles_enabled()) {
    throw runtime_error(
      "Cannot put a title as no title properties was given during the "
      "instantiation of the window '" + window_name + "'"
    );
  }

  /* Checking whether the title has already been rendered. */
  if (title == title_cache[index])
    return;

  title_cache[index] = title;

  /* Extracting the appropriate ROI from the buffer. */
  Mat title_roi = buffer(title_rois[index]);
  if (index == 2) { //////////////////////////////////////////////////////////////////////////////////////
    Rect r = title_rois[2];
    r.x = r.x + r.width / 2;
    title_roi = buffer(r);
  }
  title_roi = title_properties->get_background_color();

  /* Adapting scale to avoid overflows. */
  double scale = title_properties->get_scale();

  while (title_properties->estimate_width(title, scale) >= width) {
    if (scale <= 0) {
      throw runtime_error(
        "The title '" + title + "' is too large to be rendered"
      );
    }

    scale -= ADAPTIVE_SCALE_TERM;
  }

  /* Compute x offset regarding the chosen justification. */
  int32_t x_offset = 0;

  switch (title_properties->get_justification()) {
    case TextProperties::Justification::LEFT:
      x_offset = 1;
      break;

    case TextProperties::Justification::CENTER:
      x_offset = (width - title_properties->estimate_width(title, scale)) / 2;
      break;

    case TextProperties::Justification::RIGHT:
      x_offset = width - title_properties->estimate_width(title, scale) - 1;
      break;
  }

  /* Rendering. */
  putText(
    title_roi,
    title,
    Point(
      x_offset,
      title_properties->get_text_height() - title_properties->get_baseline()
    ),
    title_properties->get_font(),
    scale,
    title_properties->get_color(),
    title_properties->get_thickness(),
    title_properties->get_line_type()
  );
}

/******************************************************************************/

void GridWindow::put_title(const string& title, int32_t row, int32_t col) {
  put_title(title, width * row + col);
}

/******************************************************************************/

bool GridWindow::are_titles_enabled() const {
  return title_properties != nullptr;
}

/******************************************************************************/

void GridWindow::keep_ratio() {
  k_ratio = true;
}

/******************************************************************************/

void GridWindow::ignore_ratio() {
  k_ratio = false;
}

/******************************************************************************/

bool GridWindow::is_ratio_respected() const {
  return k_ratio;
}

/******************************************************************************/

const Mat& GridWindow::get_buffer() const {
  return buffer;
}

/******************************************************************************/

GridWindow::Interpolation GridWindow::get_interpolation_algorithm() const {
  return interpolation;
}

/******************************************************************************/

void GridWindow::set_interpolation_algorithm(Interpolation algorithm) {
  interpolation = algorithm;
}
