/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          gameplay.zrc.inc
 *  Type:          Module
 *  Description:   Modifies game behaviors to fit ZRClassic gameplay.
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
 * TODO:
 * Unhook cvar hooks when module is disabled.
 * Waiting for SM to not error when i do this on OnPluginEnd
 */

/**
 * This module's identifier.
 */
new Module:g_moduleZRCGPlay;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ZRCGPlay_GetIdentifier() { return g_moduleZRCGPlay; }

/**
 * Timer handle for counting down the round time.
 */
new Handle:g_hZRCRoundTimer;

/**
 * Register this module.
 */
ZRCGPlay_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ZRC Gameplay Modifier");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "zrcgameplay");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Modifies game behaviors to fit classic ZRClassic gameplay.");
    moduledata[ModuleData_Dependencies][0] = ZRC_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleZRCGPlay = ModuleMgr_Register(moduledata);
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnEventsRegister",          "ZRCGPlay_OnEventsRegister");
}

/**
 * Register all events here.
 */
public ZRCGPlay_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnEventsReady",             "ZRCGPlay_OnEventsReady");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnMyModuleEnable",          "ZRCGPlay_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnMyModuleDisable",         "ZRCGPlay_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnClientDisconnect",        "ZRCGPlay_OnClientDisconnect");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnMapStart",                "ZRCGPlay_OnMapStart");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_OnClientInfect",            "ZRCGPlay_OnClientInfect");
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_RoundStart",                "ZRCGPlay_RoundStart");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_RoundFreezeEnd",            "ZRCGPlay_RoundFreezeEnd");
    EventMgr_RegisterEvent(g_moduleZRCGPlay, "Event_RoundEnd",                  "ZRCGPlay_RoundEnd");
    
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public ZRCGPlay_OnEventsReady()
{
    // Push down priority for Event_OnClientInfect to below zrteammanager module.
    EventMgr_TakePriority("Event_OnClientDisconnect", g_moduleZRCGPlay, ZRCInfect_GetIdentifier());
    EventMgr_TakePriority("Event_OnClientInfect", g_moduleZRCGPlay, ZRCTM_GetIdentifier());
}


/**
 * Plugin is loading.
 */
ZRCGPlay_OnPluginStart()
{
    // Register the module.
    ZRCGPlay_Register();
    
    #if defined PROJECT_GAME_CSS
        HookConVarChange(FindConVar("mp_restartgame"), ZRCGPlay_CvarHookMPRGame);
        CvarLib_LockCvar("mp_limitteams", "0");
        CvarLib_LockCvar("mp_autoteambalance", "0");
    #endif
}

/**
 * The module that hooked this event callback has been enabled.
 */
