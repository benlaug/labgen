/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/research/sbg
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
 * along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <vector>

#include <cv.h>
#include <highgui.h>

#include "Utils.hpp"

#define CHANNELS                                                              3

/* ========================================================================== *
 * HistoryMat                                                                 *
 * ========================================================================== */

struct HistoryMat {
  cv::Mat mat;
  uint32_t positives;

  /****************************************************************************/

  HistoryMat(const cv::Mat& mat, const uint32_t positives) :
    mat(mat.clone()), positives(positives) {}

  /****************************************************************************/

  HistoryMat(const HistoryMat& copy) :
    mat(copy.mat.clone()), positives(copy.positives) {}

  /****************************************************************************/

  HistoryMat& operator=(const HistoryMat& copy) {
    copy.mat.copyTo(mat);
    positives = copy.positives;

    return *this;
  }
};

/******************************************************************************
 * Operator(s) overloading                                                    *
 ******************************************************************************/

inline bool operator<(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives < rhs.positives;
}

/******************************************************************************/

inline bool operator<=(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives <= rhs.positives;
}

/******************************************************************************/

inline bool operator==(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives == rhs.positives;
}

/******************************************************************************/

inline bool operator<(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives < rhs;
}

/******************************************************************************/

inline bool operator<=(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives <= rhs;
}

/******************************************************************************/

inline bool operator==(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives == rhs;
}

/******************************************************************************/

inline bool operator<(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs < rhs.positives;
}

/******************************************************************************/

inline bool operator<=(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs <= rhs.positives;
}

/******************************************************************************/

inline bool operator==(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs == rhs.positives;
}

/* ========================================================================== *
 * HistoryInterface                                                           *
 * ========================================================================== */

struct HistoryInterface {
  virtual void insert(const cv::Mat& segmentationMap, const cv::Mat& frame) = 0;
  virtual void median(cv::Mat& result, size_t size) const = 0;
};

/* ========================================================================== *
 * History                                                                    *
 * ========================================================================== */

struct History : public HistoryInterface {
  typedef std::vector<HistoryMat>                                   HistoryVec;

  /****************************************************************************/

  HistoryVec history;
  size_t bufferSize;

  /****************************************************************************/

  explicit History(size_t bufferSize) : history(), bufferSize(bufferSize) {
    history.reserve(bufferSize + 1);
  }

  /****************************************************************************/

  HistoryVec& operator*() { return history; }

  /****************************************************************************/

  const HistoryVec& operator*() const { return history; }

  /****************************************************************************/

  virtual void insert(const cv::Mat& segmentationMap, const cv::Mat& frame) {
    uint32_t positives = countNonZero(segmentationMap);

    if (history.empty())
      history.push_back(HistoryMat(frame, positives));
    else {
      bool inserted = false;

      for (
        HistoryVec::iterator it = history.begin(), end = history.end();
        it != end;
        ++it
      ) {
        if (positives <= (*it)) {
          history.insert(it, HistoryMat(frame, positives));
          inserted = true;

          if (history.size() > bufferSize)
            history.erase(history.end() - 1);

          break;
        }
      }

      if ((history.size() < bufferSize) && !inserted)
        history.push_back(HistoryMat(frame, positives));
    }
  }

  /****************************************************************************/

  virtual void median(cv::Mat& result, size_t size) const {
    if (history.size() == 1 || size == 1)
      history[0].mat.copyTo(result);

    static std::vector<unsigned char> bufferR(bufferSize);
    static std::vector<unsigned char> bufferG(bufferSize);
    static std::vector<unsigned char> bufferB(bufferSize);

    size_t _size = std::min(history.size(), size);

    for (size_t i = 0; i < (history[0].mat.total() * CHANNELS); i += CHANNELS) {
      for (size_t num = 0; num < _size; ++num) {
        bufferR[num] = history[num].mat.data[i    ];
        bufferG[num] = history[num].mat.data[i + 1];
        bufferB[num] = history[num].mat.data[i + 2];
      }

      std::sort(bufferR.begin(), bufferR.begin() + _size);
      std::sort(bufferG.begin(), bufferG.begin() + _size);
      std::sort(bufferB.begin(), bufferB.begin() + _size);

      size_t middle = _size / 2;

      if (_size & 1) {
        std::nth_element(bufferR.begin(), bufferR.begin() + middle, bufferR.begin() + _size);
        std::nth_element(bufferG.begin(), bufferG.begin() + middle, bufferG.begin() + _size);
        std::nth_element(bufferB.begin(), bufferB.begin() + middle, bufferB.begin() + _size);

        result.data[i    ] = bufferR[middle];
        result.data[i + 1] = bufferG[middle];
        result.data[i + 2] = bufferB[middle];
      }
      else {
        std::nth_element(bufferR.begin(), bufferR.begin() + (middle - 1), bufferR.begin() + _size);
        std::nth_element(bufferG.begin(), bufferG.begin() + (middle - 1), bufferG.begin() + _size);
        std::nth_element(bufferB.begin(), bufferB.begin() + (middle - 1), bufferB.begin() + _size);

        std::nth_element(bufferR.begin() + middle, bufferR.begin() + middle, bufferR.begin() + _size);
        std::nth_element(bufferG.begin() + middle, bufferG.begin() + middle, bufferG.begin() + _size);
        std::nth_element(bufferB.begin() + middle, bufferB.begin() + middle, bufferB.begin() + _size);

        result.data[i   ] = (((int)bufferR[middle - 1]) + ((int)bufferR[middle])) / 2;
        result.data[i + 1] = (((int)bufferG[middle - 1]) + ((int)bufferG[middle])) / 2;
        result.data[i + 2] = (((int)bufferB[middle - 1]) + ((int)bufferB[middle])) / 2;
      }
    }
  }
};

/* ========================================================================== *
 * PatchesHistory                                                             *
 * ========================================================================== */

struct PatchesHistory : public HistoryInterface {
  typedef std::vector<History>                               PatchesHistoryVec;

  /****************************************************************************/

  PatchesHistoryVec pHistory;
  const Utils::ROIs& rois;

  /****************************************************************************/

  PatchesHistory(const Utils::ROIs& rois, size_t bufferSize) :
    pHistory(), rois(rois) {

    pHistory.reserve(rois.size());

    for (size_t i = 0; i < rois.size(); ++i)
      pHistory.push_back(History(bufferSize));
  }

  /****************************************************************************/

  virtual void insert(const cv::Mat& segmentationMap, const cv::Mat& frame) {
    for (size_t i = 0; i < rois.size(); ++i) {
      pHistory[i].insert(
        segmentationMap(rois[i]),
        frame(rois[i])
      );
    }
  }

  /****************************************************************************/

  virtual void median(cv::Mat& result, size_t size) const {
    for (size_t i = 0; i < rois.size(); ++i) {
      cv::Mat patch(
        pHistory[i].history.back().mat.rows,
        pHistory[i].history.back().mat.cols,
        CV_8UC3
      );

      pHistory[i].median(patch, size);
      patch.copyTo(result(rois[i]));
    }
  }
};
