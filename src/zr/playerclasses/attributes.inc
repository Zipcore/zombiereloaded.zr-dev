/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          attributes.inc
 *  Type:          Core
 *  Description:   Retrieving class attributes from certain caches.
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
 * ------------------------------------
 *
 *         GENERAL ATTRIBUTES
 *
 * ------------------------------------
 */
 
/**
 * Checks if the specified class is enabled.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if it's enabled, false otherwise.
 */
stock bool:ClassIsEnabled(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Enabled];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Enabled];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Enabled];
        }
    }
    return false;
}

/**
 * Gets the team ID for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          The team ID if successful, -1 otherwise.
 */
stock ClassGetTeamID(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Team];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Team];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Team];
        }
    }
    return -1;
}

/**
 * Validates the specified team ID.
 *
 * @param teamID    Team ID to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidTeamID(teamID)
{
    if (teamID >= ZR_CLASS_TEAM_MIN || teamID <= ZR_CLASS_TEAM_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Checks if the specified class is marked as the default class for its current
 * team.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if it's default for its current team, false otherwise.
 */
stock bool:ClassGetTeamDefault(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_TeamDefault];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_TeamDefault];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_TeamDefault];
        }
    }
    return false;
}

/**
 * Gets flags for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          Class flags, or -1 if failed.
 */
stock ClassGetFlags(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Flags];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Flags];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Flags];
        }
    }
    return -1;
}

/**
 * Do bitwise compare on flags in the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param flags     Class flags to check.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if the flags are set, false otherwise or if failed.
 */
stock bool:ClassHasFlags(index, flags, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return bool:(ClassData[index][Class_Flags] & flags);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return bool:(ClassDataCache[index][Class_Flags] & flags);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return bool:(ClassPlayerCache[index][Class_Flags] & flags);
        }
    }
    return false;
}

/**
 * Validates the specified flags.
 *
 * @param flags     Flags to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidFlags(flags)
{
    if (flags >= ZR_CLASS_FLAGS_MIN || flags <= ZR_CLASS_FLAGS_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the class group required to be a member of to use the class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          Number of cells written. -1 on error.
 */
stock ClassGetGroup(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][Class_Group]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][Class_Group]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][Class_Group]);
        }
    }
    
    return -1;
}

/**
 * Validates the specified group.
 *
 * @param group     Group to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidGroup(const String:group[])
{
    if (strlen(group))
    {
        // Check if the group exist.
        if (FindAdmGroup(group) == INVALID_GROUP_ID)
        {
            return false;
        }
        return true;
    }
    
    // No group specified. Consider it valid.
    return true;
}

/**
 * Gets the class name to be displayed in the class menu.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          Number of cells written. -1 on error.
 */
stock ClassGetName(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][Class_Name]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][Class_Name]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][Class_Name]);
        }
    }
    
    return -1;
}

/**
 * Validates the specified name.
 *
 * @pram name       Name to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidName(const String:name[])
{
    if (strlen(name) >= ZR_CLASS_NAME_MIN)
    {
        // Check for reserved name keyworks. These aren't allowed as names.
        if (StrEqual(name, "all", false) ||
            StrEqual(name, "humans", false) ||
            StrEqual(name, "zombies", false) ||
            StrEqual(name, "admins", false))
        {
            return false;
        }
        return true;
    }
    return false;
}

/**
 * Gets the menu description string for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetDescription(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][Class_Description]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][Class_Description]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][Class_Description]);
        }
    }
    return -1;
}

/**
 * Validates the specified description.
 *
 * @pram description    Description to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidDescription(const String:description[])
{
    if (strlen(description) >= ZR_CLASS_DESCRIPTION_MIN)
    {
        return true;
    }
    return false;
}



/*
 * ------------------------------------
 *
 *          MODEL ATTRIBUTES
 *
 * ------------------------------------
 */

