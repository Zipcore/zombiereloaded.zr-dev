/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          genericattributes.inc
 *  Type:          Class attribue module
 *  Description:   Stores the generic class attributes. This is a required module, without it classes won't work.
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
new Module:g_moduleClsGeneric;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ClsGeneric_GetIdentifier() { return g_moduleClsGeneric; }

/**
 * Data structure for generic class data (the class manager's own attributes).
 */
enum ClassGenericAttributes
{
    bool:ClassAttrib_Enabled,                       /** Specifies whether the class is enabled or not. */
    ClassTeam:ClassAttrib_Team,                     /** Which team the class belongs to. */
    bool:ClassAttrib_TeamDefault,                   /** Specifies whether it's the default class in its team. */
    bool:ClassAttrib_IsAdminClass,                  /** Specifies whether the class is a admins-only class or not. */
    bool:ClassAttrib_IsMotherZombie,                /** Specifies whether the class is mother zombie class or not (only valid in zombie team). */
    String:ClassAttrib_Group[CLASS_NAME_LEN],       /** Restricts the class to only players in the specified SourceMod group. */
    String:ClassAttrib_Name[CLASS_NAME_LEN],        /** Unique name used to identify the class (section name in keyvalue file). */
    
    String:ClassAttrib_DisplayName[CLASS_NAME_LEN], /** Name displayed in the class menu. */
    String:ClassAttrib_Description[CLASS_NAME_LEN]  /** Description displayed in the class menu. */
}

/**
 * Class data cache. This one applies to both original and modified cache
 * because generic attributes are read-only anyways.
 */
new ClassGenericData[CLASS_MAX][ClassGenericAttributes];

/**
 * Buffer to store whether the class name have been saved.
 */
new bool:ClassGotName[CLASS_MAX];

/**
 * Player class data cache.
 */
new ClassPlayerData[MAXPLAYERS + 1][ClassGenericAttributes];


/**
 * Register this module.
 */
ClsGeneric_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "Generic class attributes");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "genericattributes");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Stores generic class attributes. This is a required module, without it classes won't work.");
    moduledata[ModuleData_Dependencies][0] = ClassMgr_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleClsGeneric = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnEventsRegister",      "ClsGeneric_OnEventsRegister");
    
    // Add attributes to the attribute register.
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "enabled");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "team");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "team_default");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "is_admin_class");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "is_mother_zombie");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "group");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "display_name");
    ClassAttribReg_AddAttrib(g_moduleClsGeneric, "description");
    
    // Add public API functions to function register.
    new Handle:pluginHandle = GetMyHandle();
    FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsGeneric_ValidateRequirements"), "Class_ValidateRequirements");
    FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsGeneric_IsValidTeam"), "Class_IsValidTeam");
    FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsGeneric_IsValidGroup"), "Class_IsValidGroup");
    FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsGeneric_IsValidDisplayName"), "Class_IsValidDisplayName");
    FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsGeneric_IsValidDescription"), "Class_IsValidDescription");
}

/**
 * Plugin is loading.
 */
ClsGeneric_OnPluginStart()
{
    // Register the module.
    ClsGeneric_Register();
}

/**
 * Register all events here.
 */
public ClsGeneric_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnAllPluginsLoaded",      "ClsGeneric_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnPluginEnd",             "ClsGeneric_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnAllModulesLoaded",      "ClsGeneric_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnModuleEnable",          "ClsGeneric_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnMyModuleEnable",        "ClsGeneric_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnModuleDisable",         "ClsGeneric_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnMyModuleDisable",       "ClsGeneric_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnMapStart",              "ClsGeneric_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnMapEnd",                "ClsGeneric_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnAutoConfigsBuffered",   "ClsGeneric_OnAutoCfgsBuffered");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnConfigsExecuted",       "ClsGeneric_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClientPutInServer",     "ClsGeneric_ClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClientDisconnect",      "ClsGeneric_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_RoundStart",              "ClsGeneric_RoundStart");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_RoundFreezeEnd",          "ClsGeneric_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_RoundEnd",                "ClsGeneric_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_PlayerTeam",              "ClsGeneric_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_PlayerSpawn",             "ClsGeneric_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_PlayerHurt",              "ClsGeneric_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_PlayerDeath",             "ClsGeneric_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_PlayerJump",              "ClsGeneric_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_WeaponFire",              "ClsGeneric_WeaponFire");
    
    #endif
    
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassAttribLoad",           "ClsGeneric_OnClassAttribLoad");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassValidate",             "ClsGeneric_OnClassValidate");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassAllLoaded",          "ClsGeneric_OnClassAllLoaded");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerPreload",        "ClsGeneric_OnClassPlayerPreload");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassApply",              "ClsGeneric_OnClassApply");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerInfected",     "ClsGeneric_OnClassPlayerInfected");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerHuman",        "ClsGeneric_OnClassPlayerHuman");
}

