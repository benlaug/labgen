/*!
 * \file   sdLaMa091.c
 * \brief  Implementation of a Sigma-Delta background subtraction algorithm.
 * \author Benjamin Laugraud
 *
 * This file implements the Sigma-Delta background subtraction algorithm. The
 * implementation is based on the section 2.1 from the article:
 *
 * MOTION DETECTION: FAST AND ROBUST ALGORITHMS FOR EMBEDDED SYSTEMS
 *
 * written by L. Lacassage and A. Manzanera in 2009.
 *
 * You have to take a look at the sdLaMa091.h file for further documentation.
 */
#include "sdLaMa091.h"

/******************************************************************************
 * Macro(s) related to the default values of the parameters                   *
 ******************************************************************************/

//! Default amplification factor.
#define DEFAULT_N                                                             1
//! Default minimal variance.
#define DEFAULT_VMIN                                                          2
//! Default maximal variance.
#define DEFAULT_VMAX                                                        255

/******************************************************************************
 * Internal macro(s) of the implementation                                    *
 ******************************************************************************/

//! Number of channels in a trichromatic image.
#define C3_CHANNELS                                                           3
//! Number of channels in a monochromatic image.
#define C1_CHANNELS                                                           1

/******************************************************************************
 * Structure(s)                                                               *
 ******************************************************************************/

struct sdLaMa091 {
  //! Width of the sequence.
  uint32_t width;
  //! Height of the sequence.
  uint32_t height;
  //! Stride of a frame in the sequence.
  uint32_t stride;
  //! Number of channels in a frame of the sequence.
  uint32_t channels;
  //! Total number of bytes in a frame the sequence.p VC++ happy if yo
  uint32_t numBytes;
  //! Unused bytes in a row of a frame in the sequence.
  uint32_t unusedBytes;

  //! Amplification factor.
  uint8_t N;
  //! Minimal variance.
  uint8_t Vmin;
  //! Maximal variance.
  uint8_t Vmax;

  //! Background estimator.
  uint8_t* Mt;
  //! Sigma-Delta estimator.
  uint8_t* Ot;
  //! Variance estimator (dispersion parameter).
  uint8_t* Vt;
};

/******************************************************************************
 * Inline function(s)                                                         *
 ******************************************************************************/

/*!
 * \brief  Finds the minimum.
 *
 * Function to find the minimum value between two uint8_t parameters.
 *
 * \param  a
 *         First uint8_t parameter.
 *
 * \param  b
 *         Second uint8_t parameter.
 *
 * \return The minimum value between a and b.
 */
static inline uint8_t min(uint8_t a, uint8_t b) {
  return (a < b) ? a : b;
}

/*!
 * \brief  Finds the maximum.
 *
 * Function to find the maximum value between two uint8_t parameters.
 *
 * \param  a
 *         First uint8_t parameter.
 *
 * \param  b
 *         Second uint8_t parameter.
 *
 * \return The maximum value between a and b.
 */
static inline uint8_t max(uint8_t a, uint8_t b) {
  return (a > b) ? a : b;
}

/******************************************************************************
 * Allocation(s) function(s)                                                  *
 ******************************************************************************/

sdLaMa091_t* sdLaMa091New(void) {
  sdLaMa091_t* sdLaMa091 = (sdLaMa091_t*)malloc(sizeof(*sdLaMa091));

#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_ALLOC_STRUCT;
    return NULL;
  }
#endif  /* NDEBUG */

  sdLaMa091->width       = 0;
  sdLaMa091->height      = 0;
  sdLaMa091->stride      = 0;
  sdLaMa091->channels    = 0;
  sdLaMa091->numBytes    = 0;
  sdLaMa091->unusedBytes = 0;

  sdLaMa091->N           = DEFAULT_N;
  sdLaMa091->Vmin        = DEFAULT_VMIN;
  sdLaMa091->Vmax        = DEFAULT_VMAX;

  sdLaMa091->Mt          = NULL;
  sdLaMa091->Ot          = NULL;
  sdLaMa091->Vt          = NULL;

  return sdLaMa091;
}

/******************************************************************************/

