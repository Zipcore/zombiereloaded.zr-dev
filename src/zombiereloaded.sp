/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zombiereloaded.sp
 *  Type:          Base
 *  Description:   Plugin's base file.
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

// Comment to use ZR Tools Extension, otherwise SDK Hooks Extension will be used.
//#define USE_SDKHOOKS

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <clientprefs>
#include <cstrike>

#if defined USE_SDKHOOKS
    #include <sdkhooks>
    
    #define ACTION_CONTINUE     Plugin_Continue
    #define ACTION_HANDLED      Plugin_Handled
#else
    #include <zrtools>
    
    #define ACTION_CONTINUE     ZRTools_Continue
    #define ACTION_HANDLED      ZRTools_Handled
#endif

#define VERSION "3.0.0-dev"

// Comment this line to exclude version info command. Enable this if you have
// the repository and HG installed (Mercurial or TortoiseHG).
#define ADD_VERSION_INFO

// Header includes.
#include "zr/log.h"
#include "zr/models.h"
#include "zr/playerclasses/classheaders.h"
#include "zr/weapons/ammoheaders.h"
//#include "zr/weapons/restrictheaders.h"
#include "zr/weapons/weaponprofileheaders.h"

#if defined ADD_VERSION_INFO
#include "zr/hgversion.h"
#endif

// Core includes.
#include "zr/zombiereloaded"

#if defined ADD_VERSION_INFO
#include "zr/versioninfo"
#endif

#include "zr/translation"
#include "zr/cvars"
#include "zr/admintools"
#include "zr/log"
#include "zr/config"
#include "zr/tools"
#include "zr/steamidcache"
#include "zr/sayhooks"
#include "zr/menu"
#include "zr/cookies"
#include "zr/paramtools"
#include "zr/paramparser"
#include "zr/shoppinglist"
#include "zr/downloads"
#include "zr/overlays"
#include "zr/playerclasses/playerclasses"
#include "zr/models"
#include "zr/weapons/weapons"
#include "zr/hitgroups"
#include "zr/roundstart"
#include "zr/roundend"
#include "zr/infect"
#include "zr/damage"
#include "zr/event"
#include "zr/zadmin"
#include "zr/commands"

// Modules
#include "zr/account"
#include "zr/visualeffects/visualeffects"
#include "zr/soundeffects/soundeffects"
#include "zr/antistick"
#include "zr/knockback"
#include "zr/spawnprotect"
#include "zr/respawn"
#include "zr/napalm"
#include "zr/jumpboost"
#include "zr/zspawn"
#include "zr/ztele"
#include "zr/zhp"
#include "zr/zcookies"
#include "zr/jumpboost"
#include "zr/volfeatures/volfeatures"
#include "zr/debugtools"

// Include API last because no file uses the API while the API calls functions from every file.
#include "zr/api"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = "Zombie:Reloaded",
    author = "Greyscale | Richard Helgeby",
    description = "Infection/survival style gameplay",
    version = VERSION,
    url = "http://www.zombiereloaded.com"
};

/**
 * Called before plugin is loaded.
 * 
 * @param myself	Handle to the plugin.
 * @param late		Whether or not the plugin was loaded "late" (after map load).
 * @param error		Error message buffer in case load failed.
 * @param err_max	Maximum number of characters for error message buffer.
 * @return			APLRes_Success for load success, APLRes_Failure or APLRes_SilentFailure otherwise.
 */
public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    // Forward event to the API module.
    APIInit();
    
    // Let plugin load successfully.
    return APLRes_Success;
}

/**
 * Plugin is loading.
 */
public OnPluginStart()
{
    // Forward event to modules.
    LogInit();          // Doesn't depend on CVARs.
    TranslationInit();
    CvarsInit();
    ToolsInit();
    CookiesInit();
    CommandsInit();
    WeaponsInit();
    EventInit();
}

/**
 * All plugins have finished loading.
 */
public OnAllPluginsLoaded()
{
    // Forward event to modules.
    WeaponsOnAllPluginsLoaded();
}

/**
 * The map is starting.
 */
public OnMapStart()
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
 * The map is ending.
 */
public OnMapEnd()
{
    // Forward event to modules.
    VolOnMapEnd();
}

/**
 * Main configs were just executed.
 */
public OnAutoConfigsBuffered()
{
	// Load map configurations.
    ConfigLoad();
}

/**
 * Configs just finished getting executed.
 */
public OnConfigsExecuted()
{
    // Forward event to modules. (OnConfigsExecuted)
    ModelsLoad();
    DownloadsLoad();
    WeaponsLoad();
    HitgroupsLoad();
    InfectLoad();
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
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
public OnClientPutInServer(client)
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
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    InfectOnClientDisconnect(client);
    DamageOnClientDisconnect(client);
    AntiStickOnClientDisconnect(client);
    ZSpawnOnClientDisconnect(client);
    VolOnPlayerDisconnect(client);
}
