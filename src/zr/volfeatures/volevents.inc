/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volevents.inc
 *  Type:           Module
 *  Description:    Handles generic events for volumetric features.
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
 * Called when a player enters a volume.
 *
 * @param client            The client index.
 * @param volumeIndex       The volume index.
 */
VolOnPlayerEnter(client, volumeIndex)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Event", "Player %N entered volume %d.", client, volumeIndex);
    
    // Get the list of features linked to the current volume.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureTypes];
    
    // Check if any features are linked to this volume.
    if (ZR_HasHandle(featureTypes))
    {
        // Get number of linked features.
        new size = GetArraySize(featureTypes);
        
        // Loop through each feature.
        for (new i = 0; i < size; i++)
        {
            new featureIndex = GetArrayCell(featureIndexes, i);
            new VolumeFeatureTypes:featureType = GetArrayCell(featureTypes, i);
            
            // Forward event to feature.
            switch (featureType)
            {
                case VolFeature_ClassEdit:
                {
                    VolClassEditOnPlayerEnter(client, featureIndex);
                }
            }
        }
    }
}

/**
 * Called when a player leaves a volume.
 *
 * @param client            The client index.
 * @param volumeIndex       The volume index.
 */
VolOnPlayerLeave(client, volumeIndex)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Event", "Player %N left volume %d.", client, volumeIndex);
    
    // Get the list of features linked to the current volume.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureTypes];
    
    // Check if any features are linked to this volume.
    if (ZR_HasHandle(featureTypes))
    {
        // Get number of linked features.
        new size = GetArraySize(featureTypes);
        
        // Loop through each feature.
        for (new i = 0; i < size; i++)
        {
            new featureIndex = GetArrayCell(featureIndexes, i);
            new VolumeFeatureTypes:featureType = GetArrayCell(featureTypes, i);
            
            // Forward event to feature.
            switch (featureType)
            {
                case VolFeature_Anticamp:
                {
                    VolAnticampOnPlayerLeave(client, featureIndex);
                }
                case VolFeature_ClassEdit:
                {
                    VolClassEditOnPlayerLeave(client, featureIndex);
                }
            }
        }
    }
}

/**
 * Called when a player spawned. Used for initializing player data.
 *
 * @param client    The client index.
 */
VolOnPlayerSpawn(client)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    // Cache player location.
    VolUpdatePlayerLocation(client);
}

/**
 * Called when a player died.
 *
 * @param client    The client index.
 */
VolOnPlayerDeath(client)
{
    // Send player left volume event to all volumes the player was in.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if volume is unused.
        if (!Volumes[volumeIndex][Vol_InUse])
        {
            continue;
        }
        
        // Check if volume is disabled.
        if (!Volumes[volumeIndex][Vol_Enabled])
        {
            continue;
        }
        
        // Check if player is inside the volume.
        if (VolPlayerInVolume[client][volumeIndex])
        {
            // Mark as not in the volume and trigger leave-event.
            VolPlayerInVolume[client][volumeIndex] = false;
            VolOnPlayerLeave(client, volumeIndex);
        }
    }
}

/**
 * Called when a player disconnects.
 *
 * @param client    The client index.
 */
VolOnPlayerDisconnect(client)
{
    // Disable trigger delay counters.
    VolResetCountDown(client);
    
    // Trigger death event to clean up.
    VolOnPlayerDeath(client);
}

/**
 * Called when the round starts. Main enable event for volumetric features.
 */
VolOnRoundStart()
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    // Start main timer.
    VolStartUpdateTimer();
    
    // Start volumes.
    VolEnableVolumes();
}

/**
 * Called when the round ends. Main disable event for volumetric features.
 */
VolOnRoundEnd()
{
    // Stop main timer.
    VolStopUpdateTimer();
    
    // Stop volumes.
    VolDisableVolumes();
}

/**
 * Called right before the map ends.
 */
VolOnMapEnd()
{
	// Make sure timers and volumes are stopped. Foreward event.
	VolOnRoundEnd();
}

/**
 * Called when a volume is disabled.
 *
 * @param volumeIndex       The volume index.
 */
VolOnDisabled(volumeIndex)
{
    // Get the list of features linked to the current volume.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureTypes];
    
    // Check if any features are linked to this volume.
    if (ZR_HasHandle(featureTypes))
    {
        // Get number of linked features.
        new size = GetArraySize(featureTypes);
        
        // Loop through each feature.
        for (new i = 0; i < size; i++)
        {
            new featureIndex = GetArrayCell(featureIndexes, i);
            new VolumeFeatureTypes:featureType = GetArrayCell(featureTypes, i);
            
            // Forward event to feature.
            switch (featureType)
            {
                case VolFeature_Anticamp:
                {
                    VolAnticampOnDisabled(featureIndex);
                }
                case VolFeature_ClassEdit:
                {
                    VolClassEditOnDisabled(featureIndex);
                }
            }
        }
        
        // Log debug message.
        LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Event", "Volume %d disabled.", volumeIndex);
    }
}

/**
 * Called when a volume is enabled.
 *
 * @param volumeIndex       The volume index.
 */
VolOnEnabled(volumeIndex)
{
    // Get the list of features linked to the current volume.
    new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
    new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureTypes];
    
    // Check if any features are linked to this volume.
    if (ZR_HasHandle(featureTypes))
    {
        // Get number of linked features.
        new featureCount = GetArraySize(featureTypes);
        
        // Loop through each feature.
        for (new i = 0; i < featureCount; i++)
        {
            new featureIndex = GetArrayCell(featureIndexes, i);
            new VolumeFeatureTypes:featureType = GetArrayCell(featureTypes, i);
            
            // Forward event to feature.
            switch (featureType)
            {
                case VolFeature_Anticamp:
                {
                    VolAnticampOnEnabled(featureIndex);
                }
            }
        }
        
        // Log debug message.
        LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Event", "Volume %d enabled.", volumeIndex);
    }
}
