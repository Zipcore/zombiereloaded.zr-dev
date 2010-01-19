/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          respawn.inc
 *  Type:          Module 
 *  Description:   Players come back to life
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
 * Array for storing respawn timer handles per client.
 */
new Handle:tRespawn[MAXPLAYERS + 1];

/**
 * Array for flagging zombies who were killed by world.
 */ 
new bool:g_bKilledByWorld[MAXPLAYERS + 1];

/**
 * Client is joining the server.
 */ 
RespawnClientInit(client)
{
    // Reset timer handle.
    ZREndTimer(tRespawn[client], false);
    
    // Init g_bKilledByWorld for client.
    g_bKilledByWorld[client] = false;
}

/**
 * Client is leaving the server.
 */ 
RespawnOnClientDisconnect(client)
{
    // Reset timer handle.
    ZREndTimer(tRespawn[client], true);
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
RespawnOnClientSpawn(client)
{
    // If timer is running, kill it.
    ZREndTimer(tRespawn[client], true);
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
RespawnOnClientDeath(client, attacker)
{
    // If client is a zombie, check if they were killed by world.
    if (InfectIsClientInfected(client))
    {
        // Set g_bKilledByWorld to true if attacker is not a valid client.
        g_bKilledByWorld[client] = !ZRIsClientValid(attacker);
    }
    
    // If timer is running, kill it.
    ZREndTimer(tRespawn[client], true);
    
    // If respawn is disabled, stop here.
    new bool:respawn = GetConVarBool(g_hCvarsList[CVAR_RESPAWN]);
    if (!respawn)
    {
        return;
    }
    
    // Start respawn timer.
    new Float:delay = GetConVarFloat(g_hCvarsList[CVAR_RESPAWN_DELAY]);
    tRespawn[client] = CreateTimer(delay, RespawnTimer, client, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
RespawnOnRoundEnd()
{
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Kill the timer if it's running.
        ZREndTimer(tRespawn[x], true);
    }
}

/**
 * Spawns a player into the round following rules set by cvars.
 * 
 * @param client            The client index.
 * @param zombie            True to spawn as zombie, false to spawn as human.
 * @param zombie_if_suicide If true, client will be respawned as a zombie if they died by suicide.
 */
RespawnSpawnClient(client, bool:zombie = false, bool:zombie_if_suicide = false)
{
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // Forward pre-event to modules.
    new Action:result = APIOnClientRespawnPre(client, zombie, zombie_if_suicide);
    
    // If the forward returned Plugin_Handled or Plugin_Stop, then stop this function.
    if (result == Plugin_Handled || result == Plugin_Stop)
    {
        return;
    }
    
    // Spawn player.
    CS_RespawnPlayer(client);
    
    // Stop here if the first zombie hasn't spawned yet.
    if (!g_bZombieSpawned)
    {
        return;
    }
    
    // Infect if zombie is true.
    if (zombie)
    {
        InfectHumanToZombie(client);
        return;
    }
    
    // Infect if they committed suicide.
    if (zombie_if_suicide && g_bKilledByWorld[client])
    {
        InfectHumanToZombie(client);
        g_bKilledByWorld[client] = false;
    }
    
    // Forward event to modules.
    APIOnClientRespawn(client, zombie, zombie_if_suicide);
}

/**
 * Timer callback, respawns a player.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */    
public Action:RespawnTimer(Handle:timer, any:client)
{
    // Reset timer handle.
    ZREndTimer(tRespawn[client], false);
    
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // If player isn't alive, then stop.
    if (IsPlayerAlive(client))
    {
        return;
    }
    
    // Get client team.
    new team = GetClientTeam(client);
    
    // If player isn't on a team, then stop.
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        return;
    }
    
    // Get respawn team.
    new bool:respawn_zombie = GetConVarBool(g_hCvarsList[CVAR_RESPAWN_TEAM_ZOMBIE]);
    
    // Respawn client, and if the cvar is enabled, then respawn as a zombie if they suicided.
    RespawnSpawnClient(client, respawn_zombie, GetConVarBool(g_hCvarsList[CVAR_RESPAWN_TEAM_ZOMBIE_WORLD]));
}
