/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ztele.inc
 *  Type:          Module
 *  Description:   Player teleport manager.
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
 * This module's identifier.
 */
new Module:g_moduleZTele;

/**
 * Teleport events.
 */
new ProjectEvent:g_EvOnTeleportTimerStarted;
new ProjectEvent:g_EvOnPlayerTeleported;
new ProjectEvent:g_EvOnPlayerTeleportAborted;

/**
 * Teleport convars.
 */
new Handle:g_hCvarZTeleZombie;
new Handle:g_hCvarZTeleHumanBefore;
new Handle:g_hCvarZTeleHumanAfter;
new Handle:g_hCvarZTeleDelayZombie;
new Handle:g_hCvarZTeleDelayHuman;
new Handle:g_hCvarZTeleMaxZombie;
new Handle:g_hCvarZTeleMaxHuman;
new Handle:g_hCvarZTeleAutocancel;
new Handle:g_hCvarZTeleAutocancelDistance;

/**
 * Teleport command to register.
 */
#define ZTELE_TELEPORT_COMMAND "ztele"

/**
 * Array to store client's spawn location.
 */
new Float:g_vecZTeleSpawn[MAXPLAYERS + 1][3];

/**
 * Array to store client's current location.
 */
new Float:g_vecZTeleOrigin[MAXPLAYERS + 1][3];

/**
 * Array to store the tele count of each client.
 */
new g_iZTeleCount[MAXPLAYERS + 1];

/**
 * Array for storing ZTele timer handles per client.
 */
new Handle:tZTele[MAXPLAYERS + 1];

/**
 * Array to store time left before teleport.
 */
new g_iZTeleTimeLeft[MAXPLAYERS + 1];

/**
 * Teleport abort reasons.
 */
enum ZTele_AbortReason
{
    ZTeleAbort_Manual,      /** Player manually aborted teleport. */
    ZTeleAbort_Autocancel   /** Teleport was aborted because of autocancel distance. */
}

/**
 * Register this module.
 */
ZTele_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "ZTele");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "ztele");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Player teleport manager.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleZTele = ModuleMgr_Register(moduledata);
    
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleZTele, "OnClientConnected",              "ZTele_OnClientConnected");
    
    // TODO: Need event OnClientInfected and OnClientHuman. For now the event forwards are hard coded.
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleZTele, "Event_PlayerSpawn",              "ZTele_PlayerSpawn");
    EventMgr_RegisterEvent(g_moduleZTele, "Event_PlayerDeath",              "ZTele_PlayerDeath");
    
    #endif
    
    // Create events.
    g_EvOnTeleportTimerStarted =    EventMgr_CreateEvent("OnTeleportTimerStarted");
    g_EvOnPlayerTeleported =        EventMgr_CreateEvent("OnPlayerTeleported");
    g_EvOnPlayerTeleportAborted =   EventMgr_CreateEvent("OnPlayerTeleportAbort");
}

/**
 * Creates teleport console commands.
 */
ZTele_CreateCommands()
{
    // Register ZTele command.
    RegConsoleCmd(ZTELE_TELEPORT_COMMAND, ZTele_Command, "Teleport back to spawn if you are stuck.");
    
    // Register admin command to force ZTele.
    Project_RegConsoleCmd("ztele_force", ZTele_ForceCommand, "Force ZTele on a client. Usage: <prefix>_ztele_force <client>");
}

/**
 * Creates teleport console variables.
 */
