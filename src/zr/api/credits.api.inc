/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          credits.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Credits module)
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
new Handle:g_hAPIFwdOnCreditsModifiedPre = INVALID_HANDLE;
new Handle:g_hAPIFwdOnCreditsModified = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to infection.
 */
APICreditsInit()
{
    // Infect module natives/forwards (infect.zr.inc)
    
    // Natives
    CreateNative("ZR_GiveCredits", APIGiveCredits);
    CreateNative("ZR_TakeCredits", APITakeCredits);
    CreateNative("ZR_SetCredits", APISetCredits);
    CreateNative("ZR_GetCredits", APIGetCredits);
    CreateNative("ZR_ConvertCurrency", APIConvertCurrency);
    
    // Forwards
    g_hAPIFwdOnCreditsModifiedPre = CreateGlobalForward("ZR_OnCreditsModifiedPre", ET_Hook, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnCreditsModified = CreateGlobalForward("ZR_OnCreditsModified", ET_Ignore, Param_Cell, Param_Cell, Param_Cell);
}

/**
 * Native call function (ZR_GiveCredits)
 * Give credits to a client.
 * 
 * CreditsGiveCredits(client, amount)
 */
public APIGiveCredits(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    new amount = GetNativeCell(2);
    
    return CreditsGiveCredits(client, amount);
}

/**
 * Native call function (ZR_TakeCredits)
 * Take credits from a client.
 * 
 * CreditsTakeCredits(client, amount)
 */
public APITakeCredits(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    new amount = GetNativeCell(2);
    
    return CreditsTakeCredits(client, amount);
}

/**
 * Native call function (ZR_SetCredits)
 * Set the amount of credits a client has.
 * 
 * CreditsSetCredits(client, amount)
 */
public APISetCredits(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    new amount = GetNativeCell(2);
    
    CreditsSetCredits(client, amount);
}

/**
 * Native call function (ZR_GetCredits)
 * Get the amount of credits a client has.
 * 
 * CreditsGetCredits(client)
 */
public APIGetCredits(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    return CreditsGetCredits(client);
}

/**
 * Native call function (ZR_ConvertCurrency)
 * Converts currencies.
 * 
 * CreditsConvertCurrency(value, Float:exchangerate)
 */
public APIConvertCurrency(Handle:plugin, numParams)
{
    new value = GetNativeCell(1);
    new Float:exchangerate = Float:GetNativeCell(2);
    
    return CreditsConvertCurrency(value, exchangerate);
}

/**
 * Called when credits on a client are about to be modified.
 * 
 * @param client    The client index.
 * @param newvalue  The current amount of credits.
 */
Action:APIOnCreditsModifiedPre(&client, &newvalue)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnCreditsModifiedPre);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushCellRef(newvalue);
    
    // Get what they returned.
    new Action:result;
    Call_Finish(result);
    return result;
}

/**
 * Called when credits on a client are modified.
 * 
 * @param client    The client index.
 * @param newvalue  The current amount of credits.
 * @param oldvalue  How many the client had before the change.
 */
APIOnCreditsModified(client, newvalue, oldvalue)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnCreditsModified);
    
    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(newvalue);
    Call_PushCell(oldvalue);
    
    // Finish the call.
    Call_Finish();
}
