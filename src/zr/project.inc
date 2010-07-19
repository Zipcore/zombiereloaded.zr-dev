/*
 * ============================================================================
 *
 *  Zombie Reloaded
 *
 *  File:          project.inc
 *  Type:          Base
 *  Description:   Contains defines, enums, etc available to anywhere in the project.
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

/*
 * Base version: 68:1771c3b5b623
 */

/**
 * Add defines for supported games here.
 * Uncomment, or add a define for the game you are compiling for here.
 */
#define PROJECT_GAME_CSS
//#define PROJECT_GAME_DOD
//#define PROJECT_GAME_HL2DM
//#define PROJECT_GAME_TF2
//#define PROJECT_GAME_L4D
//#define PROJECT_GAME_L4D2

/**
 * This is where it all begins.
 * Forward the OnPluginStart event here to all modules so they can register.
 */
stock ForwardOnPluginStart()
{
    // Forward event to non-modules.
    
    
    // Forward the OnPluginStart event to all modules here.
    BaseAdapter_OnPluginStart();
    MapConfig_OnPluginStart();
    ZTele_OnPluginStart();
    FuncRegister_OnPluginStart();
    ClassMgr_OnPluginStart();
}

// **********************************************
//                   Reference
// **********************************************

/**
 * The client index of the server.
 */
#define SERVER_INDEX 0


// **********************************************
//                Module Manager
// **********************************************

/**
 * The amount of registered modules.
 * All integers less than this (and greater than or equal to zero) are valid module identifiers.
 */
#define MODULE_COUNT GetArraySize(ModuleMgr_GetList())

/**
 * Takes either a module ID or module identifier and converts it to its respective opposite.
 */
#define MODULE_TO_ID(%1) _:%1 + 1
#define ID_TO_MODULE(%1) Module:(%1 - 1)

/**
 * Module identifier tag.
 */
enum Module
{
    INVALID_MODULE = -1     /** Used as an invalid module. */
}

/**
 * Module data flags.
 */
#define MODULE_DATA_DISABLED    (1 << 0)
#define MODULE_DATA_HIDDEN      (1 << 1)
#define MODULE_DATA_FULLNAME    (1 << 2)
#define MODULE_DATA_SHORTNAME   (1 << 3)
#define MODULE_DATA_DESCRIPTION (1 << 4)

/**
 * The max string/array lengths of module data variables.
 */
#define CM_DATA_FULLNAME        32
#define CM_DATA_SHORTNAME       16
#define CM_DATA_DESCRIPTION     256
#define CM_DATA_DEPENDENCIES    8

/**
 * All the data modules need to register.
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,                           /** True if the module is disabled. */
    bool:ModuleData_Hidden,                                 /** True to hide the module from console. (base cmd can't touch it) */
    String:ModuleData_FullName[CM_DATA_FULLNAME],           /** Used for displaying modules. */
    String:ModuleData_ShortName[CM_DATA_SHORTNAME],         /** Used for client interaction with modules. */
    String:ModuleData_Description[CM_DATA_DESCRIPTION],     /** A description of the module */
    Module:ModuleData_Dependencies[CM_DATA_DEPENDENCIES],   /** An array of module IDs that the module depends on.  {INVALID_MODULE} = no dependencies.  Must end with INVALID_MODULE. */
    any:ModuleData_Extra1,                                  /** An extra spot to store any value that other modules can read.  This can be renamed to whatever you want. */
    any:ModuleData_Extra2,                                  /** An extra spot to store any value that other modules can read.  This can be renamed to whatever you want. */
    any:ModuleData_Extra3                                   /** An extra spot to store any value that other modules can read.  This can be renamed to whatever you want. */
}

/**
 * Return values for ModuleMgr_Enable/Disable
 */
enum ModuleToggleQuery
{
    ToggleQuery_Successful, /** The module was successfully enabled/disabled. */
    ToggleQuery_Stopped,    /** The query was stopped because it was redundant. (already enabled/disabled) */
    ToggleQuery_Refuse,     /** A module refused to enable/disable. */
    ToggleQuery_Dependent,  /** Enable - The module couldn't be loaded because at least one dependency isn't loaded. */
                            /** Disable - Other dependent modules were disabled as a result of this module being disabled. */
}

/**
 * The longest string that can be stored in the module's data.
 */
#define MODULE_DATA_LONGEST_STRING 256

// **********************************************
//                 Event Manager
// **********************************************
// Note: Ignore this section if eventmanager.inc isn't included in the project.
// Look in project_events.inc to setup and hook all events your project will need.

// **********************************************
//                    General
// **********************************************

/**
 * Basic project information.  This will also be displayed in 'sm plugins *' commands.
 */

// Used when printing the plugin name anywhere.
#define PROJECT_FULLNAME    "Zombie:Reloaded"

// Shorter version of the full name, used in file paths, and other things.
#define PROJECT_SHORTNAME   "zr"

// Author of the plugin.
#define PROJECT_AUTHOR      "Andrew \"Greyscale\" Borba, Richard \"rhelgeby\" Helgeby"

// Description of the plugin.
#define PROJECT_DESCRIPTION "Infection/survival style gameplay"

// http://wiki.eclipse.org/Version_Numbering
#define PROJECT_VERSION     "3.0.0-dev"

// Any URL associated with the project.
#define PROJECT_URL         "http://forums.alliedmods.net/forumdisplay.php?f=132"

/**
 * Additional information displayed in the version info.
 */
#define PROJECT_COPYRIGHT "Copyright (C) 2009-2010  PROJECT_AUTHOR"
#define PROJECT_LICENSE "GNU GPL, Version 3"

