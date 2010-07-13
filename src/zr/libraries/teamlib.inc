/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          teamlib.inc
 *  Type:          Library
 *  Description:   Generic zombie-based team managing API.
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
 * Team defines.
 */
#if defined PROJECT_GAME_CSS
  #define TEAM_0 CS_TEAM_NON
  #define TEAM_1 CS_TEAM_SPECTATOR
  #define TEAM_2 CS_TEAM_T
  #define TEAM_3 CS_TEAM_CT
#endif

#if defined PROJECT_GAME_TF2
  #define TEAM_0 _:TFTeam_Unassigned
  #define TEAM_1 _:TFTeam_Spectator
  #define TEAM_2 _:TFTeam_Red
  #define TEAM_3 _:TFTeam_Blue
#endif

/**
 * Zombie-based teams.
 */
enum ZTeam
{
    ZTeam_Unassigned = 0,   /** Client isn't on a team. */
    ZTeam_Spectator,        /** Client is a spectator. */
    ZTeam_Zombie,           /** Client is a zombie. */
    ZTeam_Human             /** Client is a human. */
}

/**
 * Global array to store if a client is zombie or human.
 */
new ZTeam:g_TLibClientTeam[MAXPLAYERS + 1];

/**
 * Function to return the client's team.
 */
stock ZTeam:TLib_GetClientTeam(client) { return g_TLibClientTeam[client]; }

/**
 * Check client's team via boolean return type.
 */
stock bool:TLib_IsClientUnassigned(client) { return (g_TLibClientTeam[client] == ZTeam_Unassigned); }
stock bool:TLib_IsClientSpectator(client) { return (g_TLibClientTeam[client] == ZTeam_Spectator); }
stock bool:TLib_IsClientZombie(client) { return (g_TLibClientTeam[client] == ZTeam_Zombie); }
stock bool:TLib_IsClientHuman(client) { return (g_TLibClientTeam[client] == ZTeam_Human); }

/**
 * Check if a client is either a human or zombie.
 * 
 * @param client    The client index.
 * 
 * @return          True if client is a zombie or human, false if not.
 */
stock bool:TLib_IsClientPlaying(client)
{
    return (TLib_IsClientZombie(client) || TLib_IsClientHuman(client));
}

/**
 * Set a client's team.
 * 
 * @param client    The client index.
 * @param team      The team from enum ZTeam to set.
 */
stock TLib_SetClientTeam(client, ZTeam:team) { g_TLibClientTeam[client] = team; }

/**
 * Count clients on a specific team.
 * 
 * @param team  The team to count clients of.
 * @param alive Only count client if they're alive. 
 * 
 * @return      The number of clients on this team.  
 */
stock TLib_CountTeam(ZTeam:team, bool:alive)
{
    new count;
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        // Skip dead players if they must be alive.
        if (alive && !IsPlayerAlive(client))
            continue;
        
        if (team == g_TLibClientTeam[client])
            count++;
    }
    
    return count;
}
