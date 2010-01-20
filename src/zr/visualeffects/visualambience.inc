/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          visualambience.inc
 *  Type:          Module
 *  Description:   Fog, light style, sky, sun rendering, etc
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
 * Default sky of current map.
 */
new String:g_VAmbienceDefaultSky[PLATFORM_MAX_PATH];

/**
 * The two components of fog.
 */
enum VAmbienceFogType
{
    FogType_Primary,
    FogType_Secondary
}

/**
 * The map is ending.
 */
VAmbienceOnMapEnd()
{
    // Reset the default sky cache to empty, to be re-cached for the next map.
    g_VAmbienceDefaultSky[0] = '\0';
}

/**
 * Validate cvar data.
 */
VAmbienceLoad()
{
    // Apply all visual effects now
    VAmbienceApplyAll();
    
    // If sky is disabled, then stop.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    if (!sky)
    {
        return;
    }
    
    decl String:downloadpath[PLATFORM_MAX_PATH];
    decl String:skypath[PLATFORM_MAX_PATH];
    
    // Get sky path.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], skypath, sizeof(skypath));
    
    // Prepend materials/skybox to the path.
    Format(downloadpath, sizeof(downloadpath), "materials/skybox/%s", skypath);
    
    // Add skybox file to downloads table.
    AddFileToDownloadsTable(downloadpath);
}

/**
 * Hook zr_veffects_* cvar changes.
 * 
 * @param unhook    If true, cvars will be unhooked, false to hook cvars.
 */
VAmbienceCvarsHook(bool:unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook lightstyle cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE], VAmbienceCvarsHookLightStyle);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], VAmbienceCvarsHookLightStyle);
        
        // Unhook sky cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY], VAmbienceCvarsHookSky);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], VAmbienceCvarsHookSky);
        
        // Unhook sun cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE], VAmbienceCvarsHookSunDisable);
        
        // Unhook fog cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_BLEND], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ANGLES], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST], VAmbienceCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST], VAmbienceCvarsHookFog);
        
        // Stop after unhooking cvars.
        return;
    }
    
    // Hook lightstyle cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE], VAmbienceCvarsHookLightStyle);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], VAmbienceCvarsHookLightStyle);
    
    // Hook sky cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY], VAmbienceCvarsHookSky);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], VAmbienceCvarsHookSky);
    
    // Hook sun cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE], VAmbienceCvarsHookSunDisable);
    
    // Hook fog cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_BLEND], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ANGLES], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST], VAmbienceCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST], VAmbienceCvarsHookFog);
}

/**
 * Cvar hook callback (zr_veffects_lightstyle, zr_veffects_lightstyle_value)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VAmbienceCvarsHookLightStyle(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If lightstyle is disabled, then disable.
    new bool:lightstyle = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]);
    
    // Apply light style.
    VAmbienceApplyLightStyle(!lightstyle);
}

/**
 * Cvar hook callback (zr_veffects_sky, zr_veffects_sky_path)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VAmbienceCvarsHookSky(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If sky is disabled, then disable.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    
    // Apply new sky.
    VAmbienceApplySky(!sky);
}

/**
 * Cvar hook callback (zr_veffects_sun_disable)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VAmbienceCvarsHookSunDisable(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If sun disable is disabled, then disable.  (Lol)
    new bool:sun = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE]);
    
    // Apply fog.
    VAmbienceApplySunDisable(!sun);
}

/**
 * Cvar hook callback (zr_veffects_fog_*)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VAmbienceCvarsHookFog(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If fog is disabled, then disable.
    new bool:fog = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG]);
    
    // Apply fog.
    VAmbienceApplyFog(!fog);
}

/**
 * Apply all cvar values on server.
 */
