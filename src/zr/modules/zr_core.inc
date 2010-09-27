/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zr_core.inc
 *  Type:          Module
 *  Description:   Game mode-independent stuff.
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

// Include libraries.
#include "zr/libraries/teamlib"

/**
 * This module's identifier.
 */
new Module:g_moduleZRCore;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ZRCore_GetIdentifier() { return g_moduleZRCore; }

/**
 * Cvar handles.
 */
new Handle:g_hCvarZombieTeam;

/**
 * Register this module.
 */
ZRCore_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ZRCore");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "core");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Game mode-independent stuff.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleZRCore = ModuleMgr_Register(moduledata);
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleZRCore, "Event_OnEventsRegister",              "ZRCore_OnEventsRegister");
}

/**
 * Register all events here.
 */
public ZRCore_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleZRCore, "Event_OnConfigsExecuted",             "ZRCore_OnConfigsExecuted");
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleZRCore, "Event_RoundStart",                    "ZRCore_RoundStart");
    
    #endif
}

/**
 * Plugin is loading.
 */
ZRCore_OnPluginStart()
{
    // Register the module.
    ZRCore_Register();
    
    // Create cvars.
    g_hCvarZombieTeam  = Project_CreateConVar("zombie_team", "t", "Define the game team that will be the zombie team.  Opposite team will be the human team.  ['t' = Terrorists | 'ct' = Counter-Terrorists]");
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public ZRCore_OnConfigsExecuted()
{
    ZRCore_UpdateZombieTeam();
}

#if defined PROJECT_GAME_CSS

/**
 * Round has started.
 */
public ZRCore_RoundStart()
{
    // Update the zombie team every round start.
    // This shouldn't be updated real-time with the cvar, or else zombies would be on both teams during the round.
    ZRCore_UpdateZombieTeam();
}

#endif

/**
 * Update the game teams associated with the virtual teams, as set by cvar.
 */
ZRCore_UpdateZombieTeam()
{
    decl String:strZombieTeam[32];
    GetConVarString(g_hCvarZombieTeam, strZombieTeam, sizeof(strZombieTeam));
    new iZombieTeamIndex = TLib_GTeamStringToIndex(strZombieTeam);
    if (iZombieTeamIndex > 1)
    {
        // Associate the zombie virtual team with the given value.
        TLib_SetGameTeamIndex(VTeam_Zombie, iZombieTeamIndex);
        
        // For humans, use the opposite team as the one given for zombies.
        TLib_SetGameTeamIndex(VTeam_Human, (iZombieTeamIndex == 2) ? 3 : 2);
    }
    else
    {
        LogMgr_Print(g_moduleZRCore, LogType_Error, "Cvar Validation", "Invalid team name (%s) in cvar \"zr_zombie_team\".  Check the description for it in zombiereloaded.cfg.  Assuming team index 2..", strZombieTeam);
        TLib_SetGameTeamIndex(VTeam_Zombie, 2);
        TLib_SetGameTeamIndex(VTeam_Zombie, 3);
    }
}
