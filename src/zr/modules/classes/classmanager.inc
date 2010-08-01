/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          classmanager.inc
 *  Type:          Module
 *  Description:   Manages player classes.
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
new Module:g_moduleClassMgr;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ClassMgr_GetIdentifier() { return g_moduleClassMgr; }

/**
 * @section Event ID variables with documentation.
 */

/**
 * A class attribute is loading and ready to be cached.
 *
 * @param classIndex    Class index.
 * @param kv            Handle to keyvalue tree, ready to read attribute value.
 * @param attribute     Name of the current attribute.
 * @param className     Name of the current class (section name in keyvalue tree).
 * OnClassAttribLoad(classIndex, Handle:kv, const String:attribute[], const String:className[])
 */
new ProjectEvent:g_EvOnClassAttribLoad;

/**
 * Class manager sent an validation request. Attribute modules do validation on
 * all their attributes, and log errors if any.
 *
 * @param classIndex    Class index.
 * @param kv            Handle to keyvalue tree, ready to read attribute value.
 * @param attribute     Name of the current attribute.
 *
 * @return              Attribute module returns Plugin_Handled on validation error,
 *                      or Plugin_Continue if ok.
 * Action:OnClassValidate(classIndex)
 */
new ProjectEvent:g_EvOnClassValidate;

/**
 * All classes are loaded now. Attribute modules should now make a copy of their array
 * so the original values can be kept.
 *
 * @param classCount    Number of classes loaded.
 * Action:OnClassAllLoaded(classCount)
 */
new ProjectEvent:g_EvOnClassAllLoaded;

/**
 * Preloads player info before player preferences are loaded. The class manger
 * sets initial selected class indexes. Attribute modules may initialize players too.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 * OnClassPlayerPreload(client, classIndex)
 */
new ProjectEvent:g_EvOnClassPlayerPreload;

/**
 * Loads player info with player preferences (from cookies). The class manger
 * sets new selected class indexes according to player preferences. Attribute modules
 * may initialize players with their preferences too.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 * OnClassPlayerLoad(client, classIndex)
 */
new ProjectEvent:g_EvOnClassPlayerLoad;

/**
 * Class attributes are applied. Attribute modules should apply their own attributes
 * on the player now.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 * OnClassApply(client, classIndex)
 */
new ProjectEvent:g_EvOnClassApply;

/**
 * A player was infected. This event is fired AFTER class attributes are applied.
 * Attribute modules should use this event and not the infection module's event when
 * initializing zombie specific features.
 *
 * @param client        Client index.
 * @param attacker      Attacker client index.
 * @param motherZombie  Specifies whether the attacker was a mother zombie.
 * OnClassPlayerInfect(client, attacker, bool:motherZombie)
 */
new ProjectEvent:g_EvOnClassPlayerInfect;

/**
 * A player was turned back into a human. This event is fired AFTER class attributes
 * are applied. Attribute modules should use this event and not the infection module's
 * event when initializing human specific features.
 *
 * @param client        Client index.
 * OnClassPlayerHuman(client)
 */
new ProjectEvent:g_EvOnClassPlayerHuman;

/**
 * @endsection
 */

/**
 * Cvar handles.
 */
new Handle:g_hCvarClassFile;
//new Handle:g_hCvarClassesMenuSpawn;
//new Handle:g_hCvarClassesMenuJoin;
//new Handle:g_hCvarClassesRandom;
new Handle:g_hCvarClassesChangeTimelimit;
//new Handle:g_hCvarClassesSave;
new Handle:g_hCvarClassesDefaultZombie;
//new Handle:g_hCvarClassesDefaultMZombie;
new Handle:g_hCvarClassesDefaultHuman;
new Handle:g_hCvarClassesZombieSelect;
new Handle:g_hCvarClassesHumanSelect;

#define CLASS_MAX           48
#define CLASS_NAME_LEN      64
#define CLASS_STRING_LEN    256

