/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           cuboid.inc
 *  Type:           Module
 *  Description:    Cuboid data.
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
 * Cuboid data.
 */
new Cuboids[ZR_CUBOIDS_MAX][CuboidAttributes];

/**
 * Number of cuboid in use.
 */
new CuboidCount;

/**
 * Clears all cuboids and marks them as free.
 */
VolCuboidClear()
{
    for (new cuboid = 0; cuboid < ZR_CUBOIDS_MAX; cuboid++)
    {
        VolCuboidClearIndex(cuboid, false);
    }
    
    // Reset counter.
    CuboidCount = 0;
}

/**
 * Clears the specified cuboid and mark it as free.
 *
 * Note: Does not validate index.
 *
 * @param cuboid            Cuboid to clear.
 * @param decrementCounter  Optional. Decrement the cuboid counter by one.
 *                          Default is true.
 */
VolCuboidClearIndex(cuboid, bool:decrementCounter = true)
{
    Cuboids[cuboid][Cuboid_InUse]   = false;
    
    // Clear name.
    Cuboids[cuboid][Cuboid_Name][0] = 0;
    
    Cuboids[cuboid][Cuboid_xMin]    = 0.0;
    Cuboids[cuboid][Cuboid_xMax]    = 0.0;
    
    Cuboids[cuboid][Cuboid_yMin]    = 0.0;
    Cuboids[cuboid][Cuboid_yMax]    = 0.0;
    
    Cuboids[cuboid][Cuboid_zMin]    = 0.0;
    Cuboids[cuboid][Cuboid_zMax]    = 0.0;
    
    if (decrementCounter)
    {
        CuboidCount--;
    }
}

/**
 * Returns wether a cuboid is marked as in use.
 *
 * Note: Does not validate index.
 *
 * @param cuboid    Cuboid index.
 * @return          True if in use, false otherwise.
 */
bool:VolCuboidInUse(cuboid)
{
    return Cuboids[cuboid][Cuboid_InUse];
}

/**
 * Validates a cuboid index.
 *
 * @param cuboid    Cuboid index.
 * @return          True if valid, false otherwise.
 */