int32_t sdLaMa091Init_8u_C1R(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (image_data == NULL) {
    errno = SD_LA_MA_091_NULL_DATA;
    return EXIT_FAILURE;
  }

  if (width == 0) {
    errno = SD_LA_MA_091_ZERO_WIDTH;
    return EXIT_FAILURE;
  }

  if (height == 0) {
    errno = SD_LA_MA_091_ZERO_HEIGHT;
    return EXIT_FAILURE;
  }

  if (stride < width) {
    errno = SD_LA_MA_091_LITTLE_STRIDE;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  /* Initialization. */
  sdLaMa091->width       = width;
  sdLaMa091->height      = height;
  sdLaMa091->stride      = stride;
  sdLaMa091->channels    = C1_CHANNELS;
  sdLaMa091->numBytes    = stride * height;
  sdLaMa091->unusedBytes = stride - sdLaMa091->width;

  /* Initialization of Mt with a copy of the first frame. */
  sdLaMa091->Mt = (uint8_t*)malloc(sdLaMa091->numBytes);

#ifndef    NDEBUG
  if (sdLaMa091->Mt == NULL) {
    errno = SD_LA_MA_091_ALLOC_MT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  memcpy(sdLaMa091->Mt, image_data, sdLaMa091->numBytes);

  /* Initialization of Ot with zero values. */
  sdLaMa091->Ot = (uint8_t*)malloc(sdLaMa091->numBytes);

#ifndef    NDEBUG
  if (sdLaMa091->Ot == NULL) {
    errno = SD_LA_MA_091_ALLOC_OT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  memset(sdLaMa091->Ot, 0, sdLaMa091->numBytes);

  /* Initialization of Vt with Vmin values. */
  sdLaMa091->Vt = (uint8_t*)malloc(sdLaMa091->numBytes);

#ifndef    NDEBUG
  if (sdLaMa091->Vt == NULL) {
    errno = SD_LA_MA_091_ALLOC_VT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  memset(sdLaMa091->Vt, sdLaMa091->Vmin, sdLaMa091->numBytes);

  return EXIT_SUCCESS;
}

/******************************************************************************/

int32_t sdLaMa091Init_8u_C3R(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride
) {
  int32_t success = sdLaMa091Init_8u_C1R(
    sdLaMa091,
    image_data,
    width * C3_CHANNELS,
    height,
    stride
  );

  if (success == EXIT_SUCCESS)
    sdLaMa091->channels = C3_CHANNELS;

  return success;
}

/******************************************************************************
 * Parameter(s) function(s)                                                   *
 ******************************************************************************/

int32_t sdLaMa091SetAmplificationFactor(
  sdLaMa091_t* sdLaMa091,
  const uint8_t ampFactor
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (ampFactor == 0) {
    errno = SD_LA_MA_091_ZERO_AMP_FACTOR;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  sdLaMa091->N = ampFactor;

  return EXIT_SUCCESS;
}

/******************************************************************************/

uint8_t sdLaMa091GetAmplificationFactor(const sdLaMa091_t* sdLaMa091) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  return sdLaMa091->N;
}

/******************************************************************************/

int32_t sdLaMa091SetMaximalVariance(
  sdLaMa091_t* sdLaMa091,
  const uint8_t maxVariance
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (maxVariance == 0) {
    errno = SD_LA_MA_091_ZERO_MAX_VARIANCE;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  sdLaMa091->Vmax = maxVariance;

  return EXIT_SUCCESS;
}

/******************************************************************************/

uint8_t sdLaMa091GetMaximalVariance(const sdLaMa091_t* sdLaMa091) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  return sdLaMa091->Vmax;
}

/******************************************************************************/

int32_t sdLaMa091SetMinimalVariance(
  sdLaMa091_t* sdLaMa091,
  const uint8_t minVariance
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (minVariance == 0) {
    errno = SD_LA_MA_091_ZERO_MIN_VARIANCE;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  sdLaMa091->Vmin = minVariance;

  return EXIT_SUCCESS;
}

/******************************************************************************/

uint8_t sdLaMa091GetMinimalVariance(const sdLaMa091_t* sdLaMa091) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  return sdLaMa091->Vmin;
}

/******************************************************************************
 * Background model function(s)                                               *
 ******************************************************************************/

uint32_t sdLaMa091GetBackgroundModel(
  const sdLaMa091_t* sdLaMa091,
  uint8_t* background_model
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (background_model == NULL) {
    errno = SD_LA_MA_091_NULL_DATA;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Mt == NULL) {
    errno = SD_LA_MA_091_NULL_MT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  uint8_t* workMt = sdLaMa091->Mt;

  /* Row by row. */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row. */
    for (
      uint32_t j = 0;
      j < sdLaMa091->width;
      ++j, ++workMt, ++background_model
    )
      *background_model = *workMt;

    /* Skip end of stride. */
    if (sdLaMa091->unusedBytes > 0) {
      workMt += sdLaMa091->unusedBytes;
      background_model += sdLaMa091->unusedBytes;
    }
  }

  return EXIT_SUCCESS;
}

/******************************************************************************
 * Model update function(s)                                                   *
 ******************************************************************************/

int32_t sdLaMa091Update_8u(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data
) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }

  if (image_data == NULL) {
    errno = SD_LA_MA_091_NULL_DATA;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Mt == NULL) {
    errno = SD_LA_MA_091_NULL_MT;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Ot == NULL) {
    errno = SD_LA_MA_091_NULL_OT;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vt == NULL) {
    errno = SD_LA_MA_091_NULL_VT;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->width == 0) {
    errno = SD_LA_MA_091_ZERO_WIDTH;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->height == 0) {
    errno = SD_LA_MA_091_ZERO_HEIGHT;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->stride < sdLaMa091->width) {
    errno = SD_LA_MA_091_LITTLE_STRIDE;
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vmax < sdLaMa091->Vmin) {
    errno = SD_LA_MA_091_BAD_VARIANCE;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  /* Update. */
  const uint8_t* workImage = image_data;
  uint8_t* workMt = sdLaMa091->Mt;
  uint8_t* workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  /* Row by row. */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row. */
    for (
      uint32_t j = 0;
      j < sdLaMa091->width;
      ++j, ++workImage, ++workMt, ++workOt, ++workVt
    ) {
      /* Mt estimation (step 1). */
      if (*workMt < *workImage)
        ++(*workMt);
      else if (*workMt > *workImage)
        --(*workMt);

      /* Ot computation (step 2). */
      *workOt = (uint8_t)abs(*workMt - *workImage);

      /* Vt update (step 3). */
      uint32_t ampOt = sdLaMa091->N * *workOt;

      if (*workVt < ampOt)
        ++(*workVt);
      else if (*workVt > ampOt)
        --(*workVt);

      *workVt = max(min(*workVt, sdLaMa091->Vmax), sdLaMa091->Vmin);
    }

    /* Skip end of stride. */
    if (sdLaMa091->unusedBytes > 0) {
      workImage += sdLaMa091->unusedBytes;
      workMt += sdLaMa091->unusedBytes;
      workOt += sdLaMa091->unusedBytes;
      workVt += sdLaMa091->unusedBytes;
    }
  }

  return EXIT_SUCCESS;
}

/******************************************************************************
 * Segmentation function(s)                                                   *
 ******************************************************************************/

int32_t sdLaMa091Segmentation_8u(
  sdLaMa091_t* sdLaMa091,
  uint8_t* segmentation_map
) {
  /* Et estimation (step 4). */
  uint8_t* workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  /* Row by row. */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* To know whether the current pixel is from foreground or not. */
    bool isForeground = false;

    /* Significant bytes of a row. */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workOt, ++workVt) {
      isForeground |= (*workOt >= *workVt);

      /* Populate segmentation map. */
      if ((j % sdLaMa091->channels) == (sdLaMa091->channels - 1)) {
        *segmentation_map = (isForeground) ? FOREGROUND : BACKGROUND;
        ++segmentation_map;
        isForeground = false;
      }
    }

    /* Skip end of stride. */
    if (sdLaMa091->unusedBytes > 0) {
      workOt += sdLaMa091->unusedBytes;
      workVt += sdLaMa091->unusedBytes;
    }
  }
}

/******************************************************************************
 * Free memory function(s)                                                    *
 ******************************************************************************/

int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091) {
  /* Sanity check. */
#ifndef    NDEBUG
  if (sdLaMa091 == NULL) {
    errno = SD_LA_MA_091_NULL_STRUCT;
    return EXIT_FAILURE;
  }
#endif  /* NDEBUG */

  free(sdLaMa091->Mt);
  free(sdLaMa091->Ot);
  free(sdLaMa091->Vt);

  free(sdLaMa091);

  return EXIT_SUCCESS;
}
