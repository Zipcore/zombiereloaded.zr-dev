/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          classevents.inc
 *  Type:          Core
 *  Description:   Functions for handling class related events.
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

/* ------------------------------------
 *
 * GAME EVENTS
 *
 * ------------------------------------
 */

/**
 * Create class-related cookies here.
 */
ClassOnCookiesCreate()
{
    // Forward event to sub-modules.
    ClassOverlayOnCookiesCreate();
    
    // Create cookie handles only if they don't exist.
    if (g_hClassCookieClassSelected[ZR_CLASS_TEAM_HUMANS] == INVALID_HANDLE)
    {
        g_hClassCookieClassSelected[ZR_CLASS_TEAM_HUMANS] = RegClientCookie("zr_humanclass", "The last human class selected.", CookieAccess_Protected);
    }
    if (g_hClassCookieClassSelected[ZR_CLASS_TEAM_ZOMBIES] == INVALID_HANDLE)
    {
        g_hClassCookieClassSelected[ZR_CLASS_TEAM_ZOMBIES] = RegClientCookie("zr_zombieclass", "The last zombie class selected.", CookieAccess_Protected);
    }
    if (g_hClassCookieClassSelected[ZR_CLASS_TEAM_ADMINS] == INVALID_HANDLE)
    {
        g_hClassCookieClassSelected[ZR_CLASS_TEAM_ADMINS] = RegClientCookie("zr_adminclass", "The last admin mode class selected.", CookieAccess_Protected);
    }
}

/**
 * Called when all modules are done loading.
 */
ClassOnModulesLoaded()
{
    ClassModulesLoaded = true;
    
    // Fire ready event on all clients that are ready to be initialized.
    ClassCheckReadyStates();
}

/**
 * Called when map is loading, before configs are loaded. Used for
 * initializing class module.
 */
ClassOnMapStart()
{
	// Clear multipliers.
	ClassResetMultiplierCache();
}

/**
 * Called when all configs are executed.
 */
ClassOnConfigsExecuted()
{
    new ClassSpeedMethods:speedMethod = ClassGetSpeedMethod();
    
    if (speedMethod != ClassSpeed_Invalid)
    {
        // Set speed method.
        ClassSpeedMethod = speedMethod;
    }
    else
    {
        // Fall back on default to avoid errors.
        ClassSpeedMethod = ClassSpeed_Prop;
        LogEvent(false, LogTypeOld_Normal, LOG_CORE_EVENTS, LogModules:LogModule_Playerclasses, "Config validation", "Warning: Invalid value in zr_classes_speed_method. Using default value.");
    }
}

/**
 * Client has just connected to the server.
 */
ClassOnClientConnected(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d connected.", client);
    
    // Preload class indexes.
    ClassClientSetDefaultIndexes(client, ClassLoadMode_Preload);
    
    // Fire ready event if ready to be initialized.
    ClassCheckReadyState(client);
}

/**
 * Called when a client enters the server (OnClientPutInServer).
 */
ClassClientInit(client)
{
    // Hook "PreThinkPost" on the client.
    SDKHook(client, SDKHook_PreThinkPost, ClassPreThinkPost);
    
    // Reset spawn flag.
    ClassPlayerSpawned[client] = false;
    
    // Reset instant change timer handle.
    ZREndTimer(tClassInstantChange[client], false);
    
    // Forward event to sub-modules.
    HealthRegenClientInit(client);
}

/**
 * Called once a client is authorized and fully in-game, and 
 * after all post-connection authorizations have been performed.  
 *
 * This callback is gauranteed to occur on all clients, and always 
 * after each OnClientPutInServer() call.
 *
 * @param client		Client index.
 * @noreturn
 */
ClassOnClientPostAdminCheck(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d admin checked.", client);
    
    // Client has been checked.
    ClassAdminChecked[client] = true;
    
    // Fire ready event if ready.
    ClassCheckReadyState(client);
}

/**
 * Called once a client's saved cookies have been loaded from the database.
 * 
 * @param client		Client index.
 */
