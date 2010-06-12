/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           shapecommands.inc
 *  Type:           Module
 *  Description:    Console commands for managing shapes.
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

VolShapeOnCommandsCreate()
{
    RegConsoleCmd("zr_vol_shape_add", VolShapeAddCommand, "Creates a new shape to be used on a volume. Usage zr_vol_shape_add <shape> [params...]");
    RegConsoleCmd("zr_vol_shape_remove", VolShapeRemoveCommand, "Removes the specified shape. Usage zr_vol_shape_remove <shape> <shape id> [recursive removal: 0|1]");
    //RegConsoleCmd("zr_vol_shape_list", VolShapeListCommand, "Lists all shapes or prints details about a shape. Usage: zr_vol_shape_list [<shape> <shape id>]");
}

/**
 * Command callback for creating a shape.
 */
public Action:VolShapeAddCommand(client, argc)
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
        VolBuildShapeSyntaxString(client, buffer, sizeof(buffer));
        ReplyToCommand(client, buffer);
        return Plugin_Handled;
    }
    
    // Get shape type.
    new VolumeShapes:shape;
    decl String:strShape[32];
    strShape[0] = 0;
    GetCmdArg(1, strShape, sizeof(strShape));
    shape = VolStringToShape(strShape);
    
    // Validate shape.
    if (shape == VolShape_Invalid)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape", strShape);
        return Plugin_Handled;
    }
    
    // Join last parameters, if any.
    decl String:argbuffer[255];
    buffer[0] = 0;
    argbuffer[0] = 0;
    if (argc >= 2)
    {
        // Join the last parameters in a string.
        for (new arg = 2; arg <= argc; arg++)
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
    new Float:min[3];
    new Float:max[3];
    new Float:loc[3];
    new Float:radius;
    new FadeTypes:fadeType;
    new Float:minDist;
    new Float:maxDist;
    
    // Get and validate parameters.
    for (new param = 0; param < paramCount; param++)
    {
        if (parameters[param][Param_IsFlag])
        {
            // Skip flags.
            continue;
        }
        
        strcopy(parameter, sizeof(parameter), parameters[param][Param_Name]);
        strcopy(value, sizeof(value), parameters[param][Param_Value]);
        
        if (StrEqual(parameter, "name", false))
        {
            strcopy(name, sizeof(name), value);
        }
        else if (StrEqual(parameter, "x1", false))
        {
            min[0] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "y1", false))
        {
            min[1] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "z1", false))
        {
            min[2] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "x2", false))
        {
            max[0] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "y2", false))
        {
            max[1] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "z2", false))
        {
            max[2] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "x", false))
        {
            loc[0] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "y", false))
        {
            loc[1] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "z", false))
        {
            loc[2] = StringToFloat(value);
        }
        else if (StrEqual(parameter, "radius", false))
        {
            radius = StringToFloat(value);
        }
        else if (StrEqual(parameter, "fadetype", false))
        {
            fadeType = VolStringToFadeType(value);
            if (fadeType == FadeType_Invalid)
            {
                TranslationReplyToCommand(client, "Vol Invalid Fade Type", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "mindist", false))
        {
            minDist = StringToFloat(value);
        }
        else if (StrEqual(parameter, "maxdist", false))
        {
            maxDist = StringToFloat(value);
        }
    }
    
    // Return if errors.
    if (hasErrors)
    {
        return Plugin_Handled;
    }
    
    // Create shape.
    new shapeIndex = -1;
    
    switch (shape)
    {
        case VolShape_Cuboid:
        {
            shapeIndex = VolCuboidAdd(min, max, name);
            
            // Validate cuboid.
            if (shapeIndex == -1)
            {
                TranslationReplyToCommand(client, "Vol Cuboid Full");
                return Plugin_Handled;
            }
            else if (shapeIndex == -2)
            {
                TranslationReplyToCommand(client, "Vol Cuboid Full");
                return Plugin_Handled;
            }
        }
        case VolShape_Sphere:
        {
            shapeIndex = VolSphereAdd(loc, radius, fadeType, minDist, maxDist, name);
            
            // Validate sphere.
            if (shapeIndex == -1)
            {
                TranslationReplyToCommand(client, "Vol Sphere Full");
                return Plugin_Handled;
            }
            else if (shapeIndex == -2)
            {
                TranslationReplyToCommand(client, "Vol Sphere Invalid Radius");
                return Plugin_Handled;
            }
            else if (shapeIndex == -3)
            {
                TranslationReplyToCommand(client, "Vol Sphere Invalid Fade Distance");
                return Plugin_Handled;
            }
        }
    }
    
    TranslationReplyToCommand(client, "Vol Shape Added", strShape, shapeIndex, name);
    return Plugin_Handled;
}

