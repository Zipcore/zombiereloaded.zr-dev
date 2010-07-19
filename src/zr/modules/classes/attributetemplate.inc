/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          <file>.inc
 *  Type:          Class attribue module
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
new Module:g_moduleClass<ModuleVarName>;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:Class<FuncPrefix>_GetIdentifier() { return g_moduleClass<ModuleVarName>; }

/**
 * Register this module.
 */
Class<FuncPrefix>_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "");
    moduledata[ModuleData_Dependencies][0] = ClassMgr_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleClass<ModuleVarName> = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnEventsRegister",      "Class<FuncPrefix>_OnEventsRegister");
    
    // Register attributes in the attribute register.
    //ClassAttribReg_AddAttrib(g_moduleClass<ModuleVarName>, "<attribute>");
    //ClassAttribReg_AddAttrib(g_moduleClass<ModuleVarName>, "<attribute2>");
}

/**
 * Plugin is loading.
 */
Class<FuncPrefix>_OnPluginStart()
{
    // Register the module.
    Class<FuncPrefix>_Register();
}

/**
 * All plugins have loaded.
 */
public Class<FuncPrefix>_OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public Class<FuncPrefix>_OnPluginEnd()
{
}

/**
 * Register all events here.
 */
public Class<FuncPrefix>_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnAllPluginsLoaded",      "Class<FuncPrefix>_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnPluginEnd",             "Class<FuncPrefix>_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnAllModulesLoaded",      "Class<FuncPrefix>_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnModuleEnable",          "Class<FuncPrefix>_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnMyModuleEnable",        "Class<FuncPrefix>_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnModuleDisable",         "Class<FuncPrefix>_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnMyModuleDisable",       "Class<FuncPrefix>_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnMapStart",              "Class<FuncPrefix>_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnMapEnd",                "Class<FuncPrefix>_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnAutoConfigsBuffered",   "Class<FuncPrefix>_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnConfigsExecuted",       "Class<FuncPrefix>_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnClientPutInServer",     "Class<FuncPrefix>_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_OnClientDisconnect",      "Class<FuncPrefix>_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_RoundStart",              "Class<FuncPrefix>_RoundStart");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_RoundFreezeEnd",          "Class<FuncPrefix>_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_RoundEnd",                "Class<FuncPrefix>_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_PlayerTeam",              "Class<FuncPrefix>_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_PlayerSpawn",             "Class<FuncPrefix>_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_PlayerHurt",              "Class<FuncPrefix>_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_PlayerDeath",             "Class<FuncPrefix>_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_PlayerJump",              "Class<FuncPrefix>_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleClass<ModuleVarName>, "Event_WeaponFire",              "Class<FuncPrefix>_WeaponFire");
    
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public Class<FuncPrefix>_OnEventsReady()
{
}

/**
 * All modules have been registered.
 */
public Class<FuncPrefix>_OnAllModulesLoaded()
{
}

/**
 * A module has been enabled.
 */
public Class<FuncPrefix>_OnModuleEnable(Module:module)
{
}

/**
 * A module has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:Class<FuncPrefix>_OnModuleEnable(Module:module)
{
}

/**
 * The module that hooked this event callback has been enabled.
 */
public Class<FuncPrefix>_OnMyModuleEnable()
{
}

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:Class<FuncPrefix>_OnMyModuleEnable()
{
}

/**
 * A module has been disabled.
 */
public Class<FuncPrefix>_OnModuleDisable(Module:module)
{
}

/**
 * A module has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:Class<FuncPrefix>_OnModuleDisable(Module:module)
{
}

/**
 * The module that hooked this event callback has been disabled.
 */
public Class<FuncPrefix>_OnMyModuleDisable()
{
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:Class<FuncPrefix>_OnMyModuleDisable()
{
}

/**
 * The map has started.
 */
public Class<FuncPrefix>_OnMapStart()
{
}

/**
 * The map has ended.
 */
public Class<FuncPrefix>_OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public Class<FuncPrefix>_OnAutoConfigsBuffered()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public Class<FuncPrefix>_OnConfigsExecuted()
{
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public Class<FuncPrefix>_OnConfigReload(configindex)
{
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public Class<FuncPrefix>_OnClientPutInServer(client)
{
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public Class<FuncPrefix>_OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public Class<FuncPrefix>_RoundStart()
{
}

/**
 * Pre-round freezetime has finished.
 */
public Class<FuncPrefix>_RoundFreezeEnd()
{
}

/**
 * Round has ended.
 */
public Class<FuncPrefix>_RoundEnd()
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
public Class<FuncPrefix>_PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public Class<FuncPrefix>_PlayerSpawn(client)
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
public Class<FuncPrefix>_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
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
public Class<FuncPrefix>_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public Class<FuncPrefix>_PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public Class<FuncPrefix>_WeaponFire(client, const String:weapon[])
{
}
