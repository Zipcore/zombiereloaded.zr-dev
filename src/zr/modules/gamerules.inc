/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          gamerules.inc
 *  Type:          Module
 *  Description:   Game rule controller.
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
new Module:g_moduleGameRules;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:GameRules_GetIdentifier() { return g_moduleGameRules; }

/**
 * Cvar handles.
 */
new Handle:g_hCvarGameRulesConfigFile;
new Handle:g_hCvarGameRuleSet;

/**
 * Available game rule actions.
 */
enum GRuleModuleCmds
{
    GRuleModuleCmd_Invalid = -1,
    GRuleModuleCmd_On,
    GRuleModuleCmd_ForceOn,
    GRuleModuleCmd_Off,
    GRuleModuleCmd_ForceOff
}

/**
 * Array/string sizes for the enum members below.
 */
#define GAMERULES_NAME_LEN 64
#define GAMERULES_MAX_MODULES 128

/**
 * Game rule data structure.
 */
enum GameRuleSet
{
    String:GameRuleSet_Name[GAMERULES_NAME_LEN],                            /** Name of rule set. */
    Module:GameRuleSet_Core,                                                /** The root module for the game mode. */
    String:GameRuleSet_Cfg[PLATFORM_MAX_PATH],                              /** The cfg file to execute for this rule set. */
    Module:GameRuleSet_Module[GAMERULES_MAX_MODULES],                       /** Modules that are listed in the rule set. */
    GRuleModuleCmds:GameRuleSet_ModuleCmd[GAMERULES_MAX_MODULES],   /** The command for each module. */
    GameRuleSet_NumModules                                                  /** Number of modules in the rule set. */
}

/**
 * Dummy array used to see how many cells are required to store all game rules.
 */
stock g_DummyGameRulesData[GameRuleSet];

/**
 * Array handle for game rule sets.
 */
new Handle:g_hGameRules;

/**
 * Fallback game rule set.
 */
#define GRULESET_FALLBACK "zrclassic"

/**
 * Cache for the current rule set that's loaded.
 */
new g_RuleSetCache[GameRuleSet];

/**
 * Readability macros for reading data from the game rule set cache.
 */
#define GRULESET_NAME           g_RuleSetCache[GameRuleSet_Name]
#define GRULESET_CORE           g_RuleSetCache[GameRuleSet_Core]
#define GRULESET_CFG            g_RuleSetCache[GameRuleSet_Cfg]
#define GRULESET_MODULE(%1)     g_RuleSetCache[GameRuleSet_Module][%1]
#define GRULESET_MODULECMD(%1)  g_RuleSetCache[GameRuleSet_ModuleCmd][%1]
#define GRULESET_NUMMODULES     g_RuleSetCache[GameRuleSet_NumModules]

/**
 * The number of game rule sets that are configured in the game rules config file.
 */
new g_iGameRuleCount;

/**
 * The currently applied game rule set.
 */
new g_iCurGameRuleSet = -1;

/**
 * Register this module.
 */
GameRules_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Game Rules");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "gamerules");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Manipulates modules and configs to create unique game modes via rule sets.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleGameRules = ModuleMgr_Register(moduledata);
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnEventsRegister",         "GameRules_OnEventsRegister");
    
    // Register config file(s) that this module will use.
    ConfigMgr_Register(g_moduleGameRules, "GameRules_OnConfigReload", "");
}

/**
 * Register all events here.
 */
public GameRules_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnEventsReady",            "GameRules_OnEventsReady");
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnModuleEnable",           "GameRules_OnModuleEnable");
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnModuleDisable",          "GameRules_OnModuleDisable");
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnMyModuleDisable",        "GameRules_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleGameRules, "Event_OnConfigsExecuted",        "GameRules_OnConfigsExecuted");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public GameRules_OnEventsReady()
{
    // Bump up priority for Event_OnConfigsExecuted to above mapconfig the module to ensure game rule set cfgs are executed before map configs.
    EventMgr_GivePriority("Event_OnConfigsExecuted", g_moduleGameRules, MapConfig_GetIdentifier());
}

/**
 * Plugin is loading.
 */
GameRules_OnPluginStart()
{
    // Register the module.
    GameRules_Register();
    
    // Create cvars.
    g_hCvarGameRulesConfigFile  = Project_CreateConVar("gamerules_configfile", "configs/zr/gamerules.txt", "Path to game rules config file.  Path is relative to the \"sourcemod/\" directory.");
    g_hCvarGameRuleSet          = Project_CreateConVar("gamerules_ruleset", "zrclassic", "Name of game rule set to use.");
    
    // Create array.
    g_hGameRules = CreateArray(sizeof(g_DummyGameRulesData));
}