/**
 * Class cache types. Specifies which data array to use.
 */
enum ClassCacheType
{
    ClassCache_Original,        /** Points to original data array. Data in this array is never changed once loaded. */
    ClassCache_Modified,        /** Points to writable data array. Data in this array may be modified through console commands. */
    ClassCache_Player           /** Points to player cache array. Data in this array is always valid and ready to read/modify. */
}

/**
 * Class teams.
 * TODO: Rename without 'New' once old class system is removed.
 */
enum ClassTeam
{
    ClassTeamNew_Invalid = -1,     /** Not valid in a class! */
    ClassTeamNew_Zombies,
    ClassTeamNew_Humans,
    ClassTeamNew_All               /** Not valid in a class! It's only used by filters. */
}

/**
 * Number of classes loaded.
 */
new g_ClassCount;

/**
 * Function for outside files to use to return number of classes loaded.
 */
stock ClassMgr_GetClassCount() { return g_ClassCount; }

/**
 * Specifies whether classes are loaded.
 */
new bool:g_ClassLoaded = false;

/**
 * Function for outside files to use to return if classes are loaded.
 */
stock bool:ClassMgr_IsClassesLoaded() { return g_ClassLoaded; }

/**
 * Selected classes.
 */
new g_ClassSelected[MAXPLAYERS + 1][ClassTeam];
new g_ClassNextClass[MAXPLAYERS + 1][ClassTeam];

/**
 * Specifies whether a player is allowed to change class with instant effect.
 * This is only used on human classes, and in combination with the
 * zr_classes_change_timelimit time limit, but could be used other places too.
 * The class menu will automatically check this setting and apply attributes if
 * set to true.
 */
new bool:g_ClassAllowInstantChange[MAXPLAYERS + 1];

/**
 * Timers for instant class change.
 */
new Handle:g_hClassInstantChange[MAXPLAYERS + 1];

/**
 * Specifies whether a player has spawned at least once, to check when a player
 * spawn for the first time.
 */
new bool:g_ClassPlayerSpawned[MAXPLAYERS + 1];

/**
 * @section Flags for special classes.
 */
#define CLASS_FLAG_ADMIN_ONLY       (1<<0)  /** Class is usable by admins only. */
#define CLASS_FLAG_MOTHER_ZOMBIE    (1<<1)  /** Class is usable by mother zombies only. */

/** A combination of special class flags. Used to exclude special classes. */
#define CLASS_SPECIALFLAGS           CLASS_FLAG_ADMIN_ONLY + CLASS_FLAG_MOTHER_ZOMBIE
/**
 * @endsection
 */

/**
 * Structure for class filter settings passed to various functions.
 */
// TODO: Uncomment when old class system is removed.
//enum ClassFilter
//{
//    bool:ClassFilter_IgnoreEnabled,     /** Ignore whether the class is disabled or not. */
//    ClassFilter_RequireFlags,           /** Flags the classes must have set. */
//    ClassFilter_DenyFlags,              /** Flags the classes cannot have set. */
//    ClassFilter_Client                  /** The client to check for class group permissions. Use 0 to ignore group filter, and negative to exclude classes with groups set. */
//}

/**
 * Empty filter structure.
 */
stock g_ClassNoFilter[ClassFilter];

/**
 * Filter structure for excluding special classes.
 */
stock g_ClassNoSpecialClasses[ClassFilter] = {false, 0, CLASS_SPECIALFLAGS, -1};


// Utility and logic includes.
#include "zr/modules/classes/attributeregister"
#include "zr/modules/classes/nextindexes"
#include "zr/modules/classes/instantchange"
#include "zr/modules/classes/classfilter"

// Attribute module includes
#include "zr/modules/classes/genericattributes"
#include "zr/modules/classes/model"


/**
 * Register this module.
 */
