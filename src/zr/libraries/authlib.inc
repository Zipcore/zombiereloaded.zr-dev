/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          authlib.inc
 *  Type:          Library
 *  Description:   Various utilities for authentication and authorization.
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

#if defined _authlib_included
 #endinput
#endif
#define _authlib_included

/**
 * Max size of group list.
 */
const AUTHLIB_MAX_GROUPS = 64;

/**
 * Size of group name buffer.
 */
const AUTHLIB_GROUP_LEN = 64;

/**
 * Size of flag name buffer.
 */
const AUTHLIB_FLAG_LEN = 16;

/**
 * Authorization modes.
 */
enum Auth_Modes
{
    Auth_Either,    // Match either flag or group.
    Auth_Both,      // Match both flag and group.
    Auth_All        // Match all flags and all groups.
}


/**
 * Returns whether a player is admin or not.
 *
 * @param client        The client index.
 *
 * @return              True if admin, false otherwise.
 */
stock bool:Auth_IsClientAdmin(client)
{
    return GetUserAdmin(client) != INVALID_ADMIN_ID;
}

/**
 * Returns whether a player is in the specified group.
 *
 * @param client        The client index.
 * @param groupName     Group name to check.
 *
 * @return              True if in the group, false otherwise.
 */
stock bool:Auth_IsClientInGroup(client, const String:groupName[])
{
    new AdminId:id = GetUserAdmin(client);
    
    if (id == INVALID_ADMIN_ID)
    {
        // Not admin.
        return false;
    }
    
    // Get number of groups.
    new count = GetAdminGroupCount(id);
    decl String:currentGroupName[64];
    
    if (count > 0)
    {
        // Loop through each group.
        for (new group = 0; group < count; group++)
        {
            // Get group name.
            GetAdminGroup(id, group, currentGroupName, sizeof(currentGroupName));
            
            // Compare names.
            if (StrEqual(groupName, groupName, false))
            {
                return true;
            }
        }
    }
    
    // No groups or no match.
    return false;
}

/**
 * Returns whether a player is in one or more groups.
 *
 * @param client        The client index.
 * @param groupList     List of group names to check, separated by a
 *                      separator string.
 * @param separator     Separator string used to separate groups in the list.
 * @param mode          Optional. Comparision mode. Valid values:
 *                          Auth_Either - Client must be in any group.
 *                          Auth_All    - Client must be in all groups.
 *                      Default is Auth_Either.
 *                      Note: Invalid mode will return false.
 *
 * @return              True if in a group, false otherwise.
 */
stock bool:Auth_IsClientInGroups(client, const String:groupList[], const String:separator[], Auth_Modes:mode = Auth_Either)
{
    if (!Auth_IsClientAdmin(client))
    {
        // Not admin.
        return false;
    }
    
    if (!(mode == Auth_Either || mode == Auth_All))
    {
        // Invalid comparision mode.
        return false;
    }
    
    // Initialize filter value.
    // Starts with false for Auth_Either and true for Auth_All.
    new bool:filterPassed = (mode == Auth_All);
    
    // Split group list.
    decl String:groups[AUTHLIB_MAX_GROUPS][AUTHLIB_GROUP_LEN];
    new count = ExplodeString(groupList, separator, groups, AUTHLIB_MAX_GROUPS, AUTHLIB_GROUP_LEN);
    
    // Loop through each group in the list.
    for (new group = 0; group < count; group++)
    {
        new bool:inGroup = Auth_IsClientInGroup(client, groups[group]);
        if (inGroup && mode == Auth_Either)
        {
            filterPassed = true;
            break;
        }
        else if (!inGroup && mode == Auth_All)
        {
            filterPassed = false;
            break;
        }
    }
    
    return filterPassed && count > 0;
}

/**
 * Returns whether a player has a flag or not.
 *
 * @param client    The client index.
 * @param flagName  Flag to check.
 *
 * @return          True if client has the flag, false otherwise.
 */
