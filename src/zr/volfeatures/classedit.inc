/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volclassedit.inc
 *  Type:           Module
 *  Description:    Class editor volumetric feature.
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
 * The player's selected class index. Used by volumes in "name" mode.
 */
new VolClassEditSelectedClass[MAXPLAYERS + 1];

/**
 * Returns whether the specified class editor is in use.
 *
 * @param dataIndex     Class editor to check.
 * @return              True if in use, false otherwise.
 */
bool:VolClassEditInUse(dataIndex)
{
    return VolClassEditData[dataIndex][VolClassEdit_InUse];
}

/**
 * Validates the specified class edit index.
 *
 * @param dataIndex     Index to validate.
 * @return              True if valid, false otherwise.
 */
bool:VolClassEditIsValidIndex(dataIndex)
{
    if (dataIndex >= 0 && dataIndex < ZR_CLASSEDIT_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns whether a class editor is valid and in use.
 *
 * @param dataIndex     Class editor index to validate.
 * @return              True if valid and in use, false otherwise. 
 */
bool:VolClassEditIsValid(dataIndex)
{
    return VolClassEditIsValidIndex(dataIndex) && VolClassEditInUse(dataIndex);
}

/**
 * Returns whether the specified class editor is attached to a volume.
 *
 * @param dataIndex     Class editor to check.
 * @return              True if attached, false otherwise.
 */
bool:VolClassEditIsAttached(dataIndex)
{
    return (VolClassEditData[dataIndex][VolClassEdit_VolumeIndex] >= 0);
}

/**
 * Gets the volume the class editor is linked to.
 *
 * @param dataIndex     Class editor to check.
 * @return              Volume index.
 */
VolClassEditGetVolumeIndex(dataIndex)
{
    return VolClassEditData[dataIndex][VolClassEdit_VolumeIndex];
}

/**
 * Sets the volume index to a class editor.
 *
 * @param dataIndex     Class editor data index.
 * @param volumeIndex   New volume index.
 */
VolClassEditSetVolumeIndex(dataIndex, volumeIndex)
{
    VolClassEditData[dataIndex][VolClassEdit_VolumeIndex] = volumeIndex;
}

/**
 * Gets the first free data index.
 *
 * @return  Data index or -1 if failed.
 */
VolClassEditGetFreeIndex()
{
    // Loop through all indexes.
    for (new dataindex = 0; dataindex < ZR_VOLUMES_MAX; dataindex++)
    {
        // Check if unused.
        if (!VolClassEditInUse(dataindex))
        {
            // Mark as in use.
            VolClassEditData[dataindex][VolClassEdit_InUse] = true;
            ClassEditCount++;
            
            // Unused index found.
            return dataindex;
        }
    }
    
    // Unused index not found.
    return -1;
}

/**
 * Resets the specified class edit volume.
 *
 * @param dataIndex         Class editor.
 * @param decrementCounter  Optional. Decrement class editor counter by one.
 *                          Default is true.
 */
VolClassEditReset(dataIndex, bool:decrementCounter = true)
{
    VolClassEditData[dataIndex][VolClassEdit_InUse]         = VOL_CLASSEDIT_DEF_IN_USE;
    strcopy(VolClassEditData[dataIndex][VolClassEdit_Name], VOL_NAME_LEN, VOL_CLASSEDIT_DEF_NAME);
    VolClassEditData[dataIndex][VolClassEdit_VolumeIndex]   = VOL_CLASSEDIT_DEF_VOLUME_INDEX;
    VolClassEditData[dataIndex][VolClassEdit_Mode]          = VOL_CLASSEDIT_DEF_MODE;
    VolClassEditData[dataIndex][VolClassEdit_ClassIndex]    = VOL_CLASSEDIT_DEF_CLASS_INDEX;
    VolClassEditClassAttributes[dataIndex] = VolEmptyAttributes;
    
    if (decrementCounter)
    {
        ClassEditCount--;
    }
}

/**
 * Initialize class editor.
 */
VolClassEditInit()
{
    for (new dataindex = 0; dataindex < ZR_VOLUMES_MAX; dataindex++)
    {
        VolClassEditReset(dataindex, false);
    }
    
    ClassEditCount = 0;
}

/**
 * Dumps data to be used by zr_vol_list command.
 *
 * @param dataIndex     Index in anticamp data array.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
VolClassEditBuildList(dataIndex, String:buffer[], maxlen)
{
    #define CLASSEDIT_DUMP_FORMAT "%30s %s\n"
    
    decl String:linebuffer[128];
    decl String:langbuffer[128];
    decl String:valuebuffer[256];
    new cache[VolTypeClassEdit];
    new cellswritten;
    
    // Validate index.
    if (!VolIsValidIndex(dataIndex))
    {
        return 0;
    }
    
    // Initialize and clear buffer.
    buffer[0] = 0;
    
    // Cache data.
    cache = VolClassEditData[dataIndex];
    
    VolClassEditModeToString(cache[VolClassEdit_Mode], valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Mode:", valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);
    
    switch (cache[VolClassEdit_Mode])
    {
        case ClassEditMode_Name:
        {
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Name");
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, cache[VolClassEdit_Name]);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
        }
        case ClassEditMode_Attributes:
        {
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Alpha Initial");
            VolClassEditIntToString(dataIndex, ClassEdit_AlphaInitial, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Alpha Damaged");
            VolClassEditIntToString(dataIndex, ClassEdit_AlphaDamaged, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Alpha Damage");
            VolClassEditIntToString(dataIndex, ClassEdit_AlphaDamage, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Overlay");
            VolClassEditStringToHumanStr(dataIndex, ClassEdit_OverlayPath, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib NVGs");
            VolClassEditIntToString(dataIndex, ClassEdit_Nvgs, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib FOV");
            VolClassEditIntToString(dataIndex, ClassEdit_Fov, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Has Napalm");
            VolClassEditIntToString(dataIndex, ClassEdit_HasNapalm, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Napalm Time");
            VolClassEditFloatToString(dataIndex, ClassEdit_NapalmTime, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Immunity Mode");
            VolClassEditIntToString(dataIndex, ClassEdit_ImmunityMode, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Immunity Amount");
            VolClassEditFloatToString(dataIndex, ClassEdit_ImmunityAmount, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib No Fall Damage");
            VolClassEditIntToString(dataIndex, ClassEdit_NoFallDamage, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Regen Interval");
            VolClassEditFloatToString(dataIndex, ClassEdit_RegenInterval, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Regen Amount");
            VolClassEditIntToString(dataIndex, ClassEdit_RegenAmount, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Kill Bonus");
            VolClassEditIntToString(dataIndex, ClassEdit_KillBonus, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Infect Gain");
            VolClassEditIntToString(dataIndex, ClassEdit_InfectGain, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Speed");
            VolClassEditFloatToString(dataIndex, ClassEdit_Speed, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Knockback");
            VolClassEditFloatToString(dataIndex, ClassEdit_KnockBack, valuebuffer, sizeof(valuebuffer), ZR_CLASS_KNOCKBACK_IGNORE);
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Jump Height");
            VolClassEditFloatToString(dataIndex, ClassEdit_JumpHeight, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
            
            Format(langbuffer, sizeof(langbuffer), "%t:", "Classes Attrib Jump Distance");
            VolClassEditFloatToString(dataIndex, ClassEdit_JumpDistance, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, langbuffer, valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
        }
    }
    
    return cellswritten;
}


/**************************************
 *                                    *
 *  EVENTS                            *
 *                                    *
 **************************************/

/**
 * Applies new class attributes to the player.
 *
 * @param client        The client index.
 * @param dataIndex     Local data index.
 */
VolClassEditApply(client, dataIndex)
{
    switch (VolClassEditData[dataIndex][VolClassEdit_Mode])
    {
        case ClassEditMode_Name:
        {
            new classindex = VolClassEditData[dataIndex][VolClassEdit_ClassIndex];
            
            // Save player's selected class.
            VolClassEditSelectedClass[client] = ClassGetActiveIndex(client);
            
            // Update cache with new attributes.
            ClassReloadPlayerCache(client, classindex);
        }
        case ClassEditMode_Attributes:
        {
            // Update player's entire cache with attributes from the specified
            // class attributes.
            VolClassEditUpdateAttributes(client, dataIndex);
        }
    }
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Applied class data on client %d.", client);
    
    // Apply the updated attributes.
    ClassApplyAttributes(client);
}

/**
 * Restores the player's regular class attributes (from modified cache).
 *
 * @param client        The client index.
 */
VolClassEditRestore(client, dataIndex)
{
    new classindex = ClassGetActiveIndex(client);
    
    switch (VolClassEditData[dataIndex][VolClassEdit_Mode])
    {
        case ClassEditMode_Name:
        {
            // Restore player's entire cache with attributes from the selected
            // class.
            ClassReloadPlayerCache(client, VolClassEditSelectedClass[client]);
        }
        case ClassEditMode_Attributes:
        {
            // Restore individual attributes, if specified.
            VolClassEditRestoreAttributes(client, classindex, dataIndex);
        }
    }
    
    // Apply the restored attributes.
    if (ClassApplyAttributes(client))
    {
        LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Restored class data on client %d.", client);
    }
}

/**
 * Event callback. A player entered a class edit volume.
 *
 * @param client        The client index.
 * @param dataIndex     Class editor index.
 */
VolClassEditOnPlayerEnter(client, dataIndex)
{
    VolClassEditApply(client, dataIndex);
}

/**
 * Event callback. A player left a class edit volume.
 *
 * @param client        The client index.
 * @param dataIndex     Class editor index.
 */
VolClassEditOnPlayerLeave(client, dataIndex)
{
    VolClassEditRestore(client, dataIndex);
}

/**
 * Event callback. A class edit volume was disabled.
 *
 * @param dataIndex     Class editor index.
 */
VolClassEditOnDisabled(dataIndex)
{
    new volumeIndex = VolClassEditGetVolumeIndex(dataIndex);
    
    // Trigger leave event on all players in the class editor's volume.
    for (new client = 1; client <= MaxClients; client++)
    {
        if (IsClientConnected(client) && IsClientInGame(client))
        {
            // Forward event if the player is in the volume.
            if (VolPlayerInVolume[client][volumeIndex])
            {
                VolClassEditOnPlayerLeave(client, dataIndex);
            }
        }
    }
}


/**************************************
 *                                    *
 *  ATTRIBUTE FUNCTIONS               *
 *                                    *
 **************************************/

/**
 * Converts the specified mode to a string.
 *
 * @param mode      Mode to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
VolClassEditModeToString(VolClassEditMode:mode, String:buffer[], maxlen)
{
    switch (mode)
    {
        case ClassEditMode_Name:
        {
            return Format(buffer, maxlen, "%t", "Vol Classedit Mode Name");
        }
        case ClassEditMode_Attributes:
        {
            return Format(buffer, maxlen, "%t", "Vol Classedit Mode Attrib");
        }
    }
    
    return 0;
}

/**
 * Gets a integer attribute and converts it to a human readable string.
 * 
 * Note: attribute is assumed to be a integer (cell) and is not type cheked!
 *
 * @param dataIndex     Local data index.
 * @param attribute     Attribute to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
VolClassEditIntToString(dataIndex, ClassEditableAttributes:attribute, String:buffer[], maxlen)
{
    new intVal;
    new String:strVal[8];
    
    intVal = VolClassEditClassAttributes[dataIndex][attribute];
    
    // Check if the attribute is marked as ignored.
    if (intVal == -1)
    {
        return Format(buffer, maxlen, "%t", "Vol Classedit No Change");
    }
    else
    {
        IntToString(intVal, strVal, sizeof(strVal));
        return strcopy(buffer, maxlen, strVal);
    }
}

/**
 * Gets a float attribute and converts it to a human readable string.
 *
 * Note: attribute is assumed to be a float and is not type cheked!
 *
 * @param dataIndex     Local data index.
 * @param attribute     Attribute to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
VolClassEditFloatToString(dataIndex, ClassEditableAttributes:attribute, String:buffer[], maxlen, Float:minval = -1.0)
{
    new Float:floatVal;
    new String:strVal[8];
    
    floatVal = Float:VolClassEditClassAttributes[dataIndex][attribute];
    
    // Check if the attribute is marked as ignored.
    if (floatVal == minval)
    {
        return Format(buffer, maxlen, "%t", "Vol Classedit No Change");
    }
    else
    {
        FloatToString(floatVal, strVal, sizeof(strVal));
        return strcopy(buffer, maxlen, strVal);
    }
}

/**
 * Gets a string attribute and converts it to a human readable string.
 *
 * Note: attribute is assumed to be a string and is not type cheked!
 *
 * @param dataIndex     Local data index.
 * @param attribute     Attribute to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
VolClassEditStringToHumanStr(dataIndex, ClassEditableAttributes:attribute, String:buffer[], maxlen)
{
    decl String:strVal[PLATFORM_MAX_PATH];
    strcopy(strVal, sizeof(strVal), String:VolClassEditClassAttributes[dataIndex][attribute]);
    
    // Check if the attribute is marked as ignored.
    if (StrEqual(strVal, "nochange", false))
    {
        return Format(buffer, maxlen, "%t", "Vol Classedit No Change");
    }
    else
    {
        return strcopy(buffer, maxlen, strVal);
    }
}

/**
 * Updates a player's cache with the specified attribute set. Only active
 * attributes are applied.
 *
 * Note: These attributes are not validated!
 *
 * @param client        The client index.
 * @param dataIndex     Class editor index.
 * @return              Number of attributes changed.
 */
VolClassEditUpdateAttributes(client, dataIndex)
{
    new numChanges;
    new attributes[ClassEditableAttributes];
    attributes = VolClassEditClassAttributes[dataIndex];
    
    // Alpha initial.
    if (attributes[ClassEdit_AlphaInitial] > -1)
    {
        ClassPlayerCache[client][Class_AlphaInitial] = attributes[ClassEdit_AlphaInitial];
        numChanges++;
    }
    
    // Alpha damaged.
    if (attributes[ClassEdit_AlphaDamaged] > -1)
    {
        ClassPlayerCache[client][Class_AlphaDamaged] = attributes[ClassEdit_AlphaDamaged];
        numChanges++;
    }
    
    // Alpha damage.
    if (attributes[ClassEdit_AlphaDamage] > -1)
    {
        ClassPlayerCache[client][Class_AlphaDamage] = attributes[ClassEdit_AlphaDamage];
        numChanges++;
    }
    
    // Overlay path.
    if (!StrEqual(attributes[ClassEdit_OverlayPath], "nochange"))
    {
        strcopy(ClassPlayerCache[client][Class_OverlayPath], PLATFORM_MAX_PATH, attributes[ClassEdit_OverlayPath]);
        numChanges++;
    }
    
    // Nvgs.
    if (attributes[ClassEdit_Nvgs] > -1)
    {
        ClassPlayerCache[client][Class_Nvgs] = bool:attributes[ClassEdit_Nvgs];
        numChanges++;
    }
    
    // FOV.
    if (attributes[ClassEdit_Fov] > -1)
    {
        ClassPlayerCache[client][Class_Fov] = attributes[ClassEdit_Fov];
        numChanges++;
    }
    
    // Napalm time.
    if (attributes[ClassEdit_NapalmTime] > -1.0)
    {
        ClassPlayerCache[client][Class_NapalmTime] = attributes[ClassEdit_NapalmTime];
        numChanges++;
    }
    
    // Immunity mode.
    if (attributes[ClassEdit_ImmunityMode] > -1)
    {
        ClassPlayerCache[client][Class_ImmunityMode] = attributes[ClassEdit_ImmunityMode];
        numChanges++;
    }
    
    // Immunity amount.
    if (attributes[ClassEdit_ImmunityAmount] > -1.0)
    {
        ClassPlayerCache[client][Class_ImmunityAmount] = attributes[ClassEdit_ImmunityAmount];
        numChanges++;
    }
    
    // No fall damage.
    if (attributes[ClassEdit_NoFallDamage] > -1)
    {
        ClassPlayerCache[client][Class_NoFallDamage] = bool:attributes[ClassEdit_NoFallDamage];
        numChanges++;
    }
    
    // Health regen interval.
    if (attributes[ClassEdit_RegenInterval] > -1.0)
    {
        ClassPlayerCache[client][Class_HealthRegenInterval] = attributes[ClassEdit_RegenInterval];
        numChanges++;
    }
    
    // Health regen amount.
    if (attributes[ClassEdit_RegenAmount] > -1)
    {
        ClassPlayerCache[client][Class_HealthRegenAmount] = attributes[ClassEdit_RegenAmount];
        numChanges++;
    }
    
    // Infect gain.
    if (attributes[ClassEdit_InfectGain] > -1)
    {
        ClassPlayerCache[client][Class_HealthInfectGain] = attributes[ClassEdit_InfectGain];
        numChanges++;
    }
    
    // Kill bonus.
    if (attributes[ClassEdit_KillBonus] > -1)
    {
        ClassPlayerCache[client][Class_KillBonus] = attributes[ClassEdit_KillBonus];
        numChanges++;
    }
    
    // Speed.
    if (attributes[ClassEdit_Speed] > -1.0)
    {
        ClassPlayerCache[client][Class_Speed] = attributes[ClassEdit_Speed];
        numChanges++;
    }
    
    // Knock back.
    if (attributes[ClassEdit_KnockBack] > ZR_CLASS_KNOCKBACK_IGNORE)
    {
        ClassPlayerCache[client][Class_KnockBack] = attributes[ClassEdit_KnockBack];
        numChanges++;
    }
    
    // Jump height.
    if (attributes[ClassEdit_JumpHeight] > -1.0)
    {
        ClassPlayerCache[client][Class_JumpHeight] = attributes[ClassEdit_JumpHeight];
        numChanges++;
    }
    
    // Jump distance.
    if (attributes[ClassEdit_JumpDistance] > -1.0)
    {
        ClassPlayerCache[client][Class_JumpDistance] = attributes[ClassEdit_JumpDistance];
        numChanges++;
    }
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Applied %d attribute(s).", numChanges);
    return numChanges;
}

/**
 * Restores a player's cache to the original values (from modified cache). A
 * attribute set is used as a mask to determine what attributes to restore.
 *
 * @param client        The client index.
 * @param classindex    Index of class to restore.
 * @param dataIndex     Class editor index.
 * @return              Number of attributes changed.
 */
VolClassEditRestoreAttributes(client, classindex, dataIndex)
{
    new numChanges;
    new attributes[ClassEditableAttributes];
    attributes = VolClassEditClassAttributes[dataIndex];
    
    // Alpha initial.
    if (attributes[ClassEdit_AlphaInitial] > -1)
    {
        ClassPlayerCache[client][Class_AlphaInitial] = ClassGetAlphaInitial(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Alpha damaged.
    if (attributes[ClassEdit_AlphaDamaged] > -1)
    {
        ClassPlayerCache[client][Class_AlphaDamaged] = ClassGetAlphaDamaged(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Alpha damage.
    if (attributes[ClassEdit_AlphaDamage] > -1)
    {
        ClassPlayerCache[client][Class_AlphaDamage] = ClassGetAlphaDamage(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Overlay path.
    if (!StrEqual(attributes[ClassEdit_OverlayPath], "nochange"))
    {
        decl String:path[PLATFORM_MAX_PATH];
        ClassGetOverlayPath(classindex, path, sizeof(path), ZR_CLASS_CACHE_MODIFIED);
        strcopy(ClassPlayerCache[client][Class_OverlayPath], PLATFORM_MAX_PATH, path);
        numChanges++;
    }
    
    // Nvgs.
    if (attributes[ClassEdit_Nvgs] > -1)
    {
        ClassPlayerCache[client][Class_Nvgs] = ClassGetNvgs(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // FOV.
    if (attributes[ClassEdit_Fov] > -1)
    {
        ClassPlayerCache[client][Class_Fov] = ClassGetFOV(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Napalm time.
    if (attributes[ClassEdit_NapalmTime] > -1.0)
    {
        ClassPlayerCache[client][Class_NapalmTime] = ClassGetNapalmTime(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Immunity mode.
    if (attributes[ClassEdit_ImmunityMode] > -1)
    {
        ClassPlayerCache[client][Class_ImmunityMode] = ClassGetImmunityMode(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Immunity amount.
    if (attributes[ClassEdit_ImmunityAmount] > -1.0)
    {
        ClassPlayerCache[client][Class_ImmunityAmount] = ClassGetImmunityAmount(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // No fall damage.
    if (attributes[ClassEdit_NoFallDamage] > -1)
    {
        ClassPlayerCache[client][Class_NoFallDamage] = ClassGetNoFallDamage(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Health regen interval.
    if (attributes[ClassEdit_RegenInterval] > -1.0)
    {
        ClassPlayerCache[client][Class_HealthRegenInterval] = ClassGetHealthRegenInterval(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Health regen amount.
    if (attributes[ClassEdit_RegenAmount] > -1)
    {
        ClassPlayerCache[client][Class_HealthRegenAmount] = ClassGetHealthRegenAmount(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Infect gain.
    if (attributes[ClassEdit_InfectGain] > -1)
    {
        ClassPlayerCache[client][Class_HealthInfectGain] = ClassGetHealthInfectGain(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Kill bonus.
    if (attributes[ClassEdit_KillBonus] > -1)
    {
        ClassPlayerCache[client][Class_KillBonus] = ClassGetKillBonus(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Speed.
    if (Float:attributes[ClassEdit_Speed] > -1.0)
    {
        ClassPlayerCache[client][Class_Speed] = ClassGetSpeed(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Knock back.
    if (attributes[ClassEdit_KnockBack] > ZR_CLASS_KNOCKBACK_IGNORE)
    {
        ClassPlayerCache[client][Class_KnockBack] = ClassGetKnockback(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Jump height.
    if (attributes[ClassEdit_JumpHeight] > -1.0)
    {
        ClassPlayerCache[client][Class_JumpHeight] = ClassGetJumpHeight(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    // Jump distance.
    if (attributes[ClassEdit_JumpDistance] > -1.0)
    {
        ClassPlayerCache[client][Class_JumpDistance] = ClassGetJumpDistance(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }
    
    LogEvent(_, LogTypeOld_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Restored %d attribute(s).", numChanges);
    return numChanges;
}