ZTele_CreateConvars()
{
    g_hCvarZTeleZombie =                Project_CreateConVar("ztele_zombie",                "1",    "Allow zombies to use ZTele.");
    g_hCvarZTeleHumanBefore =           Project_CreateConVar("ztele_human_before",          "1",    "Allow humans to use ZTele before the mother zombie has spawned.");
    g_hCvarZTeleHumanAfter =            Project_CreateConVar("ztele_human_after",           "1",    "Allow humans to use ZTele after the mother zombie has spawned.");
    g_hCvarZTeleDelayZombie =           Project_CreateConVar("ztele_delay_zombie",          "3.0",  "Time between using ZTele command and teleportation for zombies. [Dependency: <prefix>_ztele_zombie]");
    g_hCvarZTeleDelayHuman =            Project_CreateConVar("ztele_delay_human",           "3.0",  "Time between using ZTele command and teleportation for humans. [Dependency: <prefix>_ztele_human_(before)/(after)]");
    g_hCvarZTeleMaxZombie =             Project_CreateConVar("ztele_max_zombie",            "3",    "Max number of times a zombie is allowed to use ZTele per round. [Dependency: <prefix>_ztele_zombie]");
    g_hCvarZTeleMaxHuman =              Project_CreateConVar("ztele_max_human",             "1",    "Max number of times a human is allowed to use ZTele per round. [Dependency: <prefix>_ztele_human_(before)/(after)]");
    g_hCvarZTeleAutocancel =            Project_CreateConVar("ztele_autocancel",            "1",    "Automatically cancel ZTele if player moves out of a set boundary. [Dependency: <prefix>_ztele_(zombie)/(human)[_(before)/(after)]]");
    g_hCvarZTeleAutocancelDistance =    Project_CreateConVar("ztele_autocancel_distance",   "320",  "Maximum distance, in units, player is allowed to travel before teleport is cancelled. [Dependency: <prefix>_ztele_autocancel]");
}

/**
 * Plugin is loading.
 */
ZTele_OnPluginStart()
{
    // Register the module.
    ZTele_Register();
    ZTele_CreateCommands();
    ZTele_CreateConvars();
}

/**
 * Client connected to the server.
 * 
 * @param client    The client index.
 */