/**
 * Gets the model path string from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetModelPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][Class_ModelPath]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][Class_ModelPath]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][Class_ModelPath]);
        }
    }
    return -1;
}

/**
 * Validates the specified model.
 *
 * @pram model      Model to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidModel(const String:model[])
{
    if (strlen(model) > 0)
    {
        // Validate pre-defined settings first, then check if the file exists.
        if (StrEqual(model, "random", false) ||
            StrEqual(model, "random_public", false) ||
            StrEqual(model, "random_hidden", false) ||
            StrEqual(model, "random_admin", false) ||
            StrEqual(model, "random_mother_zombie", false) ||
            StrEqual(model, "default", false) ||
            StrEqual(model, "no_change", false) ||
            FileExists(model))
        {
            return true;
        }
    }
    return false;
}

/**
 * Gets the initial alpha value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The initial alpha value from the specified class. -1 on error.
 */
stock ClassGetAlphaInitial(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_AlphaInitial];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_AlphaInitial];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_AlphaInitial];
        }
    }
    return -1;
}

/**
 * Validates the specified alpha initial value.
 *
 * @pram alphaInitial   Alpha initial value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidAlphaInitial(alphaInitial)
{
    if (alphaInitial >= ZR_CLASS_ALPHA_INITIAL_MIN && alphaInitial <= ZR_CLASS_ALPHA_INITIAL_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the alpha value when damaged, from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The alpha value when damaged, from the specified class. -1 on
 *          error.
 */
stock ClassGetAlphaDamaged(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_AlphaDamaged];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_AlphaDamaged];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_AlphaDamaged];
        }
    }
    return -1;
}

/**
 * Validates the specified alpha damaged value.
 *
 * @pram alphaDamaged   Alpha damaged value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidAlphaDamaged(alphaDamaged)
{
    if (alphaDamaged >= ZR_CLASS_ALPHA_DAMAGED_MIN && alphaDamaged <= ZR_CLASS_ALPHA_DAMAGED_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the damage amount needed to change alpha, from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The damage amount needed to change alpha, from the specified class.
 *          -1 on error.
 */
stock ClassGetAlphaDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_AlphaDamage];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_AlphaDamage];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_AlphaDamage];
        }
    }
    return -1;
}

/**
 * Validates the specified alpha damage value.
 *
 * @pram alphaDamage    Alpha damage value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidAlphaDamage(alphaDamage)
{
    if (alphaDamage >= ZR_CLASS_ALPHA_DAMAGE_MIN && alphaDamage <= ZR_CLASS_ALPHA_DAMAGE_MAX)
    {
        return true;
    }
    return false;
}



/*
 * ------------------------------------
 *
 *           HUD ATTRIBUTES
 *
 * ------------------------------------
 */

/**
 * Gets the overlay path from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetOverlayPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][Class_OverlayPath]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][Class_OverlayPath]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][Class_OverlayPath]);
        }
    }
    return -1;    
}

/**
 * Validates the specified overlay.
 *
 * Note: File is NOT added to downloads table.
 *
 * @pram overlay    Overlay to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidOverlay(const String:overlay[])
{
    if (strlen(overlay) > 0)
    {
        // Check if the file exists.
        decl String:overlay_path[PLATFORM_MAX_PATH];
        Format(overlay_path, sizeof(overlay_path), "materials/%s.vmt", overlay);
        if (FileExists(overlay_path, true))
        {
            return true;
        }
        return false;
    }
    
    // No overlay is also valid.
    return true;
}

/**
 * Gets the night vision setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The night vision setting from the specified class. False on error.
 */
stock bool:ClassGetNvgs(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Nvgs];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Nvgs];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Nvgs];
        }
    }
    return false;
}

/**
 * Gets the field of view value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The field of view value from the specified class. -1 on error.
 */
stock ClassGetFOV(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Fov];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Fov];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Fov];
        }
    }
    return -1;
}

/**
 * Validates the specified FOV.
 *
 * @pram fov        FOV value to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidFOV(fov)
{
    if (fov >= ZR_CLASS_FOV_MIN && fov <= ZR_CLASS_FOV_MAX)
    {
        return true;
    }
    return false;
}



/*
 * ------------------------------------
 *
 *              EFFECTS
 *
 * ------------------------------------
 */

/**
 * Gets the napalm setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  True if the class has napalm grenades, false otherwise.
 */
stock bool:ClassGetHasNapalm(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_HasNapalm];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_HasNapalm];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_HasNapalm];
        }
    }
    return false;
}

