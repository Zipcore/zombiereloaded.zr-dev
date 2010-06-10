/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volcommands.inc
 *  Type:           Module
 *  Description:    Command handler for volumetric features.
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
 * Creates commands for managing volumes.
 */
VolOnCommandsCreate()
{
    //RegConsoleCmd("zr_vol_help", VolHelpCommand, "Lists available volume commands");
    
    RegConsoleCmd("zr_vol_add", VolAddCommand, "Creates a empty volume (no features attached) in the map. Usage: zr_vol_add <shape> <shape id> [params...]");
    RegConsoleCmd("zr_vol_remove", VolRemoveCommand, "Removes an existing volume. Usage: zr_vol_remove <index|name> [recursive]");
    RegConsoleCmd("zr_vol_list", VolListCommand, "Lists existing volumes in the map, or dumps detail data to the specified volume. Usage: zr_vol_list [volume index]");
    RegConsoleCmd("zr_vol_dumpstates", VolDumpStatesCommand, "Dumps volume states for the specified player. Usage: zr_vol_dumpstates <index|targetname>");
    
    VolShapeOnCommandsCreate();
}

/**
 * Command callback for creating a new volume.
 */
public Action:VolAddCommand(client, argc)
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
        VolBuildVolumeSyntaxString(client, buffer, sizeof(buffer));
        ReplyToCommand(client, buffer);
        return Plugin_Handled;
    }
    
    // Get shape type and validate it.
    GetCmdArg(1, buffer, sizeof(buffer));
    new VolumeShapes:shapeType = VolStringToShape(buffer);
    if (shapeType == VolShape_Invalid)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape", buffer);
        return Plugin_Handled;
    }
    
    // Get shape id and validate it.
    new shapeIndex;
    decl String:shape[64];
    shape[0] = 0;
    GetCmdArg(2, shape, sizeof(shape));
    if (IsCharNumeric(shape[0]))
    {
        // It's a index.
        shapeIndex = StringToInt(shape);
        switch (shapeType)
        {
            case VolShape_Cuboid:
            {
                if (!VolCuboidIsValid(shapeIndex))
                {
                    shapeIndex = -1;
                }
            }
            case VolShape_Sphere:
            {
                if (!VolSphereIsValid(shapeIndex))
                {
                    shapeIndex = -1;
                }
            }
        }
    }
    else
    {
        // It's a name.
        switch (shapeType)
        {
            case VolShape_Cuboid: shapeIndex = VolCuboidFind(shape);
            case VolShape_Sphere: shapeIndex = VolSphereFind(shape);
        }
    }
    
    // Validate shape index.
    if (shapeIndex < 0)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape Id", shape);
        return Plugin_Handled;
    }
    
    // Join last parameters, if any.
    decl String:argbuffer[255];
    buffer[0] = 0;
    argbuffer[0] = 0;
    if (argc >= 3)
    {
        // Join the last parameters in a string.
        for (new arg = 3; arg <= argc; arg++)
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
    new String:name[ZR_VOL_NAME_LEN] = ZR_VOL_DEFAULT_NAME;
    new VolumeEffects:effect = ZR_VOL_DEFAULT_EFFECT;
    new effectColor[3] = ZR_VOL_DEFAULT_EFFECT_COLOR;
    new VolumeTeamFilters:teamFilter = ZR_VOL_DEFAULT_TEAM_FILTER;
    new Float:triggerDelay = ZR_VOL_DEFAULT_TRIGGER_DELAY;
    new priority = ZR_VOL_DEFAULT_PRIORITY;
    new VolumeConflictActions:conflictAction = ZR_VOL_DEFAULT_CONFLICT_ACTION;
    
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
        else if (StrEqual(parameter, "effect", false))
        {
            effect = VolStringToEffect(value);
            if (effect == VolEffect_Invalid)
            {
                TranslationReplyToCommand(client, "Vol Invalid Effect", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "effect_color", false))
        {
            new color[3];
            if (VolStringToEffectColor(value, color))
            {
                effectColor = color;
            }
            else
            {
                TranslationReplyToCommand(client, "Vol Invalid Effect Color", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "teamfilter", false))
        {
            teamFilter = VolStringToTeam(value);
            if (teamFilter == VolTeam_Invalid)
            {
                TranslationReplyToCommand(client, "Vol Invalid Teamfilter", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "delay", false))
        {
            triggerDelay = StringToFloat(value);
            if (triggerDelay < 0.0)
            {
                triggerDelay = 0.0;
            }
        }
        else if (StrEqual(parameter, "priority", false))
        {
            priority = StringToInt(value);
        }
        else if (StrEqual(parameter, "conflict_action", false))
        {
            conflictAction = VolStringToConflictAction(value);
            if (conflictAction == VolConflict_Invalid)
            {
                TranslationReplyToCommand(client, "Vol Invalid Conflict Action", value);
                hasErrors = true;
            }
        }
    }
    
    // Return if errors.
    if (hasErrors)
    {
        return Plugin_Handled;
    }
    
    // Add volume.
    new volumeIndex = VolAdd(shapeIndex, shapeType, effect, effectColor, teamFilter, triggerDelay, priority, conflictAction, name);
    
    // Check for errors.
    if (volumeIndex == -1)
    {
        TranslationReplyToCommand(client, "Vol Full");
        return Plugin_Handled;
    }
    else if (volumeIndex == -2)
    {
        TranslationReplyToCommand(client, "Vol Invalid Shape Id", shape);
        return Plugin_Handled;
    }
    
    TranslationReplyToCommand(client, "Vol Added", volumeIndex, name);
    return Plugin_Handled;
}

/**
 * Command callback for removing a volume.
 */
public Action:VolRemoveCommand(client, argc)
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
        TranslationReplyToCommand(client, "Vol Syntax Remove");
        return Plugin_Handled;
    }
    
    decl String:id[64];
    decl String:strRecursive[16];
    
    id[0] = 0;
    strRecursive[0] = 0;
    
    new volumeIndex = -1;
    new bool:recursive = false;
    
    // Get id.
    GetCmdArg(1, id, sizeof(id));
    if (IsCharNumeric(id[0]))
    {
        // It's a index.
        volumeIndex = StringToInt(id);
    }
    else
    {
        // It's a volume name.
        volumeIndex = VolFind(id);
    }
    
    // Validate index.
    if (!VolIsValid(volumeIndex))
    {
        TranslationReplyToCommand(client, "Vol Invalid Volume Id", id);
        return Plugin_Handled;
    }
    
    // Get recursive parameter.
    if (argc == 2)
    {
        GetCmdArg(2, strRecursive, sizeof(strRecursive));
        
        if (StringToInt(strRecursive) == 1)
        {
            recursive = true;
        }
    }
    
    // Remove volume.
    VolRemove(volumeIndex, recursive);
    
    // Print message.
    if (recursive)
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "Yes");
    }
    else
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "No");
    }
    TranslationReplyToCommand(client, "Vol Removed", volumeIndex, strRecursive);
    
    return Plugin_Handled;
}

