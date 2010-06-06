/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volfeatures.inc
 *  Type:           Module
 *  Description:    Volumetric feature manager.
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
 * List of player locations. Updated by a timer.
 */
new Float:VolPlayerLoc[MAXPLAYERS + 1][3];

/**
 * Cache that specifies if a player is in a volume or not.
 */
new bool:VolPlayerInVolume[MAXPLAYERS + 1][ZR_VOLUMES_MAX];

/**
 * Specifies whether the volumetric features module is enabled or not. Synced
 * with zr_vol CVAR.
 */
new bool:VolEnabled;

/**
 * Counter for trigger delay.
 */
new Float:VolPlayerCountDown[MAXPLAYERS + 1][ZR_VOLUMES_MAX];

/**
 * The handle for a timer that updates player locations. This is the main timer
 * and any feature events can't be updated faster than this interval.
 *
 * Note: Some features may have its own timer for actions on players.
 */
new Handle:hVolUpdateTimer = INVALID_HANDLE;

/**
 * The handle for a timer that do count down on trigger delays.
 */
new Handle:hVolTriggerTimer = INVALID_HANDLE;

/**
 * Cached interval value for trigger timer.
 */
new Float:VolTriggerInterval;

// Sub features.
#include "zr/volfeatures/volanticamp"
#include "zr/volfeatures/anticampmanager"
#include "zr/volfeatures/volclassedit"
#include "zr/volfeatures/classeditmanager"

#include "zr/volfeatures/volume"
#include "zr/volfeatures/cuboid"
#include "zr/volfeatures/sphere"
#include "zr/volfeatures/voltools"
#include "zr/volfeatures/volevents"
#include "zr/volfeatures/volgenericattributes"
#include "zr/volfeatures/volmanager"
#include "zr/volfeatures/cuboidmanager"
#include "zr/volfeatures/spheremanager"
#include "zr/volfeatures/volconversion"
#include "zr/volfeatures/volcommands"
#include "zr/volfeatures/shapecommands"

/**
 * Initialize volumetric features.
 */
VolInit()
{
    // Clear all volumes.
    VolClear();
    
    // Initialize sub features.
    VolAnticampInit();
    VolClassEditInit();
} 

/**
 * Initialize volumetric feature settings.
 */
VolLoad()
{
    // Cache CVAR value.
    VolEnabled = GetConVarBool(g_hCvarsList[CVAR_VOL]);
}

/**
 * Function alias for fully stopping volumetric features.
 */
VolDisable()
{
    VolEnabled = false;
    VolStopUpdateTimer();
    VolDisableVolumes();
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Disabled", "Volfeatures disabled.");
}

/**
 * Function alias for starting volumetric features.
 */
VolEnable()
{
    VolEnabled = true;
    VolStartUpdateTimer();
    VolEnableVolumes();
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Enabled", "Volfeatures enabled.");
}

/**
 * Disables all enabled volumes.
 */
VolDisableVolumes()
{
    // Trigger disable event on all enabled volumes in use.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        if (Volumes[volumeIndex][Vol_InUse] && Volumes[volumeIndex][Vol_Enabled])
        {
            // Mark as disabled.
            Volumes[volumeIndex][Vol_Enabled] = false;
            
            // Trigger player left volume event if inside a volume.
            for (new client = 1; client <= MaxClients; client++)
            {
                // Validate client's connection state.
                if (!IsClientConnected(client) || !IsClientInGame(client))
                {
                    continue;
                }
                
                // Check if player is inside the volume.
                if (VolPlayerInVolume[client][volumeIndex])
                {
                    // Mark as not in the volume and trigger event.
                    VolPlayerInVolume[client][volumeIndex] = false;
                    VolOnPlayerLeave(client, volumeIndex);
                }
            }
            
            // Trigger disabled event.
            VolOnDisabled(volumeIndex);
        }
    }
}

/**
 * Enables all disabled volumes.
 */
VolEnableVolumes()
{
    // Trigger enable event on all volumes in use.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        if (Volumes[volumeIndex][Vol_InUse] && !Volumes[volumeIndex][Vol_Enabled])
        {
            Volumes[volumeIndex][Vol_Enabled] = true;
            VolOnEnabled(volumeIndex);
        }
    }
}

/**
 * Starts the update timer.
 *
 * @return      True if timer is started, false otherwise.
 */
