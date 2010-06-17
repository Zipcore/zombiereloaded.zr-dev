/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          filtertools.inc
 *  Type:          Core 
 *  Description:   Class system tools; validating, getting indexes or lists
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
 * Validates the team requirements in a class cache and check that theres at
 * least one class for each team. Minium team requirements are zombies and
 * humans. The admin team is optinal and not validated.
 *
 * @param cachetype Optional. Specifies what class cache to validate. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL (default, unchanged class data),
 *                  ZR_CLASS_CACHE_MODIFIED (modified class data).
 * @return          True if validation was successful, false otherwise.
 */
stock bool:ClassValidateTeamRequirements(cachetype = ZR_CLASS_CACHE_ORIGINAL)
{
    new zombieindex;
    new humanindex;
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Test if a zombie and human class was found.
    zombieindex = ClassGetFirstClass(ZR_CLASS_TEAM_ZOMBIES, _, cachetype);
    humanindex = ClassGetFirstClass(ZR_CLASS_TEAM_HUMANS, _, cachetype);
    
    // Validate indexes.
    if (ClassValidateIndex(zombieindex) && ClassValidateIndex(humanindex))
    {
        return true;
    }
    
    return false;
}

/**
 * Validates that there's a class marked as team default for each team.
 *
 * @param cachetype Optional. Specifies what class cache to validate. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL (default, unchanged class data),
 *                  ZR_CLASS_CACHE_MODIFIED (modified class data).
 * @return          True if validation was successful, false otherwise.
 */
stock bool:ClassValidateTeamDefaults(cachetype = ZR_CLASS_CACHE_ORIGINAL)
{
    new zombieindex;
    new humanindex;
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Test if a default zombie and human class was found.
    zombieindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ZOMBIES, _, cachetype);
    humanindex = ClassGetDefaultClass(ZR_CLASS_TEAM_HUMANS, _, cachetype);
    
    // Validate indexes.
    if (ClassValidateIndex(zombieindex) && ClassValidateIndex(humanindex))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Validates the specified attributes.
 *
 * @param onlyEditable          Only validate editable attributes, skip others.
 * @param logErrors             Log warnings to error log if there are invalid attributes.
 * @param classIndex
 * @return                      Flags with attributes that failed validation. 0 if successful.
 */
