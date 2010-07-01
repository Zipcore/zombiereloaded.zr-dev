/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           voltools.inc
 *  Type:           Module
 *  Description:    Provides functions for managing volumes.
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

#if defined _voltools_included
 #endinput
#endif
#define _voltools_included

/**
 * Gets whether a client is within volumes or not. Result is stored in a boolean
 * array.
 *
 * @param client    The client index.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of volumes the client is within.
 */
VolGetPlayerStates(client, bool:buffer[], maxlen)
{
    new volCount = 0;
    
    // Loop through all available volumes.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX && volCount < maxlen; volumeIndex++)
    {
        if (VolInUse(volumeIndex))
        {
            // Check if player is inside the volume.
            if (VolIsPointInVolume(VolPlayerLoc[client], volumeIndex))
            {
                // Mark player as in volume.
                buffer[volumeIndex] = true;
                volCount++;
            }
            else
            {
                // Do explicit reset.
                buffer[volumeIndex] = false;
            }
        }
    }
    
    return volCount;
}

/**
 * Gets the shape index from a shape id string.
 *
 * @param shapeType     Shape type.
 * @param shapeId       Shape id string (name or index).
 * @return              Shape index, or -1 if not found/valid.
 */
VolGetShapeIndex(VolumeShapes:shapeType, const String:shapeId[])
{
    new shapeIndex;
    if (IsCharNumeric(shapeId[0]))
    {
        // It's a index.
        shapeIndex = StringToInt(shapeId);
        switch (shapeType)
        {
            case VolShape_Cuboid:
            {
                if (VolCuboidIsValid(shapeIndex))
                {
                    return shapeIndex;
                }
                else
                {
                    return -1;
                }
            }
            case VolShape_Sphere:
            {
                if (VolSphereIsValid(shapeIndex))
                {
                    return shapeIndex;
                }
                else
                {
                    return -1;
                }
            }
        }
    }
    else
    {
        // It's a name.
        switch (shapeType)
        {
            case VolShape_Cuboid: return VolCuboidFind(shapeId);
            case VolShape_Sphere: return VolSphereFind(shapeId);
        }
    }
    
    return -1;
}

/**
 * Gets the feature index from a feature id string.
 *
 * @param featureType   Feature type.
 * @param featureId     Feature id string (name or index).
 * @return              Feature index, or -1 if not found/valid.
 */
VolGetFeatureIndex(VolumeFeatureTypes:featureType, const String:featureId[])
{
    new featureIndex;
    if (IsCharNumeric(featureId[0]))
    {
        // It's a index.
        featureIndex = StringToInt(featureId);
        switch (featureType)
        {
            case VolFeature_Anticamp:
            {
                if (VolAnticampIsValid(featureIndex))
                {
                    return featureIndex;
                }
                else
                {
                    return -1;
                }
            }
            case VolFeature_ClassEdit:
            {
                if (VolClassEditIsValid(featureIndex))
                {
                    return featureIndex;
                }
                else
                {
                    return -1;
                }
            }
        }
    }
    else
    {
        // It's a name.
        switch (featureType)
        {
            case VolFeature_Anticamp: return VolAnticampFind(featureId);
            case VolFeature_ClassEdit: return VolClassEditFind(featureId);
        }
    }
    
    return -1;
}