ClassMgr_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Class Manager");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "classmanager");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Manages player classes.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleClassMgr = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnEventsRegister",      "ClassMgr_OnEventsRegister");
    
    // Register config file that this module will use.
    ConfigMgr_Register(g_moduleClassMgr, "ClassMgr_OnConfigReload", "");
    
    // Create events.
    g_EvOnClassAttribLoad =         EventMgr_CreateEvent("Event_OnClassAttribLoad");
    g_EvOnClassValidate =           EventMgr_CreateEvent("Event_OnClassValidate");
    g_EvOnClassAllLoaded =          EventMgr_CreateEvent("Event_OnClassAllLoaded");
    g_EvOnClassPlayerPreload =      EventMgr_CreateEvent("Event_OnClassPlayerPreload");
    g_EvOnClassPlayerLoad =         EventMgr_CreateEvent("Event_OnClassPlayerLoad");
    g_EvOnClassApply =              EventMgr_CreateEvent("Event_OnClassApply");
    g_EvOnClassPlayerInfect =       EventMgr_CreateEvent("Event_OnClassPlayerInfect");
    g_EvOnClassPlayerHuman =        EventMgr_CreateEvent("Event_OnClassPlayerHuman");
    
    // Prepare attribute register.
    ClassAttribReg_Create();
}

/**
 * Register all events here.
 */
public ClassMgr_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnAllPluginsLoaded",      "ClassMgr_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnPluginEnd",             "ClassMgr_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnAllModulesLoaded",      "ClassMgr_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnModuleEnable",          "ClassMgr_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnMyModuleEnable",        "ClassMgr_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnModuleDisable",         "ClassMgr_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnMyModuleDisable",       "ClassMgr_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnMapStart",              "ClassMgr_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnMapEnd",                "ClassMgr_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnAutoConfigsBuffered",   "ClassMgr_OnAutoCfgBuffered");
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnConfigsExecuted",         "ClassMgr_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnClientPutInServer",     "ClassMgr_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnClientDisconnect",      "ClassMgr_OnClientDisconnect");
    
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnPlayerReady",             "ClassMgr_OnPlayerReady");
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerSpawn",               "ClassMgr_PlayerSpawn");
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerSpawn",               "ClassMgr_PlayerSpawnPost");
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerDeath",               "ClassMgr_PlayerDeath");
    
    #endif
    
    // TODO: Activate these events when the new infection module is imported.
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnPlayerInfected",        "ClassMgr_OnPlayerInfected");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnPlayerHuman",           "ClassMgr_OnPlayerHuman");
}

/**
 * Loads classes from file.
 * Note: Classes must NOT be loaded BEFORE OnConfigsExecuted event!
 */
ClassMgr_LoadClasses()
{
    decl String:configfile[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarClassFile, configfile, sizeof(configfile));
    g_ClassCount = 0;
    
    // Validate path.
    if (ConfigMgr_ValidateFile(configfile))
    {
        ConfigMgr_WriteString(g_moduleClassMgr, CM_CONFIGINDEX_FIRST, ConfigData_Path, CM_DATA_PATH, configfile);
    }
    else
    {
        LogMgr_Print(g_moduleClassMgr, LogType_Fatal_Plugin, "Config Validation", "Error: Invalid config file path in cvar zr_classes_file: \"%s\"", configfile);
        return;
    }
    
    // Log loading-message.
    LogMgr_Print(g_moduleClassMgr, LogType_Normal, "Config Loading", "Loading classes from file \"%s\".", configfile);
    
    // Parse class file.
    g_ClassCount = ConfigMgr_CacheKv(g_moduleClassMgr, CM_CONFIGINDEX_FIRST, "ClassMgr_LoadClass");
    
    // Log loaded-message.
    LogMgr_Print(g_moduleClassMgr, LogType_Normal, "Config Loading", "%d class(es) loaded.", g_ClassCount);
    
    // Check if there are no classes.
    if (g_ClassCount == 0)
    {
        LogMgr_Print(g_moduleClassMgr, LogType_Fatal_Plugin, "Config Validation", "Error: No usable data found in class config file: %s", configfile);
    }
    
    
}