bool:VolStartUpdateTimer()
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return false;
    }
    
    // Stop timer if it exist.
    VolStopUpdateTimer();
    
    // Get update interval.
    new Float:interval = GetConVarFloat(g_hCvarsList[CVAR_VOL_UPDATE_INTERVAL]);
    
    // Validate interval.
    if (interval > 0.0)
    {
        // Create a new timer.
        hVolUpdateTimer = CreateTimer(interval, Event_VolUpdateTimer, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
        
        // Also start the trigger delay timer.
        VolStartTriggerTimer();
        
        // Volumetric features started.
        return true;
    }
    else
    {
        // Volumetric features disabled.
        LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Volfeatures, "Config Validation", "Warning: Console variable \"zr_vol_update_interval\" is zero or negative. Must be positive.");
        return false;
    }
}

/**
 * Kills the update timer if it exists.
 */
VolStopUpdateTimer()
{
    // Kill the timer if it's running.
    ZREndTimer(hVolUpdateTimer);
    
    // Also stop trigger delay timer.
    VolStopTriggerTimer();
    
    // Reset all trigger delay counters.
    VolResetCountDown();
}

/**
 * Starts the update timer if it exists.
 *
 * @return      True if timer is started, false otherwise.
 */
bool:VolStartTriggerTimer()
{
    // Make sure existing timer is killed.
    VolStopTriggerTimer();
    
    // Get trigger interval and cache it.
    VolTriggerInterval = GetConVarFloat(g_hCvarsList[CVAR_VOL_TRIGGER_INTERVAL]);
    
    // Validate interval.
    if (VolTriggerInterval > 0.0)
    {
        // Start the timer.
        hVolTriggerTimer = CreateTimer(VolTriggerInterval, Event_VolTriggerTimer, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
        
        // Trigger timer started.
        return true;
    }
    else
    {
        // Trigger timer not running. Either disabled or invalid interval.
        LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Volfeatures, "Config Validation", "Warning: Console variable \"zr_vol_trigger_interval\" is zero or negative. Must be positive.");
        return false;
    }
}

/**
 * Kills the trigger delay timer if it exists.
 */
VolStopTriggerTimer()
{
    // Kill the timer if it's running.
    ZREndTimer(hVolTriggerTimer);
}

/**
 * Resets volume trigger delay counters on one or more players.
 *
 * @param client    Optional. Specifies a single player to reset. Default is
 *                  -1, all players.
 */
VolResetCountDown(client = -1)
{
    // Check if a client is specified.
    if (client > -1)
    {
        // Reset volume counters.
        for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
        {
            VolPlayerCountDown[client][volumeIndex] = -1.0;
        }
    }
    else
    {
        // Reset all volume counters.
        for (new clientIndex = 0; clientIndex < MAXPLAYERS + 1; clientIndex++)
        {
            for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
            {
                VolPlayerCountDown[clientIndex][volumeIndex] = -1.0;
            }
        }
    }
}

/**
 * Updates all player locations. Used for initialization.
 *
 * Note: If a client is specified, it's NOT validated. This function assumes
 *       the specified client is in game and alive.
 *
 * @param client    Optional. Specify single client to be updated. Default is
 *                  -1.
 */
VolUpdatePlayerLocation(client = -1)
{
    if (client > 0)
    {
        // Assume the client is valid and save location in array.
        GetClientAbsOrigin(client, VolPlayerLoc[client]);
    }
    else
    {
        for (client = 1; client <= MaxClients; client++)
        {
            // Validate client's connection state.
            if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
            {
                continue;
            }
            
            // Save location in array.
            GetClientAbsOrigin(client, VolPlayerLoc[client]);
        }
    }
}

/**
 * Updates player locations and trigger events for each player that enter or
 * leave a volume.
 */