ClassValidateAttributes(bool:onlyEditable = false, bool:logErrors = false,
                        classIndex,
                        team,
                        classFlags,
                        const String:group[],
                        const String:name[],
                        const String:description[],
                        const String:model[],
                        alphaInitial,
                        alphaDamaged,
                        alphaDamage,
                        const String:overlay[],
                        fov,
                        Float:napalmTime,
                        weaponProfile,
                        health,
                        Float:regenInterval,
                        regenAmount,
                        infectGain,
                        killBonus,
                        Float:speed,
                        Float:knockback,
                        Float:jumpHeight,
                        Float:jumpDistance)
{
    new flags;
    
    // Team.
    if (!onlyEditable && !ClassIsValidTeamID(team))
    {
        flags += ZR_CLASS_TEAM;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid team at index %d: %d", classIndex, team);
        }
    }
    
    // Class flags.
    if (!onlyEditable && !ClassIsValidFlags(classFlags))
    {
        flags += ZR_CLASS_FLAGS;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid flags at index %d: %d", classIndex, classFlags);
        }
    }
    
    // Group.
    if (!onlyEditable && strlen(group) > 0)
    {
        if (!ClassIsValidGroup(group))
        {
            flags += ZR_CLASS_GROUP;
            if (logErrors)
            {
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid group at index %d: \"%s\"", classIndex, group);
            }
        }
    }
    
    // Name.
    if (!onlyEditable && !ClassIsValidName(name))
    {
        flags += ZR_CLASS_NAME;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid name at index %d: \"%s\"", classIndex, name);
        }
    }
    
    // Description.
    if (!onlyEditable && !ClassIsValidDescription(description))
    {
        flags += ZR_CLASS_DESCRIPTION;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid description at index %d: \"%s\"", classIndex, description);
        }
    }
    
    // Model path.
    if (!onlyEditable && !ClassIsValidModel(model))
    {
        flags += ZR_CLASS_MODEL_PATH;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid model_path at index %d: \"%s\"", classIndex, model);
        }
    }
    
    // Alpha, initial.
    if (!ClassIsValidAlphaInitial(alphaInitial))
    {
        flags += ZR_CLASS_ALPHA_INITIAL;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid alpha_inital at index %d: %d", classIndex, alphaInitial);
        }
    }
    
    // Alpha, damaged.
    if (!ClassIsValidAlphaDamaged(alphaDamaged))
    {
        flags += ZR_CLASS_ALPHA_DAMAGED;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid alpha_damaged at index %d: %d", classIndex, alphaDamaged);
        }
    }
    
    // Alpha, damage.
    if (!ClassIsValidAlphaDamage(alphaDamage))
    {
        flags += ZR_CLASS_ALPHA_DAMAGE;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid alpha_damage at index %d: %d", classIndex, alphaDamage);
        }
    }
    
    // Overlay path.
    decl String:overlayPath[PLATFORM_MAX_PATH];
    overlayPath[0] = 0;
    Format(overlayPath, sizeof(overlayPath), "materials/%s.vmt", overlay);
    if (!ClassIsValidOverlay(overlay))
    {
        flags += ZR_CLASS_OVERLAY_PATH;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid overlay_path at index %d: \"%s\"", classIndex, overlayPath);
        }
    }
    
    // Field of view.
    if (!ClassIsValidFOV(fov))
    {
        flags += ZR_CLASS_FOV;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid fov at index %d: %d", classIndex, fov);
        }
    }
    
    // Napalm time.
    if (!ClassIsValidNapalmTime(napalmTime))
    {
        flags += ZR_CLASS_NAPALM_TIME;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid napalm_time at index %d: %0.2f", classIndex, napalmTime);
        }
    }
    
    // Immunity mode (not implemented).
    
    // Immunity amount (not implemented).
    
    // Weapon profile. Only validate if weapons module is enabled and the class
    // is a human class.
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    new bool:weaponsLoaded = ConfigIsConfigLoaded(File_Weapons);
    if (weapons && weaponsLoaded && team == ZR_CLASS_TEAM_HUMANS)
    {
        if (!ClassIsValidWeaponProfile(weaponProfile))
        {
            flags += ZR_CLASS_WEAPON_PROFILE;
            if (logErrors)
            {
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid weapon_profile at index %d.", classIndex);
            }
        }
    }
    
    // Health.
    if (!onlyEditable && !ClassIsValidHealth(health))
    {
        flags += ZR_CLASS_HEALTH;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid health at index %d: %d", classIndex, health);
        }
    }
    
    // Health regen interval.
    if (!ClassIsValidRegenInterval(regenInterval))
    {
        flags += ZR_CLASS_HEALTH_REGEN_INTERVAL;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid health_regen_interval at index %d: %0.2f", classIndex, regenInterval);
        }
    }
    
    // Health regen amount.
    if (!ClassIsValidRegenAmount(regenAmount))
    {
        flags += ZR_CLASS_HEALTH_REGEN_AMOUNT;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid health_regen_amount at index %d: %d", classIndex, regenAmount);
        }
    }
    
    // Health infect gain.
    if (!ClassIsValidInfectGain(infectGain))
    {
        flags += ZR_CLASS_HEALTH_INFECT_GAIN;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid health_infect_gain at index %d: %d", classIndex, infectGain);
        }
    }
    
    // Kill bonus.
    if (!ClassIsValidKillBonus(killBonus))
    {
        flags += ZR_CLASS_KILL_BONUS;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid kill_bonus at index %d: %d", classIndex, killBonus);
        }
    }
    
    // Speed.
    if (!ClassIsValidSpeed(speed))
    {
        flags += ZR_CLASS_SPEED;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid speed at index %d: %0.2f", classIndex, speed);
        }
    }
    
    // Knockback.
    if (!ClassIsValidKnockback(knockback))
    {
        flags += ZR_CLASS_KNOCKBACK;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid knockback at index %d: %0.2f", classIndex, knockback);
        }
    }
    
    // Jump height.
    if (!ClassIsValidJumpHeight(jumpHeight))
    {
        flags += ZR_CLASS_JUMP_HEIGHT;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid jump_height at index %d: %0.2f", classIndex, jumpHeight);
        }
    }
    
    // Jump distance.
    if (!ClassIsValidJumpHeight(jumpDistance))
    {
        flags += ZR_CLASS_JUMP_DISTANCE;
        if (logErrors)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid jump_distance at index %d: %0.2f", classIndex, jumpDistance);
        }
    }
    
    return flags;
}
                        