stock bool:Auth_ClientHasFlag(client, const String:flagName[])
{
    new AdminId:id = GetUserAdmin(client);
    
    if (id == INVALID_ADMIN_ID)
    {
        // Not admin.
        return false;
    }
    
    new flags = GetAdminFlags(id, Access_Effective);
    new AdminFlag:flag;
    
    // Get flag.
    if (FindFlagByName(flagName, flag))
    {
        // Compare flag bits.
        if (flags & FlagToBit(flag))
        {
            return true;
        }
    }
    
    // No match or invalid flag name.
    return false;
}

/**
 * Returns whether a player has one or more of the specified flags.
 *
 * @param client        The client index.
 * @param flagList      List of SourceMod flag names to check, separated by a
 *                      separator string.
 * @param separator     Separator string used to separate flags in the list.
 * @param mode          Optional. Comparision mode. Valid values:
 *                          Auth_Either - Client must have any of the flags.
 *                          Auth_All    - Client must have all flags.
 *                      Default is Auth_Either.
 *                      Note: Invalid mode will return false.
 *
 * @return              True if client has a flag, false otherwise.
 */
stock bool:Auth_ClientHasFlags(client, const String:flagList[], const String:separator[], Auth_Modes:mode = Auth_Either)
{
    if (!Auth_IsClientAdmin(client))
    {
        // Not admin.
        return false;
    }
    
    if (!(mode == Auth_Either || mode == Auth_All))
    {
        // Invalid comparision mode.
        return false;
    }
    
    // Initialize filter value.
    // Starts with false for Auth_Either and true for Auth_All.
    new bool:filterPassed = (mode == Auth_All);
    
    // Split flag list.
    decl String:flags[AdminFlags_TOTAL][AUTHLIB_FLAG_LEN];
    new count = ExplodeString(flagList, separator, flags, AdminFlags_TOTAL, AUTHLIB_FLAG_LEN);
    
    // Loop through each flag in the list.
    for (new flag = 0; flag < count; flag++)
    {
        new bool:hasFlag = Auth_ClientHasFlag(client, flags[flag]);
        if (hasFlag && mode == Auth_Either)
        {
            filterPassed = true;
            break;
        }
        else if (!hasFlag && mode == Auth_All)
        {
            filterPassed = false;
            break;
        }
    }
    
    return filterPassed && count > 0;
}

/**
 * Returns whether a client is authorized or not according to a list of groups
 * and flags.
 *
 * @param client        The client index.
 * @param groupList     List of SourceMod group names to check, separated by the
 *                      separator string.
 * @param flagList      List of SourceMod flag names to check, separated by the
 *                      separator string.
 * @param separator     Separator string used to separate list entries.
 * @param mode          Optional. Flag and group comparision mode. Default is
 *                      either; any group or any flag in the lists.
 *                      Note: Invalid mode will return false.
 * @param allowRoot     Optional. Always give access to root admins. Default is
 *                      true.
 *
 * @return              True if authorized, false otherwise.
 */
stock bool:Auth_IsClientAuthorized(client, const String:groupList[], const String:flagList[], const String:separator[], Auth_Modes:mode = Auth_Either, bool:allowRoot = true)
{
    if (!Auth_IsClientAdmin(client))
    {
        // Not admin.
        return false;
    }
    
    if (allowRoot && Auth_ClientHasFlag(client, "root"))
    {
        // Root admins always get access.
        return true;
    }
    
    new Auth_Modes:filterMode = mode;
    
    // Convert to a valid filter mode.
    if (filterMode == Auth_Both)
    {
        filterMode = Auth_Either;
    }
    
    new bool:groupPassed = Auth_IsClientInGroups(client, groupList, separator, filterMode);
    new bool:flagPassed = Auth_ClientHasFlags(client, flagList, separator, filterMode);
    
    switch (mode)
    {
        case Auth_Either:
        {
            return groupPassed || flagPassed;
        }
        case Auth_Both, Auth_All:
        {
            return groupPassed && flagPassed;
        }
    }
    
    // Invalid comparision mode.
    return false;
}
