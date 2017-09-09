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
#include <opencv2/imgproc/imgproc.hpp>

#include <labgen/TextProperties.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen;

/* ========================================================================== *
 * TextProperties                                                             *
 * ========================================================================== */

TextProperties::TextProperties(
  Font font,
  double scale,
  const Scalar& color,
  const Scalar& background_color,
  int32_t thickness,
  LineType line_type,
  Justification justification
) :
font(font),
scale(scale),
color(color),
background_color(background_color),
thickness(thickness),
line_type(line_type),
justification(justification) {
  compute_metrics();
}

/******************************************************************************/

TextProperties::Font TextProperties::get_font() const {
  return font;
}

/******************************************************************************/

double TextProperties::get_scale() const {
  return scale;
}

/******************************************************************************/

const cv::Scalar& TextProperties::get_color() const {
  return color;
}

/******************************************************************************/

const cv::Scalar& TextProperties::get_background_color() const {
  return background_color;
}

/******************************************************************************/

int32_t TextProperties::get_thickness() const {
  return thickness;
}

/******************************************************************************/

TextProperties::LineType TextProperties::get_line_type() const {
  return line_type;
}

/******************************************************************************/

TextProperties::Justification TextProperties::get_justification() const {
  return justification;
}

/******************************************************************************/

int32_t TextProperties::get_text_height() const {
  return text_height;
}

/******************************************************************************/

int32_t TextProperties::get_baseline() const {
  return baseline;
}

/******************************************************************************/

int32_t TextProperties::estimate_width(const string& text, double scale) const {
  Size text_size = getTextSize(text, font, scale, thickness, nullptr);
  return text_size.width;
}

/******************************************************************************/

void TextProperties::compute_metrics() {
  Size text_size = getTextSize("Tp", font, scale, thickness, &baseline);
  text_height = text_size.height + baseline;

  if (line_type == LineType::LINE_ANTI_ALIASED) {
    text_height += 4;
    baseline += 2;
  }
}
