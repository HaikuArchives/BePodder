/*******************************************************************************
/
/	File:			ColorTools.h
/
/   Description:    Additional experimental color manipulation functions.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _COLOR_TOOLS_H
#define _COLOR_TOOLS_H

#include <GraphicsDefs.h>

#if B_BEOS_VERSION <= B_BEOS_VERSION_MAUI

namespace BExperimental {

// Comparison operators.

inline bool operator==(const rgb_color c1, const rgb_color c2)
{
	return (*((uint32*)&c1)) == (*((uint32*)&c2));
}

inline bool operator!=(const rgb_color c1, const rgb_color c2)
{
	return (*((uint32*)&c1)) != (*((uint32*)&c2));
}

// Blend two colors together, weighting by their relative alpha channels.
// The resulting color is the same as mix_color(), except that the amount
// used from color1 and color2's color channels is dependent on that color's
// alpha channel.  For example, if color1.alpha is 0 and color2.alpha is
// 255, the resulting red, green, and blue values will be the same as those
// in color2, regardless of 'amount'.
rgb_color blend_color(rgb_color color1, rgb_color color2, uint8 amount);

// Return a color that is the disabled representation of 'color' when drawn
// on a solid color 'background'.
rgb_color disable_color(rgb_color color, rgb_color background);

}	// namespace BExperimental

using namespace BExperimental;

#endif

#endif
