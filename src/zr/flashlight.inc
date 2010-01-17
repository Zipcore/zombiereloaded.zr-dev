/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          flashlight.inc
 *  Type:          Core
 *  Description:   Control anything to do with flashlights.
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
 * Impulse number for flashlight.
 */
#define INFECT_FLASHLIGHT_IMPULSE 100

/**
 * Initialize global SDKTools handles.
 */
new Handle:g_hToolsFlashlightIsOn = INVALID_HANDLE;
new Handle:g_hToolsFlashlightTurnOn = INVALID_HANDLE;
new Handle:g_hToolsFlashlightTurnOff = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Sets up gamedata for the flashlight module.
 */
FlashlightOnGameDataPrepped()
{
    // Prep the SDKCall for "FlashlightIsOn"
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Virtual, "FlashlightIsOn");
    PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
    g_hToolsFlashlightIsOn = EndPrepSDKCall();
    
    // If offset "FlashlightIsOn" can't be found, then stop the plugin.
    if(g_hToolsFlashlightIsOn == INVALID_HANDLE)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Virtual offset \"CBasePlayer::FlashlightIsOn\" was not found.");
    }
    
    // Prep the SDKCall for "FlashlightTurnOn"
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Virtual, "FlashlightTurnOn");
    g_hToolsFlashlightTurnOn = EndPrepSDKCall();
    
    // If offset "FlashlightTurnOn" can't be found, then stop the plugin.
    if(g_hToolsFlashlightTurnOn == INVALID_HANDLE)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Virtual offset \"CBasePlayer::FlashlightTurnOn\" was not found.");
    }
    
    // Prep the SDKCall for "FlashlightTurnOff"
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Virtual, "FlashlightTurnOff");
    g_hToolsFlashlightTurnOff = EndPrepSDKCall();
    
    // If offset "FlashlightTurnOff" can't be found, then stop the plugin.
    if(g_hToolsFlashlightTurnOff == INVALID_HANDLE)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Virtual offset \"CBasePlayer::FlashlightTurnOff\" was not found.");
    }
}

/**
 * Check if the client's flashlight is turned on.
 * 
 * @param client    The client index.
 * 
 * @return          True if the light is on, false if off.
 */
stock bool:FlashlightIsOn(client)
{
    return bool:SDKCall(g_hToolsFlashlightIsOn, client);
}

/**
 * Force a client's flashlight on.
 * 
 * @param client    The client index.
 */
stock FlashlightTurnOn(client)
{
    SDKCall(g_hToolsFlashlightTurnOn, client);
}

/**
 * Force a client's flashlight off.
 * 
 * @param client    The client index.
 */
stock FlashlightTurnOff(client)
{
    SDKCall(g_hToolsFlashlightTurnOff, client);
}