/**
 * A module has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:GameRules_OnModuleEnable(Module:module)
{
    // Ignore this module.
    if (module == g_moduleGameRules)
        return Plugin_Continue;
    
    // If no game rule set is applied yet, then let it be enabled.
    if (g_iCurGameRuleSet == -1)
        return Plugin_Continue;
    
    // Don't allow a root module that isn't defined by the current game mode be enabled.
    if (ModuleMgr_ReadCell(module, ModuleData_Root) && module != GRULESET_CORE)
        return Plugin_Handled;
    
    // Find module in current game rule set.
    new moduleindex = GameRules_FindModuleInRuleSet(g_iCurGameRuleSet, module);
    if (moduleindex == -1)
        return Plugin_Continue;
    
    // If this module is forced off then don't let it enable.
    if (GRULESET_MODULECMD(moduleindex) == GRuleModuleCmd_ForceOff)
        return Plugin_Handled;
    
    return Plugin_Continue;
}

/**
 * A module has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:GameRules_OnModuleDisable(Module:module)
{
    // Ignore this module, it's already handled in GameRules_OnMyModuleDisable
    if (module == g_moduleGameRules)
        return Plugin_Continue;
    
    // If no game rule set is applied yet, then let it be disabled.
    if (g_iCurGameRuleSet == -1)
        return Plugin_Continue;
    
    // Don't allow a root module defined by the current game mode be disabled.
    if (module == GRULESET_CORE)
        return Plugin_Handled;
    
    // Find module in current game rule set.
    new moduleindex = GameRules_FindModuleInRuleSet(g_iCurGameRuleSet, module);
    if (moduleindex == -1)
        return Plugin_Continue;
    
    // If this module is forced on then don't let it disable.
    if (GRULESET_MODULECMD(moduleindex) == GRuleModuleCmd_ForceOn)
        return Plugin_Handled;
    
    return Plugin_Continue;
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:GameRules_OnMyModuleDisable()
{
    return Plugin_Handled;
}

/**
 * Loops through each section of the keyvalues tree.
 * 
 * @param kv            The keyvalues handle of the config file. (Don't close this)
 * @param sectionindex  The index of the current keyvalue section, starting from 0.
 * @param sectionname   The name of the current keyvalue section.
 * 
 * @return              See enum KvCache.
 */
public KvCache:GameRules_Cache(Handle:kv, sectionindex, const String:sectionname[])
{
    new gameRuleSet[GameRuleSet];
    
    // Section name = game rule set name.
    strcopy(gameRuleSet[GameRuleSet_Name], GAMERULES_NAME_LEN, sectionname);
    
    // Store data from known key names.
    
    // Get value and validate for "core"
    decl String:rootmodulename[MM_DATA_SHORTNAME];
    KvGetString(kv, "core", rootmodulename, sizeof(rootmodulename));
    new Module:rootmodule = ModuleMgr_FindByString(ModuleData_ShortName, rootmodulename);
    if (rootmodule != INVALID_MODULE)
    {
        gameRuleSet[GameRuleSet_Core] = rootmodule;
    }
    else
    {
        gameRuleSet[GameRuleSet_Core] = INVALID_MODULE;
        LogMgr_Print(g_moduleGameRules, LogType_Error, "Config Validation", "No core specified for game rule set \"%s\"!", sectionname);
    }
    
    // Get value for "cfg"
    KvGetString(kv, "cfg", gameRuleSet[GameRuleSet_Cfg], sizeof(gameRuleSet[GameRuleSet_Cfg]));
    
    // Go down a level and store module command data.
    if (KvJumpToKey(kv, "modulecmds"))
    {
        decl String:moduleshortname[MM_DATA_SHORTNAME];
        decl String:strModuleCmd[64];
        new numModules;
        new Module:module;
        new GRuleModuleCmds:modulecmd;
        KvGotoFirstSubKey(kv, false);
        do
        {
            // Check if the module limit is reached.
            if (numModules >= GAMERULES_MAX_MODULES)
            {
                // Log a warning. Too many modules.
                LogMgr_Print(g_moduleGameRules, LogType_Error, "Config Validation", "Not enough memory allocated to store more than %d modules from the \"%s\" game rule set.  Contact a plugin developer.", GAMERULES_MAX_MODULES, sectionname);
                break;
            }
            
            KvGetSectionName(kv, moduleshortname, sizeof(moduleshortname));
            KvGoBack(kv);
            KvGetString(kv, moduleshortname, strModuleCmd, sizeof(strModuleCmd));
            KvJumpToKey(kv, moduleshortname);
            
            // Get the module identifier.
            module =  ModuleMgr_FindByString(ModuleData_ShortName, moduleshortname);
            
            // Check if module is valid.
            if (module != INVALID_MODULE)
            {
                // Validate and set rule action.
                modulecmd = GameRules_ModuleCmdToSymbol(strModuleCmd);
                if (modulecmd != GRuleModuleCmd_Invalid)
                {
                    gameRuleSet[GameRuleSet_Module][numModules] = module;
                    gameRuleSet[GameRuleSet_ModuleCmd][numModules] = modulecmd;
                    numModules++;
                }
                else
                {
                    LogMgr_Print(g_moduleGameRules, LogType_Error, "Config Validation", "Invalid module command in rule set \"%s\": \"%s\".", sectionname, strModuleCmd);
                }
            }
            else
            {
                LogMgr_Print(g_moduleGameRules, LogType_Error, "Config Validation", "Invalid module in rule \"%s\": \"%s\".", sectionname, moduleshortname);
            }
            
        } while (KvGotoNextKey(kv, false));
        KvGoBack(kv);   // Go back to 2nd level.
        
        gameRuleSet[GameRuleSet_NumModules] = numModules;
        
        // Go back to 1st level.
        KvGoBack(kv);
    }
    else
    {
        // Set the first one to INVALID_MODULE so the plugin knows there are no module commands.
        gameRuleSet[GameRuleSet_Module][0] = INVALID_MODULE;
    }
    
    // Push all this collected data to an array.
    PushArrayArray(g_hGameRules, gameRuleSet[0], sizeof(gameRuleSet));
    
    return KvCache_Continue;
}

