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

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>

#include <boost/program_options.hpp>

namespace ns_labgen {
  /* ======================================================================== *
   * ArgumentsHandler                                                         *
   * ======================================================================== */

  class ArgumentsHandler {
    protected:

      boost::program_options::options_description opt_desc;
      boost::program_options::variables_map vars_map;
      std::string input;
      std::string output;
      bool default_set;
      bool universal_set;
      std::string a_param;
      int32_t s_param;
      int32_t n_param;
      int32_t p_param;
      bool visualization;
      bool split_vis;
      bool record;
      std::string record_path;
      int32_t record_fps;
      int32_t v_height;
      int32_t v_width;
      bool keep_ratio;
      int32_t wait;

    public:

      ArgumentsHandler(int argc, char** argv);

      bool ask_for_help() const;

      void print_help(std::ostream& os = std::cout) const;

      void parse_vars_map();

      const std::string& get_input() const;

      const std::string& get_output() const;

      const std::string& get_a_param() const;

      int32_t get_s_param() const;

      int32_t get_n_param() const;

      int32_t get_p_param() const;

      bool get_visualization() const;

      bool get_split_vis() const;

      bool get_record() const;

      const std::string& get_record_path() const;

      int32_t get_record_fps() const;

      int32_t get_v_height() const;

      int32_t get_v_width() const;

      bool get_keep_ratio() const;

      int32_t get_wait() const;

      void print_parameters(std::ostream& os = std::cout) const;

    protected:

      void define_options();

      void parse_input();

      void parse_output();

      void parse_default_params();

      void parse_universal_params();

      void check_preset_params();

      void parse_a_param();

      void parse_s_param();

      void parse_n_param();

      void parse_p_param();

      void parse_visualization();

      void parse_split_vis();

      void parse_record();

      void parse_v_height();

      void parse_v_width();

      void parse_keep_ratio();

      void parse_wait();
  };
} /* ns_labgen */
