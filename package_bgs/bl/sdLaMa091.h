/*!
 * \file   sdLaMa091.h
 * \brief  Interface of a Sigma-Delta background subtraction algorithm.
 * \author Benjamin Laugraud
 *
 * This file is an interface for a Sigma-Delta background subtraction algorithm.
 * The implementation is based on the section 2.1 from the article:
 *
 * MOTION DETECTION: FAST AND ROBUST ALGORITHMS FOR EMBEDDED SYSTEMS
 *
 * written by L. Lacassage and A. Manzanera in 2009.
 *
 * <b>Example of code</b>
 *
 * The following code snippet shows a brief example of how to use this library
 * for monochromatic images:
 *
 * \code
 * #include <stdint.h>
 * #include <stdlib.h>
 *
 * #include "sdLaMa091.h"
 *
 * // Allocation.
 * sdLaMa091_t* sdLaMa091 = sdLaMa091New();
 *
 * // Initialization.
 * uint8_t* image_data = getFirstFrame();
 * uint32_t width = getWidth();
 * uint32_t height = getHeight();
 * uint32_t stride = getStride();
 * sdLaMa091AllocInit_8u_C1R(sdLaMa091, image_data, width, height, stride);
 *
 * // Create segmentation map.
 * uint8_t* segmentation_map = (uint8_t*)malloc(stride * height);
 *
 * // Update the algorithm.
 * while ("We are not at the last frame") {
 *   image_data = getNextFrame();
 *   sdLaMa091Update_8u(sdLaMa091, image_data);
 *   sdLaMa091Segmentation_8u(sdLaMa091, segmentation_map);
 *   // Code to use the segmentation map.
 * }
 *
 * // Cleaning
 * free(segmentation_map);
 * sdLaMa091Free(sdLaMa091);
 * \endcode
 */
#ifndef _SD_LA_MA_091_H_
#define _SD_LA_MA_091_H_

#ifndef    NDEBUG
#include <errno.h>
#endif  /* NDEBUG */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef    NDEBUG
typedef enum {
  SD_LA_MA_091_ALLOC_STRUCT, // Problemes d'allocation
  SD_LA_MA_091_ALLOC_MT,
  SD_LA_MA_091_ALLOC_OT,
  SD_LA_MA_091_ALLOC_VT,
  SD_LA_MA_091_BAD_VARIANCE,
  SD_LA_MA_091_NULL_STRUCT,  // Paramètres null
  SD_LA_MA_091_NULL_DATA,
  SD_LA_MA_091_NULL_MT,
  SD_LA_MA_091_NULL_OT,
  SD_LA_MA_091_NULL_VT,
  SD_LA_MA_091_ZERO_WIDTH,   // Valeurs 0
  SD_LA_MA_091_ZERO_HEIGHT,
  SD_LA_MA_091_ZERO_AMP_FACTOR,
  SD_LA_MA_091_ZERO_MAX_VARIANCE,
  SD_LA_MA_091_ZERO_MIN_VARIANCE,
  SD_LA_MA_091_LITTLE_STRIDE // Stride inférieur à la largeur
} sdLaMa091_errno_t;
#endif  /* NDEBUG */

/******************************************************************************
 * Macro(s) for implementation                                                *
 ******************************************************************************/

//! Value in the segmentation map which represents the background.
#define BACKGROUND                                                            0
//! Value in the segmentation map which represents the foreground.
#define FOREGROUND                                                          255

/******************************************************************************
 * Structure(s)                                                               *
 ******************************************************************************/

/*!
 * \brief  Structure of the algorithm.
 *
 * sdLaMa091 is a structure which contains the informations needed by the
 * the implementation.
 */
typedef struct sdLaMa091 sdLaMa091_t;

/******************************************************************************
 * Allocation(s) function(s)                                                  *
 ******************************************************************************/

/*!
 * \brief  Allocates an empty sdLaMa091 structure.
 *
 * This function allocates an empty sdLaMa091 structure. The
 * amplification factor (N), maximal variance (Vmax) and minimal variance (Vmin)
 * will be set to 1, 255 and 2 respectively.
 *
 * \return A pointer to the structure or NULL if something failed.
 */
sdLaMa091_t* sdLaMa091New(void);

