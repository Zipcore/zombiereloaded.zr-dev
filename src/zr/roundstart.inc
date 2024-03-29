/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          roundstart.inc
 *  Type:          Core
 *  Description:   Handles round start actions.
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
 * List of objective entities.
 */
#define ROUNDSTART_OBJECTIVE_ENTITIES "func_bomb_target|func_hostage_rescue|c4|hostage_entity"

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
RoundStartOnClientSpawn(client)
{
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    // Get public chat trigger prefix.
    decl String:publictrigger[4];
    SayHooksGetPublicChatTrigger(publictrigger, sizeof(publictrigger));
    
    // If public chat trigger is disabled, then don't print message.
    if (StrEqual(publictrigger, "N/A", false))
    {
        return;
    }
    
    // Print to client, how to access ZMenu.
    TranslationPrintToChat(client, "General zmenu reminder", publictrigger, SAYHOOKS_KEYWORD_ZMENU);
}

/**
 * The round is starting.
 */
RoundStartOnRoundStart()
{
    // Print round objective to all clients.
    TranslationPrintToChatAll(true, false, "General round objective");
    
    // Kill all objective entities.
    RoundStartKillObjectives();
}

/**
 * Kills all objective entities.
 */
stock RoundStartKillObjectives()
{
    decl String:classname[64];
    
    // Get max entity count.
    new maxentities = GetMaxEntities();
    
    // x = entity index.
    for (new x = 0; x <= maxentities; x++)
    {
        // If entity isn't valid, then stop.
        if(!IsValidEdict(x))
        {
            continue;
        }
        
        // Get valid edict's classname.
        GetEdictClassname(x, classname, sizeof(classname));
        
        // Check if it matches any objective entities, then stop if it doesn't.
        if(StrContains(ROUNDSTART_OBJECTIVE_ENTITIES, classname) > -1)
        {
            // Entity is an objective, kill it.
            RemoveEdict(x);
        }
    }
}
