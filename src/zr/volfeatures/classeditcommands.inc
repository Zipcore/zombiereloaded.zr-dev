/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           classeditcommands.inc
 *  Type:           Module
 *  Description:    Console commands for managing class editors.
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

VolClassEditOnCommandsCreate()
{
    RegConsoleCmd("zr_vol_classedit_add", VolClassEditAddCommand, "Creates a new class editor feature. Usage zr_vol_classedit_add [params...]");
    RegConsoleCmd("zr_vol_classedit_remove", VolClassEditRemoveCommand, "Removes the specified class editor feature. Usage zr_vol_classedit_remove <class editor id> [recursive removal: 0|1]");
}

/**
 * Command callback for creating a class editor feature.
 */
public Action:VolClassEditAddCommand(client, argc)
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
        VolBuildClassEditSyntaxString(client, buffer, sizeof(buffer));
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
    new VolClassEditMode:mode = VOL_CLASSEDIT_DEF_MODE;
    new classIndex = VOL_CLASSEDIT_DEF_CLASS_INDEX;
    new classData[ClassEditableAttributes];
    classData = VolEmptyAttributes;
    new classErrors;
    
    // Get and validate parameters.
    for (new param = 0; param < paramCount; param++)
    {
        strcopy(parameter, sizeof(parameter), parameters[param][Param_Name]);
        strcopy(value, sizeof(value), parameters[param][Param_Value]);
        
        if (parameters[param][Param_IsFlag])
        {
            // Skip flags.
            continue;
        }
        
        if (StrEqual(parameter, "name", false))
        {
            strcopy(name, sizeof(name), value);
        }
        else if (StrEqual(parameter, "mode", false))
        {
            mode = VolStringToClassEditMode(value);
            if (mode == ClassEditMode_Invalid)
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Mode", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "class", false))
        {
            classIndex = ClassGetIndex(value);
            if (classIndex < 0)
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Class", value);
                hasErrors = true;
            }
        }
        
        // Editable class attributes.
        else if (StrEqual(parameter, "alpha_initial", false))
        {
            classData[ClassEdit_AlphaInitial] = StringToInt(value);
            if (!ClassIsValidAlphaInitial(classData[ClassEdit_AlphaInitial]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "alpha_initial", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "alpha_damaged", false))
        {
            classData[ClassEdit_AlphaDamaged] = StringToInt(value);
            if (!ClassIsValidAlphaDamaged(classData[ClassEdit_AlphaDamaged]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "alpha_damaged", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "alpha_damage", false))
        {
            classData[ClassEdit_AlphaDamage] = StringToInt(value);
            if (!ClassIsValidAlphaDamage(classData[ClassEdit_AlphaDamage]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "alpha_damage", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "overlay_path", false))
        {
            strcopy(classData[ClassEdit_OverlayPath], PLATFORM_MAX_PATH, value);
            if (!ClassIsValidOverlay(value))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "overlay_path", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "nvgs", false))
        {
            classData[ClassEdit_Nvgs] = bool:ConfigSettingToBool(value);
        }
        else if (StrEqual(parameter, "fov", false))
        {
            classData[ClassEdit_Fov] = StringToInt(value);
            if (!ClassIsValidFOV(classData[ClassEdit_Fov]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "fov", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "has_napalm", false))
        {
            classData[ClassEdit_HasNapalm] = bool:ConfigSettingToBool(value);
        }
        else if (StrEqual(parameter, "napalm_time", false))
        {
            classData[ClassEdit_NapalmTime] = StringToFloat(value);
            if (!ClassIsValidNapalmTime(classData[ClassEdit_NapalmTime]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "napalm_time", value);
                hasErrors = true;
            }
        }
        // immunity mode
        // immunity amount
        else if (StrEqual(parameter, "no_fall_damage", false))
        {
            classData[ClassEdit_NoFallDamage] = bool:ConfigSettingToBool(value);
        }
        else if (StrEqual(parameter, "weapon_profile", false))
        {
            classData[ClassEdit_WeaponProfile] = StringToInt(value);
            if (!ClassIsValidWeaponProfile(classData[ClassEdit_WeaponProfile]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "weapon_profile", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "health_regen_interval", false))
        {
            classData[ClassEdit_RegenInterval] = StringToFloat(value);
            if (!ClassIsValidRegenInterval(classData[ClassEdit_RegenInterval]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "health_regen_interval", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "health_regen_amount", false))
        {
            classData[ClassEdit_RegenAmount] = StringToInt(value);
            if (!ClassIsValidRegenAmount(classData[ClassEdit_RegenAmount]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "health_regen_amount", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "infect_gain", false))
        {
            classData[ClassEdit_InfectGain] = StringToInt(value);
            if (!ClassIsValidInfectGain(classData[ClassEdit_InfectGain]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "infect_gain", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "kill_bonus", false))
        {
            classData[ClassEdit_KillBonus] = StringToInt(value);
            if (!ClassIsValidKillBonus(classData[ClassEdit_KillBonus]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "kill_bonus", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "speed", false))
        {
            classData[ClassEdit_Speed] = StringToFloat(value);
            if (!ClassIsValidSpeed(classData[ClassEdit_Speed]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "speed", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "knockback", false))
        {
            classData[ClassEdit_KnockBack] = StringToFloat(value);
            if (!ClassIsValidKnockback(classData[ClassEdit_KnockBack]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "knockback", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "jump_height", false))
        {
            classData[ClassEdit_JumpHeight] = StringToFloat(value);
            if (!ClassIsValidJumpHeight(classData[ClassEdit_JumpHeight]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "jump_height", value);
                hasErrors = true;
            }
        }
        else if (StrEqual(parameter, "jump_distance", false))
        {
            classData[ClassEdit_JumpDistance] = StringToFloat(value);
            if (!ClassIsValidJumpDistance(classData[ClassEdit_JumpDistance]))
            {
                TranslationReplyToCommand(client, "Vol Invalid ClassEdit Attribute", "jump_distance", value);
                hasErrors = true;
            }
        }
    }
    
    // Return if errors.
    if (hasErrors)
    {
        return Plugin_Handled;
    }
    
    // Create class editor.
    new classEdit = VolClassEditAdd(mode, classIndex, classData, name, classErrors);
    
    // Validate index.
    if (classEdit == -1)
    {
        TranslationReplyToCommand(client, "Vol ClassEdit Full");
        return Plugin_Handled;
    }
    else if (classEdit == -2)
    {
        // Invalid class index. Should never happen because it's validated in
        // parameter parsing loop.
        ReplyToCommand(client, "Unexpected error (-2): Invalid class index");
        return Plugin_Handled;
    }
    else if (classEdit == -3)
    {
        // Invalid class attributes. Also validated in parameter parsing loop.
        ReplyToCommand(client, "Unexpected error (-3): Invalid class attribute(s)");
        return Plugin_Handled;
    }
    
    // Print message.
    TranslationReplyToCommand(client, "Vol ClassEdit Added", classEdit, name);
    
    return Plugin_Handled;
}

/**
 * Command callback for removing a class editor feature.
 */
public Action:VolClassEditRemoveCommand(client, argc)
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
        TranslationReplyToCommand(client, "Vol Syntax ClassEdit Remove");
        return Plugin_Handled;
    }
    
    new bool:recursive = false;
    decl String:classEditId[32];
    decl String:strRecursive[32];
    classEditId[0] = 0;
    
    // Get class editor id and validate it.
    GetCmdArg(1, classEditId, sizeof(classEditId));
    new classEdit = VolGetFeatureIndex(VolFeature_ClassEdit, classEditId);
    if (classEdit < 0)
    {
        TranslationReplyToCommand(client, "Vol Invalid ClassEdit Id", classEditId);
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
    
    // Remove class editor.
    VolClassEditRemove(classEdit, recursive);
    
    // Print message.
    if (recursive)
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "Yes");
    }
    else
    {
        Format(strRecursive, sizeof(strRecursive), "%t", "No");
    }
    TranslationReplyToCommand(client, "Vol ClassEdit Removed", classEdit, classEditId, strRecursive);
    
    return Plugin_Handled;
}

/**
 * Builds a translated string with syntax and parameter info for zr_vol_classedit_add command.
 *
 * @param client    Client that will see this info.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolBuildClassEditSyntaxString(client, String:buffer[], maxlen)
{
    new numCells = 0;
    buffer[0] = 0;
    
    decl String:linebuffer[255];
    linebuffer[0] = 0;
    
    SetGlobalTransTarget(client);
    
    #define VOL_SYNTAX_FORMAT_STRING "  %t\n\n"
    
    // Main syntax.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Syntax Classedit Add");
    numCells += StrCat(buffer, maxlen, linebuffer);
    numCells += StrCat(buffer, maxlen, "\n");
    
    
    // Optional parameters.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Vol Param Params");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "name=<class editor name>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param ClassEdit Name");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "mode=name|attributes\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param ClassEdit Mode");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "class=<class id>\n");
    Format(linebuffer, sizeof(linebuffer), VOL_SYNTAX_FORMAT_STRING, "Vol Param ClassEdit Class");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    // Class attributes.
    Format(linebuffer, sizeof(linebuffer), "%t\n", "Classes Editable Attributes");
    numCells += StrCat(buffer, maxlen, linebuffer);
    
    numCells += StrCat(buffer, maxlen, "alpha_initial=<alpha initial value>\n");
    numCells += StrCat(buffer, maxlen, "alpha_damaged=<alpha when damaged>\n");
    numCells += StrCat(buffer, maxlen, "alpha_damage=<damage>\n");
    numCells += StrCat(buffer, maxlen, "overlay=<overlay path>\n");
    numCells += StrCat(buffer, maxlen, "nvgs=yes|no\n");
    numCells += StrCat(buffer, maxlen, "fov=<fov>\n");
    numCells += StrCat(buffer, maxlen, "has_napalm=yes|no\n");
    //numCells += StrCat(buffer, maxlen, "immunity_mode=<mode>\n");
    //numCells += StrCat(buffer, maxlen, "immunity_amount=<amount>\n");
    numCells += StrCat(buffer, maxlen, "no_fall_damage=yes|no\n");
    numCells += StrCat(buffer, maxlen, "health_regen_interval=<health regen interval>\n");
    numCells += StrCat(buffer, maxlen, "health_regen_amount=<health regen amount>\n");
    numCells += StrCat(buffer, maxlen, "kill_bonus=<kill bonus>\n");
    numCells += StrCat(buffer, maxlen, "infect_gain=<infect gain>\n");
    numCells += StrCat(buffer, maxlen, "speed=<speed>\n");
    numCells += StrCat(buffer, maxlen, "knockback=<knockback>\n");
    numCells += StrCat(buffer, maxlen, "jump_height=<jump_height>\n");
    numCells += StrCat(buffer, maxlen, "jump_distance=<jump_distance>\n");
    
    return numCells;
}
