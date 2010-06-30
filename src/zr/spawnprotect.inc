/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          spawnprotect.inc
 *  Type:          Module
 *  Description:   Protects late-joining players from zombies for x seconds.
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
 * @section
 * Default attribute values to use in case of invalid values in config.
 */
#define SPAWNPROTECT_DEFAULT_SPEED 300.0
#define SPAWNPROTECT_DEFAULT_ALPHA 0
/**
 * @endsection
 */

/**
 * Array for storing spawn protect timer handles per client.
 */
new Handle:tSpawnProtect[MAXPLAYERS + 1];

/**
 * Array for storing time left for spawn protection per client.
 */
new pSpawnProtectTime[MAXPLAYERS + 1];
 
/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
SpawnProtectClientInit(client)
{
    // Reset timer handle.
    ZREndTimer(tSpawnProtect[client], false);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
SpawnProtectOnClientDisconnect(client)
{
    // Reset timer handle.
    ZREndTimer(tSpawnProtect[client]);
}

/**
 * Client is spawning into the game. *Post
 * 
 * @param client    The client index.
 */   
SpawnProtectOnClientSpawnPost(client)
{
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    // If timer is currently running, kill it.
    ZREndTimer(tSpawnProtect[client]);
    
    // If protect cvar is disabled, then stop.
    new bool:protect = GetConVarBool(g_hCvarsList[CVAR_SPAWNPROTECT]);
    if (!protect)
    {
        return;
    }
    
    // Disable spawn protection on client.
    bInfectImmune[client][INFECT_TYPE_NORMAL] = false;
    
    // Start spawn protection.
    SpawnProtectStart(client);
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
SpawnProtectOnClientDeath(client)
{
    // If timer is running, kill it.
    ZREndTimer(tSpawnProtect[client]);
}

/**
 * Start spawn protection on a client.
 * 
 * @param client    The client index.
 */
SpawnProtectStart(client)
{
    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return;
    }
    
    // If zombie hasn't spawned, then stop.
    if (!g_bZombieSpawned)
    {
        return;
    }
    
    // If client is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        return;
    }
    
    // Set spawn protect flag on client.
    bInfectImmune[client][INFECT_TYPE_NORMAL] = true;
    
    // Get spawn protect attribute cvars.
    new Float:speed = GetConVarFloat(g_hCvarsList[CVAR_SPAWNPROTECT_SPEED]);
    new alpha = GetConVarInt(g_hCvarsList[CVAR_SPAWNPROTECT_ALPHA]);
    
    // Validate attributes
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
    }
    if (speed < min && speed > max)
    {
        // Reset to default and log a warning.
        speed = SPAWNPROTECT_DEFAULT_SPEED;
        LogEvent(false, LogTypeOld_Normal, LOG_CORE_EVENTS, LogModules:LogModule_Spawnprotect, "Config validation", "Out of range value in cvar zr_spawnprotect_speed (%f). Using default value.", speed);
    }
    if (alpha < ZR_CLASS_ALPHA_INITIAL_MIN && alpha > ZR_CLASS_ALPHA_INITIAL_MAX)
    {
        // Reset to default and log a warning.
        alpha = SPAWNPROTECT_DEFAULT_ALPHA;
        LogEvent(false, LogTypeOld_Normal, LOG_CORE_EVENTS, LogModules:LogModule_Spawnprotect, "Config validation", "Out of range value in cvar zr_spawnprotect_alpha (%d). Using default value.", alpha);
    }
    
    // Set spawn protect attributes.
    ClassApplySpeedEx(client, speed);
    ToolsSetClientAlpha(client, alpha);
    
    // Set time left to zr_protect_time's value.
    new protect_time = GetConVarInt(g_hCvarsList[CVAR_SPAWNPROTECT_TIME]);
    pSpawnProtectTime[client] = protect_time;
    
    // Tell client they are being protected.
    TranslationPrintToChat(client, "Spawn protection begin", protect_time);
    
    // Send time left in a hud message.
    TranslationPrintHintText(client, "Spawn Protect", pSpawnProtectTime[client]);
    
    // Start repeating timer.
    tSpawnProtect[client] = CreateTimer(1.0, SpawnProtectTimer, client, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
}

/**
 * Timer callback function, countdown for spawn protection.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */    
public Action:SpawnProtectTimer(Handle:timer, any:client)
{
    // If client leaves, then stop timer.
    if (!IsClientInGame(client))
    {
        // Reset timer handle.
        ZREndTimer(tSpawnProtect[client], false);
        return Plugin_Stop;
    }
    
    // If client has become a zombie, then stop timer.
    if (!InfectIsClientHuman(client))
    {
        // Reset timer handle.
        ZREndTimer(tSpawnProtect[client], false);
        return Plugin_Stop;
    }
    
    // Decrement time left.
    pSpawnProtectTime[client]--;
    
    // Print time left to client.
    TranslationPrintHintText(client, "Spawn Protect", pSpawnProtectTime[client]);
    
    // Time has expired.
    if (pSpawnProtectTime[client] <= 0)
    {
        // Remove protect flag.
        bInfectImmune[client][INFECT_TYPE_NORMAL] = false;
        
        // Tell client spawn protection is over.
        TranslationPrintHintText(client, "Spawn protection end");
        
        // Restore class attributes.
        new classindex = ClassGetActiveIndex(client);
        ClassApplyAlpha(client, classindex);
        ClassApplySpeed(client, classindex);
        
        // Clear timer handle.
        ZREndTimer(tSpawnProtect[client], false);
        
        // Stop timer.
        return Plugin_Stop;
    }
    
    // Allow timer to continue repeating.
    return Plugin_Continue;
}
