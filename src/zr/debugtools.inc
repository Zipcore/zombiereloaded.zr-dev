/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           debugtools.inc
 *  Type:           Core 
 *  Description:    Place to put custom functions and test stuff.
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

DebugOnCommandsCreate()
{
    // Custom test commands:
    /*
    RegConsoleCmd("zr_getparametercount", Command_GetParameterCount, "Test GetParameterCount function in paramtools.inc. Usage: zr_getparametercount <paramstring>");
    RegConsoleCmd("zr_getparametervalue", Command_GetParameterValue, "Test GetParameterValue function in paramtools.inc. Usage: zr_getparametervalue <paramname> <paramstring>");
    */
    RegConsoleCmd("zr_paramtest", Command_ParamTest, "Tests parameter parser. Usage: zr_paramtest <parameter string>");
}

public Action:Command_ParamTest(client, argc)
{
    decl String:argbuffer[256];
    decl String:paramstring[256];
    
    paramstring[0] = 0;
    
    // Join the last parameters in a string.
    for (new arg = 1; arg <= argc; arg++)
    {
        GetCmdArg(arg, argbuffer, sizeof(argbuffer));
        StrCat(paramstring, sizeof(paramstring), argbuffer);
        
        // Add space, except on the last parameter.
        if (arg < argc)
        {
            StrCat(paramstring, sizeof(paramstring), " ");
        }
    }
    
    const MAX_PARAMS = 10;
    new parameters[MAX_PARAMS][ParamParseResult];
    new err;
    new errPos;
    new paramCount = ParamParseString(parameters, MAX_PARAMS, paramstring, err, errPos);
    
    ReplyToCommand(client, "%d parameters parsed. Dumping result:\n", paramCount);

    for (new param = 0; param < paramCount; param++)
    {
        new bool:isFlag = parameters[param][Param_IsFlag];
        new String:name[PARAM_NAME_MAXLEN];
        new String:value[PARAM_VALUE_MAXLEN];
        
        strcopy(name, sizeof(name), parameters[param][Param_Name]);
        strcopy(value, sizeof(value), parameters[param][Param_Value]);
        
        ReplyToCommand(client, "Parameter: %d", param);
        ReplyToCommand(client, "Is flag  : %d", isFlag);
        ReplyToCommand(client, "Name     : \"%s\"", name);
        ReplyToCommand(client, "Value    : \"%s\"\n", value);
    }
    
    // Check for parse errors.
    if (err)
    {
        ParamPrintErrorMessage(client, paramstring, err, errPos);
        return Plugin_Handled;
    }
    
    return Plugin_Handled;
}


/*
public Action:Command_GetParameterCount(client, argc)
{
    if (argc < 1)
    {
        ReplyToCommand(client, "No parameter string passed. Usage: zr_getparametercount <paramstring>");
        return Plugin_Handled;
    }
    
    decl String:argbuffer[256];
    decl String:paramstring[256];
    
    paramstring[0] = 0;
    
    // Join the last parameters in a string.
    for (new arg = 1; arg <= argc; arg++)
    {
        GetCmdArg(arg, argbuffer, sizeof(argbuffer));
        StrCat(paramstring, sizeof(paramstring), argbuffer);
        
        // Add space, except on the last parameter.
        if (arg < argc)
        {
            StrCat(paramstring, sizeof(paramstring), " ");
        }
    }
    
    ReplyToCommand(client, "Parameter string: \"%s\"", paramstring);
    new paramcount = GetParameterCount(paramstring);
    ReplyToCommand(client, "Parameter count: %d", paramcount);
    
    return Plugin_Handled;
}

public Action:Command_GetParameterValue(client, argc)
{
    if (argc < 2)
    {
        ReplyToCommand(client, "Missing parameters. Usage: zr_getparametervalue <paramname> <paramstring>");
        return Plugin_Handled;
    }
    
    decl String:paramname[256];
    decl String:paramstring[256];
    
    decl String:valuebuffer[64];
    decl String:argbuffer[256];
    
    paramstring[0] = 0;
    valuebuffer[0] = 0;
    
    GetCmdArg(1, paramname, sizeof(paramname));
    
    // Join the last parameters in a string.
    for (new arg = 2; arg <= argc; arg++)
    {
        GetCmdArg(arg, argbuffer, sizeof(argbuffer));
        StrCat(paramstring, sizeof(paramstring), argbuffer);
        
        // Add space, except on the last parameter.
        if (arg < argc)
        {
            StrCat(paramstring, sizeof(paramstring), " ");
        }
    }
    
    new retval = GetParameterValue(valuebuffer, sizeof(valuebuffer), paramstring, paramname);
    
    ReplyToCommand(client, "Return value: %d\nParameter string: \"%s\"\nParameter value: \"%s\"", retval, paramstring, valuebuffer);
    
    return Plugin_Handled;
}
*/