/**
 * Loops through each class in the class config.
 * 
 * @param kv            The keyvalues handle of the config file. (Don't close this)
 * @param sectionindex  The index of the current keyvalue section, starting from 0.
 * @param sectionname   The name of the current keyvalue section.
 * 
 * @return              See enum KvCache.
 */
public KvCache:ClassMgr_LoadClass(Handle:kv, sectionindex, const String:sectionname[])
{
    decl String:attributeName[CLASS_NAME_LEN];
    
    // Check if maximum number of classes is reached.
    if (sectionindex == CLASS_MAX)
    {
        LogMgr_Print(g_moduleClassMgr, LogType_Error, "Config Validation", "Warning: Maximum number of classes reached (%d), ignoring classes from class \"%s\" (%d).", CLASS_MAX, sectionname, sectionindex);
        return KvCache_Hault;
    }
    
    KvGotoFirstSubKey(kv, false);
    do
    {
        // Get attribute name.
        KvGetSectionName(kv, attributeName, sizeof(attributeName));
        
        // Go back to the attribute again so it can read the value.
        KvGoBack(kv);
        
        // Get the module responsible for this attribute.
        new Module:attributeModule = ClassAttribReg_GetAttrib(attributeName);
        
        // Validate module.
        if (attributeModule != INVALID_MODULE)
        {
            // Prepare event data.
            static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_String, DataType_String};
            decl any:eventdata[sizeof(eventdatatypes)][CLASS_NAME_LEN];
            eventdata[0][0] = sectionindex;
            eventdata[1][0] = kv;
            strcopy(eventdata[2][0], CLASS_NAME_LEN, attributeName);
            strcopy(eventdata[3][0], CLASS_NAME_LEN, sectionname);
            new Module:eventFilter[2];
            eventFilter[0] = attributeModule;
            eventFilter[1] = INVALID_MODULE;
            
            // Send the load event to the attribute module only.
            EventMgr_Forward(g_EvOnClassAttribLoad, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes, eventFilter);
        }
        else
        {
            // Attribute wasn't registered. Just a debug print and should never happen
            // if all modules are available.
            PrintToServer("Class attribute not registered: %s", attributeName);
        }
        
        // Jump back to the attribute again so the parser will find the next attribute.
        KvJumpToKey(kv, attributeName);
    } while (KvGotoNextKey(kv, false));
    
    // Go one level up (to the class section level).
    KvGoBack(kv);
    
    return KvCache_Continue;
}

/**
 * Plugin is loading.
 */
