/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          models.inc
 *  Type:          Module
 *  Description:   Manages and precaches models.
 *
 *  Copyright (C) 2009-2011  Greyscale, Richard Helgeby
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
new Module:g_moduleModels;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:Models_GetIdentifier() { return g_moduleModels; }


#define MODELS_NAME_LEN     64
#define MODELS_STRING_LEN   255

/**
 * Maximum number of models.
 */
#define MODELS_MAX 48

/**
 * Maximum folder depth a model file can be located.
 */
#define MODELS_PATH_MAX_DEPTH 8

/**
 * Maximum string length of a folder a model file is located under.
 */
#define MODELS_PATH_DIR_MAX_LENGTH 32

/**
 * Model access settings.
 */
enum ModelAuthMode
{
    ModelAuth_Invalid = -1, /* Invalid authorization mode. */
    ModelAuth_Disabled,     /* No authorization. */
    ModelAuth_Flag,         /* Require flag. */
    ModelAuth_Group,        /* Require group. */
    ModelAuth_Either,       /* Require flag or group. */
    ModelAuth_Both,         /* Require flag and group. */
    ModelAuth_All,          /* Require all flags and all groups. */
}

/**
 * Avaliable teams for models. It's a mirror of ClassTeam to avoid dependency on
 * the class manager.
 */
enum ModelTeam
{
    ModelTeam_Invalid = -1,
    ModelTeam_Zombies,
    ModelTeam_Humans
}

/**
 * Model settings structure.
 */
enum ModelAttributes
{
    // General
    String:Model_Name[MODELS_NAME_LEN],         /** Unique name to identify the model. */
    String:Model_FileName[MODELS_NAME_LEN],     /** File name of model (no file extension). */
    String:Model_Path[PLATFORM_MAX_PATH],       /** Path to model files. */
    ModelTeam:Model_Team,                       /** Which team the model belongs to. */
    
    // Restrictions
    bool:Model_MotherZombiesOnly,               /** Only allow mother zombies to use the model. */
    bool:Model_AdminsOnly,                      /** Only allow admins to use the model. */
    bool:Model_IsHidden,                        /** Exclude model from random selections. */
    
    // Advanced authorization
    ModelAuthMode:Model_AuthMode,               /** Model authorization mode. */
    String:Model_Flags[MODELS_STRING_LEN],      /** Flag name whitelist. Separated by comma (,). */
    String:Model_Groups[MODELS_STRING_LEN]      /** Group name whitelist. Separated by comma (,). */
}

/**
 * Register this module.
 */
Models_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Model manager");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "models");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Manages and precaches models.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleModels = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleModels, "Event_OnEventsRegister",      "Models_OnEventsRegister");
    
    // Register config file that this module will use.
    ConfigMgr_Register(g_moduleModels, "Models_OnConfigReload", "configs/<projectshortname>/models.txt");
}

/**
 * Plugin is loading.
 */
Models_OnPluginStart()
{
    // Register the module.
    Models_Register();
}

/**
 * All plugins have loaded.
 */
public Models_OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public Models_OnPluginEnd()
{
}

/**
 * Register all events here.
 */
public Models_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnAllPluginsLoaded",      "Models_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnPluginEnd",             "Models_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnAllModulesLoaded",      "Models_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnModuleEnable",          "Models_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnMyModuleEnable",        "Models_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnModuleDisable",         "Models_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnMyModuleDisable",       "Models_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnMapStart",              "Models_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnMapEnd",                "Models_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnAutoConfigsBuffered",   "Models_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnConfigsExecuted",       "Models_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnClientPutInServer",     "Models_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_OnClientDisconnect",      "Models_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_moduleModels, "Event_RoundStart",              "Models_RoundStart");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_RoundFreezeEnd",          "Models_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_RoundEnd",                "Models_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_PlayerTeam",              "Models_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_PlayerSpawn",             "Models_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_PlayerHurt",              "Models_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_PlayerDeath",             "Models_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_PlayerJump",              "Models_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleModels, "Event_WeaponFire",              "Models_WeaponFire");
    
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public Models_OnEventsReady()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public Models_OnConfigsExecuted()
{
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public Models_OnConfigReload(configindex)
{
}