/**
 * Validates all the class attributes in the original class data array, to
 * check if they have invalid values. Boolean settings are not validated.
 *
 * @param classIndex    The index of the class to validate.
 * @param logErrors     Optional. Log invalid attributes. Default is false.
 * @return              A value with attribute error flags.
 */
ClassValidateClassAttributes(classIndex, bool:logErrors = false)
{
    // TODO: Validate immunity mode and amount.
    
    new attributes[ClassAttributes];
    attributes = ClassData[classIndex];
    
    new flags = ClassValidateAttributes(false, logErrors,
                    classIndex,
                    attributes[Class_Team],
                    attributes[Class_Flags],
                    attributes[Class_Group],
                    attributes[Class_Name],
                    attributes[Class_Description],
                    attributes[Class_ModelPath],
                    attributes[Class_AlphaInitial],
                    attributes[Class_AlphaDamaged],
                    attributes[Class_AlphaDamage],
                    attributes[Class_OverlayPath],
                    attributes[Class_Fov],
                    attributes[Class_NapalmTime],
                    attributes[Class_WeaponProfile],
                    attributes[Class_Health],
                    attributes[Class_HealthRegenInterval],
                    attributes[Class_HealthRegenAmount],
                    attributes[Class_HealthInfectGain],
                    attributes[Class_KillBonus],
                    attributes[Class_Speed],
                    attributes[Class_KnockBack],
                    attributes[Class_JumpHeight],
                    attributes[Class_JumpDistance]);
    
    // If overlay is valid.
    if (!(flags && ZR_CLASS_OVERLAY_PATH))
    {
        decl String:overlayPath[PLATFORM_MAX_PATH];
        Format(overlayPath, sizeof(overlayPath), "materials/%s.vmt", attributes[Class_OverlayPath]);
        AddFileToDownloadsTable(overlayPath);
    }
    
    return flags;
}

/**
 * Validates a set of editable attributes.
 *
 * @param attributes    Attribute set to validate.
 * @return              0 if successful, or a bit field (positivie number) of
 *                      failed attributes.
 */
stock ClassValidateEditableAttributes(attributes[ClassEditableAttributes])
{
    new flags = ClassValidateAttributes(true, false,
                    0,
                    0,
                    0,
                    "",
                    "",
                    "",
                    "",
                    attributes[Class_AlphaInitial],
                    attributes[Class_AlphaDamaged],
                    attributes[Class_AlphaDamage]
                    attributes[Class_OverlayPath],
                    attributes[Class_Fov]
                    attributes[Class_NapalmTime],
                    attributes[Class_WeaponProfile],
                    0,
                    attributes[Class_HealthRegenInterval],
                    attributes[Class_HealthRegenAmount],
                    attributes[Class_HealthInfectGain],
                    attributes[Class_KillBonus],
                    attributes[Class_Speed],
                    attributes[Class_KnockBack],
                    attributes[Class_JumpHeight],
                    attributes[Class_JumpDistance]);
    
    return flags;
}

/**
 * Checks if the specified class index is a valid index.
 *
 * @param classindex    The class index to validate.
 * @return              True if the class exist, false otherwise.
 */
