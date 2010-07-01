/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          healthregen.inc
 *  Type:          Core 
 *  Description:   Functions for managing health regeneration on a client.
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
 * Attributes for health regen.
 */
enum HealthRegenAttribute
{
    Float:RegenAttribute_Interval,
    RegenAttribute_Amount,
    RegenAttribute_MaxHealth,
    Handle:RegenAttribute_Timer
}

/**
 * Array to store health regen attributes for each client.
 */
new HealthRegenAttributes[MAXPLAYERS + 1][HealthRegenAttribute];

/**
 * Called when a client connects to the server (OnClientPutInServer).
 */
HealthRegenClientInit(client)
{
    // Reset the array index to INVALID_HANDLE because there is no regen timer running on this new client.
    ZREndTimer(HealthRegenAttributes[client][RegenAttribute_Timer], false);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
HealthRegenOnClientDisconnect(client)
{
    // Stop the client from regenerating.
    HealthRegenStop(client);
}

/**
 * Set the health regen attributes on a client.
 */

stock HealthRegenSetInterval(client, Float:interval)
{
    HealthRegenAttributes[client][RegenAttribute_Interval] = interval;
}

stock HealthRegenSetAmount(client, amount)
{
    HealthRegenAttributes[client][RegenAttribute_Amount] = amount;
}

stock HealthRegenSetMaxHealth(client, maxhealth)
{
    HealthRegenAttributes[client][RegenAttribute_MaxHealth] = maxhealth;
}

/**
 * Client has spawned into the game.
 * 
 * @param client    The client index.
 */ 
HealthRegenOnClientSpawn(client)
{
    // Kill the regen timer.
    HealthRegenStop(client);
}

/**
 * Client is being hurt.
 * 
 * @param client    The client index.
 */
HealthRegenOnClientHurt(client)
{
    // If the client is regenerating health already, then stop.
    if (ZRIsTimerRunning(HealthRegenAttributes[client][RegenAttribute_Timer]))
    {
        return;
    }
    
    // If the interval isn't a positive number, then stop.
    if (ClassGetHealthRegenInterval(client) <= 0)
    {
        return;
    }
    
    // If the client's health is lower than their max health, then start regeneration.
    if (GetClientHealth(client) < HealthRegenAttributes[client][RegenAttribute_MaxHealth])
    {
        HealthRegenStart(client);
    }
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */ 
HealthRegenOnClientDeath(client)
{
    // Kill the regen timer.
    HealthRegenStop(client);
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
HealthRegenOnClientInfected(client)
{
    // Kill the regen timer.
    HealthRegenStop(client);
}

/**
 * Client has been turned back human.
 * 
 * @param client    The client index.
 */
HealthRegenOnClientHuman(client)
{
    // Kill the regen timer.
    HealthRegenStop(client);
}

/**
 * Starts HP regeneration on a client using current client settings.
 *
 * @param client        The client index.
 * @param interval      Regeneration interval.
 */
stock HealthRegenStart(client)
{
    // Stop timer if it already exists.
    HealthRegenStop(client);
    
    // Create new timer.
    HealthRegenAttributes[client][RegenAttribute_Timer] = CreateTimer(HealthRegenAttributes[client][RegenAttribute_Interval], HealthRegenTimer, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Stops HP regeneration on a client.
 *
 * @param client    The client index.
 */
stock HealthRegenStop(client)
{
    // Kill the timer if it exists.
    ZREndTimer(HealthRegenAttributes[client][RegenAttribute_Timer]);
}

/**
 * Timer callback for HP regeneration.
 */
public Action:HealthRegenTimer(Handle:timer, any:client)
{
    // Kill the timer if the player is dead or not in game.
    if (!IsClientInGame(client) || !IsPlayerAlive(client))
    {
        ZREndTimer(HealthRegenAttributes[client][RegenAttribute_Timer], false);
        return Plugin_Stop;
    }
    
    new health = GetClientHealth(client);    
    health += HealthRegenAttributes[client][RegenAttribute_Amount];
    
    // Check if the health points is below or equal to the limit.
    if (health <= HealthRegenAttributes[client][RegenAttribute_MaxHealth])
    {
        // Increase health.
        SetEntityHealth(client, health);
        
        // Forward event to modules.
        ZHPOnClientHealthRegen(client);
    }
    // If their health is now either the max health or beyond, then end health regen.
    if (health >= HealthRegenAttributes[client][RegenAttribute_MaxHealth])
    {
        // Set their HP to the max health before ending.
        SetEntityHealth(client, HealthRegenAttributes[client][RegenAttribute_MaxHealth]);
        
        // Forward event to modules.
        ZHPOnClientHealthRegen(client);
        
        ZREndTimer(HealthRegenAttributes[client][RegenAttribute_Timer], false);
        return Plugin_Stop;
    }
    
    return Plugin_Continue;
}
