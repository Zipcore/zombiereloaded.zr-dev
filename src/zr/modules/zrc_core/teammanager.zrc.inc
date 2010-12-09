/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          teammanager.zrc.inc
 *  Type:          Module
 *  Description:   Manages zombie-based teams for ZRClassic gameplay.
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
new Module:g_moduleZRCTM;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ZRCTM_GetIdentifier() { return g_moduleZRCTM; }

/**
 * Cvar handles.
 */
new Handle:g_hCvarTeamHumans;
new Handle:g_hCvarTeamZombies;

/**
 * @section Default team indexes for classic zombie gameplay.
 */
#define ZRC_DEFAULT_ZOMBIE_TEAM TEAM_2
#define ZRC_DEFAULT_HUMAN_TEAM  TEAM_3
/**
 * @endsection
 */

/**
 * Register this module.
 */
ZRCTM_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ZRC Team Manager");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "zrcteammanager");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Manages zombie-based teams for ZRClassic gameplay.");
    moduledata[ModuleData_Dependencies][0] = ZRC_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleZRCTM = ModuleMgr_Register(moduledata);
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_OnEventsRegister",             "ZRCTM_OnEventsRegister");
}

/**
 * Register all events here.
 */
public ZRCTM_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_OnClientPutInServer",          "ZRCTM_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_OnConfigsExecuted",            "ZRCTM_OnConfigsExecuted");
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_RoundEnd",                     "ZRCTM_RoundEnd");
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_PlayerTeamPre",                "ZRCTM_PlayerTeamPre");
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_PlayerTeam",                   "ZRCTM_PlayerTeam");
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_PlayerSpawn",                  "ZRCTM_PlayerSpawn");
    
    #endif
    
    // Register custom events.
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_OnClientInfected",             "ZRCTM_OnClientInfected");
    EventMgr_RegisterEvent(g_moduleZRCTM, "Event_OnClientHuman",                "ZRCTM_OnClientHuman");
}

/**
 * Plugin is loading.
 */
ZRCTM_OnPluginStart()
{
    // Register the module.
    ZRCTM_Register();
    
    g_hCvarTeamHumans = 	CreateConVar("zrc_team_humans", "ct", "Which game team to put humans on.");
    g_hCvarTeamZombies =    CreateConVar("zrc_team_zombies", "t", "Which game team to put zombies on.");
}

/**
 * Configs are done executing.
 */
public ZRCTM_OnConfigsExecuted()
{
    ZRCTM_InitializeVTeams();
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public ZRCTM_OnClientPutInServer(client)
{
    // Initialize the client's team.
    TLib_SetClientTeam(client, VTeam_Unassigned);
}

/**
 * Client has been infected.
 * 
 * @param client        The infected client.
 * @param attacker      The zombie that infected the client.
 * @param mzombie       True if the client has been infected as a mother zombie.
 */
public ZRCTM_OnClientInfected(client, attacker, bool:mzombie)
{
    #if defined PROJECT_GAME_CSS
        Util_SwitchTeam(client, TLib_GetGameTeamIndex(VTeam_Zombie));
    #endif
}

/**
 * Client has turned back to human.
 * 
 * @param client        The client that became human.
 */
public ZRCTM_OnClientHuman(client)
{
    #if defined PROJECT_GAME_CSS
        Util_SwitchTeam(client, TLib_GetGameTeamIndex(VTeam_Human));
    #endif
}

#if defined PROJECT_GAME_CSS

/**
 * Round has ended.
 */
public ZRCTM_RoundEnd()
{
    ZRCTM_BalanceTeams();
}

/**
 * Client has joined a team. (pre)
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 * 
 * @return              Plugin_Handled to stop the event from being broadcasted to clients.  Plugin_Continue to let it continue like normal.  
 */
public Action:ZRCTM_PlayerTeamPre(client, team, oldteam, bool:disconnect)
{
    // Prevent this from being broadcasted to clients.
    return Plugin_Handled;
}

/**
 * Client has joined a team.
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public ZRCTM_PlayerTeam(client, team, oldteam, bool:disconnect)
{
    if (team == TEAM_1)
        TLib_SetClientTeam(client, VTeam_Spectator);
    // If client isn't already in the game, then assign them to human by default.
    else if (team > TEAM_1 && !TLib_IsClientPlaying(client))
        TLib_SetClientTeam(client, VTeam_Human);
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public ZRCTM_PlayerSpawn(client)
{
    // player_spawn is fired during the connection process, this is how we filter it out.
    if (!Util_IsClientOnTeam(client))
        return;
    
    // Move client the right team for their virtual team if there are zombies present.
    if (g_bZombiesPresent)
    {
        TLib_UpdateClientTeam(client);
    }
}

#endif

/**
 * Randomly put an equal amount of clients on both teams.
 */
stock ZRCTM_BalanceTeams()
{
    new bool:team1 = true;
    
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        if (!TLib_IsClientPlaying(client))
            continue;
        
        #if defined PROJECT_GAME_CSS
            if (team1)
                Util_SwitchTeam(client, TEAM_2);
            else
                Util_SwitchTeam(client, TEAM_3);
        #endif
        
        team1 = !team1;
    }
}

/**
 * Validates and initializes virtual team settings.
 */
ZRCTM_InitializeVTeams()
{
    decl String:humanTeam[32];
    decl String:zombieTeam[32];
    humanTeam[0] = 0;
    zombieTeam[0] = 0;
    
    // Get team strings.
    GetConVarString(g_hCvarTeamHumans, humanTeam, sizeof(humanTeam));
    GetConVarString(g_hCvarTeamZombies, zombieTeam, sizeof(zombieTeam));
    
    // Attempt to convert to team indexes.
    new humans = TLib_GTeamStringToIndex(humanTeam);
    new zombies = TLib_GTeamStringToIndex(zombieTeam);
    
    // Validate. Fall back to defaults on errors.
    if (humans < 0)
    {
        LogMgr_Print(g_moduleZRCTM, LogType_Error, "Config Validation", "Error: Invalid team name (%s). Check zrc_team_humans.", humanTeam);
        humans = ZRC_DEFAULT_HUMAN_TEAM;
    }
    if (zombies < 0)
    {
        LogMgr_Print(g_moduleZRCTM, LogType_Error, "Config Validation", "Error: Invalid team name (%s). Check zrc_team_zombies.", zombieTeam);
        zombies = ZRC_DEFAULT_ZOMBIE_TEAM;
    }
    if (humans == zombies)
    {
        LogMgr_Print(g_moduleZRCTM, LogType_Error, "Config Validation", "Error: Humans and zombies can't be on the same team. Check zrc_team_humans and zrc_team_zombies.");
        humans = ZRC_DEFAULT_HUMAN_TEAM;
        zombies = ZRC_DEFAULT_ZOMBIE_TEAM;
    }
    
    // Set indexes.
    TLib_SetGameTeamIndex(VTeam_Human, humans);
    TLib_SetGameTeamIndex(VTeam_Zombie, zombies);
    TLib_SetGameTeamIndex(VTeam_Spectator, TEAM_1);
}