/**
 * Gets the napalm grenades time from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The napalm grenades time from the specified class. -1.0 on error.
 */
stock Float:ClassGetNapalmTime(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_NapalmTime];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_NapalmTime];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_NapalmTime] * ClassGetAttributeMultiplier(index, ClassM_NapalmTime);
        }
    }
    return -1.0;
}

/**
 * Validates the specified napalm time.
 *
 * @pram napalmTime     Napalm time to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidNapalmTime(Float:napalmTime)
{
    if (napalmTime >= ZR_CLASS_NAPALM_TIME_MIN && napalmTime <= ZR_CLASS_NAPALM_TIME_MAX)
    {
        return true;
    }
    return false;
}



/*
 * ------------------------------------
 *
 *          PLAYER BEHAVIOUR
 *
 * ------------------------------------
 */

/**
 * Gets the immunity mode to the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Current immunity mode to the specified class. -1 on error.
 */
stock ClassGetImmunityMode(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_ImmunityMode];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_ImmunityMode];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_ImmunityMode];
        }
    }
    return -1;
}

/**
 * Validates the specified immunity mode.
 *
 * @pram immunityMode   Immunity mode to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidImmunityMode(immunityMode)
{
    // Not implemented yet.
    return false;
}

/**
 * Gets the immunity amount to the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Current immunity amount to the specified class. -1.0 on error.
 */
stock Float:ClassGetImmunityAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_ImmunityAmount];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_ImmunityAmount];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_ImmunityAmount];
        }
    }
    return -1.0;
}

/**
 * Validates the specified immunity amount.
 *
 * @pram immunityAmount     Immunity amount to validate.
 * @return                  True if valid, false otherwise.
 */
stock bool:ClassIsValidImmunityAmount(immunityAmount)
{
    // Not implemented yet.
    return false;
}

/**
 * Gets the no fall damage setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The no fall damage setting from the specified class. False on
 * error.
 */
stock bool:ClassGetNoFallDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_NoFallDamage];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_NoFallDamage];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_NoFallDamage];
        }
    }
    return false;
}

/**
 * Gets the weapon profile index from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Weapon profile index for the specified class. -1 if weapon module
 *          is disabled, or on error.
 */
stock ClassGetWeaponProfile(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    new bool:weaponsloaded = ConfigIsConfigLoaded(File_Weapons);
    
    if (weapons && weaponsloaded)
    {
        switch (cachetype)
        {
            case ZR_CLASS_CACHE_ORIGINAL:
            {
                return ClassData[index][Class_WeaponProfile];
            }
            case ZR_CLASS_CACHE_MODIFIED:
            {
                return ClassDataCache[index][Class_WeaponProfile];
            }
            case ZR_CLASS_CACHE_PLAYER:
            {
                return ClassPlayerCache[index][Class_WeaponProfile];
            }
        }
    }
    return -1;
}

/**
 * Validates the specified weapon profile index.
 *
 * @pram weaponProfile  Weapon profile index to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidWeaponProfile(weaponProfile)
{
    if (WeaponProfilesIsValidIndex(weaponProfile))
    {
        return true;
    }
    return false;
}

/**
 * Gets the health points from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Health points from the specified class. -1 on error.
 */
stock ClassGetHealth(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Health];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Health];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][Class_Health] * ClassGetAttributeMultiplier(index, ClassM_Health));
        }
    }
    return -1;
}

/**
 * Validates the specified health value.
 *
 * @pram health     Health value to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidHealth(health)
{
    if (health >= ZR_CLASS_HEALTH_MIN && health <= ZR_CLASS_HEALTH_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the health regen interval time from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health regen interval time from the specified class. -1.0 on
 *          error.
 */
stock Float:ClassGetHealthRegenInterval(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_HealthRegenInterval];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_HealthRegenInterval];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_HealthRegenInterval] * ClassGetAttributeMultiplier(index, ClassM_HealthRegenInterval);
        }
    }
    return -1.0;
}

