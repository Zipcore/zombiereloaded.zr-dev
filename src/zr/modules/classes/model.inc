/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          model.inc
 *  Type:          Class attribue module
 *  Description:   Stores model attributes.
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
new Module:g_moduleClsModel;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ClsModel_GetIdentifier() { return g_moduleClsModel; }

/**
 * Data structure for model attributes.
 */
enum ClassModelAttributes
{
    String:ClassAttrib_Model[CLASS_NAME_LEN],   /** Name of the model or a predefined model option. */
    ClassAttrib_AlphaInitial,                   /** Initial alpha value. Used when spawning. */
    ClassAttrib_AlphaDamaged,                   /** Alpha value when damaged. */
    ClassAttrib_AlphaDamage                     /** How much damage that must be done before alpha is changed. */
}

/**
 * Original class data cache (read-only after loading is done).
 */
new ClassModelData[CLASS_MAX][ClassModelAttributes];

/**
 * Second class data cache (writable).
 */
new ClassModelData2[CLASS_MAX][ClassModelAttributes];

/**
 * Player class data cache.
 */
new ClassModelPlayerData[MAXPLAYERS + 1][ClassModelAttributes];

/**
 * @section Attribute limit values. Used when validating.
 */
#define CLASS_ALPHA_INITIAL_MIN     0
#define CLASS_ALPHA_INITIAL_MAX     255
#define CLASS_ALPHA_DAMAGED_MIN     0
#define CLASS_ALPHA_DAMAGED_MAX     255
#define CLASS_ALPHA_DAMAGE_MIN      0
#define CLASS_ALPHA_DAMAGE_MAX      30000
/**
 * @endsection
 */

/**
 * Function cache for API functions that this module use.
 */
new Function:ClsModel_GetNameFunc;

/**
 * Register this module.
 */
ClsModel_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Model class attributes");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "model");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Stores model attributes.");
    moduledata[ModuleData_Dependencies][0] = ClassMgr_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleClsModel = ModuleMgr_Register(moduledata);
    
    EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnEventsRegister",      "ClsModel_OnEventsRegister");
    
    // Register attributes in the attribute register.
    ClassAttribReg_AddAttrib(g_moduleClsModel, "model");
    ClassAttribReg_AddAttrib(g_moduleClsModel, "alpha_initial");
    ClassAttribReg_AddAttrib(g_moduleClsModel, "alpha_damaged");
    ClassAttribReg_AddAttrib(g_moduleClsModel, "alpha_damage");
    
    // Add public API function to function register.
    //new Handle:pluginHandle = GetMyHandle();
    //FuncRegister_AddFunction(GetFunctionByName(pluginHandle, "ClsModel_<function>"), "Class_<function alias>");
}

/**
 * Plugin is loading.
 */
ClsModel_OnPluginStart()
{
    // Register the module.
    ClsModel_Register();
}


/**
 * Register all events here.
 */
public ClsModel_OnEventsRegister()
{
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnAllPluginsLoaded",      "ClsModel_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnPluginEnd",             "ClsModel_OnPluginEnd");
    EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnAllModulesLoaded",      "ClsModel_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnModuleEnable",          "ClsModel_OnModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnMyModuleEnable",        "ClsModel_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnModuleDisable",         "ClsModel_OnModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnMyModuleDisable",       "ClsModel_OnMyModuleDisable");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnMapStart",              "ClsModel_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnMapEnd",                "ClsModel_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnAutoConfigsBuffered",   "ClsModel_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnConfigsExecuted",       "ClsModel_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnClientPutInServer",     "ClsModel_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_OnClientDisconnect",      "ClsModel_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_RoundStart",              "ClsModel_RoundStart");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_RoundFreezeEnd",          "ClsModel_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_RoundEnd",                "ClsModel_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_PlayerTeam",              "ClsModel_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_PlayerSpawn",             "ClsModel_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_PlayerHurt",              "ClsModel_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_PlayerDeath",             "ClsModel_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_PlayerJump",              "ClsModel_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleClsModel, "Event_WeaponFire",              "ClsModel_WeaponFire");
    
    #endif
    
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassAttribLoad",           "ClsModel_OnClassAttribLoad");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassValidate",             "ClsModel_OnClassValidate");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassAllLoaded",            "ClsModel_OnClassAllLoaded");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerLoad",           "ClsModel_OnClassPlayerLoad");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerPreload",      "ClsModel_OnClassPlayerPreload");
    EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassApply",                "ClsModel_OnClassApply");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerInfected",     "ClsModel_OnClassPlayerInfected");
    //EventMgr_RegisterEvent(g_moduleClsGeneric, "Event_OnClassPlayerHuman",        "ClsModel_OnClassPlayerHuman");
}

