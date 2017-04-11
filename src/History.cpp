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

#include <labgen/History.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen::ns_internals;

/* ========================================================================== *
 * HistoryMat                                                                 *
 * ========================================================================== */

HistoryMat::HistoryMat(const Mat& mat, const uint32_t positives) :
mat(mat.clone()), positives(positives) {}

/******************************************************************************/

HistoryMat::HistoryMat(const HistoryMat& copy) :
mat(copy.mat.clone()), positives(copy.positives) {}

/******************************************************************************/

HistoryMat& HistoryMat::operator=(const HistoryMat& copy) {
  if (this != &copy) {
    copy.mat.copyTo(mat);
    positives = copy.positives;
  }

  return *this;
}

/******************************************************************************/

Mat& HistoryMat::operator*() {
  return mat;
}

/******************************************************************************/

const Mat& HistoryMat::operator*() const {
  return mat;
}

/* ========================================================================== *
 * History                                                                    *
 * ========================================================================== */

History::History(size_t buffer_size) : history(), buffer_size(buffer_size) {
  history.reserve(buffer_size + 1);
}

/******************************************************************************/

History::HistoryVec& History::operator*() {
  return history;
}

/******************************************************************************/

const History::HistoryVec& History::operator*() const {
  return history;
}

/******************************************************************************/

void History::insert(const Mat& segmentation_map, const Mat& current_frame) {
  uint32_t positives = countNonZero(segmentation_map);

  if (history.empty())
    history.push_back(HistoryMat(current_frame, positives));
  else {
    bool inserted = false;

    for (
      HistoryVec::iterator it = history.begin(), end = history.end();
      it != end;
      ++it
    ) {
      if (positives <= (*it)) {
        history.insert(it, HistoryMat(current_frame, positives));
        inserted = true;

        if (history.size() > buffer_size)
          history.erase(history.end() - 1);

        break;
      }
    }

    if ((history.size() < buffer_size) && !inserted)
      history.push_back(HistoryMat(current_frame, positives));
  }
}

/******************************************************************************/

void History::median(Mat& result, size_t size) const {
  if (history.size() == 1 || size == 1)
    (*(history[0])).copyTo(result);

  static vector<unsigned char> buffer_r(buffer_size);
  static vector<unsigned char> buffer_g(buffer_size);
  static vector<unsigned char> buffer_b(buffer_size);

  size_t _size = min(history.size(), size);

  for (size_t i = 0; i < ((*(history[0])).total() * 3); i += 3) {
    for (size_t num = 0; num < _size; ++num) {
      buffer_r[num] = (*(history[num])).data[i    ];
      buffer_g[num] = (*(history[num])).data[i + 1];
      buffer_b[num] = (*(history[num])).data[i + 2];
    }

    sort(buffer_r.begin(), buffer_r.begin() + _size);
    sort(buffer_g.begin(), buffer_g.begin() + _size);
    sort(buffer_b.begin(), buffer_b.begin() + _size);

    size_t middle = _size / 2;

    if (_size & 1) {
      nth_element(buffer_r.begin(), buffer_r.begin() + middle, buffer_r.begin() + _size);
      nth_element(buffer_g.begin(), buffer_g.begin() + middle, buffer_g.begin() + _size);
      nth_element(buffer_b.begin(), buffer_b.begin() + middle, buffer_b.begin() + _size);

      result.data[i    ] = buffer_r[middle];
      result.data[i + 1] = buffer_g[middle];
      result.data[i + 2] = buffer_b[middle];
    }
    else {
      nth_element(buffer_r.begin(), buffer_r.begin() + (middle - 1), buffer_r.begin() + _size);
      nth_element(buffer_g.begin(), buffer_g.begin() + (middle - 1), buffer_g.begin() + _size);
      nth_element(buffer_b.begin(), buffer_b.begin() + (middle - 1), buffer_b.begin() + _size);

      nth_element(buffer_r.begin() + middle, buffer_r.begin() + middle, buffer_r.begin() + _size);
      nth_element(buffer_g.begin() + middle, buffer_g.begin() + middle, buffer_g.begin() + _size);
      nth_element(buffer_b.begin() + middle, buffer_b.begin() + middle, buffer_b.begin() + _size);

      result.data[i    ] = ((static_cast<int32_t>(buffer_r[middle - 1])) + (buffer_r[middle])) / 2;
      result.data[i + 1] = ((static_cast<int32_t>(buffer_g[middle - 1])) + (buffer_g[middle])) / 2;
      result.data[i + 2] = ((static_cast<int32_t>(buffer_b[middle - 1])) + (buffer_b[middle])) / 2;
    }
  }
}

/******************************************************************************/

bool History::empty() const {
  return history.empty();
}

/* ========================================================================== *
 * PatchesHistory                                                             *
 * ========================================================================== */

PatchesHistory::PatchesHistory(const Utils::ROIs& rois, size_t buffer_size) :
p_history(), rois(rois) {
  p_history.reserve(rois.size());

  for (size_t i = 0; i < rois.size(); ++i)
    p_history.push_back(History(buffer_size));
}

/****************************************************************************/

void PatchesHistory::insert(const Mat& segmentation_map, const Mat& current_frame) {
  for (size_t i = 0; i < rois.size(); ++i) {
    p_history[i].insert(
      segmentation_map(rois[i]),
      current_frame(rois[i])
    );
  }
}

/****************************************************************************/

void PatchesHistory::median(Mat& result, size_t size) const {
  for (size_t i = 0; i < rois.size(); ++i) {
    Mat patch(
      (*((*(p_history[i])).back())).rows,
      (*((*(p_history[i])).back())).cols,
      CV_8UC3
    );

    p_history[i].median(patch, size);
    patch.copyTo(result(rois[i]));
  }
}

/******************************************************************************/

bool PatchesHistory::empty() const {
 for (History h : p_history) {
   if (h.empty())
     return true;
 }

 return false;
}
