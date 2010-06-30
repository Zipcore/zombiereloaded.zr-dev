/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           anticampcommands.inc
 *  Type:           Module
 *  Description:    Console commands for managing anti-camps.
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

VolAnticampOnCommandsCreate()
{
    RegConsoleCmd("zr_vol_anticamp_add", VolAnticampAddCommand, "Creates a new anti-camp feature. Usage zr_vol_anticamp_add [params...]");
    RegConsoleCmd("zr_vol_anticamp_remove", VolAnticampRemoveCommand, "Removes the specified anti-camp feature. Usage zr_vol_anticamp_remove <anti-camp id> [recursive removal: 0|1]");
}

/**
 * Command callback for creating a anti-camp feature.
 */
public Action:VolAnticampAddCommand(client, argc)
{
    decl String:buffer[1022];
    buffer[0] = 0;
    
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Configuration))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    // Print syntax info if there are missing arguments.
    if (argc < 1)
    {
        VolBuildAnticampSyntaxString(client, buffer, sizeof(buffer));
        ReplyToCommand(client, buffer);
        return Plugin_Handled;
    }
    
    // Join parameters, if any.
    decl String:argbuffer[255];
    buffer[0] = 0;
    argbuffer[0] = 0;
    if (argc >= 1)
    {
        // Join the last parameters in a string.
        for (new arg = 1; arg <= argc; arg++)
        {
            GetCmdArg(arg, argbuffer, sizeof(argbuffer));
            StrCat(buffer, sizeof(buffer), argbuffer);
            
            // Add space, except on the last parameter.
            if (arg < argc)
            {
                StrCat(buffer, sizeof(buffer), " ");
            }
        }
    }
    
    // Parse parameters.
    const MAX_PARAMS = 10;
    new parameters[MAX_PARAMS][ParamParseResult];
    new err;
    new errPos;
    new paramCount = ParamParseString(parameters, MAX_PARAMS, buffer, err, errPos);
    
    // Check for parse errors.
    if (err == PARAM_ERROR_EMPTY)
    {
        TranslationReplyToCommand(client, "Vol Missing Parameters");
        return Plugin_Handled;
    }
    else if (err)
    {
        ParamPrintErrorMessage(client, buffer, err, errPos);
        return Plugin_Handled;
    }
    
    decl String:parameter[64];
    decl String:value[255];
    parameter[0] = 0;
    value[0] = 0;
    new bool:hasErrors = false;
    
    // Parameter cache.
    decl String:name[VOL_NAME_LEN];
    name[0] = 0;
    new bool:makeDefault = false;
    new Float:interval;
    new VolAnticampAction:action;
    new Float:amount;
    new VolAnticampWarningType:warning;
    decl String:message[ZR_ANTICAMP_MESSAGE_LEN];
    message[0] = 0;
    
    // Get and validate parameters.
    for (new param = 0; param < paramCount; param++)
    {
        strcopy(parameter, sizeof(parameter), parameters[param][Param_Name]);
        strcopy(value, sizeof(value), parameters[param][Param_Value]);
        
        if (parameters[param][Param_IsFlag])
        {
            if (StrEqual(parameter, "default", false))
            {
                // Use default settings.
                makeDefault = true;
                
                // Note: Parameter parsing is not stopped because the name
                // attribute is still needed.
            }
            else
            {
                // Skip other flags.
                continue;
            }
        }
        
        if (StrEqual(parameter, "name", false))
        {
            strcopy(name, sizeof(name), value);
        }
        else if (StrEqual(parameter, "interval", false))
        {
            interval = StringToFloat(value);
        }
        else if (StrEqual(parameter, "action", false))
        {
            action = VolStringToAnticampAction(value);
            if (action == Anticamp_InvalidAction)
            {
                TranslationReplyToCommand(client, "Vol Invalid Anticamp Action", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "amount", false))
        {
            amount = StringToFloat(value);
        }
        else if (StrEqual(parameter, "warning_type", false))
        {
            warning = VolStringToAnticampWarning(value);
            if (warning == Anticamp_InvalidWarning)
            {
                TranslationReplyToCommand(client, "Vol Invalid Anticamp Warning", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "message", false))
        {
            strcopy(message, sizeof(message), value);
        }
    }
    
    // Return if errors.
    if (hasErrors)
    {
        return Plugin_Handled;
    }
    
    // Create anticamp.
    new anticampIndex = -1;
    if (makeDefault)
    {
        anticampIndex = VolAnticampAdd(_, _, _, _, _, name);
    }
    else
    {
        anticampIndex = VolAnticampAdd(interval, action, amount, warning, message, name);
    }
    
    // Validate anticamp.
    if (anticampIndex == -1)
    {
        TranslationReplyToCommand(client, "Vol Anticamp Full");
        return Plugin_Handled;
    }
    else if (anticampIndex == -2)
    {
        TranslationReplyToCommand(client, "Vol Anticamp Invalid Interval");
        return Plugin_Handled;
    }
    
    // Print message.
    TranslationReplyToCommand(client, "Vol Anticamp Added", anticampIndex, name);
    
    return Plugin_Handled;
}

/**
 * Command callback for removing a anti-camp feature.
 */
public Action:VolAnticampRemoveCommand(client, argc)
{
    decl String:buffer[1022];
    buffer[0] = 0;
    
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Configuration))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    // Print syntax info if there are missing arguments.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Vol Syntax Anticamp Remove");
        return Plugin_Handled;
    }
    
    new bool:recursive = false;
    decl String:anticampId[32];
    decl String:strRecursive[32];
    anticampId[0] = 0;
    
    // Get anticamp id and validate it.
    GetCmdArg(1, anticampId, sizeof(anticampId));
    new anticampIndex = VolGetFeatureIndex(VolFeature_Anticamp, anticampId);
    if (anticampIndex < 0)
    {
        TranslationReplyToCommand(client, "Vol Invalid Anticamp Id", anticampId);
        return Plugin_Handled;
    }
    
    // Get recursive parameter.
    if (argc == 2)
    {
        GetCmdArg(2, strRecursive, sizeof(strRecursive));
        
        if (StringToInt(strRecursive) != 0)
        {
            recursive = true;
        }
    }
    
    // Remove anticamp.
    VolAnticampRemove(anticampIndex, recursive);
    
    // Print message.
    if (recursive)
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "Yes");
    }
    else
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "No");
    }
    TranslationReplyToCommand(client, "Vol Anticamp Removed", anticampIndex, anticampId, strRecursive);
    
    return Plugin_Handled;
}

/**
 * Builds a translated string with syntax and parameter info for zr_vol_anticamp_add command.
 *
 * @param client    Client that will see this info.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolBuildAnticampSyntaxString(client, String:buffer[], maxlen)
{
    new numCells = 0;
    buffer[0] = 0;
    
    decl String:linebuffer[255];
    linebuffer[0] = 0;
    
    SetGlobalTransTarget(client);
    
    #define VOL_SYNTAX_FORMAT_STRING "  %t\n\n"
    
    // Main syntax.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Syntax Anticamp Add");
    numCells += StrCat(buffer, maxlen, linebuffer);
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Syntax Anticamp Default");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "\n");
    
    
    // Optional parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Params Optional");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "name=<anti-camp name>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Name");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "interval=<seconds>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Interval");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "action=none|damage|slay|drug|ignite\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Action");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "action_amount=<amount>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Amount");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "warning_type=none|chat|center|menu\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Warning");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "message=<message>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Anticamp Message");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    return numCells;
}
