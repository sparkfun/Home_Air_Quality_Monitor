///
/// @file hV_Colours565.h
/// @brief Class Library for 5-6-5 coded colours
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @author Rei Vilo
/// @date 21 Sep 2023
/// @version 700
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///

// SDK
#include "hV_HAL_Peripherals_fast.h"

// Configuration
#include "hV_Configuration_fast.h"

#ifndef hV_COLOURS_RELEASE
///
/// @brief Library release number
///
#define hV_COLOURS_RELEASE 700

///
/// @class hV_Colours565
/// @brief for 5-6-5 coded colours = 64K colours
/// @details red: 5 bits, green: 6 bits, blue: 5 bits
///
class hV_Colours565
{
  public:
    ///
    /// @brief Constructor
    ///
    hV_Colours565();

    ///
    /// @name Colours constants
    /// @note Both syntaxes are valid
    /// * myColours.black and
    /// * hV_Colours565::black
    /// with extern hV_Colours565 myColours; declared below
    ///
    // Colours RGB=565                 Red  Green Blue
    //                                 4321054321043210
    /// @{

    static const uint16_t black    = 0b0000000000000000; ///< black
    static const uint16_t white    = 0b1111111111111111; ///< white

    static const uint16_t grey     = 0b0111101111101111; ///< American-English variant for grey
    static const uint16_t darkGrey = 0b0011100111100111; ///< American-English variant for dark grey
    /// American-English variants

    /// @}
};

///
/// @brief Instantiated object
/// @details myColours
///
extern hV_Colours565 myColours;

#endif // hV_COLOURS_RELEASE