ClassOnCookiesCached(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d's cookies cached.", client);
    
    // Check if classes are loaded successfully.
    if (ClassValidated)
    {
        // Forward event to sub-modules.
        ClassOverlayOnCookiesCached(client);
    }
    
    // Client cookes are loaded.
    ClassCookiesLoaded[client] = true;
    
    // Fire ready event if ready to be initialized.
    ClassCheckReadyState(client);
}

/**
 * Called a client disconnects.
 */
ClassOnClientDisconnect(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d disconnect.", client);
    
    // Unhook "PreThinkPost" on the client.
    SDKUnhook(client, SDKHook_PreThinkPost, ClassPreThinkPost);
    
    // Reset previously selected class indexes.
    ClassResetNextIndexes(client);
    
    // Kill instant change timer if running.
    ZREndTimer(tClassInstantChange[client]);
    
    // Reset ready states.
    ClassResetReadyState(client);
    
    // Forward event to sub-modules.
    HealthRegenOnClientDisconnect(client);
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ClassOnClientSpawn(client)
{
    decl String:originalmodel[PLATFORM_MAX_PATH];
    decl String:steamid[16];
    decl String:classname[64];
    new filter[ClassFilter];
    
    // Check if the player is on a team. Spawning into the game is also a event in
    // the connection process where players get their observer camera.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d spawn.", client);
    
    // Check if there are no valid classes. Block this event if classes aren't
    // done loading.
    if (!ClassValidated)
    {
        return;
    }
    
    // Reset attributes by triggering death event.
    ClassOnClientDeath(client);
    
    // Restore class indexes to be selected on spawn, if available.
    ClassRestoreNextIndexes(client);
    
    // Cache original player model.
    GetClientModel(client, originalmodel, sizeof(originalmodel));
    strcopy(ClassOriginalPlayerModel[client], PLATFORM_MAX_PATH, originalmodel);
    
    // Check if the player should spawn in admin mode.
    if (ClassPlayerInAdminMode[client])
    {
        // Mark player as in admin mode.
        ClassPlayerInAdminMode[client] = true;
        
        // TODO: This is the place to initialize admin mode stuff like no-block
        //       and other stuff.
    }
    else
    {
        // Mark player as not in admin mode.
        ClassPlayerInAdminMode[client] = false;
        
        // Get random class setting and steam id.
        new bool:randomclass = GetConVarBool(g_hCvarsList[CVAR_CLASSES_RANDOM]);
        GetClientAuthString(client, steamid, sizeof(steamid));
        
        // Assign random classes if enabled. Always do it for bots.
        if (randomclass || StrEqual(steamid, "BOT"))
        {
            // Setup filtering
            // ---------------
            
            // Exclude special class flags like mother zombies and admin classes.
            filter[ClassFilter_DenyFlags] = ZR_CLASS_SPECIALFLAGS;
                        
            // Allow admin classes if admin.
            filter[ClassFilter_DenyFlags] -= ZRIsClientAdmin(client) ? ZR_CLASS_FLAG_ADMIN_ONLY : 0;
            
            // Specify client for checking group permissions.
            filter[ClassFilter_Client] = client;
            
            // Get classes
            // -----------
            
            // Get random classes for each type.
            new randomzombie = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES, filter);
            new randomhuman = ClassGetRandomClass(ZR_CLASS_TEAM_HUMANS, filter);
            
            // Set selected zombie class index.
            ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES] = randomzombie;
            ClassGetName(randomzombie, classname, sizeof(classname), ZR_CLASS_TEAM_ZOMBIES);
            TranslationPrintToChat(client, "Classes random assignment", classname);
            
            // Set selected human class index.
            ClassSelected[client][ZR_CLASS_TEAM_HUMANS] = randomhuman;
            ClassGetName(randomhuman, classname, sizeof(classname), ZR_CLASS_TEAM_HUMANS);
            TranslationPrintToChat(client, "Classes random assignment", classname);
        }
        
        // Display class menu if either menu cvar is set.
        new bool:menuspawn = GetConVarBool(g_hCvarsList[CVAR_CLASSES_MENU_SPAWN]);
        new bool:menujoin = GetConVarBool(g_hCvarsList[CVAR_CLASSES_MENU_JOIN]);
        if (menuspawn || (menujoin && !ClassPlayerSpawned[client]))
        {
            ClassPlayerSpawned[client] = true;
            ClassMenuMain(client);
        }
    }
    
    // Load class attributes for the active class.
    ClassReloadPlayerCache(client, ClassGetActiveIndex(client));
    
    // Note: Class attributes are applied in ClassOnClientSpawnPost.
    
    // Kill instant change timer if running.
    ZREndTimer(tClassInstantChange[client]);
    
    // Check if instant class change cvar is set.
    new Float:instantspawn = GetConVarFloat(g_hCvarsList[CVAR_CLASSES_CHANGE_TIMELIMIT]);
    if (instantspawn > 0)
    {
        // Allow instant class change.
        ClassAllowInstantChange[client] = true;
        
        // Create timer to disable instant change.
        tClassInstantChange[client] = CreateTimer(instantspawn, Event_ClassDisableInstantSpawn, client, TIMER_FLAG_NO_MAPCHANGE);
    }
    else
    {
        // Make sure instant change is not allowed.
        ClassAllowInstantChange[client] = false;
    }
    
    // Forward event to sub-modules.
    HealthRegenOnClientSpawn(client);
}