/**
 * Command callback for listing volumes or dumping data.
 */
public Action:VolListCommand(client, argc)
{
    decl String:buffer[1022];       // Two chars reserved for newline and null terminator.
    decl String:linebuffer[256];
    decl String:id[ZR_VOL_NAME_LEN];
    
    buffer[0] = 0;
    linebuffer[0] = 0;
    
    new volumeIndex;
    new volCount;
    
    SetGlobalTransTarget(client);
    
    if (argc < 1)
    {
        // No volume specified. Display syntax and list volumes.
        TranslationReplyToCommand(client, "Vol Syntax List");
        ReplyToCommand(client, "%10t %20t %t\n--------------------------------------------------\n",
                       "Vol Attrib Index", "Vol Attrib Name", "Vol Attrib Features");
        
        // Loop through all indexes.
        for (volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
        {
            // Check if in use.
            if (Volumes[volumeIndex][Vol_InUse])
            {
                // Get type list.
                VolGetFeatureListString(volumeIndex, linebuffer, sizeof(linebuffer));
                
                // Add to list.
                ReplyToCommand(client, "%-9d %s %s", volumeIndex, Volumes[volumeIndex][Vol_Name], linebuffer);
                volCount++;
            }
        }
        
        ReplyToCommand(client, "\n%t %d", "Vol Attrib Total Volumes", volCount);
    }
    else
    {
        // Dump data for the specified volume.
        
        // Get volume id.
        GetCmdArg(1, id, sizeof(id));
        if (IsCharNumeric(id[0]))
        {
            // It's a index.
            volumeIndex = StringToInt(id);
        }
        else
        {
            // It's a volume name.
            volumeIndex = VolFind(id);
        }
        
        // Validate index.
        if (!VolIsValid(volumeIndex))
        {
            TranslationReplyToCommand(client, "Vol Invalid Volume Id", id);
            return Plugin_Handled;
        }
        
        // Dump volume info.
        VolBuildList(volumeIndex, buffer, sizeof(buffer));
        ReplyToCommand(client, "%s\n", buffer);
        
        // Get shape.
        new VolumeShapes:shape = Volumes[volumeIndex][Vol_Shape];
        new shapeIndex = Volumes[volumeIndex][Vol_ShapeData];
        
        // Dump shape info.
        switch (shape)
        {
            case VolShape_Cuboid:
            {
                ReplyToCommand(client, "%t", "Vol Cuboid");
                VolCuboidBuildList(shapeIndex, buffer, sizeof(buffer));
            }
            case VolShape_Sphere:
            {
                ReplyToCommand(client, "%t", "Vol Sphere");
                VolSphereBuildList(shapeIndex, buffer, sizeof(buffer));
            }
        }
        ReplyToCommand(client, "%s\n", buffer);
        
        // Get feature indexes.
        new Handle:featureTypes = Volumes[volumeIndex][Vol_FeatureTypes];
        new Handle:featureIndexes = Volumes[volumeIndex][Vol_FeatureIndexes];
        
        // Dump feature data if available.
        if (featureTypes != INVALID_HANDLE)
        {
            new numFeatures = GetArraySize(featureTypes);
            
            // Loop through each feature and dump its data.
            for (new feature = 0; feature < numFeatures; feature++)
            {
                new VolumeFeatureTypes:featureType = VolumeFeatureTypes:GetArrayCell(featureTypes, feature);
                new featureIndex = GetArrayCell(featureIndexes, feature);
                
                // Build attribute lists.
                switch (featureType)
                {
                    case VolFeature_Anticamp:
                    {
                        TranslationReplyToCommand(client, "Vol Anticamp");
                        VolAnticampBuildList(featureIndex, buffer, sizeof(buffer));
                    }
                    case VolFeature_ClassEdit:
                    {
                        TranslationReplyToCommand(client, "Vol Classedit");
                        VolClassEditBuildList(featureIndex, buffer, sizeof(buffer));
                    }
                }
                
                // Print list.
                ReplyToCommand(client, "%s\n", buffer);
            }
        }
    }
    
    return Plugin_Handled;
}

/**
 * Command callback for dumping volume states.
 */
public Action:VolDumpStatesCommand(client, argc)
{
    decl String:target[64];
    new targetclient;
    
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Vol Syntax Dump States");
        return Plugin_Handled;
    }
    
    // Get target.
    GetCmdArg(1, target, sizeof(target));
    targetclient = FindTarget(client, target);
    
    // Validate target.
    if (targetclient <= 0)
    {
        // Note: FindTarget automatically print error messages.
        return Plugin_Handled;
    }
    
    SetGlobalTransTarget(client);
    
    // Print header.
    ReplyToCommand(client, "%10t%20t%t\n--------------------------------------------------",
                   "Vol Attrib Index", "Vol Attrib Name", "Vol Attrib In Volume");
    
    //Index:    Name:               Player in volume:
    //--------------------------------------------------
    //1         my_volume           No
    
    // Get player states.
    new bool:statebuffer[ZR_VOLUMES_MAX];
    decl String:name[ZR_VOL_NAME_LEN];
    decl String:strState[32];
    VolGetPlayerStates(targetclient, statebuffer, sizeof(statebuffer));
    
    // Set language.
    SetGlobalTransTarget(client);
    
    // Loop through each volume.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if volume is in use.
        if (VolInUse(volumeIndex))
        {
            strcopy(name, sizeof(name), Volumes[volumeIndex][Vol_Name]);
            
            if (statebuffer[volumeIndex])
            {
                Format(strState, sizeof(strState), "%t", "Yes");
            }
            else
            {
                Format(strState, sizeof(strState), "%t", "No");
            }
            
            // Dump state.
            ReplyToCommand(client, "%-9d %20s %t", volumeIndex, name, strState);
        }
    }
    
    return Plugin_Handled;
}

/**
 * Builds a translated string with syntax and parameter info for zr_vol_add command.
 *
 * @param client    Client that will see this info.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolBuildVolumeSyntaxString(client, String:buffer[], maxlen)
{
    new numCells = 0;
    buffer[0] = 0;
    
    decl String:linebuffer[255];
    linebuffer[0] = 0;
    
    SetGlobalTransTarget(client);
    
    #define VOL_SYNTAX_FORMAT_STRING "  %t\n\n"
    
    // Main syntax.
    Format(linebuffer, sizeof(linebuffer), "%t\n\n", "Vol Syntax Add");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%10s%t\n", "shape", "Vol Param Shape");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%10s%t\n", "shape id", "Vol Param Shape ID");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "\n");
    
    
    // Optional parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Params Optional");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "name=<volume name>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Vol Name");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "effect=none|wireframe|smoke\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Effect");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "effect_color=<red>,<green>,<blue>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Effect Color");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "teamfilter=all|humans|zombies\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Teamfilter");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "delay=<seconds>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Delay");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "priority=<priority>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Priority");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "conflict_action=priority|merge\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param Conflict Action");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    return numCells;
}
