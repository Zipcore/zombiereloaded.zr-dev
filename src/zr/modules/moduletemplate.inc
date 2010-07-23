/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          <file>.inc
 *  Type:          Module
 *  Description:   <description>
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
 * Template instructions:
 * 
 * All module-specific text has been replaced with text enclosed in <>
 * This is to make preparing this file to become the module you need easier.
 * Simply search-and-replace (usually CTRL+H) each of the tags below and replace it with the module specifics.
 * 
 * Tags:
 * <FuncPrefix>         The prefix of your module's functions.  Usually the module's full name with no whitespaces.
 * <ModuleVarName>      Usually the same as <FuncPrefix>.  Names the global variable that stores this module's identifier.
 * 
 * Remove these instructions after all text replacement operations are completed.
 */

/**
 * This module's identifier.
 */
new Module:g_module<ModuleVarName>;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:<FuncPrefix>_GetIdentifier() { return g_module<ModuleVarName>; }

/**
 * Register this module.
 */
<FuncPrefix>_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = ;
    moduledata[ModuleData_Hidden] = ;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "");
    moduledata[ModuleData_Dependencies][0] = ;
    moduledata[ModuleData_Dependencies][1] = ;
    // End the last one with INVALID_MODULE.
    moduledata[ModuleData_Dependencies][x] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_module<ModuleVarName> = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnEventsRegister",      "<FuncPrefix>_OnEventsRegister");
    
    // Register config file(s) that this module will use.
    // ConfigMgr_Register(g_module<ModuleVarName>, "<FuncPrefix>_OnConfigReload", "configs/<projectshortname>/<config>.txt");
}

/**
 * Plugin is loading.
 */
<FuncPrefix>_OnPluginStart()
{
    // Register the module.
    <FuncPrefix>_Register();
}

/**
 * All plugins have loaded.
 */
public <FuncPrefix>_OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public <FuncPrefix>_OnPluginEnd()
{
}

/**
 * Register all events here.
 */
public <FuncPrefix>_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnAllPluginsLoaded",      "<FuncPrefix>_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnPluginEnd",             "<FuncPrefix>_OnPluginEnd");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnAllModulesLoaded",      "<FuncPrefix>_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnModuleEnable",          "<FuncPrefix>_OnModuleEnable");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnMyModuleEnable",        "<FuncPrefix>_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnModuleDisable",         "<FuncPrefix>_OnModuleDisable");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnMyModuleDisable",       "<FuncPrefix>_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnMapStart",              "<FuncPrefix>_OnMapStart");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnMapEnd",                "<FuncPrefix>_OnMapEnd");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnAutoConfigsBuffered",   "<FuncPrefix>_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnConfigsExecuted",       "<FuncPrefix>_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnClientPutInServer",     "<FuncPrefix>_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_OnClientDisconnect",      "<FuncPrefix>_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_RoundStart",              "<FuncPrefix>_RoundStart");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_RoundFreezeEnd",          "<FuncPrefix>_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_RoundEnd",                "<FuncPrefix>_RoundEnd");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_PlayerTeam",              "<FuncPrefix>_PlayerTeam");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_PlayerSpawn",             "<FuncPrefix>_PlayerSpawn");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_PlayerHurt",              "<FuncPrefix>_PlayerHurt");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_PlayerDeath",             "<FuncPrefix>_PlayerDeath");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_PlayerJump",              "<FuncPrefix>_PlayerJump");
    //EventMgr_RegisterEvent(g_module<ModuleVarName>, "Event_WeaponFire",              "<FuncPrefix>_WeaponFire");
    
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public <FuncPrefix>_OnEventsReady()
{
}

/**
 * All modules have been registered.
 */
public <FuncPrefix>_OnAllModulesLoaded()
{
}

/**
 * A module has been enabled.
 */
public <FuncPrefix>_OnModuleEnable(Module:module)
{
}

/**
 * A module has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:<FuncPrefix>_OnModuleEnable(Module:module)
{
}

/**
 * The module that hooked this event callback has been enabled.
 */
public <FuncPrefix>_OnMyModuleEnable()
{
}

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:<FuncPrefix>_OnMyModuleEnable()
{
}

/**
 * A module has been disabled.
 */
public <FuncPrefix>_OnModuleDisable(Module:module)
{
}

/**
 * A module has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:<FuncPrefix>_OnModuleDisable(Module:module)
{
}

/**
 * The module that hooked this event callback has been disabled.
 */
public <FuncPrefix>_OnMyModuleDisable()
{
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:<FuncPrefix>_OnMyModuleDisable()
{
}

/**
 * The map has started.
 */
public <FuncPrefix>_OnMapStart()
{
}

/**
 * The map has ended.
 */
public <FuncPrefix>_OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public <FuncPrefix>_OnAutoConfigsBuffered()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public <FuncPrefix>_OnConfigsExecuted()
{
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public <FuncPrefix>_OnConfigReload(configindex)
{
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public <FuncPrefix>_OnClientPutInServer(client)
{
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public <FuncPrefix>_OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public <FuncPrefix>_RoundStart()
{
}

/**
 * Pre-round freezetime has finished.
 */
public <FuncPrefix>_RoundFreezeEnd()
{
}

/**
 * Round has ended.
 */
public <FuncPrefix>_RoundEnd()
{
}

/**
 * Client has joined a team.
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public <FuncPrefix>_PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public <FuncPrefix>_PlayerSpawn(client)
{
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public <FuncPrefix>_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public <FuncPrefix>_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public <FuncPrefix>_PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public <FuncPrefix>_WeaponFire(client, const String:weapon[])
{
}
