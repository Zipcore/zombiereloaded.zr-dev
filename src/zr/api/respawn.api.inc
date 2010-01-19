/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          respawn.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Respawn module)
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
 * @section Global forward handles.
 */
new Handle:g_hAPIFwdOnClientRespawnPre = INVALID_HANDLE;
new Handle:g_hAPIFwdOnClientRespawn = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to respawn.
 */
APIRespawnInit()
{
    // Respawn module natives/forwards (respawn.zr.inc)
    
    // Natives
    CreateNative("ZR_RespawnClient", APIRespawnClient);
    CreateNative("ZR_OverrideKilledByWorld", APIOverrideKilledByWorld);
    CreateNative("ZR_ReadKilledByWorld", APIReadKilledByWorld);
    
    // Forwards
    g_hAPIFwdOnClientRespawnPre = CreateGlobalForward("ZR_OnClientRespawnPre", ET_Hook, Param_CellByRef, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClientRespawn = CreateGlobalForward("ZR_OnClientRespawn", ET_Ignore, Param_Cell, Param_Cell, Param_Cell);
}

/**
 * Native call function (ZR_RespawnClient)
 * Spawns a player into the round following rules set by cvars.
 * 
 * RespawnSpawnClient(client, bool:zombie = false, bool:zombie_if_suicide = false)
 */
public APIRespawnClient(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);
    
    new bool:zombie = bool:GetNativeCell(2);
    new bool:zombie_if_suicide = bool:GetNativeCell(3);
    
    // Respawn the client.
    RespawnSpawnClient(client, zombie, zombie_if_suicide);
}

/**
 * Called right before ZR respawns a client.
 * 
 * @param client            The client index.
 * @param zombie            True if the client is respawning as a zombie, false if not.
 * @param zombie_if_suicide True if the client will be respawned as a zombie if they suicided.
 * 
 * RespawnSpawnClient(client, bool:zombie = false, bool:zombie_if_suicide = false)
 */
Action:APIOnClientRespawnPre(&client, &bool:zombie, &bool:zombie_if_suicide)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientRespawnPre);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushCellRef(zombie);
    Call_PushCellRef(zombie_if_suicide);
    
    // Get what they returned.
    new Action:result;
    Call_Finish(result);
    return result;
}

/**
 * Called after a client has become a zombie.
 * 
 * @param client            The client index.
 * @param zombie            True if the client is respawning as a zombie, false if not.
 * @param zombie_if_suicide True if the client will be respawned as a zombie if they suicided.
 * 
 * RespawnSpawnClient(client, bool:zombie = false, bool:zombie_if_suicide = false)
 */
Action:APIOnClientRespawn(client, bool:zombie, bool:zombie_if_suicide)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientRespawn);
    
    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(zombie);
    Call_PushCell(zombie_if_suicide);
    
    // Finish the call.
    Call_Finish();
}

/**
 * Native call function (ZR_OverrideKilledByWorld)
 * Override if the client died by a suicide.
 * 
 * bool:g_bKilledByWorld[MAXPLAYERS + 1]
 */
public APIOverrideKilledByWorld(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);
    
    new bool:suicide = bool:GetNativeCell(2);
    
    // Override the old value.
    g_bKilledByWorld[client] = suicide;
}

/**
 * Native call function (ZR_ReadKilledByWorld)
 * Read the value of if the client died by a suicide.
 * 
 * bool:g_bKilledByWorld[MAXPLAYERS + 1]
 */
public APIReadKilledByWorld(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);
    
    // Return the value.
    return _:g_bKilledByWorld[client];
}