/**
 * Validates the specified health regeneration interval.
 *
 * @pram regenInterval  Regen interval to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidRegenInterval(Float:regenInterval)
{
    if (regenInterval >= ZR_CLASS_REGEN_INTERVAL_MIN && regenInterval <= ZR_CLASS_REGEN_INTERVAL_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the health regen amount value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health regen amount value from the specified class. -1 on
 *          error.
 */
stock ClassGetHealthRegenAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_HealthRegenAmount];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_HealthRegenAmount];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][Class_HealthRegenAmount] * ClassGetAttributeMultiplier(index, ClassM_HealthRegenAmount));
        }
    }
    return -1;
}

/**
 * Validates the specified health regeneration amount.
 *
 * @pram regenAmount    Regen amount to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidRegenAmount(regenAmount)
{
    if (regenAmount >= ZR_CLASS_REGEN_AMOUNT_MIN && regenAmount <= ZR_CLASS_REGEN_AMOUNT_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the health infect gain value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health infect gain value from the specified class. -1 on
 *          error.
 */
stock ClassGetHealthInfectGain(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_HealthInfectGain];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_HealthInfectGain];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][Class_HealthInfectGain] * ClassGetAttributeMultiplier(index, ClassM_HealthInfectGain));
        }
    }
    return -1;
}

/**
 * Validates the specified health infection gain value.
 *
 * @pram infectGain     Regen interval to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidInfectGain(infectGain)
{
    if (infectGain >= ZR_CLASS_HEALTH_INFECT_GAIN_MIN && infectGain <= ZR_CLASS_HEALTH_INFECT_GAIN_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the kill bonus points from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The kill bonus points from the specified class.
 */
stock ClassGetKillBonus(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_KillBonus];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_KillBonus];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_KillBonus];
        }
    }
    return -1;
}

/**
 * Validates the specified kill bonus.
 *
 * @pram killBonus      Kill bonus to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidKillBonus(killBonus)
{
    if (killBonus >= ZR_CLASS_KILL_BONUS_MIN && killBonus <= ZR_CLASS_KILL_BONUS_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the running speed value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The running speed value from the specified class. -1.0 on error.
 */
stock Float:ClassGetSpeed(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_Speed];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_Speed];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_Speed] * ClassGetAttributeMultiplier(index, ClassM_Speed);
        }
    }
    return -1.0;
}

/**
 * Validates the specified speed.
 *
 * @pram speed      Speed to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:ClassIsValidSpeed(Float:speed)
{
    new Float:min;
    new Float:max;
    switch (ClassSpeedMethod)
    {
        case ClassSpeed_LMV:
        {
            min = ZR_CLASS_SPEED_LMV_MIN;
            max = ZR_CLASS_SPEED_LMV_MAX;
        }
        case ClassSpeed_Prop:
        {
            min = ZR_CLASS_SPEED_PROP_MIN;
            max = ZR_CLASS_SPEED_PROP_MAX;
        }
        default:
        {
            // This should never happen.
            return false;
        }
    }
    if (speed >= min && speed <= max)
    {
        return true;
    }
    return false;
}

/**
 * Gets the knock back boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The knock back boost from the specified class. 0.0 on error.
 */
stock Float:ClassGetKnockback(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_KnockBack];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_KnockBack];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_KnockBack] * ClassGetAttributeMultiplier(index, ClassM_Knockback);
        }
    }
    return 0.0;
}

/**
 * Validates the specified knock back value.
 *
 * @pram knockback      Knock back value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidKnockback(Float:knockback)
{
    if (knockback >= ZR_CLASS_KNOCKBACK_MIN && knockback <= ZR_CLASS_KNOCKBACK_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the jump height boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The jump height boost from the specified class. -1.0 on error.
 */
stock Float:ClassGetJumpHeight(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_JumpHeight];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_JumpHeight];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_JumpHeight] * ClassGetAttributeMultiplier(index, ClassM_JumpHeight);
        }
    }
    return -1.0;
}

/**
 * Validates the specified jump height value.
 *
 * @pram jumpHeight     Jump height value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidJumpHeight(Float:jumpHeight)
{
    if (jumpHeight >= ZR_CLASS_JUMP_HEIGHT_MIN && jumpHeight <= ZR_CLASS_JUMP_HEIGHT_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the jump distance boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The jump distance boost from the specified class. -1.0 on error.
 */
