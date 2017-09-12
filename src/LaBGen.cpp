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

#include <opencv2/imgproc/imgproc.hpp>

#include <labgen/BGSFactory.hpp>
#include <labgen/LaBGen.hpp>
#include <labgen/Utils.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen;
using namespace ns_labgen::ns_internals;

/* ========================================================================== *
 * LaBGen                                                                     *
 * ========================================================================== */

LaBGen::LaBGen(
  size_t height,
  size_t width,
  string a,
  int32_t s,
  int32_t n,
  int32_t p
) :
height(height),
width(width),
a(a),
s(s),
n(n),
p(p),
bgs(BGSFactory::get_bgs_algorithm(a)),
segmentation_map(Mat(height, width, CV_8UC1)),
mat_for_bgs_lib(Mat(height, width, CV_8UC3)),
history(Utils::getROIs(height, width, n), s),
first_frame(true) {}

/******************************************************************************/

void LaBGen::insert(const Mat& current_frame) {
  /* Background subtraction. */
  bgs->process(current_frame.clone(), segmentation_map, mat_for_bgs_lib);

  /* Initialization of background subtraction. */
  if (first_frame) {
    first_frame = false;
    return;
  }

  if (segmentation_map.channels() != 1)
    cvtColor(segmentation_map, segmentation_map, CV_BGR2GRAY);

  /* Insert the current frame along with the segmentation map into the
   * history.
   */
  history.insert(segmentation_map, current_frame);
}

/******************************************************************************/

void LaBGen::generate_background(Mat& background) const {
  if (history.empty()) {
    throw runtime_error(
      "Cannot generate the background with less than two inserted frames"
    );
  }

  history.median(background, s);
}

/******************************************************************************/

size_t LaBGen::get_height() const {
  return height;
}

/******************************************************************************/

size_t LaBGen::get_width() const {
  return width;
}

/******************************************************************************/

string LaBGen::get_a() const {
  return a;
}

/******************************************************************************/

int32_t LaBGen::get_s() const {
  return s;
}

/******************************************************************************/

int32_t LaBGen::get_n() const {
  return n;
}

/******************************************************************************/

int32_t LaBGen::get_p() const {
  return p;
}

/******************************************************************************/

const Mat& LaBGen::get_segmentation_map() const {
  return segmentation_map;
}
