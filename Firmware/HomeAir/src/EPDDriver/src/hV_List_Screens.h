///
/// @file hV_List_Screens.h
/// @brief List of supported Pervasive Displays screens
/// @note Legacy screens are listed in the corresponding screen libraries
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @n Content
/// * 1- List of supported Pervasive Displays screens
///
/// @author Rei Vilo
/// @date 21 Nov 2023
/// @version 702
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright All rights reserved
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// * Evaluation edition: for professionals or organisations, evaluation only, no commercial usage
/// @n All rights reserved
///
/// * Commercial edition: for professionals or organisations, commercial usage
/// @n All rights reserved
///

// SDK
#include "stdint.h"

#ifndef hV_LIST_SCREENS_RELEASE
///
/// @brief Release
///
#define hV_LIST_SCREENS_RELEASE 702

///
/// @brief Screen type
///
#define eScreen_EPD_EXT3_t uint32_t

///
/// @name Colour black-white-red "Spectra" screens
/// @note Global update mode
/// @see https://www.pervasivedisplays.com/products/?_sft_etc_itc=itc&_sft_product_colour=black-white-red
/// @{
///
#define eScreen_EPD_EXT3_213_BWR (uint32_t)0x102100 ///< reference xE2213CSxxx
#define eScreen_EPD_EXT3_266_BWR (uint32_t)0x102600 ///< reference xE2266CSxxx
/// @}

/// @name Monochrome screens with embedded fast update
/// @note Global and fast update modes
/// @see https://www.pervasivedisplays.com/products/?_sft_etc_itc=pu+itc
/// @{
///
#define eScreen_EPD_EXT3_213_0E_Fast (uint32_t)0x01210E ///< reference xE2213PS0Ex
#define eScreen_EPD_EXT3_266_0C_Fast (uint32_t)0x01260C ///< reference xE2266PS0Cx
/// @}

#endif // hV_LIST_SCREENS_RELEASE