stock bool:ClassValidateIndex(classindex)
{
    if (classindex >= 0 && classindex < ClassCount)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Compares the class team ID with a team ID.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param teamid    The team ID to compare with the class.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if equal, false otherwise.
 */
stock bool:ClassTeamCompare(index, teamid, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            if (ClassData[index][Class_Team] == teamid)
            {
                return true;
            }
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            if (ClassDataCache[index][Class_Team] == teamid)
            {
                return true;
            }
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            if (ClassPlayerCache[index][Class_Team] == teamid)
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * Gets the first class index of a class with the specified name (not a case
 * sensitive search).
 *
 * @param name      The name to search for.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 * @return          The class index if successful, -1 otherwise.
 */
stock ClassGetIndex(const String:name[], cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    decl String:current_name[64];

    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        // Get its name and compare it with the specified class name.
        ClassGetName(classindex, current_name, sizeof(current_name), cachetype);
        if (strcmp(name, current_name, false) == 0)
        {
            return classindex;
        }
    }
    
    // The class index wasn't found.
    return -1;
}

/**
 * Gets the currently active class index that the player is using.
 * Note: Does not check if the player is dead.
 *
 * @param client    The client index.
 * @return  The active class index. -1 on error or if a spectactor.
 */
stock ClassGetActiveIndex(client)
{
    new teamid;
    
    if (!ZRIsClientOnTeam(client))
    {
        // No active team.
        return -1;
    }
    
    // Check if the player currently is in admin mode.
    if (ClassPlayerInAdminMode[client])
    {
        teamid = ZR_CLASS_TEAM_ADMINS;
    }
    else
    {
        // Not in admin mode, check if player is human or zombie.
        if (InfectIsClientHuman(client))
        {
            teamid = ZR_CLASS_TEAM_HUMANS;
        }
        else
        {
            teamid = ZR_CLASS_TEAM_ZOMBIES;
        }
    }
    
    // Return the active class for the active team.
    return ClassSelected[client][teamid];
}

/**
 * Gets the multiplier for the specified team and attribute.
 *
 * @param client        The client index.
 * @param attribute     Specifies what attribute multiplier to get.
 * @return      Multiplier for the specified team and attribute. 1.0 if the
 *              client is in admin mode.
 */
stock Float:ClassGetAttributeMultiplier(client, ClassMultipliers:attribute)
{
    new teamid;
    
    // Check if player is not in admin mode.
    if (!ClassPlayerInAdminMode[client])
    {
        // Not in admin mode, check if player is human or zombie.
        if (InfectIsClientHuman(client))
        {
            teamid = ZR_CLASS_TEAM_HUMANS;
        }
        else
        {
            teamid = ZR_CLASS_TEAM_ZOMBIES;
        }
        
        // Get multiplier for the specified team and attribute.
        return Float:ClassMultiplierCache[teamid][attribute];
    }
    else
    {
        // Do not use multipliers on admin classes.
        return 1.0;
    }
}

/**
 * Check if a class pass the specified filter.
 * 
 * @param index         Index of the class in a class cache or a client index,
 *                      depending on the cache type specified.
 * @param filter        Structure with filter settings.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 *                      ZR_CLASS_CACHE_PLAYER - Player cache. If this one is
 *                      used index will be used as a client index.
 * @return              True if passed, false otherwise.
 */
stock bool:ClassFilterMatch(index, filter[ClassFilter], cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Check if the class is disabled and the enabled attribute is NOT ignored.
    if (!filter[ClassFilter_IgnoreEnabled] && !ClassIsEnabled(index, cachetype))
    {
        return false;
    }
    
    // Check if class flags pass the flag filter.
    if (!ClassFlagFilterMatch(index, filter[ClassFilter_RequireFlags], filter[ClassFilter_DenyFlags], cachetype))
    {
        return false;
    }
    
    // Get class group name.
    decl String:groupname[64];
    groupname[0] = 0;
    ClassGetGroup(index, groupname, sizeof(groupname), cachetype);
    
    // Check if a client is specified in the filter.
    new client = filter[ClassFilter_Client];
    if (ZRIsClientValid(client))
    {
        // Check if a group is set on the class.
        if (strlen(groupname))
        {
            // Check if the client is not a member of that group.
            if (!ZRIsClientInGroup(client, groupname))
            {
                return false;
            }
        }
    }
    
    // Check if classes with groups are set to be excluded.
    if (client < 0)
    {
        // Exclude class if it has a group name.
        if (strlen(groupname))
        {
            return false;
        }
    }
    
    // The class passed the filter.
    return true;
}

/**
 * Check if a class pass the specified flag filters.
 * 
 * @param index         Index of the class in a class cache or a client index,
 *                      depending on the cache type specified.
 * @param require       Class flags to require. 0 for no filter.
 * @param deny          Class flags to exclude. 0 for no filter.
 * @param cachetype     Specifies what class cache to read from. Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 *                      ZR_CLASS_CACHE_PLAYER - Player cache. If this one is
 *                      used index will be used as a client index.
 * @return              True if passed, false otherwise.
 */
stock bool:ClassFlagFilterMatch(index, require, deny, cachetype)
{
    new flags;
    new bool:requirepassed = false;
    new bool:denypassed = false;
    
    // Do quick check for optimization reasons: Check if no flags are specified.
    if (require == 0 && deny == 0)
    {
        return true;
    }
    
    // Cache flags.
    flags = ClassGetFlags(index, cachetype);
    
    // Match require filter.
    if (require == 0 || flags & require)
    {
        // All required flags are set.
        requirepassed = true;
    }
    
    // Match deny filter.
    if (deny == 0 || !(flags & deny))
    {
        // No denied flags are set.
        denypassed = true;
    }
    
    // Check if required and denied flags passed the filter.
    if (requirepassed && denypassed)
    {
        // The class pass the filter.
        return true;
    }
    
    // The class didn't pass the filter.
    return false;
}

/**
 * Decides whether a class selection menu should be enabled. The decision is
 * based on zr_class_allow_* console variables.
 *
 * @param team  Optional. Team ID to match. Default is all.
 * @return      True if allowed, false otherwise.
 */
bool:ClassAllowSelection(client, team = -1)
{
    // Get selection settings.
    new bool:zombie = GetConVarBool(g_hCvarsList[CVAR_CLASSES_ZOMBIE_SELECT]);
    new bool:human = GetConVarBool(g_hCvarsList[CVAR_CLASSES_HUMAN_SELECT]);
    new bool:admin = GetConVarBool(g_hCvarsList[CVAR_CLASSES_ADMIN_SELECT]);
    
    // Since admin mode classes are optional they must be counted to verify
    // that they exist.
    new bool:adminexist;
    
    // Check if player is admin.
    new bool:isadmin = ZRIsClientAdmin(client);
    
    // Only count admin mode classes if client is admin for better performance.
    if (isadmin)
    {
        adminexist = ClassCountTeam(ZR_CLASS_TEAM_ADMINS) > 0;
    }
    
    // Check if a team id is specified.
    if (team >= 0)
    {
        // Check team and return the corresponding selection setting.
        switch (team)
        {
            case ZR_CLASS_TEAM_ZOMBIES:
            {
                return zombie;
            }
            case ZR_CLASS_TEAM_HUMANS:
            {
                return human;
            }
            case ZR_CLASS_TEAM_ADMINS:
            {
                // Player must be admin to select admin mode classes.
                return admin && isadmin && adminexist;
            }
        }
        
        // Team ID didn't match.
        return false;
    }
    else
    {
        // Check zombie and human.
        return zombie || human;
    }
}

/**
 * Gets all class indexes or from a specified team, and adds them to the
 * specified array.
 * 
 * @param array             The destination array to add class indexes.
 * @param teamfilter        Optional. The team ID to filter. A negative value
 *                          for no filter (default).
 * @param filter            Optional. Structure with filter settings.
 * @param cachetype         Optional. Specifies what class cache to read from.
 *                          Options:
 *                          ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                          ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                          class data.
 * @return  True on success. False on error or if no classes were added or
 *          found.
 */
stock bool:ClassAddToArray(Handle:array, teamfilter = -1, filter[ClassFilter] = ClassNoFilter, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Validate the array.
    if (array == INVALID_HANDLE)
    {
        return false;
    }
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Store a local boolean that says if the user specified a team filter or not.
    new bool:hasteamfilter = bool:(teamfilter >= 0);
    new classesadded;
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        // Validate filter settings.
        if (!ClassFilterMatch(classindex, filter, cachetype))
        {
            // The class is didn't pass the filter, skip class.
            continue;
        }
        
        // Check team filtering.
        if (hasteamfilter)
        {
            // Only add classes with matching team ID.
            if (ClassGetTeamID(classindex, cachetype) == teamfilter)
            {
                // Team ID match. Add class index to array.
                PushArrayCell(array, classindex);
                classesadded++;
            }
        }
        else
        {
            // No filter. Add any class to the array.
            PushArrayCell(array, classindex);
            classesadded++;
        }
    }
    
    if (classesadded)
    {
        return true;
    }
    else
    {
        // No classes were found/added.
        return false;
    }
}