/**
 * Credits.
 * This is the exact string that's printed to the server/client.
 * Feel free to change it to anything, but the names mentioned here by default, should be credited somewhere.
 */
#define PROJECT_CREDITS " Plugin was developed by Greyscale and Richard.\n Development would not have been possible without:\n  AlliedModders, LLC. and its users\n  Andrew \"Greyscale\" Borba, ZR developer and plugin base developer\n  Richard \"rhelgeby\" Helgeby, ZR developer, compilation environment designer and advisor\n http://www.sourcemod.net/\n http://forums.alliedmods.net/forumdisplay.php?f=132"

/**
 * Base file message prefixes.
 * 
 * See Project_PrintToServer in wrappers.inc.
 * 'formatted': The text being printed.
 */
#define PROJECT_SERVER_PREFIX "[%s] %s", PROJECT_FULLNAME, formatted

/**
 * Your plugin's base command for module management.
 * This command will be very similar to the "sm" command except geared toward module management instead of plugin management.
 * Also handles printing plugin information like credits and version.
 * Comment out to remove command from the plugin.
 */
#define PROJECT_BASE_CMD PROJECT_SHORTNAME

/**
 * The prefixes for any base cvars/commands.
 * These are strictly for project base cvars/commands.
 * This will not affect cvars/commands created in your modules.
 */
#define PROJECT_CVAR_PREFIX PROJECT_SHORTNAME
#define PROJECT_CMD_PREFIX  PROJECT_SHORTNAME


// **********************************************
//            Translations Manager
// **********************************************

/**
 * The directory where the project's translation files will be in.
 * All translations files in the format of *.phrases.txt will be automatically loaded.
 * Example path: addons/sourcemod/translations/TM_BASE_DIR/projectbase.phrases.txt.
 * If the resulting path doesn't exist, errors will be logged.
 * 
 * Comment the define out to disable auto-loading.  See stock Project_LoadExtraTranslations.
 * 
 * Note: This only used if translationsmanager.inc is included.
 */
#define TM_BASE_DIR PROJECT_SHORTNAME

/**
 * The formatting string for each option in enum MsgFormatting.
 * Note: These are only used if translationsmanager.inc is included.
 * 
 * See TransMgr_PrintText in translationsmanager.inc.
 * 'translated': The translated text being printed.
 * 'modulefullname': The module's full name.
 * 
 * Color chars vary from game to game.
 */
#define TM_MSGFORMAT_PLUGIN_COLOR      "\x04[%s]\x01 %s", PROJECT_FULLNAME, translated
#define TM_MSGFORMAT_PLUGIN_NOCOLOR    "[%s] %s", PROJECT_FULLNAME, translated
#define TM_MSGFORMAT_MODULE_COLOR      "\x03[%s]\x01 %s", modulefullname, translated
#define TM_MSGFORMAT_MODULE_NOCOLOR    "[%s] %s", modulefullname, translated
#define TM_MSGFORMAT_BOTH_COLOR        "\x04[%s::\x03%s]\x01 %s", PROJECT_FULLNAME, modulefullname, translated
#define TM_MSGFORMAT_BOTH_NOCOLOR      "[%s::%s] %s", PROJECT_FULLNAME, modulefullname, translated

/**
 * Load any extra translation files here.
 * This is also called when translations are being reloaded mid-game.
 *  
 * If TM_BASE_DIR is defined, all translations files in the format of *.phrases.txt will be
 * automatically loaded from the "translations/TM_BASE_DIR/" dir.
 * 
 * Note: If the translations manager isn't included, you must load all your translations here.
 * 
 * @param transmanagerloaded    True if the translations manager is loaded, false if not.
 */
stock Project_LoadExtraTranslations(bool:transmanagerloaded)
{
    // Load translations we may need, if the translations manager isn't loaded.
    if (!transmanagerloaded)
    {
        LoadTranslations("projectbase.phrases");
        
        #if defined PROJECT_BASE_CMD
            LoadTranslations("basecmd.phrases");
        #endif
    }
    
    // Standard phrases provided by SM.
    LoadTranslations("common.phrases");
}


// **********************************************
//                 Log Manager
// **********************************************

/**
 * The formatting string for all logs.
 * Note: This is only used if logmanager.inc is included.
 *  
 * See LogMgr_Print in logmanager.inc.
 * 'modulefullname': The module's full name.
 * 'description': The log description.
 * 'logbuffer': The log text.
 */
#define LM_FORMAT "[%s] [%s] %s", modulefullname, description, logbuffer


// Access Manager

/**
 * The formatting string for module access groups/cvars.
 * Note: This is only used if accessmanager.inc is included.
 *  
 * See AccessMgr_ModuleToGroupName and AccessMgr_ModuleToCvarName in accessmanager.inc.
 * 'moduleshortname':  The short name of the module the group/cvar name is being formatted for.
 */
#define AM_FORMAT_GROUP "%s_access_%s", PROJECT_CVAR_PREFIX, moduleshortname
#define AM_FORMAT_CVAR  "%s_access_%s", PROJECT_CVAR_PREFIX, moduleshortname

// **********************************************
//                Version Info
// **********************************************

/**
 * If this is defined, then it should be in a mercurial repository.
 */
//#define PROJECT_MERCURIAL

// Name of the Mercurial branch this is being developed in.
#define PROJECT_BRANCH "zr-dev"

/**
 * Name of the public cvar for the plugin.
 * Comment this define out to not create a public cvar.
 */
//#define PROJECT_PUBLIC_CVAR "gs_zombiereloaded_version"