stock Float:ClassGetJumpDistance(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][Class_JumpDistance];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][Class_JumpDistance];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][Class_JumpDistance] * ClassGetAttributeMultiplier(index, ClassM_JumpDistance);
        }
    }
    return -1.0;
}

/**
 * Validates the specified jump distance value.
 *
 * @pram jumpDistanc    Jump distance value to validate.
 * @return              True if valid, false otherwise.
 */
stock bool:ClassIsValidJumpDistance(Float:jumpDistance)
{
    if (jumpDistance >= ZR_CLASS_JUMP_DISTANCE_MIN && jumpDistance <= ZR_CLASS_JUMP_DISTANCE_MAX)
    {
        return true;
    }
    return false;
}

/**
 * Gets the attribute flag that represent the specified attribute.
 *
 * @param attributename     The attribute name.
 * @return                  The flag that represent the specified attribute.
 *                          -1 on error.
 */
stock ClassAttributeNameToFlag(const String:attributename[])
{
    // Check attribute names.
    if (StrEqual(attributename, "enabled", false))
    {
        return ZR_CLASS_ENABLED;
    }
    else if (StrEqual(attributename, "team", false))
    {
        return ZR_CLASS_TEAM;
    }
    else if (StrEqual(attributename, "team_default", false))
    {
        return ZR_CLASS_TEAM_DEFAULT;
    }
    else if (StrEqual(attributename, "flags", false))
    {
        return ZR_CLASS_FLAGS;
    }
    else if (StrEqual(attributename, "group", false))
    {
        return ZR_CLASS_GROUP;
    }
    else if (StrEqual(attributename, "name", false))
    {
        return ZR_CLASS_NAME;
    }
    else if (StrEqual(attributename, "description", false))
    {
        return ZR_CLASS_DESCRIPTION;
    }
    else if (StrEqual(attributename, "model_path", false))
    {
        return ZR_CLASS_MODEL_PATH;
    }
    else if (StrEqual(attributename, "alpha_initial", false))
    {
        return ZR_CLASS_ALPHA_INITIAL;
    }
    else if (StrEqual(attributename, "alpha_damaged", false))
    {
        return ZR_CLASS_ALPHA_DAMAGED;
    }
    else if (StrEqual(attributename, "alpha_damage", false))
    {
        return ZR_CLASS_ALPHA_DAMAGE;
    }
    else if (StrEqual(attributename, "overlay_path", false))
    {
        return ZR_CLASS_OVERLAY_PATH;
    }
    else if (StrEqual(attributename, "nvgs", false))
    {
        return ZR_CLASS_NVGS;
    }
    else if (StrEqual(attributename, "fov", false))
    {
        return ZR_CLASS_FOV;
    }
    else if (StrEqual(attributename, "has_napalm", false))
    {
        return ZR_CLASS_HAS_NAPALM;
    }
    else if (StrEqual(attributename, "napalm_time", false))
    {
        return ZR_CLASS_NAPALM_TIME;
    }
    else if (StrEqual(attributename, "immunity_mode", false))
    {
        return ZR_CLASS_IMMUNITY_MODE;
    }
    else if (StrEqual(attributename, "immunity_amount", false))
    {
        return ZR_CLASS_IMMUNITY_AMOUNT;
    }
    else if (StrEqual(attributename, "no_fall_damage", false))
    {
        return ZR_CLASS_NO_FALL_DAMAGE;
    }
    else if (StrEqual(attributename, "weapon_profile", false))
    {
        return ZR_CLASS_WEAPON_PROFILE;
    }
    else if (StrEqual(attributename, "health", false))
    {
        return ZR_CLASS_HEALTH;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ZR_CLASS_HEALTH_REGEN_INTERVAL;
    }
    else if (StrEqual(attributename, "health_regen_amount", false))
    {
        return ZR_CLASS_HEALTH_REGEN_AMOUNT;
    }
    else if (StrEqual(attributename, "health_infect_gain", false))
    {
        return ZR_CLASS_HEALTH_INFECT_GAIN;
    }
    else if (StrEqual(attributename, "kill_bonus", false))
    {
        return ZR_CLASS_KILL_BONUS;
    }
    else if (StrEqual(attributename, "speed", false))
    {
        return ZR_CLASS_SPEED;
    }
    else if (StrEqual(attributename, "knockback", false))
    {
        return ZR_CLASS_KNOCKBACK;
    }
    else if (StrEqual(attributename, "jump_height", false))
    {
        return ZR_CLASS_JUMP_HEIGHT;
    }
    else if (StrEqual(attributename, "jump_distance", false))
    {
        return ZR_CLASS_JUMP_DISTANCE;
    }
    
    // Invalid attribute name.
    return -1;
}