/*!
 * \brief Initializes an empty sdLaMa091 structure for C1R images.
 *
 * This function initializes an empty sdLaMa091 structure for monochromatic
 * images. If an error occurred, a message will be printed on the standard error
 * output (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  image_data
 *         A pointer to the first frame to initialize the algorithm.
 *
 * \param  width
 *         The width of the sequence.
 *
 * \param  height
 *         The height of the sequence.
 *
 * \param  stride
 *         The stride of the sequence.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Init_8u_C1R(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride
);

/*!
 * \brief  Initializes an empty sdLaMa091 structure for C3R images.
 *
 * This function initializes an empty sdLaMa091 structure for trichromatic
 * images. If an error occurred, a message will be printed on the standard error
 * output (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  image_data
 *         A pointer to the first frame to initialize the algorithm.
 *
 * \param  width
 *         The width of the sequence.
 *
 * \param  height
 *         The height of the sequence.
 *
 * \param  stride
 *         The stride of the sequence.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Init_8u_C3R(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride
);

/******************************************************************************
 * Parameter(s) function(s)                                                   *
 ******************************************************************************/

/*!
 * \brief  Sets the amplification factor.
 *
 * This function sets the amplification factor (N) of a sdLaMa091 structure. The
 * author advises to set this factor between 1 and 4. If an error occurred, a
 * message will be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  ampFactor
 *         The new amplification factor to set.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetAmplificationFactor(
  sdLaMa091_t* sdLaMa091,
  const uint8_t ampFactor
);

/*!
 * \brief  Gets the amplification factor.
 *
 * This function gets the amplification factor (N) of a sdLaMa091 structure. If
 * an error occurred, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \return The amplification factor (N) of the given sdLaMa091. If an error
 *         occurred, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint8_t sdLaMa091GetAmplificationFactor(const sdLaMa091_t* sdLaMa091);

/*!
 * \brief  Sets the maximal variance.
 *
 * This function sets the maximal variance (Vmax) of a sdLaMa091 structure. The
 * author advices to set this factor to 255. If an error occured, a message will
 * be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  maxVariance
 *         The new maximal variance to set.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetMaximalVariance(
  sdLaMa091_t* sdLaMa091,
  const uint8_t maxVariance
);

/*!
 * \brief  Gets the maximal variance.
 *
 * This function gets the maximal variance (Vmax) of a sdLaMa091 structure. If
 * an error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \return The maximal variance (Vmax) of the given sdLaMa091. If an error
 *         occured, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint8_t sdLaMa091GetMaximalVariance(const sdLaMa091_t* sdLaMa091);

/*!
 * \brief  Sets the minimal variance.
 *
 * This function sets the minimal variance (Vmin) of a sdLaMa091 structure. The
 * author advices to set this factor to 2. If an error occured, a message will
 * be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  minimalVariance
 *         The new minimal variance to set.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetMinimalVariance(
  sdLaMa091_t* sdLaMa091,
  const uint8_t minVariance
);

/*!
 * \brief  Gets the minimal variance.
 *
 * This function gets the minimal variance (Vmin) of a sdLaMa091 structure. If
 * an error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \return The minimal variance (Vmin) of the given sdLaMa091. If an error
 *         occured, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint8_t sdLaMa091GetMinimalVariance(const sdLaMa091_t* sdLaMa091);

/******************************************************************************
 * Background model function(s)                                               *
 ******************************************************************************/

uint32_t sdLaMa091GetBackgroundModel(
  const sdLaMa091_t* sdLaMa091,
  uint8_t* background_model
);

/******************************************************************************
 * Model update function(s)                                                   *
 ******************************************************************************/

/*!
 * \brief  Updates the algorithm for C1R images.
 *
 * This function updates the algorithm with the next frame. The result of the
 * algorithm will be sets to the content of the segmentation_map pointer. This
 * pointer has to contain a vector of uint8_t elements which has the same size
 * of the different frames (stride * height, exactly). If a pixel is in the
 * foreground, the associated pixel in the segmentation map will have the
 * FOREGROUND value. In the contrary, it will have the BACKGROUND value. If an
 * error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * This function is designed for monochromatic images.
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \param  image_data
 *         A pointer to the next frame.
 *
 * \param  segmentation_map
 *         A pointer to the segmentation map.
 *
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Update_8u(
  sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data
);

/******************************************************************************
 * Segmentation function(s)                                                   *
 ******************************************************************************/

int32_t sdLaMa091Segmentation_8u(
  sdLaMa091_t* sdLaMa091,
  uint8_t* segmentation_map
);

/******************************************************************************
 * Free memory function(s)                                                    *
 ******************************************************************************/

/*!
 * \brief  Cleans the different allocations.
 *
 * This function cleans the different allocations related to a sdLaMa091
 * structure and to the structure itself.
 *
 * \param  sdLaMa091
 *         A sdLaMa091 structure.
 *
 * \return EXIT_SUCCESS if everything was ok. If an error occurred, EXIT_FAILURE
 *         will be returned and errno will be set to SD_LA_MA_091_NULL_STRUCT if
 *         the sdLaMa091 parameter is NULL.
 */
int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091);

#endif /* _SD_LA_MA_091_H_ */
