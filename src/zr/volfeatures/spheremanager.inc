/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           spheremanager.inc
 *  Type:           Module
 *  Description:    Sphere managing API.
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
 * Creates a new sphere with the specified attributes.
 *
 * @param loc           The spheres origin (center point).
 * @param radius        Sphere size.
 * @param fadeType      Fade style.
 * @param minDistance   Minimum fade distance.
 * @param maxDistance   Maximum fade distance.
 * @param name          Optional. Name of this sphere.
 * @return              Sphere index, or negative on error.
 *                      -1 if no free spheres.
 *                      -2 if invalid radius.
 *                      -3 if invalid fade distance.
 */
VolSphereAdd(Float:loc[3],
             Float:radius,
             FadeTypes:fadeType = ZR_SPHERE_DEF_FADE_TYPE,
             Float:minDistance = ZR_SPHERE_DEF_MIN_DISTANCE,
             Float:maxDistance = ZR_SPHERE_DEF_MAX_DISTANCE,
             String:name[] = ZR_SPHERE_DEF_NAME)
{
    // Validate radius.
    if (radius <= 0)
    {
        return -2;
    }
    
    // Validate fade distance.
    if (fadeType != FadeType_None &&
        (minDistance == maxDistance ||
        minDistance > maxDistance ||
        maxDistance < minDistance))
    {
        return -3;
    }
    
    new Float:locX = loc[0];
    new Float:locY = loc[1];
    new Float:locZ = loc[2];
    
    // Get free sphere.
    new sphere = VolGetFreeSphere();
    if (sphere < 0)
    {
        return -1;
    }
    
    // Apply attributes.
    Spheres[sphere][Sphere_LocX] = locX;
    Spheres[sphere][Sphere_LocY] = locY;
    Spheres[sphere][Sphere_LocZ] = locZ;
    Spheres[sphere][Sphere_Radius] = radius;
    Spheres[sphere][Sphere_FadeType] = fadeType;
    Spheres[sphere][Sphere_MinDistance] = minDistance;
    Spheres[sphere][Sphere_MaxDistance] = maxDistance;
    
    TrimString(name);
    strcopy(Spheres[sphere][Sphere_Name], VOL_NAME_LEN, name);
    
    return sphere;
}

/**
 * Removes the specified sphere.
 *
 * Note: Volumes and features that depend on the sphere is also removed if
 *       recursive removal is enabled. Otherwise they are disabled.
 *
 * @param sphere        Sphere to remove.
 * @param recursive     Optional. Also remove volumes and features that depend
 *                      on the sphere. Default is true.
 * @return              True if successful, false otherwise.
 */
bool:VolSphereRemove(sphere, bool:recursive = true)
{
    // Validate sphere.
    if (!VolSphereIsValid(sphere))
    {
        return false;
    }
    
    
    // Find volumes that use the sphere and remove them.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Validate volume.
        if (!VolIsValid(volumeIndex))
        {
            // Skip volume.
            continue;
        }
        
        // Check if the sphere is used on the current volume.
        if (Volumes[volumeIndex][Vol_Shape] == VolShape_Sphere &&
            Volumes[volumeIndex][Vol_ShapeData] == sphere)
        {
            // Check if recursive removal is enabled.
            if (recursive)
            {
                // Remove volume.
                VolRemove(volumeIndex, recursive);
            }
            else
            {
                // Disable volume.
                VolDisableVolume(volumeIndex);
            }
        }
    }
    
    // Remove sphere.
    VolSphereClearIndex(sphere);
    
    return true;
}


/**
 * Removes all spheres.
 *
 * Note: Volumes and features that depend on the spheres are also removed if
 *       recursive removal is enabled. Otherwise they are disabled.
 *
 * @param recursive     Optional. Also remove volumes and features that depend
 *                      on the spheres. Default is true.
 * @return              True if successful, false otherwise.
 */
stock VolSphereRemoveAll(bool:recursive = true)
{
    for (new sphere = 0; sphere < ZR_CUBOIDS_MAX; sphere++)
    {
        VolSphereRemove(sphere, recursive);
    }
}

/**
 * Search for a sphere with the specified name.
 *
 * @param name              Name to search for.
 * @param caseSensitive     Optional. Use case sensitive search. Default is
 *                          false.
 * @return                  Sphere index if found, -1 otherwise.
 */
VolSphereFind(const String:name[], bool:caseSensitive = false)
{
    for (new sphere = 0; sphere < ZR_SPHERES_MAX; sphere++)
    {
        if (VolSphereInUse(sphere))
        {
            if (StrEqual(Spheres[sphere][Sphere_Name], name, caseSensitive))
            {
                return sphere;
            }
        }
    }
    
    return -1;
}
