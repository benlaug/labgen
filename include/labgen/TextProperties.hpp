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
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <opencv2/core/core.hpp>

namespace ns_labgen {
  /* ======================================================================== *
   * TextProperties                                                           *
   * ======================================================================== */

  class TextProperties {
    public:

      typedef std::shared_ptr<TextProperties>                TextPropertiesPtr;

    public:

      /* Fonts. */
      enum Font {
        FONT_SIMPLEX        = cv::FONT_HERSHEY_SIMPLEX,
        FONT_PLAIN          = cv::FONT_HERSHEY_PLAIN,
        FONT_DUPLEX         = cv::FONT_HERSHEY_DUPLEX,
        FONT_COMPLEX        = cv::FONT_HERSHEY_COMPLEX,
        FONT_TRIPLEX        = cv::FONT_HERSHEY_TRIPLEX,
        FONT_COMPLEX_SMALL  = cv::FONT_HERSHEY_COMPLEX_SMALL,
        FONT_SCRIPT_SIMPLEX = cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
        FONT_SCRIPT_COMPLEX = cv::FONT_HERSHEY_SCRIPT_COMPLEX
      };

      /* Line types. */
      enum LineType {
        LINE_FILLED         = cv::FILLED,
        LINE_4_CONNECTED    = cv::LINE_4,
        LINE_8_CONNECTED    = cv::LINE_8,
        LINE_ANTI_ALIASED   = cv::LINE_AA
      };

      /* Justification. */
      enum Justification {
        LEFT,
        CENTER,
        RIGHT
      };

    protected:

      Font font;
      double scale;
      cv::Scalar color;
      cv::Scalar background_color;
      int32_t thickness;
      LineType line_type;
      Justification justification;
      int32_t text_height;
      int32_t baseline;

    public:

      TextProperties(
        Font font = Font::FONT_DUPLEX,
        double scale = 0.8,
        const cv::Scalar& color = cv::Scalar(0, 0, 0),
        const cv::Scalar& background_color = cv::Scalar(255, 255, 255),
        int32_t thickness = 1,
        LineType line_type = LineType::LINE_ANTI_ALIASED,
        Justification justification = Justification::CENTER
      );

      Font get_font() const;

      double get_scale() const;

      const cv::Scalar& get_color() const;

      const cv::Scalar& get_background_color() const;

      int32_t get_thickness() const;

      LineType get_line_type() const;

      Justification get_justification() const;

      int32_t get_text_height() const;

      int32_t get_baseline() const;

      int32_t estimate_width(const std::string& text, double scale) const;

    protected:

      void compute_metrics();
  };
} /* ns_labgen */
