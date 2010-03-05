/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          project_events.inc
 *  Type:          Base
 *  Description:   Event hooking and definitions.  Any event-related stuff for your project should go here.
 *
 *  Copyright (C) 2009-2010  Greyscale
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
 * This is a collection of ALL game and custom events.
 * Add any custom events here to the bottom of the list, and update define EVENT_DATA_CELL_COUNT. (below)
 * Also don't forget to add the display name of your event to g_ProjectEventNames. (below)
 */
enum ProjectEvents
{
    // Cross-game events.
    Event_OnAllPluginsLoaded,       /** All plugins have loaded. */
    Event_OnPluginEnd,              /** Plugin is ending. */
    Event_OnAllModulesLoaded,       /** All modules have been registered. */
    Event_OnModuleEnable,           /** A module has been enabled. */
    Event_OnMyModuleEnable,         /** The module that hooked this event callback has been enabled. */
    Event_OnModuleDisable,          /** A module has been disabled. */
    Event_OnMyModuleDisable,        /** The module that hooked this event callback has been disabled. */
    Event_OnMapStart,               /** The map has started. */
    Event_OnMapEnd,                 /** The map has ended. */
    Event_OnAutoConfigsBuffered,    /** This is called before OnConfigsExecuted but any time after OnMapStart. */
    Event_OnConfigsExecuted,        /** All convars are set, cvar-dependent code should use this. */
    Event_OnClientPutInServer,      /** Client has joined the server. */
    Event_OnClientConnected,        /** Client is connected to the server. */
    Event_OnClientDisconnect,       /** Client is disconnecting from the server. */
    Event_OnClientCookiesCached,    /** Client cookie has been loaded. */
    Event_OnClientPostAdminCheck,   /** Client is authorized and fully in-game. */
    
    // Custom events.
    
    
    // CS:S events.
    #if defined PROJECT_GAME_CSS
    
    Event_RoundStart,               /** Round has started. */
    Event_RoundFreezeEnd,           /** Pre-round has freezetime has finished. */
    Event_RoundEnd,                 /** Round has ended. */
    Event_PlayerTeam,               /** Client has joined a team. */
    Event_PlayerSpawn,              /** Client has spawned. */
    Event_PlayerHurt,               /** Client has been damaged. */
    Event_PlayerDeath,              /** Client has been killed. */
    Event_PlayerJump,               /** Client has jumped. */
    Event_WeaponFire,               /** Client has fired a weapon. */
    //Event_WeaponEntityFire        /** Client has fired a weapon. (returns weapon entity index) */
    
    #endif
    
    // DOD events.
    #if defined PROJECT_GAME_DOD
    
    #endif
    
    // HL2DM events.
    #if defined PROJECT_GAME_HL2DM
    
    #endif
    
    // TF2 events.
    #if defined PROJECT_GAME_TF2
    
    #endif
    
    // L4D events.
    #if defined PROJECT_GAME_L4D
    
    #endif
    
    // L4D2 events.
    #if defined PROJECT_GAME_L4D2
    
    #endif
}

/**
 * String array of display names for project events. (above)
 * Keep this updated as ProjectEvents is modified.
 */
stock String:g_ProjectEventNames[ProjectEvents][] = {
    // Cross-game events.
    "OnAllPluginsLoaded",
    "OnPluginEnd",
    "OnAllModulesLoaded",
    "OnModuleEnable",
    "OnMyModuleEnable",
    "OnModuleDisable",
    "OnMyModuleDisable",
    "OnMapStart",
    "OnMapEnd",
    "OnAutoConfigsBuffered",
    "OnConfigsExecuted",
    "OnClientPutInServer",
    "OnClientConnected",
    "OnClientDisconnect",
    "OnClientCookiesCached",
    "OnClientPostAdminCheck"
    
    // Custom events.
    
    
    // CS:S events.
    #if defined PROJECT_GAME_CSS
    
   ,"RoundStart",      // The beginning of a group of game events has to include the preceding "," from the group before it.
    "RoundFreezeEnd",
    "RoundEnd",
    "PlayerTeam",
    "PlayerSpawn",
    "PlayerHurt",
    "PlayerDeath",
    "PlayerJump",
    "WeaponFire"
    //"WeaponEntityFire"
    
    #endif
    
    // DOD events.
    #if defined PROJECT_GAME_DOD
    
    // All DOD events should be here.
    
    #endif
    
    // HL2DM events.
    #if defined PROJECT_GAME_HL2DM
    
    // All DOD events should be here.
    
    #endif
    
    // TF2 events.
    #if defined PROJECT_GAME_TF2
    
    #endif
    
    // L4D events.
    #if defined PROJECT_GAME_L4D
    
    #endif
    
    // L4D2 events.
    #if defined PROJECT_GAME_L4D2
    
    #endif
};

/**
 * Datatypes that can be passed with each event to modules.
 * Don't touch this!
 */
enum EventDataTypes
{
    DataType_Cell,
    DataType_Float,
    DataType_Array,
    DataType_String
}

/**
 * Common data type info.
 */

// When forwarding custom events with no data, use this for the 'eventdata' parameter in EventMgr_Forward
stock any:g_CommonEventData1[][] = {{0}};

// If you are using g_CommonEventData1, then the next parameter should be this.  This indicates you are passing no data.
stock EventDataTypes:g_CommonDataType1[] = {};

// This indicates that you are passing a single cell into an event.  Common for events that only pass a client index.
stock EventDataTypes:g_CommonDataType2[] = {DataType_Cell};

// **********************************************
//            SourceMod Forwards
// **********************************************

/**
 * All plugins have loaded.
 */
