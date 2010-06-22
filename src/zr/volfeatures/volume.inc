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
        VolClearIndex(volumeIndex, false);
    }
    
    VolumeCount = 0;
}

/**
 * Clears the specified volume and resets to default values.
 *
 * Note: Does not validate index.
 *
 * @param volumeIndex       Volume to clear.
 * @param inUse             Optional. Whether to mark volume as in use or not.
 *                          Default is false.
 * @param decrementCounter  Optional. Decrement the volume counter. Default is
 *                          true.
 */
VolClearIndex(volumeIndex, bool:inUse = ZR_VOL_DEFAULT_IN_USE, bool:decrementCounter = true)
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
    VolDetachFeatures(volumeIndex);
    
    Volumes[volumeIndex][Vol_FeatureTypes]      = ZR_VOL_DEFAULT_FEATURE_TYPES;
    Volumes[volumeIndex][Vol_FeatureIndexes]    = ZR_VOL_DEFAULT_FEATURE_INDEXES;
    
    if (decrementCounter)
    {
        VolumeCount--;
    }
}

/**
 * Returns whether a volume is marked as in use.
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
 * Returns whether a volume is enabled or not.
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
 * Returns whether a volume is valid and in use.
 *
 * @param volumeIndex   Volume to validate.
 * @return              True if valid and in use, false otherwise. 
 */
bool:VolIsValid(volumeIndex)
{
    return VolIsValidIndex(volumeIndex) && VolInUse(volumeIndex);
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
            Volumes[volumeIndex][Vol_InUse] = true;
            VolumeCount++;
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
 * Refreshes the arrays with linked features for a volume.
 *
 * @param volumeIndex   Volume to refresh links on.
 */
stock VolRefreshLinks(volumeIndex)
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
 * @param ignoreEnabled     Optional. Ignore whether a feature is disabled or
 *                          not. Default is true.
 * @return                  Number of linked features.
 */
stock VolGetFeatures(volumeIndex, &Handle:featureTypes, &Handle:featureIndexes, bool:ignoreEnabled = true)
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
            if (volume == volumeIndex && inUse && (ignoreEnabled || enabled))
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

/**
 * Builds a user friendly (translated) list of the volume's attributes.
 *
 * @param volumeIndex   Volume to dump.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
VolBuildList(volumeIndex, String:buffer[], maxlen)
{
    decl String:linebuffer[128];
    decl String:langbuffer[128];
    decl String:valuebuffer[256];
    new volCache[VolumeAttributes];
    new color[3];
    new cellswritten;
    
    // Validate index.
    if (!VolIsValid(volumeIndex))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    volCache = Volumes[volumeIndex];
    color[0] = volCache[Vol_EffectColor][0];
    color[1] = volCache[Vol_EffectColor][1];
    color[2] = volCache[Vol_EffectColor][2];
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Name");
    Format(linebuffer, sizeof(linebuffer), "%25s \"%s\" (%d)\n", langbuffer, volCache[Vol_Name], volumeIndex);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Enabled");
    Format(linebuffer, sizeof(linebuffer), "%25s %t\n", langbuffer, volCache[Vol_Enabled] ? "Yes" : "No");
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Shape");
    VolShapeToString(volCache[Vol_Shape], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Effect");
    VolEffectToString(volCache[Vol_Effect], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Effect Color");
    Format(linebuffer, sizeof(linebuffer), "%25s %d %d %d\n", langbuffer, color[0], color[1], color[2]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Teamfilter");
    VolTeamToString(volCache[Vol_TeamFilter], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Trigger Delay");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, volCache[Vol_TriggerDelay]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Priority");
    Format(linebuffer, sizeof(linebuffer), "%25s %d\n", langbuffer, volCache[Vol_Priority]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Conflict Action");
    VolConflictToString(volCache[Vol_ConflictAction], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    return cellswritten;
}

/**
 * Returns a list with linked feature names of the volume.
 *
 * @param volumeIndex   Volume read.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
VolGetFeatureListString(volumeIndex, String:buffer[], maxlen)
{
    decl String:linebuffer[256];
    decl String:valuebuffer[256];
    decl String:strFeature[64];
    new cellswritten;
    
    // Validate index.
    if (!VolIsValid(volumeIndex))
    {
        return 0;
    }
    
    // Initialize and clear buffers.
    linebuffer[0] = 0;
    buffer[0] = 0;
    
    // Get feature indexes.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureIndexes];
    
    if (featureTypes != INVALID_HANDLE)
    {
        new numFeatures = GetArraySize(featureTypes);
        
        // Loop through each feature and add its type and name to the list. If
        // the feature doesn't have a name, write "<type>:index".
        
        for (new feature = 0; feature < numFeatures; feature++)
        {
            new VolumeFeatureTypes:featureType = VolumeFeatureTypes:GetArrayCell(featureTypes, feature);
            new featureIndex = GetArrayCell(featureIndexes, feature);
            
            switch (featureType)
            {
                case VolFeature_Anticamp:
                {
                    strcopy(valuebuffer, sizeof(valuebuffer), AnticampData[featureIndex][Anticamp_Name]);
                    Format(strFeature, sizeof(strFeature), "%t", "Vol Anticamp");
                }
                case VolFeature_ClassEdit:
                {
                    strcopy(valuebuffer, sizeof(valuebuffer), VolClassEditData[featureIndex][VolClassEdit_Name]);
                    Format(strFeature, sizeof(strFeature), "%t", "Vol Classedit");
                }
            }
            
            if (strlen(valuebuffer) == 0)
            {
                Format(valuebuffer, sizeof(valuebuffer), "%d", featureIndex);
            }
            
            Format(linebuffer, sizeof(linebuffer), "%s:%s", strFeature, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            // Add commas after each feature except for the last one.
            if (feature < numFeatures - 1)
            {
                cellswritten += StrCat(buffer, maxlen, ", ");
            }
        }
        
        return cellswritten;
    }
    
    return 0;
}