stock VAmbienceApplyAll()
{
    // If lightstyle is disabled, then disable.
    new bool:lightstyle = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]);
    
    // Apply light style.
    VAmbienceApplyLightStyle(!lightstyle);
    
    // If sky is disabled, then disable.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    
    // Apply new sky.
    VAmbienceApplySky(!sky);
    
    // If sky is disabled, then disable.
    new bool:fog = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG]);
    
    // Apply fog.
    VAmbienceApplyFog(!fog);
}

stock VAmbienceApplyLightStyle(bool:disable = false)
{
    // If disabled, then set to normal light style.
    if (disable)
    {
        // Set light style.
        SetLightStyle(0, "n");
        
        return;
    }
    
    // Get light value.
    decl String:lightstylevalue[4];
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], lightstylevalue, sizeof(lightstylevalue));
    
    // Set light style.
    SetLightStyle(0, lightstylevalue);
}

stock VAmbienceApplySky(bool:disable = false)
{
    // If we can't find the sv_skyname cvar, then stop.
    new Handle:hSkyname = FindConVar("sv_skyname");
    if (hSkyname == INVALID_HANDLE)
    {
        return;
    }
    
    // Store map's default sky before applying new one.
    if (!g_VAmbienceDefaultSky[0])
    {
        GetConVarString(hSkyname, g_VAmbienceDefaultSky, sizeof(g_VAmbienceDefaultSky));
    }
    
    // If disabled, then set to default sky.
    if (disable)
    {
        if (g_VAmbienceDefaultSky[0])
        {
            // Set default sky on all clients.
            SetConVarString(hSkyname, g_VAmbienceDefaultSky, true);
        }
        
        return;
    }
    
    // Get sky path.
    decl String:skypath[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], skypath, sizeof(skypath));
    
    // Set new sky on all clients.
    SetConVarString(hSkyname, skypath, true);
}

stock VAmbienceApplySunDisable(bool:disable = false)
{
    // Find sun entity.
    new sun = FindEntityByClassname(-1, "env_sun");
    
    // If sun is invalid, then stop.
    if (sun == -1)
    {
        return;
    }
    
    // If disabled, then re-enable sun rendering.
    if (disable)
    {
        // Turn on sun rendering.
        AcceptEntityInput(sun, "TurnOn");
        
        return;
    }
    
    // Turn off sun rendering.
    AcceptEntityInput(sun, "TurnOff");
}

stock VAmbienceApplyFog(bool:disable = false)
{
    // Find any existing fog controlling entities.
    new fogreference = FindEntityByClassname(-1, "env_fog_controller");
    new skyreference = FindEntityByClassname(-1, "sky_camera");
    
    // If fog is disabled, then turn off the entities and stop here.
    if (disable)
    {
        if (fogreference != INVALID_ENT_REFERENCE)
        {
            AcceptEntityInput(fogreference, "TurnOff");
        }
        
        if (skyreference != INVALID_ENT_REFERENCE)
        {
            DispatchKeyValue(skyreference, "fogenable", "0");
        }
        
        return;
    }
    
    // If fog doesn't exist, then create it.
    if (fogreference == INVALID_ENT_REFERENCE)
    {
        // Create the non-networked fog entity.
        fogreference = CreateEntityByName("env_fog_controller");
        DispatchSpawn(fogreference);
    }
    
    // Enable fog if it was disabled.
    AcceptEntityInput(fogreference, "TurnOn");
    
    // Check if the map is missing a sky_camera entity
    if (skyreference == INVALID_ENT_REFERENCE)
    {
        LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_VEffects, "Fog Manipulation", "Failed to find a sky_camera entity on this map, sky fog couldn't be synced with level fog.");
    }
    else
    {
        DispatchKeyValue(skyreference, "fogenable", "1");
    }
    
    // Set fog color blending.
    new bool:blend = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG_BLEND]);
    VAmbienceSetFogBlend(fogreference, blend);
    VAmbienceSetFogBlend(skyreference, blend);
    
    decl String:fogcolor[16];
    decl String:fogangles[16];
    
    // Set primary fog color.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], fogcolor, sizeof(fogcolor));
    VAmbienceSetFogColor(fogreference, fogcolor, FogType_Primary);
    VAmbienceSetFogColor(skyreference, fogcolor, FogType_Primary);
    
    // Set secondary fog color.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], fogcolor, sizeof(fogcolor));
    VAmbienceSetFogColor(fogreference, fogcolor, FogType_Secondary);
    VAmbienceSetFogColor(skyreference, fogcolor, FogType_Secondary);
    
    // Set fog blending angles.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_FOG_ANGLES], fogangles, sizeof(fogangles));
    VAmbienceSetFogAngles(fogreference, fogangles);
    VAmbienceSetFogAngles(skyreference, fogangles);
    
    // Set fog's start distance.
    new Float:fogstart = GetConVarFloat(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST]);
    VAmbienceSetFogStartDist(fogreference, fogstart);
    VAmbienceSetFogStartDist(skyreference, fogstart);
    
    // Set fog's end distance.
    new Float:fogend = GetConVarFloat(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST]);
    VAmbienceSetFogEndDist(fogreference, fogend);
    VAmbienceSetFogEndDist(skyreference, fogend);
}

