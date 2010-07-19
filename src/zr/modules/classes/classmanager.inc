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
 * @section Event ID variables and documentation.
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
 * @param classIndex    Class index.
 * Action:OnClassAllLoaded(classIndex)
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
 * OnClassPlayerInfected(client, attacker, bool:motherZombie)
 */
new ProjectEvent:g_EvOnClassPlayerInfected;

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
new Handle:g_hCvarClassesMenuSpawn;
new Handle:g_hCvarClassesMenuJoin;
new Handle:g_hCvarClassesRandom;
new Handle:g_hCvarClassesChangeTimelimit;
new Handle:g_hCvarClassesSave;
new Handle:g_hCvarClassesDefaultZombie;
new Handle:g_hCvarClassesDefaultMZombie;
new Handle:g_hCvarClassesDefaultHuman;
new Handle:g_hCvarClassesDefaultAdmin;
new Handle:g_hCvarClassesZombieSelect;
new Handle:g_hCvarClassesHumanSelect;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ClassMgr_GetIdentifier() { return g_moduleClassMgr; }

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


// Utility includes.
#include "zr/modules/classes/attributeregister"

// Attribute includes
#include "zr/modules/classes/genericattributes"


/**
 * Register this module.
 */
ClassMgr_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "Class Manager");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "classmanager");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Manages player classes.");
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
    g_EvOnClassPlayerInfected =     EventMgr_CreateEvent("Event_OnClassPlayerInfected");
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
    EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnConfigsExecuted",       "ClassMgr_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnClientPutInServer",     "ClassMgr_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_OnClientDisconnect",      "ClassMgr_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_RoundStart",              "ClassMgr_RoundStart");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_RoundFreezeEnd",          "ClassMgr_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_RoundEnd",                "ClassMgr_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerTeam",              "ClassMgr_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerSpawn",             "ClassMgr_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerHurt",              "ClassMgr_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerDeath",             "ClassMgr_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_PlayerJump",              "ClassMgr_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleClassMgr, "Event_WeaponFire",              "ClassMgr_WeaponFire");
    
    #endif
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
            // Attribute wasn't registered.
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
}


/****************
 *    EVENTS    *
 ****************/

/**
 * All convars are set, cvar-dependent code should use this.
 */
public ClassMgr_OnConfigsExecuted()
{
    ClassMgr_LoadClasses();
    
    // Loop through all classes and send validation event. Read final result from event manager.
    for (new classIndex = 0; classIndex < g_ClassCount; classIndex++)
    {
        new any:eventdata[1][1];
        eventdata[0][0] = classIndex;
        
        new Action:result = EventMgr_Forward(g_EvOnClassValidate, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
        
        // Check for validation errors.
        if (result == Plugin_Handled)
        {
            // TODO: Tell generic attribute module to disable class.
        }
    }
    
    // TODO: Validate class minimum reqirements.
    
    // TODO: Send all classes loaded event (so modules can copy their cache).
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
 * The map has started.
 */
public ClassMgr_OnMapStart()
{
}

/**
 * The map has ended.
 */
public ClassMgr_OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public ClassMgr_OnAutoCfgBuffered()
{
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
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public ClassMgr_OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public ClassMgr_RoundStart()
{
}

/**
 * Pre-round freezetime has finished.
 */
public ClassMgr_RoundFreezeEnd()
{
}

/**
 * Round has ended.
 */
public ClassMgr_RoundEnd()
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
public ClassMgr_PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public ClassMgr_PlayerSpawn(client)
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
public ClassMgr_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
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
public ClassMgr_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public ClassMgr_PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public ClassMgr_WeaponFire(client, const String:weapon[])
{
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