/**
 * A class attribute is loading and ready to be cached.
 *
 * @param classIndex    Class index.
 * @param kv            Handle to keyvalue tree, ready to read attribute value.
 * @param attribute     Name of the current attribute.
 * @param className     Name of the current class (section name in keyvalue tree).
 */
public ClsGeneric_OnClassAttribLoad(classIndex, Handle:kv, const String:attribute[], const String:className[])
{
    // Save class name if not saved yet.
    if (!ClassGotName[classIndex])
    {
        strcopy(ClassGenericData[classIndex][ClassAttrib_Name], CLASS_NAME_LEN, className);
        ClassGotName[classIndex] = true;
    }
    
    decl String:buffer[CLASS_STRING_LEN];
    buffer[0] = 0;
    
    if (StrEqual(attribute, "enabled", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        ClassGenericData[classIndex][ClassAttrib_Enabled] = ConfigSettingToBool(buffer);
    }
    else if (StrEqual(attribute, "team", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        ClassGenericData[classIndex][ClassAttrib_Team] = ClassMgr_StringToTeam(buffer);
    }
    else if (StrEqual(attribute, "team_default", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        ClassGenericData[classIndex][ClassAttrib_TeamDefault] = ConfigSettingToBool(buffer);
    }
    else if (StrEqual(attribute, "is_admin_class", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        ClassGenericData[classIndex][ClassAttrib_IsAdminClass] = ConfigSettingToBool(buffer);
    }
    else if (StrEqual(attribute, "is_mother_zombie", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        ClassGenericData[classIndex][ClassAttrib_IsMotherZombie] = ConfigSettingToBool(buffer);
    }
    else if (StrEqual(attribute, "group", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        strcopy(ClassGenericData[classIndex][ClassAttrib_Group], CLASS_NAME_LEN, buffer);
    }
    else if (StrEqual(attribute, "display_name", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        strcopy(ClassGenericData[classIndex][ClassAttrib_DisplayName], CLASS_NAME_LEN, buffer);
    }
    else if (StrEqual(attribute, "description", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        strcopy(ClassGenericData[classIndex][ClassAttrib_Description], CLASS_NAME_LEN, buffer);
    }
}

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
 */
public Action:ClsGeneric_OnClassValidate(classIndex)
{
    new bool:hasErrors = false;
    
    // Cache attributes.
    new classCache[ClassGenericAttributes];
    classCache = ClassGenericData[classIndex];
    
    decl String:buffer[CLASS_STRING_LEN];
    buffer[0] = 0;
    
    // Get class name.
    decl String:className[CLASS_NAME_LEN];
    strcopy(className, sizeof(className), classCache[ClassAttrib_Name]);
    
    if (!ClsGeneric_IsValidTeam(classCache[ClassAttrib_Team]))
    {
        ClassMgr_TeamToString(classCache[ClassAttrib_Team], buffer, sizeof(buffer));
        LogMgr_Print(g_moduleClsGeneric, LogType_Error, "Config Validation", "Warning: Invalid team in class \"%s\" (%d): \"%s\"", className, classIndex, buffer);
        hasErrors = true;
    }
    if (!ClsGeneric_IsValidGroup(classCache[ClassAttrib_Group]))
    {
        LogMgr_Print(g_moduleClsGeneric, LogType_Error, "Config Validation", "Warning: Invalid group in class \"%s\" (%d): \"%s\"", className, classIndex, classCache[ClassAttrib_Group]);
        hasErrors = true;
    }
    if (!ClsGeneric_IsValidDisplayName(classCache[ClassAttrib_DisplayName]))
    {
        LogMgr_Print(g_moduleClsGeneric, LogType_Error, "Config Validation", "Warning: Invalid display name in class \"%s\" (%d): \"%s\"", className, classIndex, classCache[ClassAttrib_DisplayName]);
        hasErrors = true;
    }
    if (!ClsGeneric_IsValidDescription(classCache[ClassAttrib_Description]))
    {
        LogMgr_Print(g_moduleClsGeneric, LogType_Error, "Config Validation", "Warning: Invalid description in class \"%s\" (%d): \"%s\"", className, classIndex, classCache[ClassAttrib_DisplayName]);
        hasErrors = true;
    }
    
    if (hasErrors)
    {
        return Plugin_Handled;
    }
    
    return Plugin_Continue;
}

/**
 * All classes are loaded now. Attribute modules should now make a copy of their array
 * so the original values can be kept.
 *
 * @param classIndex    Class index.
 */
public ClsGeneric_OnClassAllLoaded(classIndex)
{
}

/**
 * Preloads player info before player preferences are loaded. The class manger
 * sets initial selected class indexes. Attribute modules may initialize players too.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 */
public OnClassPlayerPreload(client, classIndex)
{
}

/**
 * Loads player info with player preferences (from cookies). The class manger
 * sets new selected class indexes according to player preferences. Attribute modules
 * may initialize players with their preferences too.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 */
public OnClassPlayerLoad(client, classIndex)
{
}

/**
 * Class attributes are applied. Attribute modules should apply their own attributes
 * on the player now.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 */
public OnClassApply(client, classIndex)
{
}

/**
 * A player was infected. This event is fired AFTER class attributes are applied.
 * Attribute modules should use this event and not the infection module's event when
 * initializing zombie specific features.
 *
 * @param client        Client index.
 * @param attacker      Attacker client index.
 * @param motherZombie  Specifies whether the attacker was a mother zombie.
 */
public OnClassPlayerInfected(client, attacker, bool:motherZombie)
{
}

/**
 * A player was turned back into a human. This event is fired AFTER class attributes
 * are applied. Attribute modules should use this event and not the infection module's
 * event when initializing human specific features.
 *
 * @param client        Client index.
 */
public OnClassPlayerHuman(client)
{
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public ClsGeneric_OnEventsReady()
{
}

/**
 * All modules have been registered.
 */
public ClsGeneric_OnAllModulesLoaded()
{
}

/**
 * A module has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:ClsGeneric_OnModuleEnable(Module:module)
{
}

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:ClsGeneric_OnMyModuleEnable()
{
}

/**
 * A module has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:ClsGeneric_OnModuleDisable(Module:module)
{
}

/**
 * The module that hooked this event callback has been disabled.
 * 
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:ClsGeneric_OnMyModuleDisable()
{
}

/**
 * The map has started.
 */
public ClsGeneric_OnMapStart()
{
}

/**
 * The map has ended.
 */
public ClsGeneric_OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public ClsGeneric_OnAutoCfgsBuffered()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public ClsGeneric_OnConfigsExecuted()
{
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public ClsGeneric_OnConfigReload(configindex)
{
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public ClsGeneric_ClientPutInServer(client)
{
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public ClsGeneric_OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public ClsGeneric_RoundStart()
{
}

/**
 * Pre-round freezetime has finished.
 */
public ClsGeneric_RoundFreezeEnd()
{
}

/**
 * Round has ended.
 */
public ClsGeneric_RoundEnd()
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
public ClsGeneric_PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public ClsGeneric_PlayerSpawn(client)
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
public ClsGeneric_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
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
public ClsGeneric_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public ClsGeneric_PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public ClsGeneric_WeaponFire(client, const String:weapon[])
{
}


/******************************
 *    VALIDATION FUNCTIONS    *
 ******************************/

/**
 * Validates the minimum class requirements.
 * Note: This function will log errors, but not stop the plugin.
 *
 * @return      True if valid, false otherwise.
 */
bool:ClsGeneric_ValidateRequirements()
{
    // Setup filter
    
    // Find clean zombie class.
    
    // Find clean human class.
    
    return true;
}

/**
 * Validates team.
 *
 * @param team          Team to validate.
 *
 * @return              True if valid, false otherwise.
 */
bool:ClsGeneric_IsValidTeam(ClassTeam:team)
{
    if (team == ClassTeamNew_Zombies ||
        team == ClassTeamNew_Humans)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates group.
 *
 * @param group         Group name to validate.
 *
 * @return              True if valid, false otherwise.
 */
bool:ClsGeneric_IsValidGroup(const String:group[])
{
    // No group is also valid.
    if (strlen(group) == 0)
    {
        return true;
    }
    
    // Check if the group exist in SourceMod's admin cache.
    if (FindAdmGroup(group) != INVALID_GROUP_ID)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates display name.
 *
 * @param displayName   Name to validate.
 *
 * @return              True if valid, false otherwise.
 */
bool:ClsGeneric_IsValidDisplayName(const String:displayName[])
{
    if (strlen(displayName) > 0)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates description.
 *
 * @param description   Description to validate.
 *
 * @return              True if valid, false otherwise.
 */
bool:ClsGeneric_IsValidDescription(const String:description[])
{
    if (strlen(description) > 0)
    {
        return true;
    }
    
    return false;
}