/**
 * Client have just spawned (delayed event).
 * 
 * @param client    The client index.
 */
ClassOnClientSpawnPost(client)
{
    // Check if classes are loaded successfully and the client is ready.
    if (ClassValidated && ClassIsClientReady(client))
    {
        ClassApplyAttributes(client);
    }
}

/**
 * Client is connected, admin checked and cookies are loaded.
 *
 * @param client    The client index.
 */
ClassOnClientReady(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Playerclasses, "Event", "Client %d is ready.", client);
    
    // Check if classes are loaded successfully.
    if (ClassValidated)
    {
        // Cookies are loaded and admin check is done. Postload class indexes on the player.
        ClassClientSetDefaultIndexes(client, ClassLoadMode_Postload);
        
        // Tell player.
        TranslationPrintToChat(client, "Classes Selection Loaded");
    }
}

/**
 * Client is being hurt.
 * 
 * @param client    The client index.
 */
ClassOnClientHurt(client)
{
    // Forward event to sub-modules.
    ClassAlphaUpdate(client);
    HealthRegenOnClientHurt(client);
}

/**
 * Client died. Stops timers and reset certain attributes. Call this event to
 * clean up class related stuff.
 * 
 * @param client    The client index.
 */
ClassOnClientDeath(client)
{
    // Set client's FOV back to normal.
    ToolsSetClientDefaultFOV(client, 90);
    
    // Kill instant change timer if running.
    ZREndTimer(tClassInstantChange[client]);
    
    // Forward event to sub-modules.
    ClassOverlayOnClientDeath(client);
    HealthRegenOnClientDeath(client);
}

/**
 * Client got infected. Reloads class attributes.
 * 
 * @param client    The client index.
 */
