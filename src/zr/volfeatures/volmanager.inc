/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volmanager.inc
 *  Type:           Module
 *  Description:    Volume managing API.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
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
 * Adds a new volume with the specified attributes.
 *
 * @param shapeData         Shape data index to use.
 * @param shape             Optional. Shape type that defines volume boundaries.
 * @param effect            Optional. Effect to apply on the volume.
 * @param effectColor       Optional. Color of the effect, if any.
 * @param teamFilter        Optional. Which team(s) the volume applies to.
 * @param triggerDelay      Optional. Delay before triggering 'enter' event, in seconds.
 * @param priority          Optional. Priority for conflict action.
 * @param conflictAction    Optional. What to do in case of volume conflicts.
 * @return                  Volume index if successful, negative on error.
 *                          -1 if no free volumes
 *                          -2 if invalid shape data
 */
VolAdd(shapeData,
       VolumeShapes:shape = ZR_VOL_DEFAULT_SHAPE,
       VolumeEffects:effect = ZR_VOL_DEFAULT_EFFECT,
       effectColor[3] = ZR_VOL_DEFAULT_EFFECT_COLOR,
       VolumeTeamFilters:teamFilter = VolTeam_Humans,
       Float:triggerDelay = ZR_VOL_DEFAULT_TRIGGER_DELAY,
       priority = ZR_VOL_DEFAULT_PRIORITY,
       VolumeConflictActions:conflictAction = ZR_VOL_DEFAULT_CONFLICT_ACTION)
{
    // Validate shapeData.
    new bool:valid;
    switch (shape)
    {
        case VolShape_Cuboid:
        {
            valid = VolCuboidIsValid(shapeData) ? true : false;
        }
        case VolShape_Sphere:
        {
            valid = VolSphereIsValid(shapeData) ? true : false;
        }
    }
    if (!valid)
    {
        return -2;
    }
    
    // Get new volume index.
    new volumeIndex = VolGetFreeVolume();
    if (volumeIndex < 0)
    {
        // No free volumes.
        return -1;
    }
    
    // Set volume attributes.
    Volumes[volumeIndex][Vol_Enabled] = false;  // Start disabled because no features are attached.
    Volumes[volumeIndex][Vol_Shape] = shape;
    Volumes[volumeIndex][Vol_ShapeData] = shapeData;
    Volumes[volumeIndex][Vol_Effect] = effect;
    Volumes[volumeIndex][Vol_EffectColor] = effectColor;
    Volumes[volumeIndex][Vol_TeamFilter] = teamFilter;
    Volumes[volumeIndex][Vol_TriggerDelay] = triggerDelay;
    Volumes[volumeIndex][Vol_Priority] = priority;
    Volumes[volumeIndex][Vol_ConflictAction] = conflictAction;
    
    return volumeIndex;
}

/**
 * Disables and removes the specified volume. Its features are either detached
 * or removed depending on removeFeatures.
 *
 * @param volumeIndex       The volume to remove.
 * @param removeFeatures    Optional. Removes the attached features instead of
 *                          detaching them. Default is remove (true).
 *
 * @return                  True if removed, false otherwise.
 */
bool:VolRemove(volumeIndex, bool:removeFeatures = true)
{
    // Validate volume.
    if (!VolIsValid(volumeIndex))
    {
        // Invalid volume.
        return false;
    }
    
    // Trigger disable event.
    VolOnDisabled(volumeIndex);
    
    // Detach all features.
    VolDetachFeatures(volumeIndex, removeFeatures);
    
    // Remove volume.
    VolClearIndex(volumeIndex);
    
    return true;
}

/**
 * Disables and removes the all volumes. Its features are either detached
 * or removed depending on removeFeatures.
 *
 * @param removeFeatures    Optional. Removes the attached features instead of
 *                          detaching them. Default is remove (true).
 */
VolRemoveAll(bool:removeFeatures = true)
{
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        VolRemove(volumeIndex, removeFeatures);
    }
}


