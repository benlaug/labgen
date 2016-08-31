/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/research/sbg
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
 * along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <cv.h>
#include <highgui.h>

#include "package_bgs/IBGS.h"
#include "package_bgs/FrameDifferenceBGS.h"
#include "package_bgs/dp/DPGrimsonGMMBGS.h"
#include "package_bgs/dp/DPZivkovicAGMMBGS.h"
#include "package_bgs/dp/DPWrenGABGS.h"
#include "package_bgs/dp/DPTextureBGS.h"
#include "package_bgs/lb/LBAdaptiveSOM.h"
#include "package_bgs/av/VuMeter.h"
#include "package_bgs/ae/KDE.h"
#include "package_bgs/bl/SigmaDeltaBGS.h"
#include "package_bgs/pl/SuBSENSE.h"

#include "History.hpp"
#include "Utils.hpp"

using namespace std;
using namespace boost;
using namespace boost::program_options;

/******************************************************************************
 * Main program                                                               *
 ******************************************************************************/

int main(int argc, char** argv) {
  /***************************************************************************
   * Argument(s) handling.                                                   *
   ***************************************************************************/

  options_description optDesc(
    string("LaBGen - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016\n") +
    "http://www.montefiore.ulg.ac.be/~blaugraud\n"                                  +
    "http://www.telecom.ulg.ac.be/research/sbg\n\n"                                 +
    "Usage: LaBGen [options]"
  );

  optDesc.add_options()
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

  variables_map varsMap;
  store(parse_command_line(argc, argv, optDesc), varsMap);
  notify(varsMap);

  /* Help message. */
  if (varsMap.count("help")) {
    cout << optDesc << endl;
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
  cout << "= http://www.telecom.ulg.ac.be/research/sbg               =" << endl;
  cout << "===========================================================" << endl;
  cout << endl;

  /*
   * Extract parameters and sanity check.
   */
  string algorithm = "";
  int32_t sParam = 0;
  int32_t nParam = 0;
  int32_t pParam = 0;

  /* "input" */
  if (!varsMap.count("input"))
    throw runtime_error("You must provide the path of the input sequence!");

  string sequence(varsMap["input"].as<string>());

  /* "output" */
  if (!varsMap.count("output"))
    throw runtime_error("You must provide the path of the output folder!");

  string output(varsMap["output"].as<string>());

  /* "default" */
  bool defaultSet = varsMap.count("default");

  if (defaultSet) {
    algorithm = "frame_difference";
    sParam = 57;
    nParam = 4;
    pParam = 29;
  }

  /* "default" */
  bool universalSet = varsMap.count("universal");

  if (universalSet) {
    sParam = 19;
    nParam = 2;
    pParam = 1;
  }

  if (defaultSet && universalSet) {
    throw runtime_error(
      "You cannot use the universal and default set of parameters in the same time!"
    );
  }

  /* "a-parameter" */
  if (!defaultSet) {
    if (!defaultSet && !varsMap.count("a-parameter")) {
      throw runtime_error(
        "You must provide the name of the background subtraction algorithm (A parameter) to use!"
      );
    }

    algorithm = varsMap["a-parameter"].as<string>();

    if (!universalSet) {
      /* "s-parameter" */
      if (!varsMap.count("s-parameter"))
        throw runtime_error("You must provide the S parameter!");

      sParam = varsMap["s-parameter"].as<int32_t>();

      if (sParam < 1)
        throw runtime_error("The S parameter must be positive!");

      /* "n-parameter" */
      if (!varsMap.count("n-parameter"))
        throw runtime_error("You must provide the N parameter!");

      nParam = varsMap["n-parameter"].as<int32_t>();

      if (nParam < 0)
        throw runtime_error("The N parameter must be positive (0 = pixel-level)!");

      /* "p-parameter" */
      if (!varsMap.count("p-parameter"))
        throw runtime_error("You must provide the P parameter!");

      pParam = varsMap["p-parameter"].as<int32_t>();

      if (pParam < 1)
        throw runtime_error("The P parameter must be positive!");

      if (pParam % 2 != 1)
        throw runtime_error("The P parameter must be odd!");
    }
  }

  /* "visualization" */
  bool visualization = varsMap.count("visualization");

  /* Display parameters to the user. */
  cout << "Input sequence: "      << sequence      << endl;
  cout << "   Output path: "      << output        << endl;
  cout << "             A: "      << algorithm     << endl;
  cout << "             S: "      << sParam        << endl;
  if (nParam > 0)
  cout << "             N: "      << nParam      << endl;
  else
  cout << "             N: pixel" << endl;
  cout << "             P: "      << pParam        << endl;
  cout << " Visualization: "      << visualization << endl;
  cout << endl;

  /***************************************************************************
   * Reading sequence.                                                       *
   ***************************************************************************/

  cv::VideoCapture decoder(sequence);

  if (!decoder.isOpened())
    throw runtime_error("Cannot open the '" + sequence + "' sequence.");

  int32_t height     = decoder.get(CV_CAP_PROP_FRAME_HEIGHT);
  int32_t width      = decoder.get(CV_CAP_PROP_FRAME_WIDTH);

  cout << "Reading sequence " << sequence << "..." << endl;

  cout << "          height: " << height     << endl;
  cout << "           width: " << width      << endl;

  typedef vector<cv::Mat>                                            FramesVec;
  vector<cv::Mat> frames;
  frames.reserve(decoder.get(CV_CAP_PROP_FRAME_COUNT));

  cv::Mat frame;

  while (decoder.read(frame))
    frames.push_back(frame.clone());

  decoder.release();
  cout << frames.size() << " frames read." << endl << endl;

  /***************************************************************************
   * Processing.                                                             *
   ***************************************************************************/

  cout << "Start processing..." << endl;

  /* Initialization of the background matrix. */
  cv::Mat background = cv::Mat(height, width, CV_8UC3);

  /* Initialization of the ROIs. */
  Utils::ROIs rois = Utils::getROIs(height, width, nParam);

  /* Initialization of the segmentation map matrix. */
  cv::Mat segmentationMap = cv::Mat(height, width, CV_8UC1);

  /* Initialization of the history structure. */
  boost::shared_ptr<PatchesHistory> history =
    boost::make_shared<PatchesHistory>(rois, sParam);

  /* Misc initializations. */
  cv::Mat fake;
  boost::shared_ptr<IBGS> algo;
  bool firstFrame = true;

  FramesVec::const_iterator begin = frames.begin();
  FramesVec::const_iterator it    = begin;
  FramesVec::const_iterator end   = frames.end();

  int32_t passes = (pParam + 1) / 2;

  /* Processing loop. */
  for (int32_t pass = 0; pass < passes; ++pass) {
    cout << endl << "Processing pass number ";
    cout << boost::lexical_cast<string>((pass * 2) + 1) << "..." << endl;

    bool forward = true;

    do {
      /* Algorithm instantiation. */
      if (firstFrame) {
        if (algorithm == "frame_difference")
          algo = boost::shared_ptr<IBGS>(new FrameDifferenceBGS);
        else if (algorithm == "mog_grimson")
          algo = boost::shared_ptr<IBGS>(new DPGrimsonGMMBGS);
        else if (algorithm == "mog_zivkovic")
          algo = boost::shared_ptr<IBGS>(new DPZivkovicAGMMBGS);
        else if (algorithm == "pfinder")
          algo = boost::shared_ptr<IBGS>(new DPWrenGABGS);
        else if (algorithm == "lbp")
          algo = boost::shared_ptr<IBGS>(new DPTextureBGS);
        else if (algorithm == "som_adaptive")
          algo = boost::shared_ptr<IBGS>(new LBAdaptiveSOM);
        else if (algorithm == "vumeter")
          algo = boost::shared_ptr<IBGS>(new VuMeter);
        else if (algorithm == "kde")
          algo = boost::shared_ptr<IBGS>(new KDE);
        else if (algorithm == "sigma_delta")
          algo = boost::shared_ptr<IBGS>(new SigmaDeltaBGS);
        else if (algorithm == "subsense")
          algo = boost::shared_ptr<IBGS>(new SuBSENSEBGS);
        else
          throw runtime_error("The algorithm " + algorithm + " is not supported.");
      }

      /* Background subtraction. */
      algo->process((*it).clone(), segmentationMap, fake);

      /* Visualization of the input frame and its segmentation map. */
      if (visualization) {
        imshow("Input video", (*it));

        if (!segmentationMap.empty())
          imshow("Segmentation map", segmentationMap);

        cvWaitKey(1);
      }

      /* Skipping first frame. */
      if (firstFrame) {
        cout << "Skipping first frame..." << endl;

        ++it;
        firstFrame = false;

        continue;
      }

      /* Insert the current frame and its segmentation map into the history. */
      history->insert(segmentationMap, (*it));

      if (visualization) {
        history->median(background, sParam);

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
  string outputFile =
    output + "/" + "output"      + "_"   +
    algorithm                    + "_"   +
    lexical_cast<string>(sParam) + "_"   +
    lexical_cast<string>(nParam) + "_"   +
    lexical_cast<string>(pParam) + ".png";

  history->median(background, sParam);

  cout << "Writing " << outputFile << "..." << endl;
  cv::imwrite(outputFile, background);

  /* Cleaning. */
  if (visualization) {
    cout << endl << "Press any key to quit..." << endl;
    cvWaitKey(0);
    cvDestroyAllWindows();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