ClassMgr_OnPluginStart()
{
    // Register the module.
    ClassMgr_Register();
    
    // Create cvars.
    g_hCvarClassFile =              Project_CreateConVar("classes_file",                    "configs/zr/classes.txt",       "Path to class config file.  Path is relative to the SourceMod directory.");
    //g_hCvarClassesMenuSpawn =       Project_CreateConVar("classes_menu_spawn",              "0",                "Re-display class selection menu every spawn.");
    //g_hCvarClassesMenuJoin =        Project_CreateConVar("classes_menu_join",               "0",                "Display class selection menu when a player spawn for the first time.");
    //g_hCvarClassesRandom =          Project_CreateConVar("classes_random",                  "0",                "Player is assigned a random class every spawn. [Override: <prefix>_classes_default_*]");
    //g_hCvarClassesChangeTimelimit = Project_CreateConVar("classes_change_timelimit",        "20",               "Time limit to change human class with instant change after spawning. Time is in seconds. Use 0 or negative to disable.");
    //g_hCvarClassesSave =            Project_CreateConVar("classes_save",                    "1",                "Save players' class selections in server cookies and restore when connecting. [Override: zr_classes_default_*]");
    //g_hCvarClassesDefaultZombie =   Project_CreateConVar("classes_default_zombie",          "random",           "Zombie class assigned to players on connect. [\"random\" = Random zombie class | \"\" = Class config default]");
    //g_hCvarClassesDefaultMZombie =  Project_CreateConVar("classes_default_mother_zombie",   "motherzombies",    "Zombie class assigned to mother zombies. [\"motherzombies\" = Random mother zombie class | \"random\" = Random regular zombie class | \"disabled\" = Don't change class on mother zombies]");
    //g_hCvarClassesDefaultHuman =    Project_CreateConVar("classes_default_human",           "random",           "Human class assigned to players on connect. [\"random\" = Random human class | \"\" = Class config default]");
    //g_hCvarClassesDefaultAdmin =    Project_CreateConVar("classes_default_admin",           "default",          "(Not implemented!) Admin-only class assigned to admins on connect, if any. [\"default\" = Default human class | \"random\" = Random admin-only class]");
    //g_hCvarClassesZombieSelect =    Project_CreateConVar("classes_zombie_select",           "1",                "Allow players to select zombie classes.");
    //g_hCvarClassesHumanSelect =     Project_CreateConVar("classes_human_select",            "1",                "Allow players to select human classes.");
    
    // Forward plugin start event to attribute modules.
    ClsGeneric_OnPluginStart();
    ClsModel_OnPluginStart();
}


/****************
 *    EVENTS    *
 ****************/

/**
 * All convars are set, cvar-dependent code should use this.
 */