/**
 * Counts total classes or classes in the specified team.
 *
 * @param teamfilter        Optional. The team ID to filter. Negative value for
 *                          no filter (default).
 * @param filter            Optional. Structure with filter settings.
 * @param cachetype         Optional. Specifies what class cache to read from.
 *                          Options:
 *                          ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                          ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                          class data.
 * @return  Number of total classes or classes in the specified team.
 */
stock ClassCountTeam(teamfilter = -1, filter[ClassFilter] = ClassNoFilter, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return 0;
    }
    
    // Store a local boolean that says if the user specified a team filter or not.
    new bool:hasteamfilter = bool:(teamfilter >= 0);
    new count;
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        // Validate filter settings.
        if (!ClassFilterMatch(classindex, filter, cachetype))
        {
            // The class is didn't pass the filter, skip class.
            continue;
        }
        
        // Check team filtering.
        if (hasteamfilter)
        {
            // Only add classes with matching team ID.
            if (ClassGetTeamID(classindex, cachetype) == teamfilter)
            {
                // Team ID match. Increment counter.
                count++;
            }
        }
        else
        {
            // No filter. Increment counter.
            count++;
        }
    }
    
    // Return number of classes found.
    return count;
}

/**
 * Gets a random class index from a specified team or from all classes.
 *
 * @param teamfilter        Optional. The team ID to filter. A negative value
 *                          for no filter (default).
 * @param filter            Optional. Structure with filter settings.
 * @param cachetype         Optional. Specifies what class cache to read from.
 *                          Options:
 *                          ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                          ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                          class data.
 * @return  The class index if successful, or -1 on error.
 */
