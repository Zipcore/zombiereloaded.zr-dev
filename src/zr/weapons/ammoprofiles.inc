/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ammoprofiles.inc
 *  Description:   Weapon ammo profiles.
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

#include "zr/weapons/ammoheaders.h"

/**
 * Ammo profile storage.
 */
new AmmoProfiles[AMMO_PROFILES_MAX][AmmoProfile];

/**
 * Number of ammo profiles loaded.
 */
new AmmoProfileCount;

/**
 * Loads or reloads all ammo profiles.
 */
AmmoLoad()
{
    new Handle:kvAmmoProfiles = INVALID_HANDLE;
    
    // Register config file.
    ConfigRegisterConfig(File_AmmoProfiles, Structure_Keyvalue, CONFIG_FILE_ALIAS_AMMOPROFILES);
    
    // Get models file path.
    decl String:ammpProfilesPath[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_AMMOPROFILES, ammpProfilesPath);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogEvent(false, LogTypeOld_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Missing ammo profiles: \"%s\"", ammpProfilesPath);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_AmmoProfiles, ammpProfilesPath);
    
    // Prepare key/value structure.
    kvAmmoProfiles = CreateKeyValues(CONFIG_FILE_ALIAS_AMMOPROFILES);
    
    // Log that ammo profiles file is loading.
    LogEvent(false, LogTypeOld_Normal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Loading ammo profiles from file \"%s\".", ammpProfilesPath);
    
    // Load ammo profiles file.
    FileToKeyValues(kvAmmoProfiles, ammpProfilesPath);
    
    // Try to find the first profile.
    KvRewind(kvAmmoProfiles);
    if (!KvGotoFirstSubKey(kvAmmoProfiles))
    {
        LogEvent(false, LogTypeOld_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Can't find any ammo profiles in \"%s\"", ammpProfilesPath);
    }
    
    //decl String:buffer[256];
    decl String:name[32];
    decl String:mode[16];
    decl String:reloadMode[16];
    
    AmmoProfileCount = 0;
    new failedCount;
    
    // Loop through all profiles and store attributes in AmmoProfiles array.
    do
    {
        if (AmmoProfileCount > AMMO_PROFILES_MAX)
        {
            // Maximum number of profiles reached. Log a warning and exit the loop.
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Maximum number of ammo profiles reached (%d). Skipping other profiles.", AMMO_PROFILES_MAX + 1);
            
            break;
        }
        
        if (KvGetSectionName(kvAmmoProfiles, name, sizeof(name)))
        {
            strcopy(AmmoProfiles[AmmoProfileCount][AmmoProfile_Name], 32, name);
        }
        else
        {
            strcopy(AmmoProfiles[AmmoProfileCount][AmmoProfile_Name], 32, AMMO_DEFAULT_NAME);
        }
        
        KvGetString(kvAmmoProfiles, "mode", mode, sizeof(mode), AMMO_DEFAULT_MODE);
        AmmoProfiles[AmmoProfileCount][AmmoProfile_Mode] = AmmoStringToMode(mode);
        
        AmmoProfiles[AmmoProfileCount][AmmoProfile_ClipSize] = KvGetNum(kvAmmoProfiles, "clip_size", AMMO_DEFAULT_CLIP_SIZE);
        AmmoProfiles[AmmoProfileCount][AmmoProfile_ClipReserve] = KvGetNum(kvAmmoProfiles, "clip_reserve", AMMO_DEFAULT_CLIP_RESERVE);
        
        KvGetString(kvAmmoProfiles, "reload_mode", reloadMode, sizeof(reloadMode), AMMO_DEFAULT_RELOAD_MODE);
        AmmoProfiles[AmmoProfileCount][AmmoProfile_ReloadMode] = AmmoStringToReloadMode(reloadMode);
        
        AmmoProfiles[AmmoProfileCount][AmmoProfile_Interval] = KvGetFloat(kvAmmoProfiles, "interval", AMMO_DEFAULT_INTERVAL);
        AmmoProfiles[AmmoProfileCount][AmmoProfile_Amount] = KvGetNum(kvAmmoProfiles, "amount", AMMO_DEFAULT_AMOUNT);
        AmmoProfiles[AmmoProfileCount][AmmoProfile_Max] = KvGetNum(kvAmmoProfiles, "max", AMMO_DEFAULT_MAX);
        
        // Validate attributes.
        
        // Verify name length.
        if (!strlen(name))
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Empty \"name\" setting at index %d.", AmmoProfileCount + failedCount);
            failedCount++;
            continue;
        }
        
        // Validate mode.
        if (AmmoProfiles[AmmoProfileCount][AmmoProfile_Mode] == AmmoMode_Invalid)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Invalid \"mode\" setting at index %d: \"%s\".", AmmoProfileCount + failedCount, mode);
            failedCount++;
            continue;
        }
        
        // Validate clip size.
        new clipSize = AmmoProfiles[AmmoProfileCount][AmmoProfile_ClipSize];
        if (clipSize < AMMO_CLIP_SIZE_MIN && clipSize > AMMO_CLIP_SIZE_MAX)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Out of range \"clip_size\" setting at index %d: \"%d\".", AmmoProfileCount + failedCount, clipSize);
            failedCount++;
            continue;
        }
        
        // Validate clip reserve.
        new clipReserve = AmmoProfiles[AmmoProfileCount][AmmoProfile_ClipReserve];
        if (clipReserve < AMMO_CLIP_RESERVE_MIN && clipReserve > AMMO_CLIP_RESERVE_MAX)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Out of range \"clip_reserve\" setting at index %d: \"%d\".", AmmoProfileCount + failedCount, clipReserve);
            failedCount++;
            continue;
        }
        
        // Validate reload mode.
        if (AmmoProfiles[AmmoProfileCount][AmmoProfile_ReloadMode] == AmmoReloadMode_Invalid)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Invalid \"reload mode\" setting at index %d: \"%s\".", AmmoProfileCount + failedCount, reloadMode);
            failedCount++;
            continue;
        }
        
        // Validate interval.
        new Float:interval = AmmoProfiles[AmmoProfileCount][AmmoProfile_Interval];
        if (interval < AMMO_INTERVAL_MIN && interval > AMMO_INTERVAL_MAX)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Out of range \"interval\" setting at index %d: \"%d\".", AmmoProfileCount + failedCount, interval);
            failedCount++;
            continue;
        }
        
        // Validate amount.
        new amount = AmmoProfiles[AmmoProfileCount][AmmoProfile_Amount];
        if (amount < AMMO_AMOUNT_MIN && amount > AMMO_AMOUNT_MAX)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Out of range \"amount\" setting at index %d: \"%d\".", AmmoProfileCount + failedCount, amount);
            failedCount++;
            continue;
        }
        
        // Validate max.
        new max = AmmoProfiles[AmmoProfileCount][AmmoProfile_Max];
        if (max < AMMO_MAX_MIN && max > AMMO_MAX_MAX)
        {
            LogEvent(false, LogTypeOld_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Warning: Out of range \"max\" setting at index %d: \"%d\".", AmmoProfileCount + failedCount, max);
            failedCount++;
            continue;
        }
        
        AmmoProfileCount++;
    } while (KvGotoNextKey(kvAmmoProfiles));
    
    CloseHandle(kvAmmoProfiles);
    
    // Check if there are no ammo profiles.
    if (!AmmoProfileCount)
    {
        LogEvent(false, LogTypeOld_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Config Validation", "No valid ammo profiles in \"%s\". There must be at least one profile named \"default\".", ammpProfilesPath);
    }
    
    // Check if the default profile exist.
    if (AmmoGetIndex("default") < 0)
    {
        LogEvent(false, LogTypeOld_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Profile Validation", "Couldn't find default ammo profile in \"%s\". There must a profile named \"default\".", ammpProfilesPath);
    }
    
    // Log model validation info.
    LogEvent(false, LogTypeOld_Normal, LOG_CORE_EVENTS, LogModule_Weapons, "Ammo Config Validation", "Successful: %d | Unsuccessful: %d", AmmoProfileCount, failedCount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_AmmoProfiles, true);
    ConfigSetConfigReloadFunc(File_AmmoProfiles, GetFunctionByName(GetMyHandle(), "AmmoOnConfigReload"));
}