public ZTele_OnClientConnected(client)
{
    // Reset timer handle.
    Util_CloseHandle(tZTele[client]);
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public ZTele_PlayerSpawn(client)
{
    // Check if the player is on a team. Spawning into the game is also a event in
    // the connection process where players get their observer camera.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    // Reset tele count.
    g_iZTeleCount[client] = 0;
    
    // Get spawn location.
    GetClientAbsOrigin(client, g_vecZTeleSpawn[client]);
    
    // If timer is running, kill it.
    Util_CloseHandle(tZTele[client]);
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public ZTele_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    // If timer is running, kill it.
    Util_CloseHandle(tZTele[victim]);
}

/**
 * Player has been infected.
 * 
 * @param client    The client index.
 */
ZTele_OnClientInfected(client)
{
    // If timer is running, kill it.
    Util_CloseHandle(tZTele[client]);
    
    // Reset tele count.
    g_iZTeleCount[client] = 0;
}

/**
 * Player has been turned back human.
 * 
 * @param client    The client index.
 */
ZTele_OnClientHuman(client)
{
    // If timer is running, kill it.
    Util_CloseHandle(tZTele[client]);
    
    // Reset tele count.
    g_iZTeleCount[client] = 0;
}

/**
 * Teleports a client back to spawn if conditions are met.
 * 
 * @param client    The client index.
 * @param force     (Optional) True to force teleporting of the client, false to follow rules.
 * @param zombie    (Optional) True to teleport instantly, false to use delay.
 * @return          True if teleport was successful, false otherwise. 
 */
bool:ZTele_StartTeleport(client, bool:force = false)
{
    // If the client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return false;
    }
    
    new bool:infected = InfectIsClientInfected(client);
    
    // If zombie cvar is disabled and the client is a zombie, then stop.
    new bool:ztelezombie = GetConVarBool(g_hCvarZTeleZombie);
    if (!force && infected && !ztelezombie)
    {
        // Tell client they must be human to use this feature.
        TranslationPrintToChat(client, "Must be human");
        return false;
    }
    
    // If zombie has spawned, get before value, get the after value otherwise.
    // If the cvar is disabled and the client is a human, then stop.
    new bool:ztelehuman = g_bZombieSpawned ? GetConVarBool(g_hCvarZTeleHumanAfter) : GetConVarBool(g_hCvarZTeleHumanBefore);
    if (!force && !infected && !ztelehuman)
    {
        // Tell client that feature is restricted at this time.
        TranslationPrintToChat(client, "ZTele restricted human");
        return false;
    }
    
    // If the tele limit has been reached, then stop.
    new ztelemax = infected ? GetConVarInt(g_hCvarZTeleMaxZombie) : GetConVarInt(g_hCvarZTeleMaxHuman);
    if (!force && g_iZTeleCount[client] >= ztelemax)
    {
        // Tell client that they have already reached their limit.
        TranslationPrintToChat(client, "ZTele max", ztelemax);
        return false;
    }
    
    // If teleport is already in progress, then stop.
    if (ZRIsTimerRunning(tZTele[client]))
    {
        if (!force)
        {
            TranslationPrintToChat(client, "ZTele in progress");
        }
        
        return false;
    }
    
    // If we are forcing, then teleport now and stop.
    if (force)
    {
        // Teleport client to spawn.
        ZTele_TeleportClient(client);
        
        return true;
    }
    
    // Get current location.
    GetClientAbsOrigin(client, g_vecZTeleOrigin[client]);
    
    // Set timeleft array to value of respective cvar.
    g_iZTeleTimeLeft[client] = infected ? GetConVarInt(g_hCvarZTeleDelayZombie) : GetConVarInt(g_hCvarZTeleDelayHuman);
    if (g_iZTeleTimeLeft[client] > 0)
    {
        // Tell client how much time is left until teleport.
        TranslationPrintCenterText(client, "ZTele countdown", g_iZTeleTimeLeft[client]);
        
        // Start timer.
        tZTele[client] = CreateTimer(1.0, ZTele_Timer, client, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
        
        // Trigger event.
        ZTele_SendStartEvent(client, g_iZTeleTimeLeft[client]);
    }
    else
    {
        // Teleport client to spawn.
        ZTele_TeleportClient(client);
        
        // Increment teleport count.
        g_iZTeleCount[client]++;
        
        // If we're forcing the ZTele, then don't increment the count or print how many teleports they have used.
        // Tell client they've been teleported.
        TranslationPrintCenterText(client, "ZTele countdown end", g_iZTeleCount[client], ztelemax);
        
        // Send teleport event.
        ZTele_SendTeleportedEvent(client);
    }
    
    return true;
}

/**
 * Teleport client to their spawn location.
 * 
 * @param client    The client index.
 */
ZTele_TeleportClient(client)
{
    // Teleport client.
    TeleportEntity(client, g_vecZTeleSpawn[client], NULL_VECTOR, Float:{0.0, 0.0, 0.0});
}


/*****************
 *   CALLBACKS   *
 *****************/

/**
 * Menu callback (ztele_force)
 * Forces ZTele on a client.
 * 
 * @param menu      The menu handle.
 * @param action    Action client is doing in menu.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public ZTele_ForceHandle(Handle:menu_ztele_force, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get the client index of the selected client.
        new target = MenuGetClientIndex(menu_ztele_force, slot);
        
        // If the target is 0, then the client left before being selected from the menu.
        if (target == 0)
        {
            // Re-send the menu.
            MenuClientList(client, ZTele_ForceHandle, true, true, false, "ZTele clients title");
            
            return;
        }
        
        // Get the target's name for future use.
        decl String:targetname[MAX_NAME_LENGTH];
        GetClientName(target, targetname, sizeof(targetname));
        
        // Force ZSpawn on the target if client is allowed.
        new bool:success = CanUserTarget(client, target) ? ZTele_StartTeleport(target, true) : false;
        
        // Tell admin the outcome of the action.
        if (success)
        {
            TranslationReplyToCommand(client, "ZTele command force successful", targetname);
        }
        else
        {
            TranslationReplyToCommand(client, "ZTele command force unsuccessful", targetname);
        }
        
        // Re-send the menu.
        MenuClientList(client, ZTele_ForceHandle, true, true, false, "ZTele clients title");
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            // Re-open admin menu.
            ZAdminMenu(client);
        }
    }
    // Client exited menu.
    if (action == MenuAction_End)
    {
        CloseHandle(menu_ztele_force);
    }
}

/**
 * Command callback (zr_ztele_force)
 * Force ZSpawn on a client.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZTele_ForceCommand(client, argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "ZTele command force syntax");
        return Plugin_Handled;
    }
    
    decl String:target[MAX_NAME_LENGTH], String:targetname[MAX_NAME_LENGTH];
    new targets[MAXPLAYERS], bool:tn_is_ml, result;
    
    // Get targetname.
    GetCmdArg(1, target, sizeof(target));
    
    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE, targetname, sizeof(targetname), tn_is_ml);
        
    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }
    
    // x = Client index.
    for (new x = 0; x < result; x++)
    {
        // Give client the item.
        new bool:success = ZTele_StartTeleport(targets[x], true);
        
        // Tell admin the outcome of the command if only 1 client was targetted.
        if (result == 1)
        {
            if (success)
            {
                TranslationReplyToCommand(client, "ZTele command force successful", targetname);
            }
            else
            {
                TranslationReplyToCommand(client, "ZTele command force unsuccessful", targetname);
            }
        }
        
        // Log action to game events.
        LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_ZTele, "Force ZTele", "\"%L\" teleported \"%L\" to spawn.", client, targets[x]);
    }
    
    return Plugin_Handled;
}

/**
 * Command callback (ztele)
 * Teleport back to spawn if you are stuck.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZTele_Command(client, argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }
    
    // Start teleportation process.
    ZTele_StartTeleport(client);
    
    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Timer callback, counts down teleport to the client.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */
public Action:ZTele_Timer(Handle:timer, any:client)
{
    // If client leaves, then stop timer.
    if (!IsClientInGame(client))
    {
        // Reset timer handle.
        Util_CloseHandle(tZTele[client]);
        return Plugin_Stop;
    }
    
    new bool:zteleautocancel = GetConVarBool(g_hCvarZTeleAutocancel);
    if (zteleautocancel)
    {
        // If client has been running around after using ZTele, then stop timer.
        new Float:vecClient[3];
        GetClientAbsOrigin(client, vecClient);
        
        new Float:distance = GetVectorDistance(vecClient, g_vecZTeleOrigin[client]);
        new Float:autocanceldistance = GetConVarFloat(g_hCvarZTeleAutocancelDistance);
        
        // Check if distance has been surpassed.
        if (distance > autocanceldistance)
        {
            // Reset timer handle.
            Util_CloseHandle(tZTele[client]);
            
            // Tell client teleport has been cancelled.
            TranslationPrintCenterText(client, "ZTele autocancel centertext");
            TranslationPrintToChat(client, "ZTele autocancel text", RoundToNearest(autocanceldistance));
            
            // Send abort event.
            ZTele_SendAbortedEvent(client, ZTeleAbort_Autocancel);
            
            // Stop timer.
            return Plugin_Stop;
        }
    }
    
    // Decrement time left.
    g_iZTeleTimeLeft[client]--;
    
    // Tell client how much time is left until teleport.
    TranslationPrintCenterText(client, "ZTele countdown", g_iZTeleTimeLeft[client]);
    
    // Time has expired.
    if (g_iZTeleTimeLeft[client] <= 0)
    {
        // Teleport client.
        TeleportEntity(client, g_vecZTeleSpawn[client], NULL_VECTOR, NULL_VECTOR);
        
        // Increment teleport count.
        g_iZTeleCount[client]++;
        
        // Get max teleports per round.
        new ztelemax = InfectIsClientInfected(client) ? GetConVarInt(g_hCvarZTeleMaxZombie) : GetConVarInt(g_hCvarZTeleMaxHuman);
        
        // Tell client spawn protection is over.
        TranslationPrintCenterText(client, "ZTele countdown end", g_iZTeleCount[client], ztelemax);
        
        // Clear timer handle.
        Util_CloseHandle(tZTele[client]);
        
        // Stop timer.
        return Plugin_Stop;
    }
    
    // Allow timer to continue repeating.
    return Plugin_Continue;
}


/**********************
 *   EVENT TRIGGERS   *
 **********************/

/**
 * Sends the teleport start event.
 *
 * @param client    Client that is teleported.
 * @param time      Countdown time.
 */
ZTele_SendStartEvent(client, time)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = client;
    eventdata[1][0] = time;
    
    EventMgr_Forward(g_EvOnTeleportTimerStarted, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Sends the teleported event.
 *
 * @param client    Client that was teleported.
 */
ZTele_SendTeleportedEvent(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(g_EvOnPlayerTeleported, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType1);
}

/**
 * Sends the teleported event.
 *
 * @param client    Client that aborted teleport.
 */
ZTele_SendAbortedEvent(client, ZTele_AbortReason:reason)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = client;
    eventdata[1][0] = _:reason;
    
    EventMgr_Forward(g_EvOnPlayerTeleportAborted, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}
