/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          baseadapter.inc
 *  Type:          Module
 *  Description:   Adapter module for running the old base on the new base as a module.
 *
 *  Copyright (C) 2009-2010  Richard Helgeby
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
new Module:g_moduleBaseAdapter;

/**
 * Register this module.
 */
BaseAdapter_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = true;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "Base Adapter");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "baseadapter");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Adapter for the old base system while converting to the new base.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleBaseAdapter = ModuleMgr_Register(moduledata);
    
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnAllPluginsLoaded,       "BaseAdapter_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnPluginEnd,              "BaseAdapter_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnAllModulesLoaded,       "BaseAdapter_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnModuleEnable,           "BaseAdapter_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnMyModuleEnable,         "BaseAdapter_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnModuleDisable,          "BaseAdapter_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnMyModuleDisable,        "BaseAdapter_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnMapStart,               "BaseAdapter_OnMapStart");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnMapEnd,                 "BaseAdapter_OnMapEnd");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnAutoConfigsBuffered,    "BaseAdapter_OnAutoConfigsBuffd");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnConfigsExecuted,        "BaseAdapter_OnConfigsExecuted");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnClientPutInServer,      "BaseAdapter_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnClientConnected,        "BaseAdapter_OnClientConnected");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnClientDisconnect,       "BaseAdapter_OnClientDisconnect");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnClientCookiesCached,    "BaseAdapter_OnClientCookiesChed");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_OnClientPostAdminCheck,   "BaseAdapter_OnClientPostAdmChk");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_RoundStart,               "BaseAdapter_RoundStart");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_RoundEnd,                 "BaseAdapter_RoundEnd");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_PlayerTeam,               "BaseAdapter_PlayerTeam");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_PlayerSpawn,              "BaseAdapter_PlayerSpawn");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_PlayerJump,               "BaseAdapter_PlayerJump");

    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_RoundFreezeEnd,           "BaseAdapter_RoundFreezeEnd");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_PlayerHurt,               "BaseAdapter_PlayerHurt");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_PlayerDeath,              "BaseAdapter_PlayerDeath");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_WeaponFire,               "BaseAdapter_WeaponFire");
    EventMgr_RegisterEvent(g_moduleBaseAdapter, Event_WeaponFireOnEmpty,        "BaseAdapter_WeaponFireOnEmpty");
    
    #endif
    
    // Register config file(s) that this module will use.
    // ConfigMgr_Register(g_moduleBaseAdapter, "BaseAdapter_OnConfigReload", "configs/<projectshortname>/<config>.txt");
}

/**
 * Plugin is loading.
 */
BaseAdapter_OnPluginStart()
{
    // Register the module.
    BaseAdapter_Register();
    
    // Forward event to modules.
    LogInit();          // Doesn't depend on CVARs.
    TranslationInit();
    CvarsInit();
    ToolsInit();
    MenuAttachInit();
    CookiesInit();
    CommandsInit();
    WeaponsInit();
    AmmoManagerInit();
    //EventInit();
}

/**
 * All plugins have loaded.
 */
public BaseAdapter_OnAllPluginsLoaded()
{
    // Forward event to modules.
    WeaponsOnAllPluginsLoaded();
}

/**
 * Plugin is ending.
 */
/*public BaseAdapter_OnPluginEnd()
{
}*/

/**
 * All modules have been registered.
 */
/*public BaseAdapter_OnAllModulesLoaded()
{
}*/

/**
 * A module has been enabled.
 */
/*public BaseAdapter_OnModuleEnable(Module:module)
{
}*/

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop load, and Plugin_Continue to allow it.
 */
/*public Action:BaseAdapter_OnMyModuleEnable()
{
}*/

/**
 * A module has been disabled.
 */
/*public BaseAdapter_OnModuleDisable(Module:module)
{
}*/

/**
 * The module that hooked this event callback has been disabled.
 */
/*public BaseAdapter_OnMyModuleDisable()
{
}*/

/**
 * The map has started.
 */
public BaseAdapter_OnMapStart()
{
    // Forward event to modules.
    ClassOnMapStart();
    OverlaysOnMapStart();
    RoundEndOnMapStart();
    InfectOnMapStart();
    SEffectsOnMapStart();
    ZSpawnOnMapStart();
    VolInit();
}

