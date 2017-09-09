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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <labgen/LaBGen.hpp>
#include <labgen/GridWindow.hpp>
#include <labgen/TextProperties.hpp>

using namespace cv;
using namespace std;
using namespace boost;
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
    "LaBGen - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017\n"
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
    (
      "record,r",
      value<vector<string>>()->multitoken(),
      "record visualization window in a video file"
    )
    (
      "split-vis,l",
      "split the visualization items in separated windows"
    )
    (
      "wait,w",
      value<int32_t>()->default_value(1),
      "time to wait (in ms) between the processing of two frames with "
      "visualization"
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
  cout << "= Copyright - Benjamin Laugraud - 2017                    =" << endl;
  cout << "= http://www.montefiore.ulg.ac.be/~blaugraud              =" << endl;
  cout << "= http://www.telecom.ulg.ac.be/labgen                     =" << endl;
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

  /* "split-vis" */
  bool split_vis = vars_map.count("split-vis");

  if (split_vis && !visualization) {
    cerr << "/!\\ The split-vis option without visualization will be ignored!";
    cerr << endl << endl;
  }

  /* "record" */
  bool record = vars_map.count("record");

  string record_path = "";
  int32_t record_height = 0;
  int32_t record_width = 0;

  if (record) {
    if (!visualization)
      throw runtime_error("A record cannot be done without visualization");

    if (split_vis)
      throw runtime_error("A record cannot be done with split windows");

    vector<string> record_args = vars_map["record"].as<vector<string>>();

    if (record_args.size() != 1 && record_args.size() != 3) {
      throw runtime_error(
        "1 or 3 arguments must be given to records: <path_to_video_file> "
        "[<height>] [<width>]"
      );
    }

    record_path = record_args[0];

    if (record_args.size() == 3) {
      try {
        record_height = lexical_cast<int32_t>(record_args[1]);
      }
      catch (bad_lexical_cast& e) {
        throw runtime_error("The height of the video file is not an integer");
      }

      if (record_height < 1)
        throw runtime_error("The height of the video file must be positive");

      try {
        record_width = lexical_cast<int32_t>(record_args[2]);
      }
      catch (bad_lexical_cast& e) {
        throw runtime_error("The width of the video file is not an integer");
      }

      if (record_width < 1)
        throw runtime_error("The width of the video file must be positive");
    }
  }

  /* "wait" */
  int32_t wait = vars_map["wait"].as<int32_t>();

  if ((wait != 1) && !visualization) {
    cerr << "/!\\ The wait option without visualization will be ignored!";
    cerr << endl << endl;
  }

  if ((wait < 0) && visualization) {
    throw runtime_error(
      "The wait parameter must be positive!"
    );
  }

  /* Display parameters to the user. */
  cout << "Input sequence: "      << sequence      << endl;
  cout << "   Output path: "      << output        << endl;
  cout << "             A: "      << a_param       << endl;
  cout << "             S: "      << s_param       << endl;
  if (n_param > 0)
  cout << "             N: "      << n_param       << endl;
  else
  cout << "             N: pixel" << endl;
  cout << "             P: "      << p_param       << endl;
  cout << " Visualization: "      << visualization << endl;
  if (visualization)
  cout << "     Split vis: "      << split_vis     << endl;
  if (record)
  cout << "   Record path: "      << record_path   << endl;
  if (record_height > 0)
  cout << " Record height: "      << record_height << endl;
  if (record_width > 0)
  cout << "  Record width: "      << record_width  << endl;
  if (visualization)
  cout << "     Wait (ms): "      << wait          << endl;
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

  unique_ptr<GridWindow> window;
  unique_ptr<VideoWriter> record_stream;

  if (visualization && !split_vis) {
    TextProperties::TextPropertiesPtr t_prop = make_shared<TextProperties>();

    window = unique_ptr<GridWindow>(
      new GridWindow(
        "LaBGen",
        record ? record_height : height,
        record ? record_width : width,
        1,
        3,
        t_prop
      )
    );

    if (record) {
      const Mat& buffer = window->get_buffer();

      record_stream = unique_ptr<VideoWriter>(
        new VideoWriter(
          record_path,
          CV_FOURCC('X','2','6','4'),
          10,
          Size(buffer.cols, buffer.rows)
        )
      );
    }
  }

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
        labgen.generate_background(background);

        if (split_vis) {
          imshow("Input video", *it);
          imshow("Segmentation map", labgen.get_segmentation_map());
          imshow("Estimated background", background);
        }
        else {
          window->display(*it, 0);
          window->put_title("Input video", 0);

          window->display(labgen.get_segmentation_map(), 1);
          window->put_title("Segmentation map", 1);

          window->display(background, 2);
          window->put_title("Estimated backgroundaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 2);

          if (record)
            *record_stream << window->get_buffer();
        }

        waitKey(wait);
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
    waitKey(0);
    destroyAllWindows();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
