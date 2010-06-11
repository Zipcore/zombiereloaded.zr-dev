/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           sphere.inc
 *  Type:           Module
 *  Description:    Sphere data.
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

/**
 * Sphere data.
 */
new Spheres[ZR_SPHERES_MAX][SphereAttributes];

/**
 * Number of spheres in use.
 */
new SphereCount;

/**
 * Clears all spheres and marks them as free.
 */
VolSphereClear()
{
    for (new sphere = 0; sphere < ZR_SPHERES_MAX; sphere++)
    {
        VolSphereClearIndex(sphere, false);
    }
    
    // Reset counter.
    SphereCount = 0;
}

/**
 * Clears the specified sphere and mark it as free.
 *
 * Note: Does not validate index.
 *
 * @param sphere            Sphere to clear.
 * @param decrementCounter  Optional. Decrement the sphere counter by one.
 *                          Default is true.
 */
VolSphereClearIndex(sphere, bool:decrementCounter = true)
{
    Spheres[sphere][Sphere_InUse]       = false;
    
    strcopy(Spheres[sphere][Sphere_Name], ZR_SPHERE_NAME_LEN, ZR_SPHERE_DEF_NAME);
    
    Spheres[sphere][Sphere_LocX]        = 0.0;
    Spheres[sphere][Sphere_LocY]        = 0.0;
    Spheres[sphere][Sphere_LocZ]        = 0.0;
    
    Spheres[sphere][Sphere_Radius]      = ZR_SPHERE_DEF_RADIUS;
    
    Spheres[sphere][Sphere_FadeType]    = ZR_SPHERE_DEF_FADE_TYPE;
    Spheres[sphere][Sphere_MinDistance] = ZR_SPHERE_DEF_MIN_DISTANCE;
    Spheres[sphere][Sphere_MaxDistance] = ZR_SPHERE_DEF_MAX_DISTANCE;
    
    if (decrementCounter)
    {
        SphereCount--;
    }
}

/**
 * Returns wether a sphere is marked as in use.
 *
 * Note: Does not validate index.
 *
 * @param sphere    Sphere index.
 * @return          True if in use, false otherwise.
 */
bool:VolSphereInUse(sphere)
{
    return Spheres[sphere][Sphere_InUse];
}

/**
 * Validates a sphere index.
 *
 * @param sphere    Sphere index.
 * @return          True if valid, false otherwise.
 */
bool:VolSphereIsValidIndex(sphere)
{
    if (sphere >= 0 && sphere < ZR_SPHERES_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns whether a sphere is valid and in use.
 *
 * @param sphere    Sphere to validate.
 * @return          True if valid, false otherwise.
 */
bool:VolSphereIsValid(sphere)
{
    return VolSphereIsValidIndex(sphere) && VolSphereInUse(sphere);
}

/**
 * Gets the first free sphere index.
 *
 * @return      The first free sphere index if successful, or -1 if there are
 *              no free spheres.
 */
VolGetFreeSphere()
{
    // Loop through all spheres.
    for (new sphere = 0; sphere < ZR_SPHERES_MAX; sphere++)
    {
        // Check if it's free.
        if (!VolSphereInUse(sphere))
        {
            Spheres[sphere][Sphere_InUse] = true;
            SphereCount++;
            return sphere;
        }
    }
    
    // No free spheres found.
    return -1;
}

/**
 * Gets whether a point is inside a sphere.
 *
 * @param point         The point to check.
 * @param sphereLoc     Location of the sphere
 * @return              True if the point is within min and max values. False
 *                      otherwise.
 */
bool:VolIsPointInSphereEx(Float:point[3], Float:sphereLoc[3], Float:sphereRadius)
{
    // Get the distance from sphere centre.
    new Float:distance = GetVectorDistance(point, sphereLoc);
    
    // Check if within the radius.
    if (distance <= sphereRadius)
    {
        return true;
    }
    
    // The point is outside the sphere boundaries.
    return false;
}

/**
 * Gets whether a point is inside a sphere.
 *
 * @param point     The point to check.
 * @param cuboid    Sphere index.
 * @return          True if the point is in the sphere, false otherwise.
 */
bool:VolIsPointInSphere(Float:point[3], sphere)
{
    new Float:loc[3];
    new Float:radius;
    
    // Get location and radius.
    loc[0] = Spheres[sphere][Sphere_LocX];
    loc[1] = Spheres[sphere][Sphere_LocY];
    loc[2] = Spheres[sphere][Sphere_LocZ];
    
    radius = Spheres[sphere][Sphere_Radius];
    
    return VolIsPointInSphereEx(point, loc, radius);
}

/**
 * Builds a user friendly (translated) list of the sphere's attributes.
 *
 * @param sphere    Sphere to dump.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolSphereBuildList(sphere, String:buffer[], maxlen)
{
    decl String:linebuffer[128];
    decl String:langbuffer[128];
    decl String:valuebuffer[256];
    new sphereCache[SphereAttributes];
    new cellswritten;
    
    // Validate index.
    if (!VolSphereIsValid(sphere))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    sphereCache = Spheres[sphere];
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Name");
    Format(linebuffer, sizeof(linebuffer), "%25s \"%s\" (%d)\n", langbuffer, sphereCache[Sphere_Name], sphere);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Loc");
    Format(linebuffer, sizeof(linebuffer), "%25s %-8.2f %-8.2f %-8.2f\n", langbuffer, sphereCache[Sphere_LocX], sphereCache[Sphere_LocY], sphereCache[Sphere_LocZ]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Radius");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, sphereCache[Sphere_Radius]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Fade Type");
    VolFadeToString(sphereCache[Sphere_FadeType], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Min Fade");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, sphereCache[Sphere_MinDistance]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Max Fade");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, sphereCache[Sphere_MaxDistance]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    return cellswritten;
}