VolUpdatePlayerChanges()
{
    new bool:volumeStates[MAXPLAYERS + 1][ZR_VOLUMES_MAX];
    new bool:volumeNewStates[MAXPLAYERS + 1][ZR_VOLUMES_MAX];
    
    // Loop through players and get state changes first.
    for (new client = 1; client <= MaxClients; client++)
    {
        // Validate client's connection state.
        if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
        {
            // Skip client.
            continue;
        }
        
        // Get the current volume states based on player location cache.
        VolGetPlayerStates(client, volumeStates[client], ZR_VOLUMES_MAX);
        
        // Update player location cache.
        GetClientAbsOrigin(client, VolPlayerLoc[client]);
        
        // Get new volume states.
        VolGetPlayerStates(client, volumeNewStates[client], ZR_VOLUMES_MAX);
    }
    
    // Loop through volumes.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if the volume is disabled and unused.
        if (!VolInUse(volumeIndex) || !VolIsEnabled(volumeIndex))
        {
            // Skip volume.
            continue;
        }
        
        // Loop through players.
        for (new client = 1; client <= MaxClients; client++)
        {
            // Validate client's connection state.
            if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
            {
                // Skip client.
                continue;
            }
            
            // Check team filtering on the volume.
            if (!VolTeamFilterMatch(client, volumeIndex))
            {
                // Team filter mismatch.
                continue;
            }
            
            // Get the current client's states for the current volume.
            new bool:newState = volumeNewStates[client][volumeIndex];
            new bool:oldState = volumeStates[client][volumeIndex];
            
            // Check for no change.
            if (newState == oldState)
            {
                // No change. Skip to next volume.
                continue;
            }
            
            // Check if client entered the volume.
            if (newState && !oldState)
            {
                // Get trigger delay value.
                new Float:triggerDelay = Volumes[volumeIndex][Vol_TriggerDelay];
                
                // Check if the volume has a trigger delay.
                if (triggerDelay > 0.0)
                {
                    // Set count down value.
                    VolPlayerCountDown[client][volumeIndex] = triggerDelay;
                }
                else
                {
                    // Update cache.
                    VolPlayerInVolume[client][volumeIndex] = true;
                    
                    // No trigger delay, trigger event instantly.
                    VolOnPlayerEnter(client, volumeIndex);
                }
            }
            
            // Check if client left the volume.
            else if (!newState && oldState)
            {
                // Make sure count down value for trigger delay is reset.
                VolPlayerCountDown[client][volumeIndex] = -1.0;
                
                // Only trigger left volume event if player already is in the
                // volume, so volumes with trigger delay won't get a left event
                // before the enter event.
                if (VolPlayerInVolume[client][volumeIndex])
                {
                    // Update cache.
                    VolPlayerInVolume[client][volumeIndex] = false;
                    
                    // Trigger event.
                    VolOnPlayerLeave(client, volumeIndex);
                }
            }
        }
    }
}

/**
 * Callback for update timer. This is the main timer in volumetric features.
 */
public Action:Event_VolUpdateTimer(Handle:timer)
{
    VolUpdatePlayerChanges();
}

/**
 * Callback for trigger delay timer.
 */
public Action:Event_VolTriggerTimer(Handle:timer)
{
    new Float:countDown;
    
    // Loop through volumes.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if volume is in use and enabled.
        if (!VolInUse(volumeIndex) || !VolIsEnabled(volumeIndex))
        {
            // Not in use or enabled, skip volume.
            continue;
        }
        
        // Loop through players.
        for (new client = 1; client <= MaxClients; client++)
        {
            // Validate client's connection state.
            if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
            {
                // Skip client.
                continue;
            }
            
            // Get count down value.
            countDown = VolPlayerCountDown[client][volumeIndex];
            
            // Check if volume trigger delay is enabled.
            if (countDown > 0.0)
            {
                // Substract by trigger interval.
                countDown -= VolTriggerInterval;
                
                // Check if time is up.
                if (countDown <= 0.0)
                {
                    // Update cache.
                    VolPlayerInVolume[client][volumeIndex] = true;
                    
                    // Trigger volume enter event.
                    VolOnPlayerEnter(client, volumeIndex);
                    
                    // Reset count down value.
                    VolPlayerCountDown[client][volumeIndex] = -1.0;
                }
                
                // Update count down value and continue.
                VolPlayerCountDown[client][volumeIndex] = countDown;
            }
        }
    }
}

/**
 * Called when zr_vol CVAR is changed.
 */
public VolEnabledChanged(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    new bool:isEnabled = bool:StringToInt(newvalue);
    
    if (isEnabled)
    {
        // Volumetric features is enabled.
        VolEnable();
    }
    else
    {
        // Volumetric features is disabled.
        VolDisable();
    }
}