ClassOnClientInfected(client, bool:motherzombie = false)
{
    new classindex = ClassGetActiveIndex(client);
    new isadmin;
    new motherindex;
    new filter[ClassFilter];
    
    decl String:motherzombiesetting[64];
    
    // Make sure the player is not allowed to instantly change class.
    ZREndTimer(tClassInstantChange[client]);
    ClassAllowInstantChange[client] = false;
    
    // Check if it's a mother zombie.
    if (motherzombie)
    {
        // Set admin flag if client is admin, so it's removed in special class
        // flags.
        isadmin = ZRIsClientAdmin(client) ? ZR_CLASS_FLAG_ADMIN_ONLY : 0;
        
        // Get default mother zombie setting.
        GetConVarString(g_hCvarsList[CVAR_CLASSES_DEFAULT_M_ZOMB], motherzombiesetting, sizeof(motherzombiesetting));
        
        if (StrEqual(motherzombiesetting, "disabled", false))
        {
            // Do nothing. Keep current class.
        }
        else if (StrEqual(motherzombiesetting, "random", false))
        {
            // Setup filtering
            // ---------------
            
            // Exclude special class flags.
            filter[ClassFilter_DenyFlags] = ZR_CLASS_SPECIALFLAGS;
            
            // Allow admin classes if admin.
            filter[ClassFilter_DenyFlags] -= isadmin;
            
            // Specify client for checking group permissions.
            filter[ClassFilter_Client] = client;
            
            // Get class
            // ---------
            
            // Get random regular zombie class. Remove admin flag if admin.
            motherindex = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES, filter);
            
            // Validate index. Do not change class if it's invalid.
            if (ClassValidateIndex(motherindex))
            {
                // Save active class index to be restored next spawn.
                ClassSelectedNext[client][ZR_CLASS_TEAM_ZOMBIES] = classindex;
                
                // Change class.
                classindex = motherindex;
            }
        }
        else if (StrEqual(motherzombiesetting, "motherzombies", false))
        {
            // Setup filtering
            // ---------------
            
            // Exclude special class flags except mother zombies.
            filter[ClassFilter_DenyFlags] = ZR_CLASS_SPECIALFLAGS - ZR_CLASS_FLAG_MOTHER_ZOMBIE;
            
            // Require mother zombie class flag.
            filter[ClassFilter_RequireFlags] = ZR_CLASS_FLAG_MOTHER_ZOMBIE;
            
            // Allow admin classes if admin.
            filter[ClassFilter_DenyFlags] -= isadmin;
            
            // Specify client for checking group permissions.
            filter[ClassFilter_Client] = client;
            
            // Get class
            // ---------
            
            // Get random mother zombie class. Include admin classes if admin.
            motherindex = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES, filter);
            
            // Validate index. Do not change class if it's invalid.
            if (ClassValidateIndex(motherindex))
            {
                // This is a mother zombie class. Reset mother zombie setting
                // so class skills aren't improved.
                motherzombie = false;
                
                // Save active class index to be restored next spawn.
                ClassSelectedNext[client][ZR_CLASS_TEAM_ZOMBIES] = classindex;
                
                // Change class.
                classindex = motherindex;
            }
            else
            {
                // No mother zombie classes found. Log warning.
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Mother Zombie Infection", "Warning: Couldn't find any mother zombie classes, falling back to the player's selected zombie class.");
            }
        }
        else
        {
            // Assume it's a class name. Get index for the specified class name.
            motherindex = ClassGetIndex(motherzombiesetting);
            
            // Validate index.
            if (ClassValidateIndex(motherindex))
            {
                // Save active class index to be restored next spawn.
                ClassSelectedNext[client][ZR_CLASS_TEAM_ZOMBIES] = classindex;
                
                // Change class.
                classindex = motherindex;
            }
            else
            {
                // Invalid class. Log warning.
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Mother Zombie Infection", "Warning: Failed to get the specified zombie class, falling back to the player's selected zombie class. Check spelling in \"zr_classes_default_mother_zombie\".");
            }
        }
    }
    
    // Update the player's selected class index.
    ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES] = classindex;
    
    // Restore next indexes, if available. But don't restore the zombie index.
    ClassRestoreNextIndexes(client, ZR_CLASS_TEAM_ZOMBIES);
    
    // Update the player's cache with zombie attributes.
    ClassReloadPlayerCache(client, classindex);
    
    // Apply the new attributes.
    ClassApplyAttributes(client, motherzombie);
    
    // Forward event to sub-modules.
    HealthRegenOnClientInfected(client);
}

/**
 * Client has been turned back human.
 * 
 * @param client    The client index.
 */
ClassOnClientHuman(client)
{
    // Forward event to sub-modules.
    ClassReloadPlayer(client);
    HealthRegenOnClientHuman(client);
}

/**
 * Timer callback for disabling instant class change setting on a client.
 */
public Action:Event_ClassDisableInstantSpawn(Handle:timer, any:client)
{
    // Disable instant class change.
    ClassAllowInstantChange[client] = false;
    
    // Reset the variable.
    ZREndTimer(tClassInstantChange[client], false);
}