/**
 * A class attribute is loading and ready to be cached.
 *
 * @param classIndex    Class index.
 * @param kv            Handle to keyvalue tree, ready to read attribute value.
 * @param attribute     Name of the current attribute.
 * @param className     Name of the current class (section name in keyvalue tree).
 */
public ClsModel_OnClassAttribLoad(classIndex, Handle:kv, const String:attribute[], const String:className[])
{
    decl String:buffer[CLASS_STRING_LEN];
    buffer[0] = 0;
    
    if (StrEqual(attribute, "model", false))
    {
        KvGetString(kv, attribute, buffer, sizeof(buffer));
        strcopy(ClassModelData[classIndex][ClassAttrib_Model], CLASS_NAME_LEN, buffer);
    }
    else if (StrEqual(attribute, "alpha_initial", false))
    {
        ClassModelData[classIndex][ClassAttrib_AlphaInitial] = KvGetNum(kv, attribute);
    }
    else if (StrEqual(attribute, "alpha_damaged", false))
    {
        ClassModelData[classIndex][ClassAttrib_AlphaDamaged] = KvGetNum(kv, attribute);
    }
    else if (StrEqual(attribute, "alpha_damage", false))
    {
        ClassModelData[classIndex][ClassAttrib_AlphaDamage] = KvGetNum(kv, attribute);
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
public Action:ClsModel_OnClassValidate(classIndex)
{
    new bool:hasErrors = false;
    
    // Cache attributes.
    new classCache[ClassModelAttributes];
    classCache = ClassModelData[classIndex];
    
    decl String:buffer[CLASS_STRING_LEN];
    buffer[0] = 0;
    
    // Get class name.
    decl String:className[CLASS_NAME_LEN];
    className[0] = 0;
    Call_StartFunction(GetMyHandle(), ClsModel_GetNameFunc);
    Call_PushCell(classIndex);
    Call_PushStringEx(className, sizeof(className), 0, SM_PARAM_COPYBACK);
    Call_PushCell(sizeof(className));
    Call_Finish();
    
    strcopy(buffer, sizeof(buffer), classCache[ClassAttrib_Model]);
    if (!ClsModel_IsValidModel(buffer))
    {
        ClassMgr_LogAttribErrString(g_moduleClsModel, "model", className, classIndex, buffer);
        hasErrors = true;
    }
    if (!ClsModel_IsValidAlphaInitial(classCache[ClassAttrib_AlphaInitial]))
    {
        ClassMgr_LogAttribErrCell(g_moduleClsModel, "alpha_initial", className, classIndex, classCache[ClassAttrib_AlphaInitial]);
        hasErrors = true;
    }
    if (!ClsModel_IsValidAlphaDamaged(classCache[ClassAttrib_AlphaDamaged]))
    {
        ClassMgr_LogAttribErrCell(g_moduleClsModel, "alpha_damaged", className, classIndex, classCache[ClassAttrib_AlphaDamaged]);
        hasErrors = true;
    }
    if (!ClsModel_IsValidAlphaDamage(classCache[ClassAttrib_AlphaDamage]))
    {
        ClassMgr_LogAttribErrCell(g_moduleClsModel, "alpha_damage", className, classIndex, classCache[ClassAttrib_AlphaDamage]);
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
 * @param classCount    Number of classes loaded.
 */
public ClsModel_OnClassAllLoaded(classCount)
{
    // Check if no classes are loaded.
    if (classCount == 0)
    {
        return;
    }
    
    // Copy class data into second array.
    for (new classIndex = 0; classIndex < classCount; classIndex++)
    {
        strcopy(ClassModelData2[classIndex][ClassAttrib_Model], CLASS_NAME_LEN, ClassModelData[classIndex][ClassAttrib_Model]);
        ClassModelData2[classIndex][ClassAttrib_AlphaInitial] = ClassModelData[classIndex][ClassAttrib_AlphaInitial];
        ClassModelData2[classIndex][ClassAttrib_AlphaDamaged] = ClassModelData[classIndex][ClassAttrib_AlphaDamaged];
        ClassModelData2[classIndex][ClassAttrib_AlphaDamage] = ClassModelData[classIndex][ClassAttrib_AlphaDamage];
    }
}

/**
 * Preloads player info before player preferences are loaded. The class manger
 * sets initial selected class indexes. Attribute modules may initialize players too.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 */
public ClsModel_OnClassPlayerPreload(client, classIndex)
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
public ClsModel_OnClassPlayerLoad(client, classIndex)
{
    // TODO: Load attributes from the specified class into the player's cache.
}

/**
 * Class attributes are applied. Attribute modules should apply their own attributes
 * on the player now.
 *
 * @param client        Client index.
 * @param classIndex    Class index.
 */
public ClsModel_OnClassApply(client, classIndex)
{
    // TODO: Apply model and alpha.
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public ClsModel_OnEventsReady()
{
}

/**
 * All modules have been registered.
 */
public ClsModel_OnAllModulesLoaded()
{
    // Chache api function IDs that this module use.
    ClsModel_GetNameFunc = FuncRegister_GetFunction("Class_GetName");
}

/**
 * The module that hooked this event callback has been enabled.
 */
public ClsModel_OnMyModuleEnable()
{
    // TODO: Set model and alpha on all players, but only if classes are loaded.
}

/**
 * The module that hooked this event callback has been disabled.
 */
public ClsModel_OnMyModuleDisable()
{
    // TODO: Reset model and alpha to CS:S default on all players.
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 */
public ClsModel_PlayerSpawn(client)
{
    // TODO: Cache original model
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
public ClsModel_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    // TODO: Update alpha.
}


/******************************
 *    VALIDATION FUNCTIONS    *
 ******************************/

/**
 * Validates model.
 *
 * @param model         Model to validate.
 *
 * @return              True if valid, false otherwise.
 */
public bool:ClsModel_IsValidModel(const String:model[])
{
    if (strlen(model) > 0)
    {
        // Check for predefined options.
        if (StrEqual(model, "random", false) ||
            StrEqual(model, "random_public", false) ||
            StrEqual(model, "random_hidden", false) ||
            StrEqual(model, "random_admin", false) ||
            StrEqual(model, "random_mother_zombie", false) ||
            StrEqual(model, "default", false) ||
            StrEqual(model, "no_change", false))
        {
            return true;
        }
    }
    
    return false;
}

/**
 * Validates alpha initial value.
 *
 * @pram alphaInitial   Alpha initial value to validate.
 *
 * @return              True if valid, false otherwise.
 */
public bool:ClsModel_IsValidAlphaInitial(alphaInitial)
{
    if (alphaInitial >= CLASS_ALPHA_INITIAL_MIN && alphaInitial <= CLASS_ALPHA_INITIAL_MAX)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates alpha damaged value.
 *
 * @pram alphaDamaged   Alpha damaged value to validate.
 *
 * @return              True if valid, false otherwise.
 */
public bool:ClsModel_IsValidAlphaDamaged(alphaDamaged)
{
    if (alphaDamaged >= CLASS_ALPHA_DAMAGED_MIN && alphaDamaged <= CLASS_ALPHA_DAMAGED_MAX)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates alpha damage value.
 *
 * @pram alphaDamage    Alpha damage value to validate.
 *
 * @return              True if valid, false otherwise.
 */
public bool:ClsModel_IsValidAlphaDamage(alphaDamage)
{
    if (alphaDamage >= CLASS_ALPHA_DAMAGE_MIN && alphaDamage <= CLASS_ALPHA_DAMAGE_MAX)
    {
        return true;
    }
    
    return false;
}
