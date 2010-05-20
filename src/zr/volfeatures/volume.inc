/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volume.inc
 *  Type:           Module
 *  Description:    Volume data.
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
 * Volume data.
 */
new Volumes[ZR_VOLUMES_MAX][VolumeAttributes];

/**
 * Number of volumes in use.
 */
new VolumeCount;

/**
 * Clears all volumes and resets to default values.
 */
VolClear()
{
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        VolClearIndex(volumeIndex);
    }
}

/**
 * Clears the specified volume and resets to default values.
 *
 * Note: Does not validate index.
 *
 * @param volumeIndex   Volume to clear.
 * @param inUse         Optional. Whether to mark volume as in use or not.
 *                      Default is false.
 */
VolClearIndex(volumeIndex, bool:inUse = ZR_VOL_DEFAULT_IN_USE)
{
    Volumes[volumeIndex][Vol_Enabled]           = ZR_VOL_DEFAULT_ENABLED;
    Volumes[volumeIndex][Vol_InUse]             = inUse;
    Volumes[volumeIndex][Vol_Shape]             = ZR_VOL_DEFAULT_SHAPE;
    Volumes[volumeIndex][Vol_ShapeData]         = ZR_VOL_DEFAULT_SHAPE_DATA;
    Volumes[volumeIndex][Vol_Effect]            = ZR_VOL_DEFAULT_EFFECT;
    Volumes[volumeIndex][Vol_EffectColor]       = ZR_VOL_DEFAULT_EFFECT_COLOR;
    Volumes[volumeIndex][Vol_TeamFilter]        = ZR_VOL_DEFAULT_TEAM_FILTER;
    Volumes[volumeIndex][Vol_TriggerDelay]      = ZR_VOL_DEFAULT_TRIGGER_DELAY;
    Volumes[volumeIndex][Vol_Priority]          = ZR_VOL_DEFAULT_PRIORITY;
    Volumes[volumeIndex][Vol_ConflictAction]    = ZR_VOL_DEFAULT_CONFLICT_ACTION;
    
    // Destroy links and detach features.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureIndexes];
    VolDetachFeatures(volumeIndex, featureTypes, featureIndexes);
    
    Volumes[volumeIndex][Vol_FeatureTypes]      = ZR_VOL_DEFAULT_FEATURE_TYPES;
    Volumes[volumeIndex][Vol_FeatureIndexes]    = ZR_VOL_DEFAULT_FEATURE_INDEXES;
}

/**
 * Returns wether a volume is marked as in use.
 *
 * Note: Does not validate index.
 *
 * @param volumeIndex   The volume index.
 * @return              True if in use, false otherwise.
 */
bool:VolInUse(volumeIndex)
{
    return Volumes[volumeIndex][Vol_InUse];
}

/**
 * Returns wether a volume is enabled or not.
 *
 * Note: Does not validate index.
 *
 * @param volumeIndex   The volume index.
 * @return              True if enabled, false otherwise.
 */
bool:VolIsEnabled(volumeIndex)
{
    return Volumes[volumeIndex][Vol_Enabled];
}

/**
 * Validates a volume index.
 *
 * @param volumeIndex   The volume index.
 * @return              True if valid, false otherwise.
 */
bool:VolIsValidIndex(volumeIndex)
{
    if (volumeIndex >= 0 && volumeIndex < ZR_VOLUMES_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Gets the first free volume index.
 *
 * @return      The first free volume index if successful, or -1 if there are
 *              no free volumes.
 */
VolGetFreeVolume()
{
    // Loop through all volumes.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if it's free.
        if (!VolInUse(volumeIndex))
        {
            return volumeIndex;
        }
    }
    
    // No free volumes found.
    return -1;
}


/**
 * Checks if the specified client match the team filtering for the specified
 * volume.
 *
 * @param client        The client index.
 * @param volumeIndex   The volume to check team filtering on.
 * @return              True if client pass the team filtering, false otherwise.
 */
bool:VolTeamFilterMatch(client, volumeIndex)
{
    new VolumeTeamFilters:filter;
    
    // Chache filter value.
    filter = Volumes[volumeIndex][Vol_TeamFilter];
    
    switch (filter)
    {
        case VolTeam_All:
        {
            // All maches everyone.
            return true;
        }
        case VolTeam_Humans:
        {
            // Check if client is a human.
            return InfectIsClientHuman(client);
        }
        case VolTeam_Zombies:
        {
            // Check if client is a zombie.
            return InfectIsClientInfected(client);
        }
    }
    
    // Invalid filter value.
    return false;
}

/**
 * Attaches a feature to a volume.
 *
 * @param volumeIndex       Volume data index.
 * @param featureType       Feature types.
 * @param featureIndex      Feature data index.
 */
VolAttachFeature(volumeIndex, VolumeFeatureTypes:featureType, featureIndex)
{
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureIndexes];
    
    // Create link arrays if they don't exist.
    if (!ZR_HasHandle(featureTypes))
    {
        featureTypes = CreateArray();
        featureIndexes = CreateArray();
        
        // Assign arrays to volume.
        Volumes[volumeIndex][Vol_FeatureTypes] = featureTypes;
        Volumes[volumeIndex][Vol_FeatureIndexes] = featureIndexes;
    }
    
    // Add feature to arrays.
    PushArrayCell(featureTypes, _:featureType);
    PushArrayCell(featureIndexes, featureIndex);
}

