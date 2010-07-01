/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volanticamp.inc
 *  Type:           Module
 *  Description:    Anti-camp handler.
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
 * Initialization event for anticamp feature.
 */
VolAnticampInit()
{
    // Set default attributes.
    for (new anticampIndex = 0; anticampIndex < ZR_ANTICAMP_MAX; anticampIndex++)
    {
        VolAnticampReset(anticampIndex, false, false);
    }
}

/**
 * Validates anti camp data index.
 *
 * @param anticampIndex     Index to validate.
 * @return                  True if valid, false otherwise.
 */
bool:VolAnticampIsValidIndex(anticampIndex)
{
    if (anticampIndex >= 0 && anticampIndex < ZR_VOLUMES_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns whether a anti camp is valid and in use.
 *
 * @param volumeIndex   Anti camp to validate.
 * @return              True if valid and in use, false otherwise. 
 */
bool:VolAnticampIsValid(anticampIndex)
{
    return VolAnticampIsValidIndex(anticampIndex) && VolAnticampInUse(anticampIndex);
}

/**
 * Returns whether a anti camp is attached to a volume.
 *
 * @param volumeIndex   Anti camp to check.
 * @return              True if attached, false otherwise. 
 */
bool:VolAnticampIsAttached(anticampIndex)
{
    return (AnticampData[anticampIndex][Anticamp_VolumeIndex] >= 0);
}

/**
 * Returns the volume index that the specified feature is linked to.
 *
 * @param anticampIndex     Anti camp data index.
 */
VolAnticampGetVolumeIndex(anticampIndex)
{
    return AnticampData[anticampIndex][Anticamp_VolumeIndex];
}

/**
 * Sets the volume index to a anticamp feature.
 *
 * @param anticampIndex     Anti camp data index.
 * @param volumeIndex       New volume index.
 */
VolAnticampSetVolumeIndex(anticampIndex, volumeIndex)
{
    AnticampData[anticampIndex][Anticamp_VolumeIndex] = volumeIndex;
}

/**
 * Returns the whether the specified anti camp feature is in use.
 *
 * @param anticampIndex     Anti camp data index.
 */
bool:VolAnticampInUse(anticampIndex)
{
    return AnticampData[anticampIndex][Anticamp_InUse];
}

/**
 * Returns the whether the specified anti camp feature is enabled.
 *
 * @param anticampIndex     Anti camp data index.
 */
bool:VolAnticampIsEnabled(anticampIndex)
{
    return AnticampData[anticampIndex][Anticamp_Enabled];
}

/**
 * Gets the first free anticamp data index.
 *
 * @return      The first free anticamp data index if successful, or -1 if
 *              there are no free volumes.
 */
VolAnticampGetFreeIndex()
{
    // Loop through all indexes.
    for (new anticampIndex = 0; anticampIndex < ZR_VOLUMES_MAX; anticampIndex++)
    {
        // Check if it's free.
        if (!AnticampData[anticampIndex][Anticamp_InUse])
        {
            // Mark as in use.
            AnticampData[anticampIndex][Anticamp_InUse] = true;
            AnticampCount++;
            
            // Return the new index.
            return anticampIndex;
        }
    }
    
    // No free index found.
    return -1;
}

/**
 * Disables feature and resets data to defaults at the specified index.
 *
 * @param anticampIndex     Anti camp data index.
 * @param inUse             Optional. Sets the InUse setting. Default is false.
 * @param decrementCounter  Optional. Decrement counter by one. Default is true.
 */
VolAnticampReset(anticampIndex, bool:inUse = ZR_ANTICAMP_DEF_IN_USE, bool:decrementCounter = true)
{
    AnticampData[anticampIndex][Anticamp_InUse]         = inUse;
    AnticampData[anticampIndex][Anticamp_Enabled]       = false;
    strcopy(AnticampData[anticampIndex][Anticamp_Name], VOL_NAME_LEN, ZR_ANTICAMP_DEF_NAME);
    AnticampData[anticampIndex][Anticamp_VolumeIndex]   = ZR_ANTICAMP_DEF_VOLUME_INDEX;
    
    AnticampData[anticampIndex][Anticamp_Interval]      = ZR_ANTICAMP_DEF_INTERVAL;
    ZREndTimer(AnticampData[anticampIndex][Anticamp_Timer]);    // Make sure timer is stopped.
    
    AnticampData[anticampIndex][Anticamp_Action]        = ZR_ANTICAMP_DEF_ACTION;
    AnticampData[anticampIndex][Anticamp_Amount]        = ZR_ANTICAMP_DEF_AMOUNT;
    
    AnticampData[anticampIndex][Anticamp_Warning]       = ZR_ANTICAMP_DEF_WARNING;
    Format(String:AnticampData[anticampIndex][Anticamp_Message], 256, ZR_ANTICAMP_DEF_MESSAGE);
    
    if (decrementCounter)
    {
        AnticampCount--;
    }
}

/**
 * Event callback. Enables a anticamp volume.
 *
 * @param anticampIndex     Anti camp data index.    
 */
VolAnticampOnEnabled(anticampIndex)
{
    // Validate index.
    if (!VolAnticampIsValidIndex(anticampIndex))
    {
        return;
    }
    
    // Check if in use and disabled.
    if (AnticampData[anticampIndex][Anticamp_InUse] &&
        !AnticampData[anticampIndex][Anticamp_Enabled])
    {
        // Kill timer if it exists.
        ZREndTimer(AnticampData[anticampIndex][Anticamp_Timer]);
        
        // Get interval.
        new Float:interval = AnticampData[anticampIndex][Anticamp_Interval];
        
        // Validate interval.
        if (interval > 0.0)
        {
            AnticampData[anticampIndex][Anticamp_Timer] = CreateTimer(interval, Event_VolAnticampTrigger, anticampIndex, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
            AnticampData[anticampIndex][Anticamp_Enabled] = true;
            LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Vol state", "Enabled anticamp feature %d.", anticampIndex);
        }
        else
        {
            LogEvent(_, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Volfeatures, "Config Validation", "Warning: Invalid interval %.2f in anticamp feature %d.", interval, anticampIndex);
        }
    }
}

/**
 * Event callback. Stops existing anticamp timer on a volume.
 *
 * @param anticampIndex     Anti camp data index. 
 */
VolAnticampOnDisabled(anticampIndex)
{
    // Validate index.
    if (!VolAnticampIsValidIndex(anticampIndex))
    {
        return;
    }
    
    // Check if in use.
    if (AnticampData[anticampIndex][Anticamp_InUse])
    {
        // Stop timer.
        ZREndTimer(AnticampData[anticampIndex][Anticamp_Timer]);
        AnticampData[anticampIndex][Anticamp_Enabled] = false;
        
        LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Vol state", "Disabled anticamp feature %d.", anticampIndex);
    }
}

/**
 * Starts all existing anticamp timers.
 */
stock VolAnticampEnableAll()
{
    // Loop through all anti camp features.
    for (new anticampIndex = 0; anticampIndex < ZR_ANTICAMP_MAX; anticampIndex++)
    {
        // Check if unused.
        if (!VolAnticampInUse(anticampIndex))
        {
            // Volume not in use, skip it.
            continue;
        }
        
        VolAnticampOnEnabled(anticampIndex);
    }
}

/**
 * Stops all existing anticamp timers.
 */
stock VolAnticampDisableAll()
{
    // Loop through all volumes.
    for (new anticampIndex = 0; anticampIndex < ZR_ANTICAMP_MAX; anticampIndex++)
    {
        VolAnticampOnDisabled(anticampIndex);
    }
}

/**
 * Called when a player leave a anticamp volume.
 *
 * @param client            The client index.
 * @param anticampIndex     Anti camp data index. 
 */
VolAnticampOnPlayerLeave(client, anticampIndex)
{
    new userid = GetClientUserId(client);
    switch (AnticampData[anticampIndex][Anticamp_Action])
    {
        case Anticamp_Drug:
        {
            ServerCommand("sm_drug #%d 0", userid);
        }
        case Anticamp_Ignite:
        {
            ExtinguishEntity(client);
        }
    }
}

/**
 * Timer callback for anticamp volumes. Applies actions on players in volumes.
 */
public Action:Event_VolAnticampTrigger(Handle:timer, any:anticampIndex)
{
    new volumeIndex = VolAnticampGetVolumeIndex(anticampIndex);
    
    // Loop through all players.
    for (new client = 1; client <= MaxClients; client++)
    {
        // Validate client's connection state.
        if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
        {
            continue;
        }
        
        // Check if the volume is unused.
        if (!VolInUse(volumeIndex))
        {
            continue;
        }
        
        // Check if the volume is disabled.
        if (!VolIsEnabled(volumeIndex))
        {
            continue;
        }
        
        // Check if player is in the volume.
        if (VolPlayerInVolume[client][volumeIndex])
        {
            // Apply action.
            VolAnticampApplyAction(client, anticampIndex, volumeIndex);
        }
    }
}

/**
 * Applies action on a client for the specified volume.
 *
 * @param client            The client index.
 * @param anticampIndex     Anti camp data index.
 * @param volumeIndex       Volume data index.
 */
VolAnticampApplyAction(client, anticampIndex, volumeIndex)
{
    new Float:amount = AnticampData[anticampIndex][Anticamp_Amount];
    
    // Set client language.
    SetGlobalTransTarget(client);
    
    // Get player name.
    decl String:name[64];
    GetClientName(client, name, sizeof(name));
    
    // Send warning message.
    VolAnticampWarnPlayer(client, anticampIndex);
    
    switch (AnticampData[anticampIndex][Anticamp_Action])
    {
        case Anticamp_NoAction:
        {
            // Do nothing.
        }
        case Anticamp_Damage:
        {
            // Give damage to player. Kill if zero HP or below.
            new damage = RoundToNearest(amount);
            new health = GetClientHealth(client) - damage;
            
            if (health > 0)
            {
                SetEntityHealth(client, health);
            }
            else
            {
                // Health is zero or below. Kill player.
                ForcePlayerSuicide(client);
                
                // Log event.
                LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Slayed", name, volumeIndex);
            }
        }
        case Anticamp_Slay:
        {
            // Instantly kill the player.
            ForcePlayerSuicide(client);
            
            // Log event.
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Slayed", name, volumeIndex);
        }
        case Anticamp_Drug:
        {
            new userid = GetClientUserId(client);
            ServerCommand("sm_drug #%d 1", userid);
            
            // Log event.
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Drugged", name, volumeIndex);
        }
        case Anticamp_Ignite:
        {
            // Validate amount.
            if (amount > 0.0)
            {
                // Extinguish player first.
                ExtinguishEntity(client);
                
                // Ignite player for "amount" seconds.
                IgniteEntity(client, amount);
                
                // Log event.
                LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Igniteed", name, volumeIndex);
            }
        }
    }
}

/**
 * Gives a warning to the specified player for the specified volume.
 *
 * @param client            The client index.
 * @param anticampIndex     Anti camp data index.
 */
VolAnticampWarnPlayer(client, anticampIndex)
{
    decl String:buffer[256];
    new bool:custommessage = (strlen(AnticampData[anticampIndex][Anticamp_Message]) > 0) ? true : false;
    
    // Set language.
    SetGlobalTransTarget(client);
    
    // Format message.
    if (custommessage)
    {
        // Use custom message.
        strcopy(buffer, sizeof(buffer), AnticampData[anticampIndex][Anticamp_Message]);
    }
    else
    {
        // Use default anticamp message in translations file.
        Format(buffer, sizeof(buffer), "%t", "Vol Anticamp Def Message");
    }
    
    switch (AnticampData[anticampIndex][Anticamp_Warning])
    {
        case Anticamp_NoWarning:
        {
            // Do nothing.
        }
        case Anticamp_Chat:
        {
            // Apply ZR formatting and print chat message.
            TranslationPluginFormatString(buffer, sizeof(buffer));
            PrintToChat(client, buffer);
        }
        case Anticamp_Center:
        {
            // Print centered message.
            PrintCenterText(client, buffer);
        }
        case Anticamp_Menu:
        {
            // Display the message in a menu panel.
            new Handle:panel = CreatePanel();
            
            SetPanelTitle(panel, "Zombie:Reloaded");
            DrawPanelItem(panel, "", ITEMDRAW_SPACER);
            DrawPanelItem(panel, buffer);
            DrawPanelItem(panel, "", ITEMDRAW_SPACER);
            
            SetPanelCurrentKey(panel, 10);
            
            Format(buffer, sizeof(buffer), "%t", "Exit");
            DrawPanelItem(panel, buffer, ITEMDRAW_CONTROL);
            
            SendPanelToClient(panel, client, Handler_AnitcampDummy, 10);
            CloseHandle(panel);
        }
    }
}

/**
 * Dummy handler for panel messages.
 */
public Handler_AnitcampDummy(Handle:menu, MenuAction:action, param1, param2)
{
    // Do nothing.
}

/**
 * Builds a user friendly (translated) list of the anti-camp's attributes.
 *
 * @param anticampIndex     Index in anticamp data array.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of destination buffer.
 * @return                  Number of cells written.
 */
VolAnticampBuildList(anticampIndex, String:buffer[], maxlen)
{
    decl String:linebuffer[128];
    decl String:langbuffer[128];
    decl String:valuebuffer[256];
    new anticampcache[VolTypeAnticamp];
    new cellswritten;
    
    // Validate index.
    if (!VolAnticampIsValid(anticampIndex))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    anticampcache = AnticampData[anticampIndex];
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Name");
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, anticampcache[Anticamp_Name]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Enabled");
    Format(linebuffer, sizeof(linebuffer), "%25s %t\n", langbuffer, anticampcache[Anticamp_Enabled] ? "Yes" : "No");
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Interval");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, anticampcache[Anticamp_Interval]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Action");
    VolAnticampActionToString(anticampcache[Anticamp_Action], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Action Amount");
    Format(linebuffer, sizeof(linebuffer), "%25s %.2f\n", langbuffer, anticampcache[Anticamp_Amount]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Warning Type");
    VolAnticampWarningToString(anticampcache[Anticamp_Warning], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Warning Message");
    Format(linebuffer, sizeof(linebuffer), "%25s %s\n", langbuffer, anticampcache[Anticamp_Message]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    if (strlen(anticampcache[Anticamp_Message]) == 0)
    {
        // No warning message.
        Format(valuebuffer, sizeof(valuebuffer), "%t", "Menu default");
    }
    else
    {
        strcopy(valuebuffer, sizeof(valuebuffer), anticampcache[Anticamp_Message]);
    }
    Format(langbuffer, sizeof(langbuffer), "%t", "Vol Attrib Warning Message");
    Format(linebuffer, sizeof(linebuffer), "%25s \"%s\"\n", langbuffer, valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    return cellswritten;
}
