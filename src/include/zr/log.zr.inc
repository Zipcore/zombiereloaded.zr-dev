/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          log.zr.inc
 *  Type:          Include
 *  Description:   Log-related natives/forwards.
 *
 *  Copyright (C) 2009-10  Greyscale, Richard Helgeby
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
 * @section Log flags.
 */
#define LOG_CORE_EVENTS             (1 << 0)    /** Log events from the plugin core like config validation and other messages. */
#define LOG_GAME_EVENTS             (1 << 1)    /** Log admin commands, console commands, and game related events from modules like suicide attempts and weapon restrictions. */
#define LOG_PLAYER_COMMANDS         (1 << 2)    /** Log events that are triggered by players, like chat triggers, teleporting and class change. */
#define LOG_DEBUG                   (1 << 3)    /** Log debug messages, if any. Usually only developers enable this log flag. */
#define LOG_DEBUG_DETAIL            (1 << 4)    /** Log additional debug messages with more detail. May cause spam depending on filter settings. Usually only developers enable this log flag. */
/**
 * @endsection
 */

/**
 * Log format types.
 */
enum LogTypes
{
    LogType_Normal = 0,     // Normal log message. Printed in SourceMod logs.
    LogType_Error,          // Error message. Printed in SourceMod error logs.
    LogType_Fatal           // Fatal error. Stops the plugin with the specified message.
}

/**
 * List of modules that write log events. Add new modules if needed (in
 * alphabetical order).
 * 
 * Update following when adding modules:
 * - Admin log flag menu
 * - LogGetModuleNameString
 * - LogGetModule
 */
enum LogModules
{
    bool:LogModule_Invalid = 0,     /** Used as return value when an error occurs.*/
    
    bool:LogModule_External,        /** Use this if your logging from an external plugin */
    bool:LogModule_Account,
    bool:LogModule_AntiStick,
    bool:LogModule_Config,
    bool:LogModule_Cvars,
    bool:LogModule_Damage,
    bool:LogModule_Downloads,
    bool:LogModule_Hitgroups,
    bool:LogModule_Infect,
    bool:LogModule_Models,
    bool:LogModule_Napalm,
    bool:LogModule_Playerclasses,
    bool:LogModule_VEffects,
    bool:LogModule_SEffects,
    bool:LogModule_Tools,
    bool:LogModule_Volfeatures,
    bool:LogModule_Weapons,
    bool:LogModule_Weaponrestrict,
    bool:LogModule_ZSpawn,
    bool:LogModule_ZTele,
}

/**
 * Print a formatted message to logs depending on log settings.
 * 
 * @param isConsole     Optional. Specifies whether the log event came from
 *                      client 0. Used in console commands, do not mix with
 *                      regular log events. Default is false.
 * @param logType       Optional. Log type and action. Default is
 *                      LogType_Normal.
 * @param eventType     Optional. A log flag describing What kind of log event
 *                      it is. Default is LOG_CORE_EVENTS.
 * @param module        Module the log event were executed in.
 * @param description   Event type or function name. A short descriptive phrase
 *                      to group together similar logs.
 * @param text          Log message. Can be formatted.
 * @param ...           Formatting parameters.
 */
native ZR_LogEvent(bool:isConsole = false, LogTypes:logType = LogType_Normal, eventType = LOG_CORE_EVENTS, LogModules:module, const String:description[], const String:text[], any:...);
