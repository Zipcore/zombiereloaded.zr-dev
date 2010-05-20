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
    
    // Check if in use.
    if (AnticampData[anticampIndex][Anticamp_InUse])
    {
        // Kill timer if it exists.
        ZREndTimer(AnticampData[anticampIndex][Anticamp_Timer]);
        
        // Get interval.
        new Float:interval = AnticampData[anticampIndex][Anticamp_Interval];
        
        // Validate interval.
        if (interval > 0.0)
        {
            AnticampData[anticampIndex][Anticamp_Timer] = CreateTimer(interval, Event_VolAnticampTrigger, anticampIndex, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
            LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Vol state", "Enabled anticamp feature %d.", anticampIndex);
        }
        else
        {
            LogEvent(_, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Volfeatures, "Config Validation", "Warning: Invalid interval %.2f in anticamp feature %d.", interval, anticampIndex);
        }
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
        
        LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "Vol state", "Disabled anticamp feature %d.", anticampIndex);
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
 * Disables feature and resets data to defaults at the specified index.
 *
 * @param anticampIndex     Anti camp data index.
 * @param inUse             Optional. Sets the InUse setting. Default is false.
 * @param decrementCounter  Optional. Decrement counter by one. Default is true.
 */
VolAnticampReset(anticampIndex, bool:inUse = ZR_ANTICAMP_DEF_IN_USE, bool:decrementCounter = true)
{
    AnticampData[anticampIndex][Anticamp_InUse]         = inUse;
    AnticampData[anticampIndex][Anticamp_Enabled]       = ZR_ANTICAMP_DEF_ENABLED;
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
                LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Slay", name, volumeIndex);
            }
        }
        case Anticamp_Slay:
        {
            // Instantly kill the player.
            ForcePlayerSuicide(client);
            
            // Log event.
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Slay", name, volumeIndex);
        }
        case Anticamp_Drug:
        {
            new userid = GetClientUserId(client);
            ServerCommand("sm_drug #%d 1", userid);
            
            // Log event.
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Drug", name, volumeIndex);
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
                LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Volfeatures, "Anti-camp", "%t", "Vol Ignite", name, volumeIndex);
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
        Format(buffer, sizeof(buffer), "%t", "Vol Anticamp Message");
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
 * Dumps data to be used by zr_vol_list command.
 *
 * @param anticampIndex     Index in anticamp data array.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of destination buffer.
 * @return                  Number of cells written.
 */