/**
 * Re-cache all game rules data from disk.
 * Never use this before OnConfigsExecuted!
 */
GameRules_CacheGameRulesData()
{
    decl String:configfile[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarGameRulesConfigFile, configfile, sizeof(configfile));
    
    if (ConfigMgr_ValidateFile(configfile))
    {
        ConfigMgr_WriteString(g_moduleGameRules, CM_CONFIGINDEX_FIRST, ConfigData_Path, CM_DATA_PATH, configfile);
    }
    else
    {
        LogMgr_Print(g_moduleGameRules, LogType_Fatal_Module, "Config Validation", "Error: Invalid config file path in cvar zr_gamerules_configfile: \"%s\"", configfile);
        return;
    }
    
    ClearArray(g_hGameRules);
    g_iGameRuleCount = ConfigMgr_CacheKv(g_moduleGameRules, CM_CONFIGINDEX_FIRST, "GameRules_Cache");
    
    // There were no game rule sets configured.
    if (g_iGameRuleCount == 0)
        LogMgr_Print(g_moduleGameRules, LogType_Fatal_Module, "Config Validation", "Error: No usable data found in game rules config file: %s", configfile);
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public GameRules_OnConfigsExecuted()
{
    GameRules_CacheGameRulesData();
    GameRules_LoadRuleSetFromCvar();
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public GameRules_OnConfigReload(configindex)
{
    GameRules_CacheGameRulesData();
    GameRules_LoadRuleSetFromCvar();
}

/**
 * Converts a string value to a module cmd symbol.
 * 
 * @param modulecmd     String value to convert to the appropriate GRuleModuleCmds symbol.
 * 
 * @return              A member of enum GRuleModuleCmds, GRuleModuleCmd_Invalid if no symbol matches.
 */
GRuleModuleCmds:GameRules_ModuleCmdToSymbol(const String:modulecmd[])
{
    if (StrEqual(modulecmd, "on", false))
    {
        return GRuleModuleCmd_On;
    }
    else if (StrEqual(modulecmd, "force_on", false))
    {
        return GRuleModuleCmd_ForceOn;
    }
    else if (StrEqual(modulecmd, "off", false))
    {
        return GRuleModuleCmd_Off;
    }
    else if (StrEqual(modulecmd, "force_off", false))
    {
        return GRuleModuleCmd_ForceOff;
    }
    
    return GRuleModuleCmd_Invalid;
}

/**
 * Loads and applies the rule set specified in the config.
 */
GameRules_LoadRuleSetFromCvar()
{
    decl String:ruleSetName[GAMERULES_NAME_LEN];
    GetConVarString(g_hCvarGameRuleSet, ruleSetName[0], sizeof(ruleSetName));
    
    GameRules_LoadRuleSet(ruleSetName);
}

/**
 * Loads a named rule set.
 * 
 * @param ruleset   The ruleset as listed in gamerules config file.
 */
stock GameRules_LoadRuleSet(const String:ruleset[])
{
    // Find the index of the named ruleset.  -1 is returned if invalid.
    new rulesetindex = GameRules_FindRuleSet(ruleset);
    new fallbackrulesetindex = GameRules_FindRuleSet(GRULESET_FALLBACK);
    if (rulesetindex > -1)
    {
        GameRules_ApplyRuleSet(rulesetindex);
    }
    else if (fallbackrulesetindex > -1)
    {
        LogMgr_Print(g_moduleGameRules, LogType_Error, "Config Validation", "Invalid rule set name \"%s\", falling back to rule set \"%s\".", ruleset, GRULESET_FALLBACK);
        GameRules_ApplyRuleSet(fallbackrulesetindex);
    }
    else
    {
        LogMgr_Print(g_moduleGameRules, LogType_Fatal_Plugin, "Config Validation", "Invalid rule set name \"%s\" and failed to fall back to \"%s\", unloading plugin.", ruleset, GRULESET_FALLBACK);
    }
}

/**
 * Gets the rule set with the specified name.
 *
 * @param name  Name of rule set.
 *  
 * @return      Rule set index if found, -1 otherwise.
 */
stock GameRules_FindRuleSet(const String:name[])
{
    new gameRuleSet[GameRuleSet];
    for (new ruleSet = 0; ruleSet < g_iGameRuleCount; ruleSet++)
    {
        GetArrayArray(g_hGameRules, ruleSet, gameRuleSet[0], sizeof(gameRuleSet));  
        if (StrEqual(gameRuleSet[GameRuleSet_Name], name, false))
            return ruleSet;
    }
    
    return -1;
}

/**
 * Gets the rule set with the specified name.
 *
 * @param ruleset   The game rule set index to look for module in.
 * @param module    The module to look for.
 * 
 * @return          Index that module lies in.
 */
stock GameRules_FindModuleInRuleSet(ruleset, Module:module)
{
    for (new moduleindex = 0; moduleindex < GRULESET_NUMMODULES; moduleindex++)
    {
        if (module == GRULESET_MODULE(moduleindex))
            return moduleindex;
    }
    
    return -1;
}

/**
 * Applies a rule set and sets the module state (enable or disable them)
 * according to the rule set.
 *
 * @param ruleSet   Index of rule set to apply.
 */
stock GameRules_ApplyRuleSet(ruleSet)
{
    GetArrayArray(g_hGameRules, ruleSet, g_RuleSetCache[0], sizeof(g_RuleSetCache));
    g_iCurGameRuleSet = ruleSet;    // Hiding behind this line above in case ruleSet is invalid.
    
    // Enable the core's root module.
    if (GRULESET_CORE != INVALID_MODULE)
        ModuleMgr_Enable(GRULESET_CORE);
    
    // Disable all other cores.
    new count = MODULE_COUNT;
    for (new moduleindex = 0; moduleindex < count; moduleindex++)
    {
        if (ModuleMgr_ReadCell(Module:moduleindex, ModuleData_Root) && Module:moduleindex != GRULESET_CORE)
            ModuleMgr_Disable(Module:moduleindex);
    }
    
    // Execute this rule set's cfg file.
    ServerCommand("exec sourcemod/zombiereloaded/%s", GRULESET_CFG);
    LogMgr_Print(g_moduleGameRules, LogType_Debug, "Game Rule Configs", "Executed game rule set file: sourcemod/zombiereloaded/%s", GRULESET_CFG);
    
    // Loop through each module and apply its action.
    for (new moduleindex = 0; moduleindex < GRULESET_NUMMODULES; moduleindex++)
    {
        // Apply action.
        switch (GRULESET_MODULECMD(moduleindex))
        {
            case GRuleModuleCmd_ForceOn:    ModuleMgr_Enable(GRULESET_MODULE(moduleindex));
            case GRuleModuleCmd_Off:        ModuleMgr_Disable(GRULESET_MODULE(moduleindex));
            case GRuleModuleCmd_ForceOff:   ModuleMgr_Disable(GRULESET_MODULE(moduleindex));
        }
    }
}
