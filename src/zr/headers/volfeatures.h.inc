/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volfeatures.h.inc
 *  Type:           Header
 *  Description:    All volumetric feature headers.
 *
 *  Copyright (C) 2009-2010  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#if defined _volfeatures_included
 #endinput
#endif
#define _volfeatures_included

#include "zr/headers/cuboid.h"
#include "zr/headers/sphere.h"
#include "zr/headers/volume.h"

/**
 * Available volumetric feature types.
 */
enum VolumeFeatureTypes
{
    VolFeature_Invalid = 0,
    VolFeature_Anticamp,
    VolFeature_ClassEdit
}
