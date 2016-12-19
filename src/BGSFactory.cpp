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
#include <stdexcept>

#include <FrameDifferenceBGS.h>
#include <dp/DPGrimsonGMMBGS.h>
#include <dp/DPZivkovicAGMMBGS.h>
#include <dp/DPWrenGABGS.h>
#include <dp/DPTextureBGS.h>
#include <lb/LBAdaptiveSOM.h>
#include <av/VuMeter.h>
#include <ae/KDE.h>
#include <bl/SigmaDeltaBGS.h>
#include <pl/SuBSENSE.h>

#include <labgen/BGSFactory.hpp>

using namespace std;
using namespace ns_labgen::ns_internals;

/* ========================================================================== *
 * BGSFactory                                                                 *
 * ========================================================================== */

shared_ptr<IBGS> BGSFactory::get_bgs_algorithm(string algorithm) {
  if (algorithm == "frame_difference")
    return shared_ptr<IBGS>(new FrameDifferenceBGS);
  else if (algorithm == "mog_grimson")
    return shared_ptr<IBGS>(new DPGrimsonGMMBGS);
  else if (algorithm == "mog_zivkovic")
    return shared_ptr<IBGS>(new DPZivkovicAGMMBGS);
  else if (algorithm == "pfinder")
    return shared_ptr<IBGS>(new DPWrenGABGS);
  else if (algorithm == "lbp")
    return shared_ptr<IBGS>(new DPTextureBGS);
  else if (algorithm == "som_adaptive")
    return shared_ptr<IBGS>(new LBAdaptiveSOM);
  else if (algorithm == "vumeter")
    return shared_ptr<IBGS>(new VuMeter);
  else if (algorithm == "kde")
    return shared_ptr<IBGS>(new KDE);
  else if (algorithm == "sigma_delta")
    return shared_ptr<IBGS>(new SigmaDeltaBGS);
  else if (algorithm == "subsense")
    return shared_ptr<IBGS>(new SuBSENSEBGS);
  else {
    throw runtime_error(
      "The BGS algorithm " + algorithm + " is not supported."
    );
  }
}
