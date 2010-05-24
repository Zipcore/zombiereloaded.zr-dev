/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           sphere.h.inc
 *  Type:           Header
 *  Description:    Sphere volume headers.
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

#if defined _volfeatures_sphere_included
 #endinput
#endif
#define _volfeatures_sphere_included

/**
 * Total spheres allowed.
 */
#define ZR_SPHERES_MAX ZR_VOLUMES_MAX

/**
 * @section Default sphere attribute values.
 */
#define ZR_SPHERE_DEF_RADIUS        0.0
#define ZR_SPHERE_DEF_FADE_TYPE     FadeType_None
#define ZR_SPHERE_DEF_MIN_DISTANCE  0.0
#define ZR_SPHERE_DEF_MAX_DISTANCE  0.0
/**
 * @endsection
 */

/**
 * Represents a sphere.
 */
enum SphereAttributes
{
    bool:Sphere_InUse,          /** Marks if the sphere is in use. */

    /* Location (center point) */
    Float:Sphere_LocX,          /** X position. */
    Float:Sphere_LocY,          /** Y position. */
    Float:Sphere_LocZ,          /** Z position. */
    
    /* Dimensions */
    Float:Sphere_Radius,        /** Size. */
    
    FadeTypes:Sphere_FadeType,  /** Fade method. Note: Not all volumetric features support fading. */
    Float:Sphere_MinDistance,   /** Minimum fade distance. */
    Float:Sphere_MaxDistance,   /** Maximum fade distance. */
}

/**
 * Valid fade types.
 */
enum FadeTypes
{
    FadeType_Invalid = -1,      /** Invalid type (used by parser). */
    FadeType_None,              /** Disable fading. */
    FadeType_LinearCenter,      /** Use a linear fade algorithm based on the sphere's center point. */
    FadeType_LinearDistance     /** Use a linear fade algorithm based on max and min distance. */
    
    /* Note: Exponential and inversely proportional fade distances can be also
     *       be implemented later */
}
