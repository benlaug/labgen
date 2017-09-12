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
#include <unordered_set>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "TextProperties.hpp"
#include "Utils.hpp"

namespace ns_labgen {
  /* ======================================================================== *
   * GridWindow                                                               *
   * ======================================================================== */

  class GridWindow {
    protected:

      typedef Utils::ROIs                                                 ROIs;
      typedef std::vector<std::string>                               TextCache;

    public:

      enum Interpolation {
        NEAREST = cv::INTER_NEAREST,
        LINEAR  = cv::INTER_LINEAR,
        AREA    = cv::INTER_AREA,
        CUBIC   = cv::INTER_CUBIC,
        LANCZOS = cv::INTER_LANCZOS4
      };

    protected:

      static const double ADAPTIVE_SCALE_TERM;

    protected:

      ROIs rois;
      ROIs title_rois;
      std::string window_name;
      int32_t height;
      int32_t width;
      int32_t rows;
      int32_t cols;
      Interpolation interpolation;
      bool k_ratio;
      TextProperties::TextPropertiesPtr title_properties;
      TextCache title_cache;
      cv::Mat buffer;
      static std::unordered_set<std::string> available_windows;

    public:

      GridWindow(
        std::string window_name,
        int32_t height,
        int32_t width,
        int32_t rows = 1,
        int32_t cols = 1,
        TextProperties::TextPropertiesPtr title_properties = nullptr
      );

      virtual ~GridWindow();

      void display(const cv::Mat& mat, int32_t index = 0);

      void display(const cv::Mat& mat, int32_t row, int32_t col);

      void put_title(const std::string& title, int32_t index = 0);

      void put_title(const std::string& title, int32_t row, int32_t col);

      void refresh();

      bool are_titles_enabled() const;

      void keep_ratio();

      void ignore_ratio();

      bool is_ratio_respected() const;

      const cv::Mat& get_buffer() const;

      Interpolation get_interpolation_algorithm() const;

      void set_interpolation_algorithm(Interpolation algorithm);
  };
} /* ns_labgen */