/**
 * Command callback for removing a shape.
 */
public Action:VolShapeRemoveCommand(client, argc)
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
    if (argc < 2)
    {
        VolBuildShapeRemoveSyntax(client, buffer, sizeof(buffer));
        ReplyToCommand(client, buffer);
        return Plugin_Handled;
    }
    
    // Get shape type.
    new VolumeShapes:shape;
    new bool:recursive = false;
    decl String:strShape[32];
    decl String:strShapeID[64];
    decl String:strRecursive[32];
    strShape[0] = 0;
    strRecursive[0] = 0;
    strShapeID[0] = 0;
    GetCmdArg(1, strShape, sizeof(strShape));
    shape = VolStringToShape(strShape);
    
    // Validate shape.
    if (shape == VolShape_Invalid)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape", strShape);
        return Plugin_Handled;
    }
    
    // Get shape id and validate it.
    GetCmdArg(2, strShapeID, sizeof(strShapeID));
    new shapeIndex = VolGetShapeIndex(shape, strShapeID);
    if (shapeIndex < 0)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape ID", strShapeID);
        return Plugin_Handled;
    }
    
    // Get recursive parameter.
    if (argc == 3)
    {
        GetCmdArg(3, strRecursive, sizeof(strRecursive));
        
        if (StringToInt(strRecursive) != 0)
        {
            recursive = true;
        }
    }
    
    // Remove shape.
    switch (shape)
    {
        case VolShape_Cuboid: VolCuboidRemove(shapeIndex, recursive);
        case VolShape_Sphere: VolSphereRemove(shapeIndex, recursive);
    }
    
    // Print message.
    if (recursive)
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "Yes");
    }
    else
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "No");
    }
    TranslationReplyToCommand(client, "Vol Shape Removed", shapeIndex, strShape, strRecursive);
    
    return Plugin_Handled;
}

/**
 * Builds a translated string with syntax and parameter info for zr_vol_shape_add command.
 *
 * @param client    Client that will see this info.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolBuildShapeSyntaxString(client, String:buffer[], maxlen)
{
    new numCells = 0;
    buffer[0] = 0;
    
    decl String:linebuffer[255];
    linebuffer[0] = 0;
    
    SetGlobalTransTarget(client);
    
    #define VOL_SYNTAX_FORMAT_STRING "  %t\n\n"
    
    // Main syntax.
    Format(linebuffer, sizeof(linebuffer), "%t\n\n", "Vol Syntax Shape Add");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%10s%t\n", "shape", "Vol Param Shape");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "\n");
    
    
    // Optional parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Params Optional");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "name=<volume name>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Shape Name");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    
    // Cuboid parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Cuboid Params");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "x1, y1, z1\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Cuboid Min");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "x2, y2, z2\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Cuboid Max");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    
    // Sphere parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Sphere Params");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "x, y, z\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Sphere Loc");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "radius=<radius>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Sphere Radius");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "fadetype=none|linearcenter|lineardistance\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Sphere Fade Type");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "mindist=<min fade distance>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Sphere Min Dist");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "maxdist=<max fade distance>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Sphere Max Dist");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    return numCells;
}

/**
 * Builds a translated string with syntax and parameter info for zr_vol_shape_remove command.
 *
 * @param client    Client that will see this info.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolBuildShapeRemoveSyntax(client, String:buffer[], maxlen)
{
    new numCells = 0;
    buffer[0] = 0;
    
    decl String:linebuffer[255];
    linebuffer[0] = 0;
    
    SetGlobalTransTarget(client);
    
    // Main syntax.
    Format(linebuffer, sizeof(linebuffer), "%t\n\n", "Vol Syntax Shape Remove");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%10s%t\n", "shape", "Vol Param Shape");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%10s%t\n", "shape id", "Vol Param Shape ID Remove");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    return numCells;
}