public ZRCGPlay_OnMyModuleEnable()
{
    #if defined PROJECT_GAME_CSS
        HookConVarChange(FindConVar("mp_restartgame"), ZRCGPlay_CvarHookMPRGame);
        CvarLib_LockCvar("mp_limitteams", "0");
        CvarLib_LockCvar("mp_autoteambalance", "0");
    #endif
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public ZRCGPlay_OnMyModuleDisable()
{
    #if defined PROJECT_GAME_CSS
        UnhookConVarChange(FindConVar("mp_restartgame"), ZRCGPlay_CvarHookMPRGame);
        CvarLib_UnlockCvar("mp_limitteams");
        CvarLib_UnlockCvar("mp_autoteambalance");
    #endif
}

/**
 * The map has started.
 */
public ZRCGPlay_OnMapStart()
{
    // Reset timer handle.
    g_hZRCRoundTimer = INVALID_HANDLE;
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public ZRCGPlay_OnClientDisconnect(client)
{
    // End the round if the last human leaves.
    if (TLib_IsClientHuman(client))
    {
        if (TLib_CountTeam(VTeam_Human, true) <= 1)
        {
            static roundend_reason = -1;
            if (roundend_reason == -1)
                roundend_reason = _:Util_TeamToReason(TLib_GetGameTeamIndex(VTeam_Zombie));
            
            SDKToolsLib_TerminateRound(3.0, RoundEndReasons:roundend_reason);
        }
    }
}

/**
 * Client has been infected.
 * 
 * @param client        The infected client.
 * @param attacker      The zombie that infected the client.
 * @param mzombie       True if the client has been infected as a mother zombie.
 */
public ZRCGPlay_OnClientInfect(client, attacker, bool:mzombie)
{
    // The core won't infect all clients as mother zombie so don't waste time checking.
    // This could be a big performance boost if 10 mother zombies are being spawned simultaneously.
    if (mzombie)
        return;
    
    // If there are no more humans then zombies win.
    if (TLib_CountTeam(VTeam_Human, true) == 0)
    {
        static roundend_reason = -1;
        if (roundend_reason == -1)
            roundend_reason = _:Util_TeamToReason(TLib_GetGameTeamIndex(VTeam_Zombie));
        
        SDKToolsLib_TerminateRound(3.0, RoundEndReasons:roundend_reason);
    }
}

/**
 * Client has turned back to human.
 * 
 * @param client        The client that became human.
 */
public ZRCGPlay_OnClientHuman(client)
{
    // If there are no more humans then humans win.
    if (TLib_CountTeam(VTeam_Zombie, true) == 0)
    {
        static roundend_reason = -1;
        if (roundend_reason == -1)
            roundend_reason = _:Util_TeamToReason(TLib_GetGameTeamIndex(VTeam_Human));
        
        SDKToolsLib_TerminateRound(3.0, RoundEndReasons:roundend_reason);
    }
}

#if defined PROJECT_GAME_CSS

/**
 * Round has started.
 */
public ZRCGPlay_RoundStart()
{
    // Print ZR objective.
    TransMgr_PrintTextAll(true, false, MsgFormat_Plugin, MsgType_Chat, _, _, "ZRC gameplay round objective");
    
    // Stop round timer.
    Util_CloseHandle(g_hZRCRoundTimer);
}

/**
 * Pre-round freezetime has finished.
 */
public ZRCGPlay_RoundFreezeEnd()
{
        // Stop round timer.
        Util_CloseHandle(g_hZRCRoundTimer);
        
        // Get the roundtime, and save it for future calls.
        static Handle:hMPRoundTime;
        if (hMPRoundTime == INVALID_HANDLE)
        {
            hMPRoundTime = FindConVar("mp_roundtime");
        }
        
        g_hZRCRoundTimer = CreateTimer(GetConVarFloat(hMPRoundTime) * 60.0, ZRCGPlay_RoundEndTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Timer callback.
 * End the round, humans win.
 * 
 * @param timer     The timer handle.
 */
public Action:ZRCGPlay_RoundEndTimer(Handle:timer)
{
    g_hZRCRoundTimer = INVALID_HANDLE;
    
    // Humans held out against the zombies, they win!
    SDKToolsLib_TerminateRound(3.0, Util_TeamToReason(TLib_GetGameTeamIndex(VTeam_Human)));
}

/**
 * Round has ended.
 */
public ZRCGPlay_RoundEnd()
{
    // Stop round timer.
    Util_CloseHandle(g_hZRCRoundTimer);
}

#endif

// **********************************************
//            Cvar Hook Callbacks
// **********************************************

#if defined PROJECT_GAME_CSS

/**
 * Called when a console variable's value is changed.
 * 
 * @param convar		Handle to the convar that was changed.
 * @param oldValue		String containing the value of the convar before it was changed.
 * @param newValue		String containing the new value of the convar.
 */
public ZRCGPlay_CvarHookMPRGame(Handle:convar, const String:oldValue[], const String:newValue[])
{
    // If the value is 0 then ignore, game isn't restarting.
    new time = StringToInt(newValue);
    if (time == 0)
        return;
    
    // End round with "Game commencing"
    SDKToolsLib_TerminateRound(float(time), ROUNDEND_GAME_COMMENCING);
    
    // This basically makes mp_restartgame useless.
    SetConVarInt(convar, 0);
}

#endif