VolAnticampDumpData(anticampIndex, String:buffer[], maxlen)
{
    decl String:linebuffer[128];
    decl String:valuebuffer[256];
    new anticampcache[VolTypeAnticamp];
    new cellswritten;
    
    // Validate index.
    if (!VolAnticampValidateIndex(anticampIndex))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    anticampcache = AnticampData[anticampIndex];
    
    Format(linebuffer, sizeof(linebuffer), "Interval:           %.2f\n", anticampcache[Anticamp_Interval]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    VolAnticampActionToString(anticampcache[Anticamp_Action], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "Action:             %s\n", valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "Action amount:      %.2f\n", anticampcache[Anticamp_Amount]);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    VolAnticampWarningToString(anticampcache[Anticamp_Warning], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), "Warning type:       %s\n", valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    strcopy(valuebuffer, sizeof(valuebuffer), anticampcache[Anticamp_Message]);
    Format(linebuffer, sizeof(linebuffer), "Warning message:    \"%s\"\n", valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    return cellswritten;
}


/**************************************
 *                                    *
 *  CONVERTING FUNCTIONS              *
 *                                    *
 **************************************/

/**
 * Converts a action type to a string.
 *
 * @param actionType    Action type to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @param shortName     Optional. Write short name or human readable name.
 *                      Default is human readable (false).
 * @return              Number of cells written.
 */
VolAnticampActionToString(VolAnticampAction:actionType, String:buffer[], maxlen, bool:shortName = false)
{
    switch (actionType)
    {
        case Anticamp_NoAction:
        {
            return shortName ? strcopy(buffer, maxlen, "none") : strcopy(buffer, maxlen, "No action");
        }
        case Anticamp_Damage:
        {
            return shortName ? strcopy(buffer, maxlen, "damage") : strcopy(buffer, maxlen, "Damage player");
        }
        case Anticamp_Slay:
        {
            return shortName ? strcopy(buffer, maxlen, "slay") : strcopy(buffer, maxlen, "Kill player");
        }
        case Anticamp_Drug:
        {
            return shortName ? strcopy(buffer, maxlen, "drug") : strcopy(buffer, maxlen, "Drug player ");
        }
        case Anticamp_Ignite:
        {
            return shortName ? strcopy(buffer, maxlen, "ignite") : strcopy(buffer, maxlen, "Ignite player");
        }
    }
    
    return 0;
}

/**
 * Converts a action string type to a action type.
 *
 * @param action    Action string type to convert.
 * @return          Action type or Anticamp_NoAction if failed.
 */
stock VolAnticampAction:VolAnticampStringToAction(const String:action[])
{
    // Check if empty.
    if (strlen(action) == 0)
    {
        return Anticamp_NoAction;
    }
    
    if (StrEqual(action, "none", false))
    {
        return Anticamp_NoWarning;
    }
    else if (StrEqual(action, "damage", false))
    {
        return Anticamp_Damage;
    }
    else if (StrEqual(action, "slay", false))
    {
        return Anticamp_Slay;
    }
    else if (StrEqual(action, "drug", false))
    {
        return Anticamp_Drug;
    }
    else if (StrEqual(action, "ignite", false))
    {
        return Anticamp_Ignite;
    }
    
    // No match.
    return Anticamp_NoAction;
}

/**
 * Converts a warning type to a string.
 *
 * @param warningType   Warning type to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @param shortName     Optional. Write short name or human readable name.
 *                      Default is human readable (false).
 * @return              Number of cells written.
 */
VolAnticampWarningToString(VolAnticampeWarningType:warningType, String:buffer[], maxlen, bool:shortName = false)
{
    switch (warningType)
    {
        case Anticamp_NoWarning:
        {
            return shortName ? strcopy(buffer, maxlen, "none") : strcopy(buffer, maxlen, "No warning");
        }
        case Anticamp_Chat:
        {
            return shortName ? strcopy(buffer, maxlen, "chat") : strcopy(buffer, maxlen, "Chat area");
        }
        case Anticamp_Center:
        {
            return shortName ? strcopy(buffer, maxlen, "center") : strcopy(buffer, maxlen, "Centered message");
        }
        case Anticamp_Menu:
        {
            return shortName ? strcopy(buffer, maxlen, "menu") : strcopy(buffer, maxlen, "Message in menu panel");
        }
    }
    
    return 0;
}

/**
 * Converts a warning string type to a warning type.
 *
 * @param warning   Warning string type to convert.
 * @return          Warning type, or Anticamp_NoWarning if failed.
 */
stock VolAnticampeWarningType:VolAnticampStringToWarning(const String:warning[])
{
    // Check if empty.
    if (strlen(warning) == 0)
    {
        return Anticamp_NoWarning;
    }
    
    if (StrEqual(warning, "none", false))
    {
        return Anticamp_NoWarning;
    }
    else if (StrEqual(warning, "chat", false))
    {
        return Anticamp_Chat;
    }
    else if (StrEqual(warning, "center", false))
    {
        return Anticamp_Center;
    }
    else if (StrEqual(warning, "menu", false))
    {
        return Anticamp_Menu;
    }
    
    // No match.
    return Anticamp_NoWarning;
}


/**************************************
 *                                    *
 *  ATTRIBUTE FUNCTIONS               *
 *                                    *
 **************************************/

/**
 * Sets anticamp spesific attributes on a anticamp volume.
 *
 * @param anticampIndex     Anti camp data index.
 * @param attribName        Attribute to modify.
 * @param attribVlue        Attribute value to set.
 * @return                  True if successfully set, false otherwise.
 */
bool:VolAnticampSetAttribute(anticampIndex, const String:attribName[], const String:attribValue[])
{
    // Validate data index.
    if (!VolAnticampValidateIndex(anticampIndex))
    {
        return false;
    }
    
    // Check attribute names.
    if (StrEqual(attribName, "interval", false))
    {
        if (VolAnticampSetIntervalString(anticampIndex, attribValue))
        {
            return true;
        }
    }
    else if (StrEqual(attribName, "action", false))
    {
        if (VolAnticampSetActionString(anticampIndex, attribValue))
        {
            return true;
        }
    }
    else if (StrEqual(attribName, "amount", false))
    {
        if (VolAnticampSetAmountString(anticampIndex, attribValue))
        {
            return true;
        }
    }
    else if (StrEqual(attribName, "warning", false))
    {
        if (VolAnticampSetWarningString(anticampIndex, attribValue))
        {
            return true;
        }
    }
    else if (StrEqual(attribName, "message", false))
    {
        // Unsupported because of technical limits in command parser. Spaces
        // and quoted strings aren't supported yet.
    }
    
    return false;
}

/**
 * Parses a interval string value and applies it to the specified volume.
 *
 * @param anticampIndex     Anti camp data index.
 * @param interval          Interval to set. A floating point number formatted
 *                          as a string. 
 * @return                  True if successfully set, false otherwise.
 */
bool:VolAnticampSetIntervalString(anticampIndex, const String:interval[])
{
    new Float:anticampinterval;
    
    // Check if string value is empty.
    if (strlen(interval) == 0)
    {
        return false;
    }
    
    // Convert value.
    anticampinterval = StringToFloat(interval);
    
    // Apply value.
    AnticampData[anticampIndex][Anticamp_Interval] = anticampinterval;
    return true;
}

/**
 * Parses a action type string value and applies it to the specified volume.
 *
 * @param anticampIndex     Anti camp data index.
 * @param action            Action type to set.
 * @return                  True if successfully set, false otherwise.
 */
bool:VolAnticampSetActionString(anticampIndex, const String:action[])
{
    // Check if string value is empty.
    if (strlen(action) == 0)
    {
        return false;
    }
    
    // Check effect string values and apply them to the volume.
    if (StrEqual(action, "none", false))
    {
        AnticampData[anticampIndex][Anticamp_Action] = Anticamp_NoAction;
        return true;
    }
    else if (StrEqual(action, "damage", false))
    {
        AnticampData[anticampIndex][Anticamp_Action] = Anticamp_Damage;
        return true;
    }
    else if (StrEqual(action, "slay", false))
    {
        AnticampData[anticampIndex][Anticamp_Action] = Anticamp_Slay;
        return true;
    }
    else if (StrEqual(action, "drug", false))
    {
        AnticampData[anticampIndex][Anticamp_Action] = Anticamp_Drug;
        return true;
    }
    else if (StrEqual(action, "ignite", false))
    {
        AnticampData[anticampIndex][Anticamp_Action] = Anticamp_Ignite;
        return true;
    }
    
    // The string value didn't match any valid action types.
    return false;
}

/**
 * Parses a action amount string value and applies it to the specified volume.
 *
 * @param anticampIndex     Anti camp data index.
 * @param amount            Amount to set. A floating point number formatted as
 *                          a string. 
 * @return                  True if successfully set, false otherwise.
 */
bool:VolAnticampSetAmountString(anticampIndex, const String:amount[])
{
    new Float:actionamount;
    
    // Check if string value is empty.
    if (strlen(amount) == 0)
    {
        return false;
    }
    
    // Convert value.
    actionamount = StringToFloat(amount);
    
    // Apply value.
    AnticampData[anticampIndex][Anticamp_Amount] = actionamount;
    return true;
}

/**
 * Parses a warning type string value and applies it to the specified volume.
 *
 * @param anticampIndex     Anti camp data index.
 * @param warning           warning type to set.
 * @return                  True if successfully set, false otherwise.
 */
bool:VolAnticampSetWarningString(anticampIndex, const String:warning[])
{
    
    // Check if string value is empty.
    if (strlen(warning) == 0)
    {
        return false;
    }
    
    // Check effect string values and apply them to the volume.
    if (StrEqual(warning, "none", false))
    {
        AnticampData[anticampIndex][Anticamp_Warning] = Anticamp_NoWarning;
        return true;
    }
    else if (StrEqual(warning, "chat", false))
    {
        AnticampData[anticampIndex][Anticamp_Warning] = Anticamp_Chat;
        return true;
    }
    else if (StrEqual(warning, "center", false))
    {
        AnticampData[anticampIndex][Anticamp_Warning] = Anticamp_Center;
        return true;
    }
    else if (StrEqual(warning, "menu", false))
    {
        AnticampData[anticampIndex][Anticamp_Warning] = Anticamp_Menu;
        return true;
    }
    
    // The string value didn't match any valid action types.
    return false;
}