public ClassMgr_OnConfigsExecuted()
{
    // Load classes from file.
    ClassMgr_LoadClasses();
    
    new any:eventdata[1][1];
    new Action:result;
    
    // Loop through all classes and send validation event. Read final result from event manager.
    for (new classIndex = 0; classIndex < g_ClassCount; classIndex++)
    {
        eventdata[0][0] = classIndex;
        result = EventMgr_Forward(g_EvOnClassValidate, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
        
        // Check for validation errors.
        if (result == Plugin_Handled)
        {
            // TODO: Tell generic attribute module to disable class.
        }
    }
    
    // Validate class minimum reqirements.
    if (!ClsGeneric_ValidateRequirements())
    {
        LogMgr_Print(g_moduleClassMgr, LogType_Fatal_Plugin, "Config Validation", "Error: The class configuration doesn't match the team requirements.");
    }
    
    // Send all classes loaded event (so modules can copy their cache).
    eventdata[0][0] = g_ClassCount;
    result = EventMgr_Forward(g_EvOnClassAllLoaded, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * All plugins have loaded.
 */
public ClassMgr_OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public ClassMgr_OnPluginEnd()
{
}


/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public ClassMgr_OnEventsReady()
{
}

/**
 * All modules have been registered.
 */
public ClassMgr_OnAllModulesLoaded()
{
}

/**
 * A module has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:ClassMgr_OnModuleEnable(Module:module)
{
}

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:ClassMgr_OnMyModuleEnable()
{
}

/**
 * A module has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:ClassMgr_OnModuleDisable(Module:module)
{
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:ClassMgr_OnMyModuleDisable()
{
    // TODO: Disable classes.
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public ClassMgr_OnConfigReload(configindex)
{
    // TODO: Send clear event to attributes.
    
    // Forward event.
    // ClassMgr_OnConfigsExecuted();
    
    // TODO: Validate class selection indexes. If team mismach, use config default.
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public ClassMgr_OnClientPutInServer(client)
{
    // Reset spawned flag.
    g_ClassPlayerSpawned[client] = false;
    
    // Reset instant class change.
    ClassMgr_ResetInstantChange(client);
    
    // Reset next-indexes.
    ClassMgr_ResetNextIndexes(client);
    
    // Find clean class to preload.
    // Send event OnClassPlayerPreload.
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public ClassMgr_OnClientDisconnect(client)
{
    // Reset instant class change.
    ClassMgr_ResetInstantChange(client);
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public ClassMgr_PlayerSpawn(client)
{
    // Stop if classes aren't loaded.
    
    // Stop if not on a team.
    
    // Restore next-indexes.
    ClassMgr_ResetNextIndexes(client);
    
    // Get random class if enabled in config. Always get random class for bots.
    
    // Display class menu configs according to config.
    
    // Send OnClassPlayerLoad event with active class index.
    
    // Setup instant class change if enabled.
    ClassMgr_CheckInstantChange(client);
}

/**
 * Client has spawned (delayed event).
 * 
 * @param client    The client index.
 */
public ClassMgr_PlayerSpawnPost(client)
{
    // Stop if classes aren't loaded.
    
    // Stop if not on a team.
    
    // Send OnClassApply event.
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public ClassMgr_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    // Reset instant class change.
    ClassMgr_ResetInstantChange(victim);
}

/**
 * Client is connected, admin checked and cookies are loaded.
 *
 * @param client    Client index.
 */
public ClassMgr_OnPlayerReady(client)
{
    // Set class selection indexes if not already modified.
    
    // Tell player.
    // TranslationPrintToChat(client, "Classes Selection Loaded");
}

/**
 * Client was infected.
 *
 * @param client            Client index.
 * @param attacker          Attacker index, if any.
 * @param motherZombie      Infected as mother zombie.
 */
public ClassMgr_OnClientInfect(client, attacker, bool:motherZombie)
{
    // Reset instant class change.
    ClassMgr_ResetInstantChange(client);
    
    // Get active index.
    
    // If mother zombie, get class according to config.
    
    // Set slected zombie class to active index.
    
    // Restore next-indexes.
    ClassMgr_ResetNextIndexes(client);
    
    // Send OnClassApply event.
    
    // Send OnClassPlayerInfect event.
    
}

/**
 * Client was turned back into a human.
 *
 * @param client            Client index.
 */
public ClassMgr_OnClientHuman(client)
{
    
}

/******************************
 *    INTERNAL TOOLS/LOGIC    *
 ******************************/




/*********************************
 *    CLASS MANAGER UTILITIES    *
 *********************************/

/**
 * Returns whether the specified class valid.
 *
 * @param classIndex    The class index to validate.
 *
 * @return              True if the class exist, false otherwise.
 */
stock bool:ClassMgr_IsValidIndex(classIndex)
{
    if (classIndex >= 0 && classIndex < g_ClassCount)
    {
        return true;
    }
    return false;
}

/**
 * Compares the specified team with a class' team.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param team      The team to compare with the class.
 * @param cache     Optional. Specifies what class cache to read from. If player
 *                  cache is used 'index' will be used as a client index.
 *                  Default is modified cache.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:ClassMgr_TeamEqual(index, ClassTeam:team, ClassCacheType:cache = ClassCache_Modified)
{
    return ClsGeneric_GetTeam(index, cache) == team;
}

/**
 * Gets the currently active class index that the player is using.
 *
 * @param client    The client index.
 *
 * @return          The active class index. -1 if a spectactor or not alive.
 */
stock ClassMgr_GetActiveIndex(client)
{
    // Check if alive.
    if (!IsPlayerAlive(client))
    {
        return -1;
    }
    
    // Check if on a valid team.
    if (!ZRIsClientOnTeam(client))
    {
        return -1;
    }
    
    // Check if player is human or zombie.
    new ClassTeam:team;
    if (InfectIsClientHuman(client))
    {
        team = ClassTeamNew_Humans;
    }
    else
    {
        team = ClassTeamNew_Zombies;
    }
    
    // Return the active class for the current team.
    return g_ClassSelected[client][team];
}

/**
 * Decides whether a class selection menu should be enabled. The decision is
 * based on zr_class_allow_* console variables.
 *
 * @param team      Optional. Team to match. Default is all.
 *
 * @return          True if allowed, false otherwise.
 */
stock bool:ClassMgr_AllowSelection(client, ClassTeam:team = ClassTeamNew_All)
{
    // Get selection settings.
    new bool:zombie = GetConVarBool(g_hCvarClassesZombieSelect);
    new bool:human = GetConVarBool(g_hCvarClassesHumanSelect);
    
    // Check if a team id is specified.
    if (team != ClassTeamNew_All)
    {
        // Check team and return the corresponding selection setting.
        switch (team)
        {
            case ClassTeamNew_Zombies:
            {
                return zombie;
            }
            case ClassTeamNew_Humans:
            {
                return human;
            }
        }
        
        // Team ID didn't match.
        return false;
    }
    else
    {
        // Check zombie and human.
        return zombie || human;
    }
}

/**
 * Logs a invalid attribute warning, for string attributes.
 *
 * @param module        Module responsible for the attribute.
 * @param attribute     Name of attribute that failed validation.
 * @param className     Name of the class.
 * @param classIndex    Class index.
 * @param value         Attribute value (string).
 */
stock ClassMgr_LogAttribErrString(Module:module, const String:attribute[], const String:className[], classIndex, const String:value[])
{
    LogMgr_Print(module, LogType_Error, "Config Validation", "Warning: Invalid \"%s\" in class \"%s\" (%d): \"%s\"", attribute, className, classIndex, value);
}

/**
 * Logs a invalid attribute warning, for cell attributes.
 *
 * @param module        Module responsible for the attribute.
 * @param attribute     Name of attribute that failed validation.
 * @param className     Name of the class.
 * @param classIndex    Class index.
 * @param value         Attribute value (cell).
 */
stock ClassMgr_LogAttribErrCell(Module:module, const String:attribute[], const String:className[], classIndex, value)
{
    LogMgr_Print(module, LogType_Error, "Config Validation", "Warning: Invalid \"%s\" in class \"%s\" (%d): %d", attribute, className, classIndex, value);
}

/**
 * Logs a invalid attribute warning, for float attributes.
 *
 * @param module        Module responsible for the attribute.
 * @param attribute     Name of attribute that failed validation.
 * @param className     Name of the class.
 * @param classIndex    Class index.
 * @param value         Attribute value (float).
 */
stock ClassMgr_LogAttribErrFloat(Module:module, const String:attribute[], const String:className[], classIndex, Float:value)
{
    LogMgr_Print(module, LogType_Error, "Config Validation", "Warning: Invalid \"%s\" in class \"%s\" (%d): %0.2f", attribute, className, classIndex, value);
}


/******************************
 *    CONVERSION FUNCTIONS    *
 ******************************/

/**
 * Converts the specified string to a class team.
 *
 * @param team      Team string to convert.
 *
 * @return          Team if successful, otherwise ClassTeam_Invalid.
 */
ClassTeam:ClassMgr_StringToTeam(const String:team[])
{
    if (StrEqual(team, "zombies", false))
    {
        return ClassTeamNew_Zombies;
    }
    else if (StrEqual(team, "humans", false))
    {
        return ClassTeamNew_Humans;
    }
    else if (StrEqual(team, "all", false))
    {
        return ClassTeamNew_All;
    }
    
    return ClassTeamNew_Invalid;
}

/**
 * Converts the specified team to a string.
 *
 * @param team      Team to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
ClassMgr_TeamToString(ClassTeam:team, String:buffer[], maxlen)
{
    switch (team)
    {
        case ClassTeamNew_Zombies: return strcopy(buffer, maxlen, "zombies");
        case ClassTeamNew_Humans: return strcopy(buffer, maxlen, "humans");
        case ClassTeamNew_All: return strcopy(buffer, maxlen, "all");
        case ClassTeamNew_Invalid: return strcopy(buffer, maxlen, "INVALID_TEAM");
    }
    
    return 0;
}