stock ClassGetRandomClass(teamfilter = -1, filter[ClassFilter] = ClassNoSpecialClasses, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    new Handle:classarray;
    new arraycount;
    new randnum;
    new buffer;
    
    classarray = CreateArray();
    
    // Try to get a class list.
    if (ClassAddToArray(classarray, teamfilter, filter, cachetype))
    {
        // Get a random index from the new class array.
        arraycount = GetArraySize(classarray);
        randnum = GetRandomInt(0, arraycount - 1);
        
        // Return the value at the random index.
        buffer = GetArrayCell(classarray, randnum);
        CloseHandle(classarray);
        return buffer;
    }
    else
    {
        // Failed to get a random class.
        CloseHandle(classarray);
        return -1;
    }
}

/**
 * Gets the first class index, or the first class index with the specified team
 * ID.
 *
 * @param teamfilter        Optional. The team ID to filter. A negative value
 *                          for no filter (default).
 * @param filter            Optional. Structure with filter settings.
 * @param cachetype         Optional. Specifies what class cache to read from.
 *                          Options:
 *                          ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                          ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                          class data.
 * @return  The first class index, or the first class index with the specified
 *          team ID. -1 on error.
 */
stock ClassGetFirstClass(teamfilter = -1, filter[ClassFilter] = ClassNoSpecialClasses, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    new bool:hasteamfilter = bool:(teamfilter >= 0);
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        // Validate filter settings.
        if (!ClassFilterMatch(classindex, filter, cachetype))
        {
            // The class is didn't pass the filter, skip class.
            continue;
        }
        
        if (hasteamfilter)
        {
            if (teamfilter == ClassGetTeamID(classindex, cachetype))
            {
                // Team ID match. Return the class index.
                return classindex;
            }
        }
        else
        {
            // No team filter. Return the class index.
            return classindex;
        }
    }
    
    return -1;
}

/**
 * Gets the first class marked as default for the specified team.
 *
 * @param teamid            The team ID.
 * @param filter            Optional. Structure with filter settings. Default
 *                          is to deny classes with special flags
 *                          (ZR_CLASS_SPECIALFLAGS).
 * @param cachetype         Optional. Specifies what class cache to read from.
 *                          Options:
 *                          ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                          ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                          class data.
 * @return  The first default class index. -1 on error.
 */
