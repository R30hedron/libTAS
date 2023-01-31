/*
    Copyright 2015-2020 Clément Gallet <clement.gallet@ens-lyon.org>

    This file is part of libTAS.

    libTAS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libTAS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libTAS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#ifndef LIBTAS_SURFACEXIMAGE_H_INCL
#define LIBTAS_SURFACEXIMAGE_H_INCL

#include "SurfaceARGB.h"
#include <cstdint>
#include <vector>
#include <X11/Xlib.h>

namespace libtas {

/* Create a wrapper around an XImage for blitting */
class SurfaceXImage
{
	public:
        /* Pointer to the image */
        XImage* image;

        SurfaceXImage(XImage* image);

        /* Blit surface `src` into this surface at coords x and y */
        void blit(const SurfaceARGB* src, int x, int y);
};

}

#endif
