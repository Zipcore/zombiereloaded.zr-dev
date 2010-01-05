/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weaponprofiles.inc
 *  Description:   Weapon profiles.
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

#include "zr/weapons/weaponprofileheaders.h"

/**
 * Weapon profile data storage.
 */
new WeaponProfiles[WEAPON_PROFILES_MAX][WeaponProfile];

/**
 * Number of active weapon profiles.
 */
new WeaponProfileCount;

/**
 * Loads or reloads all ammo profiles.
 */
WeaponProfilesLoad()
{
    new Handle:kvWeaponProfiles = INVALID_HANDLE;
    
    // Register config file.
    ConfigRegisterConfig(File_WeaponProfiles, Structure_Keyvalue, CONFIG_FILE_ALIAS_WEAPONPROF);
    
    // Get models file path.
    decl String:weaponProfilesPath[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_WEAPONPROF, weaponProfilesPath);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Missing weapon profiles: \"%s\"", weaponProfilesPath);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_WeaponProfiles, weaponProfilesPath);
    
    // Prepare key/value structure.
    kvWeaponProfiles = CreateKeyValues(CONFIG_FILE_ALIAS_WEAPONPROF);
    
    // Log that ammo profiles file is loading.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Loading weapon profiles from file \"%s\".", weaponProfilesPath);
    
    // Load ammo profiles file.
    FileToKeyValues(kvWeaponProfiles, weaponProfilesPath);
    
    // Try to find the first profile.
    KvRewind(kvWeaponProfiles);
    if (!KvGotoFirstSubKey(kvWeaponProfiles))
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Can't find any weapon profiles in \"%s\"", weaponProfilesPath);
    }
    
    //decl String:buffer[256];
    decl String:name[32];
    decl String:priAmmoProfile[32];
    decl String:secAmmoProfile[32];
    //decl String:restrictProfile[32];
    
    new priAmmoIndex;
    new secAmmoIndex;
    //new restrictIndex;
    
    WeaponProfileCount = 0;
    new failedCount;
    
    // Loop through all profiles and store attributes in WeaponProfiles array.
    do
    {
        if (WeaponProfileCount > WEAPON_PROFILES_MAX)
        {
            // Maximum number of profiles reached. Log a warning and exit the loop.
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Warning: Maximum number of weapon profiles reached (%d). Skipping other profiles.", WEAPON_PROFILES_MAX + 1);
            
            break;
        }
        
        // Name.
        if (KvGetSectionName(kvWeaponProfiles, name, sizeof(name)) && strlen(name))
        {
            strcopy(WeaponProfiles[WeaponProfileCount][WeaponProfile_Name], 32, name);
        }
        else
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Warning: Empty name string at index %d.", WeaponProfileCount + failedCount);
            failedCount++;
            continue;
        }
        
        // Primary ammo profile.
        KvGetString(kvWeaponProfiles, "primary_ammo_profile", priAmmoProfile, sizeof(priAmmoProfile));
        priAmmoIndex = AmmoGetIndex(priAmmoProfile);
        if (AmmoIsValidIndex(priAmmoIndex))
        {
            WeaponProfiles[WeaponProfileCount][WeaponProfile_PriAmmoManager] = priAmmoIndex;            
        }
        else
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Warning: Invalid primary ammo profile name at index %d: \"%s\"", WeaponProfileCount + failedCount, priAmmoProfile);
            failedCount++;
            continue;
        }
        
        // Secondary ammo profile.
        KvGetString(kvWeaponProfiles, "secondary_ammo_profile", secAmmoProfile, sizeof(secAmmoProfile));
        if (StrEqual(priAmmoProfile, secAmmoProfile))
        {
            // Secondary profile is the same as primary profile.
            WeaponProfiles[WeaponProfileCount][WeaponProfile_SecAmmoManager] = priAmmoIndex;
        }
        else
        {
            secAmmoIndex = AmmoGetIndex(secAmmoProfile);
            if (AmmoIsValidIndex(secAmmoIndex))
            {
                WeaponProfiles[WeaponProfileCount][WeaponProfile_SecAmmoManager] = secAmmoIndex;
            }
            else
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Warning: Invalid secondary ammo profile name at index %d: \"%s\"", WeaponProfileCount + failedCount, secAmmoProfile);
                failedCount++;
                continue;
            }
        }
        
        // Weapon restriction profile.
        /*KvGetString(kvWeaponProfiles, "restriction_profile", restrictProfile, sizeof(restrictProfile));
        restrictIndex = RestrictProfileGetIndex(restrictProfile);
        if (RestrictIsValidIndex(restrictIndex))
        {
            WeaponProfiles[WeaponProfileCount][WeaponProfile_RestrictManager] = restrictIndex;            
        }
        else
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Warning: Invalid restriction profile name at index %d: \"%s\"", WeaponProfileCount + failedCount, restrictProfile);
            failedCount++;
            continue;
        }*/
        
        WeaponProfileCount++;
    } while (KvGotoNextKey(kvWeaponProfiles));
    
    CloseHandle(kvWeaponProfiles);
    
    // Check if there are no weapon profiles.
    if (!WeaponProfileCount)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "No valid weapon profiles in \"%s\". There must be at least one profile named \"default\".", weaponProfilesPath);
    }
    
    // Check if the default profile exist.
    if (WeaponProfilesGetIndex("default") < 0)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Couldn't find default weapon profile in \"%s\". There must a profile named \"default\".", weaponProfilesPath);
    }
    
    // Log model validation info.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Weapons, "Weapon Profile Validation", "Successful: %d | Unsuccessful: %d", WeaponProfileCount, failedCount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_WeaponProfiles, true);
    ConfigSetConfigReloadFunc(File_WeaponProfiles, GetFunctionByName(GetMyHandle(), "WeaponProfilesOnConfigReload"));
}

/**
 * Callback for ammo config reload request.
 */
public WeaponProfilesOnConfigReload(ConfigFile:config)
{
    WeaponProfilesLoad();
}

/**
 * Returns if the specified index is valid or not.
 *
 * @param index     The index to validate.
 * @return          True if valid, false otherwise.
 */
bool:WeaponProfilesIsValidIndex(index)
{
    if (index >= 0 && index < AmmoProfileCount)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Resolves a profile name and returns the profile index.
 *
 * @param profileName   Name to resolve.
 * @return              Profile index, or -1 if not found.
 */
WeaponProfilesGetIndex(const String:profileName[])
{
    // Verify that ammo profiles exist.
    if (WeaponProfileCount <= 0)
    {
        return -1;
    }
    
    // Loop through all profiles.
    for (new profile = 0; profile < WeaponProfileCount; profile++)
    {
        decl String:name[32];
        WeaponProfilesGetName(profile, name, sizeof(name));
        
        if (StrEqual(profileName, name))    // Case sensitive match.
        {
            return profile;
        }
    }
    
    // Profile not found.
    return -1;
}

/**
 * Gets the profile name at the specified index.
 *
 * @param index     Profile index.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of string buffer.
 * @return          Number of cells written.
 */
WeaponProfilesGetName(index, String:buffer[], maxlen)
{
    // Validate index.
    if (!WeaponProfilesIsValidIndex(index))
    {
        return 0;
    }
    
    return strcopy(buffer, maxlen, WeaponProfiles[index][WeaponProfile_Name]);
}
