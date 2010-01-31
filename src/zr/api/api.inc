/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API.
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
 * Application Programming Interface (API)
 * 
 * To allow other plugins or extensions to interact directly with Zombie:Reloaded we need to implement
 * an API.  SourceMod allows us to do this by creating global "natives" or "forwards."
 * 
 * Natives are basically functions that can be called from any plugin that includes its definition.
 * Forwards are functions that are called on a given event. (Such as OnClientPutInServer)
 * ZR's API files are located in sourcemod/scripting/include/zr/category.zr.inc.  We chose to create multiple
 * files simply for organization.  Including zr.inc will automatically include the rest of the files as well.
 * 
 * To better understand how natives and forwards are created, go here:
 * http://wiki.alliedmods.net/Creating_Natives_(SourceMod_Scripting)
 * http://wiki.alliedmods.net/Function_Calling_API_(SourceMod_Scripting) 
 */

#include "zr/api/tools.api"
#include "zr/api/credits.api"
#include "zr/api/infect.api"
#include "zr/api/respawn.api"

/**
 * Initializes all main natives and forwards.
 */
APIInit()
{
    // zombiereloaded.inc
    
    // Natives
    CreateNative("ZR_CreateEligibleClientList", APICreateEligibleClientList);
    CreateNative("ZR_GetVersionInfo", APIGetVersionInfo);
    
    
    // Forward event to sub-modules.
    APIToolsInit();
    APICreditsInit();
    APIInfectInit();
    APIRespawnInit();
    
    // Needed natives/forwards
    // forward ZR_OnClientDeath(client, bool:zombie) (because Player_Death is fired on infect)
    
}

/**
 * Validates a client index and when it fails, an error is thrown.
 * 
 * @param client    The client index to validate.
 * @param alive     Set to true to validate that the client is alive, false to ignore.
 * 
 * @error           Throws an error when the client isn't valid.
 */
stock APIValidateClientIndex(client, EligibleCondition:alive = Condition_Either)
{
    // If the client index is out of range, then throw an error.
    if (client < 1 || client > MaxClients)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Invalid client index. (%d)", client);
    }
    
    // If there is no client occupying this index, then throw an error.
    if (!IsClientConnected(client))
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Client %d is not connected.", client);
    }
    
    // If the condition is yes or no, and the condition isn't met, then throw an error.
    new bool:bAlive = bool:alive;
    if (alive != Condition_Either && bAlive != IsPlayerAlive(client))
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Client %d must be alive.", client);
    }
}

/**
 * Validates an entity index and when it fails, an error is thrown.
 * 
 * @param entity    The entity index to validate.
 * 
 * @error           Throws an error when the client isn't valid.
 */
stock APIValidateEntityIndex(entity)
{
    // If the entity index is out of range, then throw an error.
    if (entity < 1)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Invalid entity index. (%d)", entity);
    }
    
    // If this isn't a valid edict, then throw an error.
    if (!IsValidEdict(entity))
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Index %d is not a valid edict.", entity);
    }
    
    // If this isn't a valid entity, then throw an error.
    if (!IsValidEntity(entity))
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Index %d is not a valid entity.", entity);
    }
}

/**
 * Native call function (ZR_CreateEligibleClientList)
 * Create an array populated with eligible clients.
 * 
 * ZRCreateEligibleClientList(&Handle:arrayEligibleClients, EligibleCondition:team = Condition_Either, EligibleCondition:alive = Condition_Either, EligibleCondition:human = Condition_Either)  
 */
public APICreateEligibleClientList(Handle:plugin, numParams)
{
    new Handle:arrayEligibleClients = Handle:GetNativeCellRef(1);
    new EligibleCondition:team = EligibleCondition:GetNativeCell(2);
    new EligibleCondition:alive = EligibleCondition:GetNativeCell(3);
    new EligibleCondition:human = EligibleCondition:GetNativeCell(4);
    
    // Create the list and return the number of clients in the list.
    return ZRCreateEligibleClientList(arrayEligibleClients, team, alive, human);
}

/**
 * Native call function (ZR_GetVersionInfo)
 * Returns the current plugin version and optional revision info.
 */
public APIGetVersionInfo(Handle:plugin, numParams)
{
    // Pass the version info the string.
    SetNativeString(1, VERSION, GetNativeCell(2));
    
    // Pass the revision info only if it's available, otherwise set it to a blank string.
    #if defined ADD_VERSION_INFO
        SetNativeString(3, ZR_VER_REVISION, GetNativeCell(4));
        
        return true;
    #else
        SetNativeString(2, "", 1);
        
        return false;
    #endif
}