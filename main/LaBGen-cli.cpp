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
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/program_options.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <labgen/LaBGen.hpp>

using namespace cv;
using namespace std;
using namespace boost::program_options;
using namespace ns_labgen;

/******************************************************************************
 * Main program                                                               *
 ******************************************************************************/

int main(int argc, char** argv) {
  /****************************************************************************
   * Argument(s) handling.                                                    *
   ****************************************************************************/

  options_description opt_desc(
    "LaBGen - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016\n"
    "http://www.montefiore.ulg.ac.be/~blaugraud\n"
    "http://www.telecom.ulg.ac.be/labgen\n\n"
    "Usage: ./LaBGen-cli [options]"
  );

  opt_desc.add_options()
    (
      "help,h",
      "print this help message"
    )
    (
      "input,i",
      value<string>(),
      "path to the input sequence"
    )
    (
      "output,o",
      value<string>(),
      "path to the output folder"
    )
    (
      "a-parameter,a",
      value<string>(),
      "name of the background subtraction algorithm to use (A parameter)"
    )
    (
      "s-parameter,s",
      value<int32_t>()->multitoken(),
      "value of the S parameter"
    )
    (
      "n-parameter,n",
      value<int32_t>(),
      "value of the N parameter"
    )
    (
      "p-parameter,p",
      value<int32_t>(),
      "value of the P parameter"
    )
    (
      "universal,u",
      "use the universal set of parameters"
    )
    (
      "default,d",
      "use the default set of parameters"
    )
    (
      "visualization,v",
      "enable visualization"
    )
  ;

  variables_map vars_map;
  store(parse_command_line(argc, argv, opt_desc), vars_map);
  notify(vars_map);

  /* Help message. */
  if (vars_map.count("help")) {
    cout << opt_desc << endl;
    return EXIT_SUCCESS;
  }

  /*
   * Welcome message.
   */

  cout << "===========================================================" << endl;
  cout << "= LaBGen                                                  =" << endl;
  cout << "===========================================================" << endl;
  cout << "= Copyright - Benjamin Laugraud - 2016                    =" << endl;
  cout << "= http://www.montefiore.ulg.ac.be/~blaugraud              =" << endl;
  cout << "= http://www.telecom.ulg.ac.be/research/labgen            =" << endl;
  cout << "===========================================================" << endl;
  cout << endl;

  /*
   * Extract parameters and sanity check.
   */

  string  a_param = "";
  int32_t s_param = 0;
  int32_t n_param = 0;
  int32_t p_param = 0;

  /* "input" */
  if (!vars_map.count("input"))
    throw runtime_error("You must provide the path of the input sequence!");

  string sequence(vars_map["input"].as<string>());

  /* "output" */
  if (!vars_map.count("output"))
    throw runtime_error("You must provide the path of the output folder!");

  string output(vars_map["output"].as<string>());

  /* "default" */
  bool default_set = vars_map.count("default");

  if (default_set) {
    a_param = "frame_difference";
    s_param = 57;
    n_param = 4;
    p_param = 29;
  }

  /* "universal" */
  bool universal_set = vars_map.count("universal");

  if (universal_set) {
    s_param = 19;
    n_param = 2;
    p_param = 1;
  }

  if (default_set && universal_set) {
    throw runtime_error(
      "You cannot use the universal and default set of parameters in the same "
      "time!"
    );
  }

  /* Other parameters. */
  if (!default_set) {
    if (!default_set && !vars_map.count("a-parameter")) {
      throw runtime_error(
        "You must provide the name of the background subtraction algorithm (A "
        "parameter) to use!"
      );
    }

    /* "a-parameter" */
    a_param = vars_map["a-parameter"].as<string>();

    if (!universal_set) {
      /* "s-parameter" */
      if (!vars_map.count("s-parameter"))
        throw runtime_error("You must provide the S parameter!");

      s_param = vars_map["s-parameter"].as<int32_t>();

      if (s_param < 1)
        throw runtime_error("The S parameter must be positive!");

      /* "n-parameter" */
      if (!vars_map.count("n-parameter"))
        throw runtime_error("You must provide the N parameter!");

      n_param = vars_map["n-parameter"].as<int32_t>();

      if (n_param < 0) {
        throw runtime_error(
          "The N parameter must be positive (0 = pixel-level)!"
        );
      }

      /* "p-parameter" */
      if (!vars_map.count("p-parameter"))
        throw runtime_error("You must provide the P parameter!");

      p_param = vars_map["p-parameter"].as<int32_t>();

      if (p_param < 1)
        throw runtime_error("The P parameter must be positive!");

      if (p_param % 2 != 1)
        throw runtime_error("The P parameter must be odd!");
    }
  }

  /* "visualization" */
  bool visualization = vars_map.count("visualization");

  /* Display parameters to the user. */
  cout << "Input sequence: "      << sequence      << endl;
  cout << "   Output path: "      << output        << endl;
  cout << "             A: "      << a_param     << endl;
  cout << "             S: "      << s_param        << endl;
  if (n_param > 0)
  cout << "             N: "      << n_param      << endl;
  else
  cout << "             N: pixel" << endl;
  cout << "             P: "      << p_param        << endl;
  cout << " Visualization: "      << visualization << endl;
  cout << endl;

  /****************************************************************************
   * Reading sequence.                                                        *
   ****************************************************************************/

  VideoCapture decoder(sequence);

  if (!decoder.isOpened())
    throw runtime_error("Cannot open the '" + sequence + "' sequence.");

  int32_t height = decoder.get(CV_CAP_PROP_FRAME_HEIGHT);
  int32_t width  = decoder.get(CV_CAP_PROP_FRAME_WIDTH);

  cout << "Reading sequence " << sequence << "..." << endl;

  cout << "        height: " << height     << endl;
  cout << "         width: " << width      << endl;

  typedef vector<Mat>                                                FramesVec;
  vector<Mat> frames;
  frames.reserve(decoder.get(CV_CAP_PROP_FRAME_COUNT));

  Mat frame;

  while (decoder.read(frame))
    frames.push_back(frame.clone());

  decoder.release();
  cout << frames.size() << " frames read." << endl << endl;

  /****************************************************************************
   * Processing.                                                              *
   ****************************************************************************/

  cout << "Start processing..." << endl;

  /* Initialization of the background matrix. */
  Mat background = Mat(height, width, CV_8UC3);

  /* Initialization of the LaBGen-P algorithm. */
  LaBGen labgen(height, width, a_param, s_param, n_param, p_param);

  /* Processing loop. */
  cout << endl << "Processing..." << endl;
  bool first_frame = true;

  FramesVec::const_iterator begin = frames.begin();
  FramesVec::const_iterator it    = begin;
  FramesVec::const_iterator end   = frames.end();

  for (int32_t pass = 0, passes = (p_param + 1) / 2; pass < passes; ++pass) {
    cout << endl << "Processing pass number ";
    cout << boost::lexical_cast<string>((pass * 2) + 1) << "..." << endl;

    bool forward = true;

    do {
      labgen.insert(*it);

      /* Skipping first frame. */
      if (first_frame) {
        cout << "Skipping first frame..." << endl;

        ++it;
        first_frame = false;

        continue;
      }

      /* Visualization. */
      if (visualization) {
        imshow("Input video", (*it));
        imshow("Segmentation map", labgen.get_segmentation_map());

        labgen.generate_background(background);
        imshow("Estimated background", background);

        cvWaitKey(1);
      }

      /* Move iterator. */
      it = (forward) ? ++it : --it;

      /* If iterator is at the end. */
      if (it == end) {
        /* Kitchen with the iterator. */
        if (pass == (passes - 1))
          break;

        it -= 2;
        forward = false;

        cout << endl << "Processing pass number ";
        cout << boost::lexical_cast<string>((pass + 1) * 2) << "..." << endl;
      }
    } while (it != begin);
  }

  /* Compute background and write it. */
  stringstream output_file;
  output_file << output << "/output_" << a_param << "_" << s_param << "_" <<
                 n_param << "_" << p_param << ".png";

  labgen.generate_background(background);

  cout << "Writing " << output_file.str() << "..." << endl;
  imwrite(output_file.str(), background);

  /* Cleaning. */
  if (visualization) {
    cout << endl << "Press any key to quit..." << endl;
    cvWaitKey(0);
    cvDestroyAllWindows();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