/**
 * Attaches a feature to a volume and sets the volume reference in the feature.
 *
 * @param volumeIndex       Volume data index.
 * @param featureType       Feature types.
 * @param featureIndex      Feature data index.
 * @return                  True if attached, false otherwise.
 */
bool:VolAttachFeature(volumeIndex, VolumeFeatureTypes:featureType, featureIndex)
{
    // Validate volume.
    if (!VolIsValid(volumeIndex))
    {
        // Invalid volume.
        return false;
    }
    
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
    
    // Set volume index in feature.
    switch (featureType)
    {
        case VolFeature_Anticamp:  VolAnticampSetVolumeIndex(featureIndex, volumeIndex);
        case VolFeature_ClassEdit: VolClassEditSetVolumeIndex(featureIndex, volumeIndex);
    }
    
    // Add feature to arrays.
    PushArrayCell(featureTypes, _:featureType);
    PushArrayCell(featureIndexes, featureIndex);
}

/**
 * Detaches a feature from the specified volume.
 *
 * @param volumeIndex       Volume to detach from.
 * @param featureType       The feature type.
 * @param featureIndex      Feature data index.
 * @param removeFeature     Optional. Also remove the feature instead of just
 *                          detaching it. Default is false.
 * @return                  True if successful, false otherwise.
 */
bool:VolDetachFeature(volumeIndex, VolumeFeatureTypes:featureType, featureIndex, bool:removeFeature = false)
{
    // Validate volume.
    if (!VolIsValid(volumeIndex))
    {
        // Invalid volume.
        return false;
    }
    
    // Validate feature.
    new bool:valid;
    switch (featureType)
    {
        case VolFeature_Anticamp:
        {
            valid = VolAnticampIsValid(featureIndex) ? true : false;
        }
        case VolFeature_ClassEdit:
        {
            valid = VolClassEditIsValid(featureIndex) ? true : false;
        }
    }
    if (!valid)
    {
        // Invalid feature.
        return false;
    }
    
    // Detach from volume; disable and reset volume index or
    // remove feature if enabled.
    switch (featureType)
    {
        case VolFeature_Anticamp:
        {
            VolAnticampOnDisabled(featureIndex);
            if (removeFeature)
            {
                VolAnticampReset(featureIndex);
            }
            else
            {
                VolAnticampSetVolumeIndex(featureIndex, -1);
            }
        }
        case VolFeature_ClassEdit:
        {
            VolClassEditOnDisabled(featureIndex);
            if (removeFeature)
            {
                VolClassEditReset(featureIndex);
            }
            else
            {
                VolClassEditSetVolumeIndex(featureIndex, -1);
            }
        }
    }
    
    return true;
}

/**
 * Removes all references to a volume.
 *
 * @param volumeIndex       Volume to derefer.
 * @param removeFeatures    Optional. Also remove the linked features instead
 *                          of just detaching them. Default is false.
 * @return                  True if successful, false otherwise.
 */
bool:VolDetachFeatures(volumeIndex, bool:removeFeatures = false)
{
    // Validate volume.
    if (!VolIsValid(volumeIndex))
    {
        // Invalid volume.
        return false;
    }
    
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureIndexes];
    
    // Check if link arrays exists.
    if (ZR_HasHandle(featureTypes))
    {
        new size = GetArraySize(featureTypes);
        
        if (size > 0)
        {
            // Loop through each feature in the list and detach it.
            for (new i = 0; i < size; i++)
            {
                // Get type and index.
                new VolumeFeatureTypes:featureType = VolumeFeatureTypes:GetArrayCell(featureTypes, i);
                new feature = GetArrayCell(featureIndexes, i);
                
                VolDetachFeature(volumeIndex, featureType, feature, removeFeatures);
            }
        }
        
        // Attempt to close existing handles.
        ZR_CloseHandle(featureTypes);
        ZR_CloseHandle(featureIndexes);
        
        // Update handles.
        Volumes[volumeIndex][Vol_FeatureTypes] = featureTypes;
        Volumes[volumeIndex][Vol_FeatureIndexes] = featureTypes;
    }
    
    return true;
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