/**
 * Removes all references to a volume.
 *
 * @param volumeIndex       Volume to derefer.
 * @param featureTypes      List of feature types linked to the volume.
 * @param featureIndexes    List of the feature indexes.
 */
VolDetachFeatures(volumeIndex, Handle:featureTypes, Handle:featureIndexes)
{
    // Check if link arrays exists.
    if (ZR_HasHandle(featureTypes))
    {
        new size = GetArraySize(featureTypes);
        
        if (size > 0)
        {
            // Loop through each feature in the list.
            for (new i = 0; i < size; i++)
            {
                // Get type and index.
                new VolumeFeatureTypes:featureType = VolumeFeatureTypes:GetArrayCell(featureTypes, i);
                new feature = GetArrayCell(featureIndexes, i);
                
                // Detach from volume; disable and reset volume index.
                switch (featureType)
                {
                    case VolFeature_Anticamp:
                    {
                        VolAnticampOnDisabled(feature);
                        VolAnticampSetVolumeIndex(feature, -1);
                    }
                    case VolFeature_ClassEdit:
                    {
                        VolClassEditOnDisabled(feature);
                        VolClassEditSetVolumeIndex(feature, -1);
                    }
                }
            }
        }
        
        // Attempt to close existing handles.
        ZR_CloseHandle(featureTypes);
        ZR_CloseHandle(featureIndexes);
        
        // Update handles.
        Volumes[volumeIndex][Vol_FeatureTypes] = featureTypes;
        Volumes[volumeIndex][Vol_FeatureIndexes] = featureTypes;
    }
}

/**
 * Destroys all link arrays on all volumes.
 */
VolDetatchAll()
{
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        VolDetachFeatures(volumeIndex);
    }
}

/**
 * Refreshes the arrays with linked features for a volume.
 *
 * @param volumeIndex   Volume to refresh links on.
 */
VolRefreshLinks(volumeIndex)
{
    // Attempt to close existing handles.
    ZR_CloseHandle(Volumes[volumeIndex][Vol_FeatureTypes]);
    ZR_CloseHandle(Volumes[volumeIndex][Vol_FeatureIndexes]);
    
    new Handle:featureTypes;
    new Handle:featureIndexes;
    
    // Recreate link arrays.
    VolGetFeatures(volumeIndex, featureTypes, featureIndexes);
    
    // Assign arrays to volume.
    Volumes[volumeIndex][Vol_FeatureTypes] = featureTypes;
    Volumes[volumeIndex][Vol_FeatureIndexes] = featureIndexes;
}

/**
 * Creates two arrays with feature types and indexes that are linked to the
 * specified volume.
 *
 * @param volumeIndex       Volume to check.
 * @param featureTypes      Output: Handle to an array with type index for each
 *                          feature. INVALID_HANDLE if no features are linked.
 * @param featureIndexes    Output: Handle to an array with indexes for each feature.
 *                          INVALID_HANDLE if no features are linked.
 * @return                  Number of linked features.
 */
VolGetFeatures(volumeIndex, &Handle:featureTypes, &Handle:featureIndexes)
{
    featureTypes = CreateArray();
    featureIndexes = CreateArray();
    new count = 0;
    
    // Loop through each feature type.
    for (new f = 0; f < VOL_NUM_FEATURES; f++)
    {
        // Convert to feature type.
        new VolumeFeatureTypes:feature = VolumeFeatureTypes:f;
        
        // Get upper limit.
        new featureCount;
        switch (feature)
        {
            case VolFeature_Anticamp: featureCount = AnticampCount;
            case VolFeature_ClassEdit: featureCount = ClassEditCount;
        }
        
        // Loop through features of the current type.
        for (new i = 0; i < featureCount; i++)
        {
            // Get volume settings.
            new bool:inUse;
            new bool:enabled;
            new volume;
            switch (feature)
            {
                case VolFeature_Anticamp:
                {
                    inUse = VolAnticampInUse(i);
                    enabled = VolAnticampIsEnabled(i);
                    volume = VolAnticampGetVolumeIndex(i);
                }
                case VolFeature_ClassEdit:
                {
                    inUse = VolClassEditInUse(i);
                    enabled = VolClassEditIsEnabled(i);
                    volume = VolClassEditGetVolumeIndex(i);
                }
            }
            
            // Add to arrays if linked to volume, in use, and enabled.
            if (volume == volumeIndex && inUse && enabled)
            {
                PushArrayCell(featureTypes, _:feature);
                PushArrayCell(featureIndexes, i);
                count++;
            }
        }
    }
    
    // Close and reset handles if nothing was found.
    if (count == 0)
    {
        CloseHandle(featureTypes);
        CloseHandle(featureIndexes);
        
        featureTypes = INVALID_HANDLE;
        featureIndexes = INVALID_HANDLE;
    }
    
    return count;
}

/**
 * Gets whether a point is inside a volume.
 *
 * @param point         The point to check.
 * @param volumeIndex   Volume index.
 * @return              True if the point is in the volume, false otherwise.
 */
bool:VolIsPointInVolume(Float:point[3], volumeIndex)
{
    // Get volume shape type and index.
    new VolShapes:shapeType = VolShapes:Volumes[volumeIndex][Vol_Shape];
    new shapeIndex = Volumes[volumeIndex][Vol_ShapeData];
    
    switch (shapeType)
    {
        case VolShape_Cuboid: return VolIsPointInCuboid(point, shapeIndex);
        case VolShape_Sphere: return VolIsPointInSphere(point, shapeIndex);
    }
    
    return false;
}

