/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/labgen
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
#include <labgen/Utils.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen::ns_internals;

/* ========================================================================== *
 * Utils                                                                      *
 * ========================================================================== */

string Utils::getMethod(string method) {
  size_t pos = method.rfind("/");
  pos = (pos == string::npos) ? 0 : pos + 1;

  return method.substr(pos, method.rfind(".") - pos);
}

/******************************************************************************/

Utils::ROIs Utils::getROIs(size_t height, size_t width, size_t segments) {
  if (segments == 0)
    return getROIs(height, width);

  ROIs rois;
  rois.reserve(segments * segments);

  size_t patch_height = height / segments;
  size_t h_reminder = height % segments;

  size_t patch_width = width / segments;
  size_t w_reminder = width % segments;

  /* Algorithm. */
  size_t h_reminder_tmp = h_reminder;
  size_t previous_y = 0;
  size_t previous_height = 0;

  /*
   * If the modulo X of a dimension divided by the number of segments is
   * superior to 0, then it is distributed among the X first patches.
   */
  for (size_t i = 0; i < segments; ++i) {
    size_t w_reminder_tmp = w_reminder;
    size_t previous_x = 0;
    size_t previous_width = 0;

    for (size_t j = 0; j < segments; ++j) {
      rois.push_back(
        Rect(
          (j == 0) ? 0 : (previous_x + previous_width),
          (i == 0) ? 0 : (previous_y + previous_height),
          patch_width  + ((w_reminder_tmp > 0) ? 1 : 0),
          patch_height + ((h_reminder_tmp > 0) ? 1 : 0)
        )
      );

      previous_x = rois.back().x;
      previous_width = rois.back().width;

      if (w_reminder_tmp > 0)
        --w_reminder_tmp;
    }

    previous_y = rois.back().y;
    previous_height = rois.back().height;

    if (h_reminder_tmp > 0)
      --h_reminder_tmp;
  }

  return rois;
}

/******************************************************************************/

Utils::ROIs Utils::getROIs(size_t height, size_t width) {
  ROIs rois;
  rois.reserve(height * width);

  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j)
      rois.push_back(Rect(j, i, 1, 1));
  }

  return rois;
}