/**
 * Set if fog should blend colors.
 * 
 * @param entityreference   Entity reference of the fog to modify.
 * @param blend             True to blend, false to only use primary color.
 */
stock VAmbienceSetFogBlend(entityreference, bool:blend)
{
    // If the entity reference is invalid, then stop.
    if (entityreference == INVALID_ENT_REFERENCE)
    {
        return;
    }
    
    // Set fog blending.
    if (blend)
    {
        DispatchKeyValue(entityreference, "fogblend", "1");
    }
    else
    {
        DispatchKeyValue(entityreference, "fogblend", "0");
    }
}

/**
 * Set fog's primary or secondary color.
 * 
 * @param entityreference   Entity reference of the fog to modify.
 * @param color             The rgb color of the fog.
 * @param primary           (Optional) True to set primary, false otherwise.
 */
stock VAmbienceSetFogColor(entityreference, const String:color[], VAmbienceFogType:type = FogType_Primary)
{
    // If the entity reference is invalid, then stop.
    if (entityreference == INVALID_ENT_REFERENCE)
    {
        return;
    }
    
    // Set primary color.
    if (type == FogType_Primary)
    {
        // Set new color.
        DispatchKeyValue(entityreference, "fogcolor", color);
    }
    // Set secondary color.
    else if (type == FogType_Secondary)
    {
        // Set new color.
        DispatchKeyValue(entityreference, "fogcolor2", color);
    }
}

/**
 * Set fog's blending angles.
 * 
 * @param entityreference   Entity reference of the fog to modify.
 * @param angles            The blending angles to set.
 */
stock VAmbienceSetFogAngles(entityreference, const String:angles[])
{
    // If the entity reference is invalid, then stop.
    if (entityreference == INVALID_ENT_REFERENCE)
    {
        return;
    }
    
    // Set blending angles.
    DispatchKeyValue(entityreference, "fogdir", angles);
}

/**
 * Set fog's start distance.
 * 
 * @param entityreference       Entity reference of the fog to modify.
 * @param startdist             The start distance of the fog.
 */
stock VAmbienceSetFogStartDist(entityreference, Float:startdist)
{
    // If the entity reference is invalid, then stop.
    if (entityreference == INVALID_ENT_REFERENCE)
    {
        return;
    }
    
    // Set start distance.
    DispatchKeyValueFloat(entityreference, "fogstart", startdist);
}

/**
 * Set fog's end distance.
 * 
 * @param entityreference       Entity reference of the fog to modify.
 * @param enddist               The end distance of the fog.
 */
stock VAmbienceSetFogEndDist(entityreference, Float:enddist)
{
    // If the entity reference is invalid, then stop.
    if (entityreference == INVALID_ENT_REFERENCE)
    {
        return;
    }
    
    // Set end distance.
    DispatchKeyValueFloat(entityreference, "fogend", enddist);
}
