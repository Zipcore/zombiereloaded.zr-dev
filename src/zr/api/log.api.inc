/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Log module)
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
 * Initializes all natives and forwards related to logging.
 */
APILogInit()
{
    // Log module natives (log.zr.inc)
    
    // Natives
    CreateNative("ZR_LogEvent", APILogEvent);
}

/**
 * Native call function (ZR_LogEvent)
 * Returns true if the client is infected, false if not.
 * 
 * LogEvent(bool:isConsole = false, LogTypes:logType = LogType_Normal, eventType = LOG_CORE_EVENTS, LogModules:module, const String:description[], const String:text[], any:...)
 */
public APILogEvent(Handle:plugin, numParams)
{
    // Retrieve all the given values.
    
    new bool:isConsole = bool:GetNativeCell(1);
    new LogTypes:logType = LogTypes:GetNativeCell(2);
    new eventType = GetNativeCell(3);
    new LogModules:module = LogModules:GetNativeCell(4);
    
    // Get string params.
    new length;
    
    GetNativeStringLength(5, length);
    decl String:description[length + 1];
    GetNativeString(5, description, length + 1);
    
    decl String:text[LOG_MAX_LENGTH_FILE], written;
    FormatNativeString(0, 6, 7, sizeof(text), written, text);
    
    // Finally, send the log message to ZR to interpret further.
    LogEvent(isConsole, logType, eventType, module, description, text);
}
