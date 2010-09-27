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

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <adminmenu>
#include <regex>
#include <sdktools>
#include <clientprefs>
#include <cstrike>
#include <sdkhooks>

#define VERSION "3.0.0-dev"

// Comment this line to exclude version info command. Enable this if you have
// the repository and HG installed (Mercurial or TortoiseHG).
#define ADD_VERSION_INFO

#if defined ADD_VERSION_INFO
#include "zr/hgversion.h"
#include "zr/versioninfo"
#endif

// Header includes.
// TODO: To be removed when base conversion is done.
#include "zr/log.h"
#include "zr/tools.h"
#include "zr/models.h"
#include "zr/playerclasses/classheaders.h"
#include "zr/weapons/ammoheaders.h"
//#include "zr/weapons/restrictheaders.h"
#include "zr/weapons/weaponprofileheaders.h"
#include "zr/headers/volfeatures.h"

// Project settings.
#include "zr/project"

// Base project includes.
#include "zr/base/wrappers"
#include "zr/base/versioninfo"
#include "zr/base/accessmanager"
#include "zr/base/logmanager"
#include "zr/base/translationsmanager"
#include "zr/base/configmanager"
#include "zr/base/eventmanager"
#include "zr/base/modulemanager"

// Libraries and generic utilities.
#include "zr/libraries/authcachelib"
#include "zr/libraries/conversionlib"
#include "zr/libraries/cookielib"
#include "zr/libraries/cvarlib"
#include "zr/libraries/functionlib"
#include "zr/libraries/menulib"
#include "zr/libraries/offsetlib"
#include "zr/libraries/sdktoolslib"
#include "zr/libraries/shoppinglistlib"
#include "zr/libraries/teamlib"
#include "zr/libraries/utilities"
#include "zr/libraries/weaponlib"

// Core modules (for the new base)
#include "zr/baseadapter"

#include "zr/modules/mapconfig"
#include "zr/modules/gamedata"      // Depends on mapconfig
#include "zr/modules/gamerules"     // Depends on mapconfig
#include "zr/modules/sdkhooksadapter"
#include "zr/modules/zr_core"
#include "zr/modules/zrc_core/root.zrc"
//#include "zr/modules/zriot_core/root.zriot" (not imported yet)

// Game components
#include "zr/modules/ztele"
#include "zr/modules/stripobjectives"
#include "zr/modules/classes/classmanager"
// models, zmenu, cookies, etc.


// ----- Old base -----

// Core includes (old).
#include "zr/zombiereloaded"

#include "zr/translation"
#include "zr/cvars"
#include "zr/admintools"
#include "zr/log"
#include "zr/config"
#include "zr/tools"
#include "zr/credits"
#include "zr/steamidcache"
#include "zr/sayhooks"
#include "zr/menu"
#include "zr/menu_attach"
#include "zr/cookies"
#include "zr/paramtools"
#include "zr/paramparser"
//#include "zr/shoppinglist"
#include "zr/downloads"
#include "zr/overlays"
#include "zr/playerclasses/playerclasses"
#include "zr/models"
#include "zr/weapons/weapons"
#include "zr/hitgroups"
#include "zr/roundstart"
#include "zr/roundend"
#include "zr/flashlight"
//#include "zr/infect"
//#include "zr/damage"
#include "zr/zadmin"
#include "zr/commands"

// Modules
#include "zr/account"
#include "zr/visualeffects/visualeffects"
#include "zr/soundeffects/soundeffects"
#include "zr/antistick"
#include "zr/knockback"
//#include "zr/spawnprotect"    // Disabled due to incompatibility with new infection modules.
#include "zr/respawn"
#include "zr/napalm"
#include "zr/jumpboost"
#include "zr/zspawn"
//#include "zr/ztele"
#include "zr/zhp"
#include "zr/zcookies"
#include "zr/jumpboost"
#include "zr/volfeatures/volfeatures"
#include "zr/debugtools"

// Include this last since nothing should be using it anyway.  Aside from external plugins.
#include "zr/api/api"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = "Zombie:Reloaded",
    author = "Greyscale | Richard Helgeby",
    description = "Infection/survival style gameplay",
    version = VERSION,
    url = "http://code.google.com/p/zombiereloaded/"
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
    decl String:gamefolder[32];
    GetGameFolderName(gamefolder, sizeof(gamefolder));
    if (!StrEqual(gamefolder, "cstrike"))
    {
        SetFailState("This mod is not supported by Zombie:Reloaded.");
    }
    
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
    // Forward event to other project base components.
    
    ModuleMgr_OnPluginStart();
    
    #if defined EVENT_MANAGER
        EventMgr_OnPluginStart();
    #endif
    
    #if defined CONFIG_MANAGER
        ConfigMgr_OnPluginStart();
    #endif
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnPluginStart();
    #else
        Project_LoadExtraTranslations(false); // Call this to load translations if the translations manager isn't included.
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnPluginStart();
    #endif
    
    #if defined ACCESS_MANAGER
        AccessMgr_OnPluginStart();
    #endif
    
    #if defined VERSION_INFO
        VersionInfo_OnPluginStart();
    #endif
    
    // Forward the OnPluginStart event to all modules.
    ForwardOnPluginStart();
    
    // All modules should be registered by this point!
    
    #if defined EVENT_MANAGER
        // Forward the OnEventsRegister to all modules.
        EventMgr_Forward(g_EvOnEventsRegister, g_CommonEventData1, 0, 0, g_CommonDataType1);
        
        // Forward the OnEventsReady to all modules.
        EventMgr_Forward(g_EvOnEventsReady, g_CommonEventData1, 0, 0, g_CommonDataType1);
        
        // Forward the OnAllModulesLoaded to all modules.
        EventMgr_Forward(g_EvOnAllModulesLoaded, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * Plugin is ending.
 */
public OnPluginEnd()
{
    // Unload in reverse order of loading.
    
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(g_EvOnPluginEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
    
    // Forward event to other project base components.
    
    #if defined VERSION_INFO
        VersionInfo_OnPluginEnd();
    #endif
    
    #if defined ACCESS_MANAGER
        AccessMgr_OnPluginEnd();
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnPluginEnd();
    #endif
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnPluginEnd();
    #endif
    
    #if defined CONFIG_MANAGER
        ConfigMgr_OnPluginEnd();
    #endif
    
    #if defined EVENT_MANAGER
        EventMgr_OnPluginEnd();
    #endif
    
    ModuleMgr_OnPluginEnd();
}

/**
 * TODO: Add this forward to the new base.
 * Called when a clients movement buttons are being processed.
 * 
 * @param client	Index of the client.
 * @param buttons	Copyback buffer containing the current commands (as bitflags - see entity_prop_stocks.inc).
 * @param impulse	Copyback buffer containing the current impulse command.
 * @param vel		Players desired velocity.
 * @param angles	Players desired view angles.
 * @param weapon	Entity index of the new weapon if player switches weapon, 0 otherwise.
 * 
 * @return 			Plugin_Handled to block the commands from being processed, Plugin_Continue otherwise.
 */
/*public Action:OnPlayerRunCmd(client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
    // Forward event to modules.
    //return InfectOnPlayerRunCmd(client, impulse);
}*/

/**
 * TODO: Add this forward to the new base.
 * Called when an entity is created.
 *
 * @param entity    Entity index.
 * @param classname Class name.
 */
/*public OnEntityCreated(entity, const String:classname[])
{
    // Forward event to modules.
    //DamageOnEntityCreated(entity);
}*/