stock ClassGetDefaultClass(teamid, filter[ClassFilter] = ClassNoSpecialClasses, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    new Handle:classarray;
    new arraycount;
    new classindex;
    
    classarray = CreateArray();
    
    // Get all classes from the specified team.
    if (!ClassAddToArray(classarray, teamid, filter, cachetype))
    {
        // Failed to get classes.
        CloseHandle(classarray);
        return -1;
    }
    
    // Loop through all classes and return the first class marked as team default.
    arraycount = GetArraySize(classarray);
    for (new i = 0; i < arraycount; i++)
    {
        // Get class index from the array.
        classindex = GetArrayCell(classarray, i);
        
        // Check if the current class is marked as team default.
        if (ClassGetTeamDefault(classindex, cachetype))
        {
            // Default class found.
            CloseHandle(classarray);
            return classindex;
        }
    }
    
    CloseHandle(classarray);
    return -1;
}

/**
 * Gets the default class index for the specified team configured to be used
 * when players join the server.
 *
 * @param teamid    The team ID.
 * @param filter    Optional. Structure with filter settings. Default is to
 *                  deny classes with special flags (ZR_CLASS_SPECIALFLAGS).
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 * @return  The class index of the default class for the specified team if
 *          successful. -1 on critical errors. Otherwise it will try to fall
 *          back on the first class in the specified team.
 */
stock ClassGetDefaultSpawnClass(teamid, filter[ClassFilter] = ClassNoSpecialClasses, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    decl String:classname[64];
    new classindex;
    
    // Get the default class name from the correct CVAR depending on teamid.
    switch (teamid)
    {
        case ZR_CLASS_TEAM_ZOMBIES:
        {
            GetConVarString(g_hCvarsList[CVAR_CLASSES_DEFAULT_ZOMBIE], classname, sizeof(classname));
        }
        case ZR_CLASS_TEAM_HUMANS:
        {
            GetConVarString(g_hCvarsList[CVAR_CLASSES_DEFAULT_HUMAN], classname, sizeof(classname));
        }
        case ZR_CLASS_TEAM_ADMINS:
        {
            GetConVarString(g_hCvarsList[CVAR_CLASSES_DEFAULT_ADMIN_MODE], classname, sizeof(classname));
        }
        default:
        {
            // Invalid team ID.
            return -1;
        }
    }
    
    // Check if the class name isn't empty.
    if (strlen(classname) > 0)
    {
        // Check if the user set "random" as default class.
        if (StrEqual(classname, "random", false))
        {
            // Get a list of all classes with the specified team ID. Deny
            // classes with special flags.
            classindex = ClassGetRandomClass(teamid, filter, cachetype);
            
            // Validate the result, in case there were errors.
            if (ClassValidateIndex(classindex))
            {
                return classindex;
            }
            else
            {
                // Invalid index. The ClassGetRandomClass function is pretty
                // failsafe. So if we can't get a class index here, it's a
                // critical error. No reason to fall back on other solutions.
                return -1;
            }
        }
        else
        {
            // The user set a spesific class.
            
            // Try to get the class index with the specified class name.
            classindex = ClassGetIndex(classname, cachetype);
            
            // Validate the class index and check if the team IDs match.
            if (ClassValidateIndex(classindex) && (teamid == ClassGetTeamID(classindex, cachetype)))
            {
                return classindex;
            }
            else
            {
                // The class index is invalid or the team IDs didn't match.
                // Because it's user input, we'll fall back to the first class
                // in the specified team, and log a warning.
                classindex = ClassGetFirstClass(teamid, filter, cachetype);
                
                LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Default Spawn Class", "Warning: Failed to set \"%s\" as default spawn class for team %d. The class doesn't exist or the team IDs doesn't match. Falling back to the first class in the team.", classname, teamid);
                
                // Validate the new index.
                if (ClassValidateIndex(classindex))
                {
                    // Log a warning.
                    //LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Default Spawn Class", "Warning: The default class name \"%s\" does not exist or matches the team ID.", classname);
                    return classindex;
                }
                else
                {
                    // Something went wrong. This is a critical error. There's
                    // probably missing classes with no special flags set.
                    return -1;
                }
            }
        }
    }
    else
    {
        // Blank class name, get the default class and return the index.
        return ClassGetDefaultClass(teamid, filter, cachetype);
    }
}
