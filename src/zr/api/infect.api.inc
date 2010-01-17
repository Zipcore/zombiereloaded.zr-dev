/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Infect module)
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
new Handle:g_hAPIFwdOnClientInfectedPre = INVALID_HANDLE;
new Handle:g_hAPIFwdOnClientInfected = INVALID_HANDLE;
new Handle:g_hAPIFwdOnClientHumanPre = INVALID_HANDLE;
new Handle:g_hAPIFwdOnClientHuman = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to infection.
 */
APIInfectInit()
{
    // Infect module natives (infect.zr.inc)
    
    // Natives
    CreateNative("ZR_IsClientInfected", APIIsClientInfected);
    CreateNative("ZR_IsClientHuman", APIIsClientHuman);
    
    // Forwards
    g_hAPIFwdOnClientInfectedPre = CreateGlobalForward("ZR_OnClientInfectedPre", ET_Hook, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClientInfected = CreateGlobalForward("ZR_OnClientInfected", ET_Ignore, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
    g_hAPIFwdOnClientHumanPre = CreateGlobalForward("ZR_OnClientHumanPre", ET_Hook, Param_CellByRef, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClientHuman = CreateGlobalForward("ZR_OnClientHuman", ET_Ignore, Param_Cell, Param_Cell, Param_Cell);
}

/**
 * Native call function (ZR_IsClientInfected)
 * Returns true if the client is infected, false if not.
 * 
 * bool:InfectIsClientInfected(client)
 */
public APIIsClientInfected(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    return InfectIsClientInfected(client);
}

/**
 * Native call function (ZR_IsClientHuman)
 * Returns true if the client is human, false if not.
 * 
 * bool:InfectIsClientHuman(client)
 */
public APIIsClientHuman(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    return InfectIsClientHuman(client);
}

/**
 * Called when a client is about to become a zombie.
 * 
 * @param client            The client to infect.
 * @param attacker          The attacker who did the infect.
 * @param motherinfect      Indicates a mother zombie infect.
 * @param respawnoverride   Set to true to override respawn cvar.
 * @param respawn           Value to override with.
 * 
 * InfectHumanToZombie(client, attacker = -1, bool:motherinfect = false, bool:respawnoverride = false, bool:respawn = false)
 */
Action:APIOnClientInfectedPre(&client, &attacker, &bool:motherinfect, &bool:respawnoverride, &bool:respawn)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientInfectedPre);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushCellRef(attacker);
    Call_PushCellRef(motherinfect);
    Call_PushCellRef(respawnoverride);
    Call_PushCellRef(respawn);
    
    // Get what they returned.
    new Action:result;
    Call_Finish(result);
    return result;
}

/**
 * Called after a client has become a zombie.
 * 
 * @param client            The client to infect.
 * @param attacker          The attacker who did the infect.
 * @param motherinfect      Indicates a mother zombie infect.
 * @param respawnoverride   Set to true to override respawn cvar.
 * @param respawn           Value to override with.
 * 
 * InfectHumanToZombie(client, attacker = -1, bool:motherinfect = false, bool:respawnoverride = false, bool:respawn = false)
 */
APIOnClientInfected(client, attacker, bool:motherinfect, bool:respawnoverride, bool:respawn)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientInfected);
    
    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(attacker);
    Call_PushCell(motherinfect);
    Call_PushCell(respawnoverride);
    Call_PushCell(respawn);
    
    // Finish the call.
    Call_Finish();
}

/**
 * Called when a client is about to become a human. (Through either zr_human or admin menu)
 * 
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 * 
 * InfectZombieToHuman(client, bool:respawn = false, bool:protect = false)
 */
Action:APIOnClientHumanPre(&client, &bool:respawn, &bool:protect)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientHumanPre);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushCellRef(respawn);
    Call_PushCellRef(protect);
    
    // Get what they returned.
    new Action:result;
    Call_Finish(result);
    return result;
}

/**
 * Called after a client has become a human. (Through either zr_human or admin menu)
 * 
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 *  
 * InfectZombieToHuman(client, bool:respawn = false, bool:protect = false)
 */
APIOnClientHuman(client, bool:respawn, bool:protect)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientHuman);
    
    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(respawn);
    Call_PushCell(protect);
    
    // Finish the call.
    Call_Finish();
}