/**
 * The map has ended.
 */
public BaseAdapter_OnMapEnd()
{
    // Forward event to modules.
    VEffectsOnMapEnd();
    VolOnMapEnd();
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here.
 */
public BaseAdapter_OnAutoConfigsBuffd()
{
	// Load map configurations.
    ConfigLoad();
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public BaseAdapter_OnConfigsExecuted()
{
    // Forward event to modules. (OnConfigsExecuted)
    ModelsLoad();
    DownloadsLoad();
    WeaponsLoad();
    HitgroupsLoad();
    RoundEndLoad();
    InfectLoad();
    DamageLoad();
    VEffectsLoad();
    SEffectsLoad();
    AntiStickLoad();
    ClassLoad();
    VolLoad();
    
    // Forward event to modules. (OnModulesLoaded)
    ConfigOnModulesLoaded();
    ClassOnModulesLoaded();
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public BaseAdapter_OnConfigReload(configindex)
{
}

/**
 * Client has just connected to the server.
 */
public BaseAdapter_OnClientConnected(client)
{
    // Forward event to modules.
    ClassOnClientConnected(client);
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public BaseAdapter_OnClientPutInServer(client)
{
    // Forward event to modules.
    ClassClientInit(client);
    OverlaysClientInit(client);
    WeaponsClientInit(client);
    InfectClientInit(client);
    DamageClientInit(client);
    SEffectsClientInit(client);
    AntiStickClientInit(client);
    SpawnProtectClientInit(client);
    RespawnClientInit(client);
    ZTeleClientInit(client);
    ZHPClientInit(client);
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public BaseAdapter_OnClientDisconnect(client)
{
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    InfectOnClientDisconnect(client);
    DamageOnClientDisconnect(client);
    SEffectsOnClientDisconnect(client);
    AntiStickOnClientDisconnect(client);
    SpawnProtectOnClientDisconnect(client);
    RespawnOnClientDisconnect(client);
    ZSpawnOnClientDisconnect(client);
    ZHPOnClientDisconnect(client);
    VolOnPlayerDisconnect(client);
    AmmoOnClientDisconnect(client);
}

/**
 * Called once a client's saved cookies have been loaded from the database.
 * 
 * @param client		Client index.
 */
public BaseAdapter_OnClientCookiesChed(client)
{
    LogEvent(false, LogTypeOld_Normal, LOG_DEBUG, LogModule_Config, "Event", "OnCookiesCached fired on client %d", client);
    
    // Forward "OnCookiesCached" event to modules.
    ClassOnCookiesCached(client);
    WeaponsOnCookiesCached(client);
    CreditsOnCookiesCached(client);
    ZHPOnCookiesCached(client);
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
public BaseAdapter_OnClientPostAdmChk(client)
{
    // Forward authorized event to modules that depend on client admin info.
    ClassOnClientPostAdminCheck(client);
}

/**
 * Round has started.
 */
public BaseAdapter_RoundStart()
{
    // Forward event to sub-modules.
    OverlaysOnRoundStart();
    RoundStartOnRoundStart();
    RoundEndOnRoundStart();
    InfectOnRoundStart();
    SEffectsOnRoundStart();
    ZSpawnOnRoundStart();
    VolOnRoundStart();
}

/**
 * Pre-round has freezetime has finished.
 */
public BaseAdapter_RoundFreezeEnd()
{
    // Forward events to modules.
    RoundEndOnRoundFreezeEnd();
    InfectOnRoundFreezeEnd();
    ZSpawnOnRoundFreezeEnd();
}

/**
 * Round has ended.
 */
public BaseAdapter_RoundEnd(winner, reason, const String:message[])
{
    // Forward event to modules.
    WeaponsOnRoundEnd();
    RoundEndOnRoundEnd(reason);
    InfectOnRoundEnd();
    SEffectsOnRoundEnd();
    RespawnOnRoundEnd();
    ZSpawnOnRoundEnd();
    VolOnRoundEnd();
    AmmoOnRoundEnd();
}

/**
 * Client has joined a team.
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public BaseAdapter_PlayerTeam(client, team, oldteam, bool:disconnect)
{
    // Forward event to modules.
    InfectOnClientTeam(client, team);
    
    // TODO: Should this event be blocked? In events.inc it is. How do we block it here?
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public BaseAdapter_PlayerSpawn(client)
{
    // Forward event to modules.
    InfectOnClientSpawn(client);     // Some modules depend on this to finish first.
    AccountOnClientSpawn(client);    // Some modules depend on this to finish first.
    ClassOnClientSpawn(client);
    WeaponsOnClientSpawn(client);
    RoundStartOnClientSpawn(client);
    SEffectsOnClientSpawn(client);
    RespawnOnClientSpawn(client);
    ZTeleOnClientSpawn(client);
    ZHPOnClientSpawn(client);
    VolOnPlayerSpawn(client);
    AmmoOnClientSpawn(client);		// Depends on class attributes!
    
    // Fire post player_spawn event.
    CreateTimer(0.0, BaseAdapter_PlayerSpawnPost, client);
}

/**
 * Event callback (player_spawn)
 * Client is spawning into the game. *Post
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:BaseAdapter_PlayerSpawnPost(Handle:timer, any:client)
{
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // Forward event to modules.
    WeaponsOnClientSpawnPost(client);
    SEffectsOnClientSpawnPost(client);
    ClassOnClientSpawnPost(client);
    SpawnProtectOnClientSpawnPost(client);	// Must be executed after class attributes are applied.
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
public BaseAdapter_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    // Forward event to modules.
    ClassOnClientHurt(victim);
    InfectOnClientHurt(victim, attacker, weapon);
    AccountOnClientHurt(victim, attacker, dmg_health);
    SEffectsOnClientHurt(victim);
    KnockbackOnClientHurt(victim, attacker, weapon, hitgroup, dmg_health);
    NapalmOnClientHurt(victim, attacker, weapon);
    ZHPOnClientHurt(victim);
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public BaseAdapter_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    // If client is being infected, then stop.
    if (StrEqual(weapon, "zombie_claws_of_death", false))
    {
        return;
    }
    
    // Forward event to modules.
    ClassOnClientDeath(victim);
    InfectOnClientDeath(victim, attacker);
    VEffectsOnClientDeath(victim);
    SEffectsOnClientDeath(victim);
    SpawnProtectOnClientDeath(victim);
    RespawnOnClientDeath(victim, attacker);
    NapalmOnClientDeath(victim);
    ZSpawnOnClientDeath(victim);
    ZTeleOnClientDeath(victim);
    ZHPOnClientDeath(victim);
    VolOnPlayerDeath(victim);
    RoundEndOnClientDeath();
    AmmoOnClientDeath(victim);
    APIOnClientDeath(victim, attacker, weapon, headshot);
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public BaseAdapter_PlayerJump(client)
{
    // Fire post player_jump event.
    CreateTimer(0.0, BaseAdapter_PlayerJumpPost, client);
}

/**
 * Event callback (player_jump)
 * Client is jumping. *Post
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:BaseAdapter_PlayerJumpPost(Handle:timer, any:client)
{
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // Forward event to modules.
    JumpBoostOnClientJumpPost(client);
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public BaseAdapter_WeaponFire(client, const String:weapon[])
{
    // TODO: Does this function have a performance impact when called 600 times/sec (60 players, 10 bullets/sec)?
    new weaponIndex = WeaponsNameToIndex(weapon);
    
    // Forward event to modules.
    NapalmOnWeaponFire(client, weapon);
    
    // Validate weapon index.
    if (weaponIndex >= 0)
    {
        new WeaponsSlot:weaponSlot = WeaponsGetSlot(weaponIndex);
        AmmoOnWeaponFire(client, weaponSlot);
    }
}

/**
 * Client attempted to fire a weapon with no bullets.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public BaseAdapter_WeaponFireOnEmpty(client, const String:weapon[])
{
    new weaponIndex = WeaponsNameToIndex(weapon);
    
    // Forward event to modules.
    
    // Validate weapon index.
    if (weaponIndex >= 0)
    {
        new WeaponsSlot:weaponSlot = WeaponsGetSlot(weaponIndex);
        AmmoOnWeaponFireOnEmpty(client, weaponSlot);
    }
}