/**
 * Converts a attribute name to a multiplier type.
 *
 * @param attributename     The attribute name.
 * @return                  The multiplier that represent the specified attribute.
 *                          ClassM_Invalid if failed.
 */
stock ClassMultipliers:ClassAttributeNameToMultiplier(const String:attributename[])
{
    // Check attribute names.
    if (StrEqual(attributename, "napalm_time", false))
    {
        return ClassM_NapalmTime;
    }
    else if (StrEqual(attributename, "health", false))
    {
        return ClassM_Health;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ClassM_HealthRegenInterval;
    }
    else if (StrEqual(attributename, "health_regen_amount", false))
    {
        return ClassM_HealthRegenAmount;
    }
    else if (StrEqual(attributename, "health_infect_gain", false))
    {
        return ClassM_HealthInfectGain;
    }
    else if (StrEqual(attributename, "speed", false))
    {
        return ClassM_Speed;
    }
    else if (StrEqual(attributename, "knockback", false))
    {
        return ClassM_Knockback;
    }
    else if (StrEqual(attributename, "jump_height", false))
    {
        return ClassM_JumpHeight;
    }
    else if (StrEqual(attributename, "jump_distance", false))
    {
        return ClassM_JumpDistance;
    }
    
    // Invalid attribute name or not a multiplier.
    return ClassM_Invalid;
}

/**
 * Returns the datatype used in the specified attribute.
 *
 * @param attributeflag     A flag specifying the attribute to check.
 * @return      The data type used in the specified attribute, or
 *              ClassType_InvalidType if failed.
 */
stock ClassDataTypes:ClassGetAttributeType(attributeflag)
{
    switch (attributeflag)
    {
        // Boolean.
        case ZR_CLASS_ENABLED,
              ZR_CLASS_NVGS,
              ZR_CLASS_HAS_NAPALM,
              ZR_CLASS_NO_FALL_DAMAGE:
        {
            return ClassDataType_Boolean;
        }
        
        // Integer.
        case ZR_CLASS_FLAGS,
              ZR_CLASS_ALPHA_INITIAL,
              ZR_CLASS_ALPHA_DAMAGED,
              ZR_CLASS_ALPHA_DAMAGE,
              ZR_CLASS_FOV,
              ZR_CLASS_IMMUNITY_MODE,
              ZR_CLASS_HEALTH,
              ZR_CLASS_HEALTH_REGEN_AMOUNT,
              ZR_CLASS_HEALTH_INFECT_GAIN,
              ZR_CLASS_KILL_BONUS:
        {
            return ClassDataType_Integer;
        }
        
        // Float.
        case ZR_CLASS_NAPALM_TIME,
              ZR_CLASS_IMMUNITY_AMOUNT,
              ZR_CLASS_HEALTH_REGEN_INTERVAL,
              ZR_CLASS_SPEED,
              ZR_CLASS_KNOCKBACK,
              ZR_CLASS_JUMP_HEIGHT,
              ZR_CLASS_JUMP_DISTANCE:
        {
            return ClassDataType_Float;
        }
        
        // String.
        case ZR_CLASS_GROUP,
              ZR_CLASS_NAME,
              ZR_CLASS_DESCRIPTION,
              ZR_CLASS_MODEL_PATH,
              ZR_CLASS_OVERLAY_PATH,
              ZR_CLASS_WEAPON_PROFILE:      // Weapon profile is marked as string because the name must be resolved.
        {
            return ClassDataType_String;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return ClassDataType_InvalidType;
}
