/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           cuboid.h.inc
 *  Type:           Header
 *  Description:    Cuboid volume headers.
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

#if defined _volfeatures_cuboid_included
 #endinput
#endif
#define _volfeatures_cuboid_included

/**
 * Total cuboids allowed.
 */
#define ZR_CUBOIDS_MAX ZR_VOLUMES_MAX

/**
 * Represents a rectangular volume.
 */
enum CuboidAttributes
{
    bool:Cuboid_InUse,                  /** Marks if the volume is used. */
    String:Cuboid_Name[VOL_NAME_LEN],   /** Optional. Name of the cuboid (alphanumeric). Cannot start with a number. */

    Float:Cuboid_xMin,  /** Minimum x position. */
    Float:Cuboid_xMax,  /** Maximum x position. */
    
    Float:Cuboid_yMin,  /** Minimum y position. */
    Float:Cuboid_yMax,  /** Maximum y position. */
    
    Float:Cuboid_zMin,  /** Minimum z position. */
    Float:Cuboid_zMax,  /** Maximum z position. */
}