bool:VolCuboidIsValidIndex(cuboid)
{
    if (cuboid >= 0 && cuboid < ZR_CUBOIDS_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns whether a cuboid is valid and in use, or not.
 *
 * @param cuboid    Cuboid index.
 * @return          True if valid and in use, false otherwise.
 */
bool:VolCuboidIsValid(cuboid)
{
    return VolCuboidIsValidIndex(cuboid) && VolCuboidInUse(cuboid);
}

/**
 * Gets the first free cuboid index.
 *
 * @return      The first free cuboid index if successful, or -1 if there are
 *              no free cuboids.
 */
VolGetFreeCuboid()
{
    // Loop through all cuboids.
    for (new cuboid = 0; cuboid < ZR_CUBOIDS_MAX; cuboid++)
    {
        // Check if it's free.
        if (!VolCuboidInUse(cuboid))
        {
            Cuboids[cuboid][Cuboid_InUse] = true;
            CuboidCount++;
            return cuboid;
        }
    }
    
    // No free cuboids found.
    return -1;
}

/**
 * Gets whether a point is inside two cuboid coordinates.
 *
 * @param point     The point to check.
 * @param min       Minimum x, y and z values of the cuboid.
 * @param max       Maximum x, y and z values of the cuboid.
 * @return          True if the point is within min and max values. False
 *                  otherwise.
 */
bool:VolIsPointInCuboidEx(Float:point[3], Float:min[3], Float:max[3])
{
    // Cache to avoid re-indexing arrays.
    new Float:posX = point[0];
    new Float:posY = point[1];
    new Float:posZ = point[2];
    
    // Check if within x boundaries.
    if ((posX >= min[0]) && (posX <= max[0]))
    {
        // Check if within y boundaries.
        if ((posY >= min[1]) && (posY <= max[1]))
        {
            // Check if within x boundaries.
            if ((posZ >= min[2]) && (posZ <= max[2]))
            {
                // The point is within the location boundaries.
                return true;
            }
        }
    }
    
    // The point is outside the cuboid boundaries.
    return false;
}

/**
 * Gets whether a point is inside a cuboid.
 *
 * @param point     The point to check.
 * @param cuboid    Cuboid index.
 * @return          True if the point is in the cuboid, false otherwise.
 */
bool:VolIsPointInCuboid(Float:point[3], cuboid)
{
    new Float:min[3];
    new Float:max[3];
    
    // Get max and min values.
    min[0] = Cuboids[cuboid][Cuboid_xMin];
    min[1] = Cuboids[cuboid][Cuboid_yMin];
    min[2] = Cuboids[cuboid][Cuboid_zMin];
    
    max[0] = Cuboids[cuboid][Cuboid_xMax];
    max[1] = Cuboids[cuboid][Cuboid_yMax];
    max[2] = Cuboids[cuboid][Cuboid_zMax];
    
    return VolIsPointInCuboidEx(point, min, max);
}

/**
 * Builds a user friendly (translated) list of the cuboid's attributes.
 *
 * @param cuboid    Cuboid to dump.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolCuboidBuildList(cuboid, String:buffer[], maxlen)
{
    decl String:linebuffer[128];
    decl String:langbuffer[128];
    new cuboidCache[CuboidAttributes];
    new cellswritten;
    
    // Validate index.
    if (!VolCuboidIsValid(cuboid))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    cuboidCache = Cuboids[cuboid];
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Name");
    Format(linebuffer, sizeof(linebuffer), "%25s \"%s\" (%d)\n", langbuffer, cuboidCache[Cuboid_Name], cuboid);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Min Loc");
    Format(linebuffer, sizeof(linebuffer), "%25s %-8.2f %-8.2f %-8.2f\n", langbuffer, cuboidCache[Cuboid_xMin], cuboidCache[Cuboid_yMin], cuboidCache[Cuboid_zMin]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Max Loc");
    Format(linebuffer, sizeof(linebuffer), "%25s %-8.2f %-8.2f %-8.2f\n", langbuffer, cuboidCache[Cuboid_xMax], cuboidCache[Cuboid_yMax], cuboidCache[Cuboid_zMax]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    return cellswritten;
}

/**
 * Creates a wireframe effect on the specified cuboid.
 */
stock VolCuboidSendWireframe(client, cuboid, color[4])
{
    new cuboidCache[CuboidAttributes];
    cuboidCache = Cuboids[cuboid];
    
    new Float:start[3];
    new Float:end[3];
    
    new Float:corner1[3];
    corner1[0] = cuboidCache[Cuboid_xMin];
    corner1[1] = cuboidCache[Cuboid_yMin];
    corner1[2] = cuboidCache[Cuboid_zMin];
    
    new Float:corner2[3];
    corner2[0] = cuboidCache[Cuboid_xMax];
    corner2[1] = cuboidCache[Cuboid_yMax];
    corner2[2] = cuboidCache[Cuboid_zMax];
    
    /*******************
     *   Front frame   *
     *******************/
    
    // left vertical
    end[0] = corner1[0];
    end[1] = corner2[1];
    end[2] = corner1[2];
    TE_SetupBeamPoints(corner1, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // right vertical
    start[0] = corner2[0];
    start[1] = corner1[1];
    start[2] = corner1[2];
    end[0] = corner2[0];
    end[1] = corner2[1];
    end[2] = corner1[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // top horizontal
    start[0] = corner1[0];
    start[1] = corner2[1];
    start[2] = corner1[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // bottom horizontal
    end[0] = corner2[0];
    end[1] = corner1[1];
    end[2] = corner1[2];
    TE_SetupBeamPoints(corner1, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    /******************
     *   Back frame   *
     ******************/
    
    // left vertical
    start[0] = corner1[0];
    start[1] = corner1[1];
    start[2] = corner2[2];
    end[0] = corner1[0];
    end[1] = corner2[1];
    end[2] = corner2[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // right vertical
    start[0] = corner2[0];
    start[1] = corner1[1];
    start[2] = corner2[2];
    TE_SetupBeamPoints(start, corner2, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // top horizontal
    start[0] = corner1[0];
    start[1] = corner2[1];
    start[2] = corner2[2];
    TE_SetupBeamPoints(start, corner2, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // bottom horizontal
    start[0] = corner1[0];
    start[1] = corner1[1];
    start[2] = corner2[2];
    end[0] = corner2[0];
    end[1] = corner1[1];
    end[2] = corner2[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    /****************************
     *   Lines between frames   *
     ****************************/
    
    // top left
    start[0] = corner1[0];
    start[1] = corner2[1];
    start[2] = corner1[2];
    end[0] = corner1[0];
    end[1] = corner2[1];
    end[2] = corner2[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // top right
    start[0] = corner2[0];
    start[1] = corner2[1];
    start[2] = corner1[2];
    TE_SetupBeamPoints(start, corner2, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // bottom left
    end[0] = corner1[0];
    end[1] = corner1[1];
    end[2] = corner2[2];
    TE_SetupBeamPoints(corner1, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
    
    // bottom right
    start[0] = corner2[0];
    start[1] = corner1[1];
    start[2] = corner1[2];
    end[0] = corner2[0];
    end[1] = corner1[1];
    end[2] = corner2[2];
    TE_SetupBeamPoints(start, end, VolBeamSprite, VolBeamHalo, 0, 10, ZR_VOL_WIREFRAME_LIFE, ZR_VOL_WIREFRAME_WIDTH, ZR_VOL_WIREFRAME_END_WIDTH, ZR_VOL_WIREFRAME_FADE_LEN, ZR_VOL_WIREFRAME_AMP, color, ZR_VOL_WIREFRAME_SPEED);
    TE_SendToClient(client);
}