/**
 * Callback for ammo config reload request.
 */
public AmmoOnConfigReload(ConfigFile:config)
{
    AmmoLoad();
}

/**
 * Returns if the specified index is valid or not.
 *
 * @param index     The index to validate.
 * @return          True if valid, false otherwise.
 */
stock bool:AmmoIsValidIndex(index)
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
stock AmmoGetIndex(const String:profileName[])
{
    // Verify that ammo profiles exist.
    if (AmmoProfileCount <= 0)
    {
        return -1;
    }
    
    // Loop through all profiles.
    for (new profile = 0; profile < AmmoProfileCount; profile++)
    {
        decl String:name[32];
        AmmoGetName(profile, name, sizeof(name));
        
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
stock AmmoGetName(index, String:buffer[], maxlen)
{
    // Validate index.
    if (!AmmoIsValidIndex(index))
    {
        return 0;
    }
    
    return strcopy(buffer, maxlen, AmmoProfiles[index][AmmoProfile_Name]);
}

/**
 * Gets the profile mode at the specified index.
 *
 * @param index     Profile index.
 * @return          Ammo mode.
 */
stock AmmoMode:AmmoGetMode(index)
{
    return AmmoProfiles[index][AmmoProfile_Mode];
}

/**
 * Converts a ammo mode string to a mode type.
 *
 * @param modeString    String to convert.
 * @param return        Ammo mode or AmmoMode_Invalid on error.
 */
stock AmmoMode:AmmoStringToMode(const String:modeString[])
{
    if (StrEqual(modeString, "disabled", false))
    {
        return AmmoMode_Disabled;
    }
    else if (StrEqual(modeString, "unlimited", false))
    {
        return AmmoMode_Unlimited;
    }
    else if (StrEqual(modeString, "fill_clip", false))
    {
        return AmmoMode_FillClip;
    }
    else if (StrEqual(modeString, "fill_reserve", false))
    {
        return AmmoMode_FillReserve;
    }
    
    return AmmoMode_Invalid;
}

/**
 * Converts a ammo reload mode string to a reload mode type.
 *
 * @param reloadModeString  String to convert.
 * @param return            Ammo reload mode or AmmoReloadMode_Invalid on error.
 */
stock AmmoReloadMode:AmmoStringToReloadMode(const String:reloadModeString[])
{
    if (StrEqual(reloadModeString, "default", false))
    {
        return AmmoReloadMode_Default;
    }
    else if (StrEqual(reloadModeString, "realistic", false))
    {
        return AmmoReloadMode_Realistic;
    }
    
    return AmmoReloadMode_Invalid;
}

/**
 * Gets the reload mode at the specified index.
 *
 * @param index     Profile index.
 * @return          Reload mode.
 */
stock AmmoReloadMode:AmmoGetReloadMode(index)
{
    return AmmoProfiles[index][AmmoProfile_ReloadMode];
}

/**
 * Gets the interval value at the specified index.
 *
 * @param index     Profile index.
 * @return          Interval value.
 */
stock Float:AmmoGetInterval(index)
{
    return AmmoProfiles[index][AmmoProfile_Interval];
}

/**
 * Gets the interval amount value at the specified index.
 *
 * @param index     Profile index.
 * @return          Interval amount.
 */
stock AmmoGetIntervalAmount(index)
{
    return AmmoProfiles[index][AmmoProfile_Amount];
}

/**
 * Gets the maximum interval amount value at the specified index.
 *
 * @param index     Profile index.
 * @return          Maximum interval amount.
 */
stock AmmoGetMaxAmount(index)
{
    return AmmoProfiles[index][AmmoProfile_Max];
}

/**
 * Gets the clip size at the specified index.
 *
 * @param index     Profile index.
 * @return          Clip size.
 */
stock AmmoGetClipSize(index)
{
    return AmmoProfiles[index][AmmoProfile_ClipSize];
}

/**
 * Gets the clip reserve size at the specified index.
 *
 * @param index     Profile index.
 * @return          Clip reserve size.
 */
stock AmmoGetClipReserve(index)
{
    return AmmoProfiles[index][AmmoProfile_ClipReserveSize];
}