public OnAllPluginsLoaded()
{
    // Forward event to all modules.
    EventMgr_Forward(Event_OnAllPluginsLoaded, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * The map has started.
 */
public OnMapStart()
{
    // Forward event to all modules.
    EventMgr_Forward(Event_OnMapStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * The map has ended.
 */
public OnMapEnd()
{
    // Forward event to all modules.
    EventMgr_Forward(Event_OnMapEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public OnAutoConfigsBuffered()
{
    // Forward event to all modules.
    EventMgr_Forward(Event_OnAutoConfigsBuffered, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public OnConfigsExecuted()
{
    // Forward event to all modules.
    EventMgr_Forward(Event_OnConfigsExecuted, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public OnClientPutInServer(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(Event_OnClientPutInServer, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client is connecting to the server.
 * 
 * @param client    The client index.
 */
public OnClientConnected(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(Event_OnClientConnected, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(Event_OnClientDisconnect, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client cookie has been loaded.
 * 
 * @param client    The client index.
 */
public OnClientCookiesCached(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(Event_OnClientCookiesCached, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client is authorized and fully in-game.
 * 
 * @param client    The client index.
 */
public OnClientPostAdminCheck(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(Event_OnClientPostAdminCheck, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}


// TODO: OnPlayerRunCmd

// TODO: OnEntityCreated (SDK Hooks forward)


// **********************************************
//            Custom Event Forwarding
// **********************************************


// **********************************************
//              Game Event Hooking
// **********************************************

/**
 * This is called from the event manager in OnPluginStart.
 * Hook the events you want to forward to modules here.
 */
stock EventMgr_HookEvents()
{
    // Hook events to manage and forward to modules.
    
    // CS:S events.
    #if defined PROJECT_GAME_CSS
        HookEvent("round_start", GameEvent_RoundStart);
        HookEvent("round_freeze_end", GameEvent_RoundFreezeEnd);
        HookEvent("round_end", GameEvent_RoundEnd);
        HookEvent("player_team", GameEvent_PlayerTeam);
        HookEvent("player_spawn", GameEvent_PlayerSpawn);
        HookEvent("player_hurt", GameEvent_PlayerHurt);
        HookEvent("player_death", GameEvent_PlayerDeath);
        HookEvent("player_jump", GameEvent_PlayerJump);
        HookEvent("weapon_fire", GameEvent_WeaponFire);
    #endif
    
    // HL2DM events.
    #if defined PROJECT_GAME_HL2DM
    
    #endif
    
    // DOD events.
    #if defined PROJECT_GAME_DOD
    
    #endif
    
    // TF2 events.
    #if defined PROJECT_GAME_TF2
    
    #endif
    
    // L4D events.
    #if defined PROJECT_GAME_L4D
    #endif
    
    // L4D2 events.
    #if defined PROJECT_GAME_L4D2
    #endif
}

// **********************************************
//           CS:S Game Event Callbacks
// **********************************************

#if defined PROJECT_GAME_CSS

/**
 * Round has started.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(Event_RoundStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Pre-round has freezetime has finished.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(Event_RoundFreezeEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Round has ended.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_String};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetEventInt(event, "winner");
    eventdata[1][0] = GetEventInt(event, "reason");
    GetEventString(event, "message", eventdata[2], sizeof(eventdata[]));
    
    EventMgr_Forward(Event_RoundEnd, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has joined a team.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventInt(event, "team");
    eventdata[2][0] = GetEventInt(event, "oldteam");
    eventdata[3][0] = GetEventBool(event, "disconnect");
    
    EventMgr_Forward(Event_PlayerTeam, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has spawned.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(Event_PlayerSpawn, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has been damaged.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell, DataType_String, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    eventdata[2][0] = GetEventInt(event, "health");
    eventdata[3][0] = GetEventInt(event, "armor");
    GetEventString(event, "weapon", eventdata[4], sizeof(eventdata[]));
    eventdata[5][0] = GetEventInt(event, "dmg_health");
    eventdata[6][0] = GetEventInt(event, "dmg_armor");
    eventdata[7][0] = GetEventInt(event, "hitgroup");
    
    EventMgr_Forward(Event_PlayerHurt, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has been killed.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_String, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    GetEventString(event, "weapon", eventdata[2], sizeof(eventdata[]));
    eventdata[3][0] = GetEventInt(event, "headshot");
    
    EventMgr_Forward(Event_PlayerDeath, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has jumped.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(Event_PlayerJump, eventdata, 1, 1, g_CommonDataType2);
}

/**
 * Client has fired a weapon.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_WeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules. (WeaponFire)
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_String};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    GetEventString(event, "weapon", eventdata[1], sizeof(eventdata[]));
    
    EventMgr_Forward(Event_WeaponFire, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
    
    /*
    // Forward event to all modules. (WeaponEntityFire)
    new eventdata2[sizeof(eventdatatypes)][1];
    static EventDataTypes:eventdatatypes2[] = {DataType_Cell, DataType_Cell};
    
    decl String:classname[32];
    new weaponentity;
    
    // Loop through all entities.
    new maxentities = GetMaxEntities();
    for (new entity = MaxClients; entity < maxentities; entity++)
    {
        if (!IsValidEntity(entity))
            continue;
        
        GetEdictClassname(entity, classname, sizeof(classname));
        if (StrContains(classname, eventdata[1], false) == -1)
            continue;
        
        if (eventdata[0][0] == GetEntPropEnt(entity, Prop_Data, "m_hOwner"))
        {
            weaponentity = entity;
            break;
        }
    }
    
    eventdata2[0][0] = eventdata[0][0];
    eventdata2[1][0] = weaponentity;
    
    EventMgr_Forward(Event_WeaponEntityFire, eventdata2, sizeof(eventdata2), sizeof(eventdata2[]), eventdatatypes2);
    */
}

#endif
