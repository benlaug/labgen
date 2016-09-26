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
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include <opencv2/core/core.hpp>

#include <IBGS.h>

#include "History.hpp"

namespace ns_labgen {
  /* ======================================================================== *
   * LaBGen                                                                   *
   * ======================================================================== */

  class LaBGen {
    protected:

      size_t height;
      size_t width;
      std::string a;
      int32_t s;
      int32_t n;
      int32_t p;
      std::shared_ptr<IBGS> bgs;
      cv::Mat segmentation_map;
      cv::Mat mat_for_bgs_lib;
      ns_internals::PatchesHistory history;
      bool first_frame;

    public:

      LaBGen(
        size_t height,
        size_t width,
        std::string a,
        int32_t s,
        int32_t n,
        int32_t p
      );

      void insert(const cv::Mat& current_frame);

      void generate_background(cv::Mat& background) const;

      size_t get_height() const;

      size_t get_width() const;

      std::string get_a() const;

      int32_t get_s() const;

      int32_t get_n() const;

      int32_t get_p() const;

      const cv::Mat& get_segmentation_map() const;
  };
} /* ns_labgen_p */
