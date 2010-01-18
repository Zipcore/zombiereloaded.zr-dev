/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Tools module)
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
 * Initializes all tool natives.
 */
APIToolsInit()
{
    // Tools module natives (tools.zr.inc)
    
    // Natives
    CreateNative("ZR_SetClientVelocity", APISetClientVelocity);
    CreateNative("ZR_GetClientVelocity", APIGetClientVelocity);
    CreateNative("ZR_SetClientLMV", APISetClientLMV);
    CreateNative("ZR_GetClientLMV", APIGetClientLMV);
    CreateNative("ZR_SetClientNVG", APISetClientNVG);
    CreateNative("ZR_GetClientNVG", APIGetClientNVG);
    CreateNative("ZR_SetClientDefaultFOV", APISetClientDefaultFOV);
    CreateNative("ZR_SetClientScore", APISetClientScore);
    CreateNative("ZR_GetClientScore", APIGetClientScore);
    CreateNative("ZR_SetClientAlpha", APISetClientAlpha);
    CreateNative("ZR_GetEntityAlpha", APIGetEntityAlpha);
}

/**
 * Native call function (ZR_SetClientVelocity)
 * Set a client's velocity.
 * 
 * ToolsSetClientVelocity(client, const Float:vecVelocity[3], bool:stack = true)
 */
public APISetClientVelocity(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new Float:vecVelocity[3];
    GetNativeArray(2, vecVelocity, 3);
    
    new bool:stack = GetNativeCell(3);
    
    // Set the client's velocity.
    ToolsSetClientVelocity(client, vecVelocity, stack);
}

/**
 * Native call function (ZR_GetClientVelocity)
 * Get a client's velocity.
 * 
 * ToolsGetClientVelocity(client, Float:vecVelocity[3])
 */
public APIGetClientVelocity(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    // Get the client's velocity.
    new Float:vecVelocity[3];
    ToolsGetClientVelocity(client, vecVelocity);
    
    // Set the output variable to the client's velocity.
    SetNativeArray(2, vecVelocity, 3);
}

/**
 * Native call function (ZR_SetClientLMV)
 * Set a client's lagged movement value.
 * 
 * ToolsSetClientLMV(client, Float:fLMV)
 */
public APISetClientLMV(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new Float:flLMV = Float:GetNativeCell(2);
    
    // Set the client's lagged movement value.
    ToolsSetClientLMV(client, flLMV);
}

/**
 * Native call function (ZR_GetClientLMV)
 * Get a client's lagged movement value.
 * 
 * Float:ToolsGetClientLMV(client)
 */
public APIGetClientLMV(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    // Set the client's lagged movement value.
    return _:ToolsGetClientLMV(client);
}

/**
 * Native call function (ZR_SetClientNVG)
 * Set nightvision properties on a client.
 * 
 * ToolsSetClientNVG(client, ToolsNVGProps:prop, bool:value)
 */
public APISetClientNVG(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new ToolsNVGProps:prop = ToolsNVGProps:GetNativeCell(2);
    new bool:value = bool:GetNativeCell(3);
    
    // Set the client's nightvision property value.
    ToolsSetClientNVG(client, prop, value);
}

/**
 * Native call function (ZR_GetClientNVG)
 * Get a client's lagged movement value.
 * 
 * bool:ToolsGetClientNVG(client, ToolsNVGProps:prop)
 */
public APIGetClientNVG(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new ToolsNVGProps:prop = ToolsNVGProps:GetNativeCell(2);
    
    // Get the client's nightvision property value.
    return _:ToolsGetClientNVG(client, prop);
}

/**
 * Native call function (ZR_SetClientDefaultFOV)
 * Set a client's default field of vision.
 * 
 * ToolsSetClientDefaultFOV(client, FOV)
 */
public APISetClientDefaultFOV(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new FOV = GetNativeCell(2);
    
    // Get the client's default FOV value.
    ToolsSetClientDefaultFOV(client, FOV);
}

/**
 * Native call function (ZR_SetClientScore)
 * Set a client's score or deaths.
 * 
 * ToolsSetClientScore(client, ToolsScoreProps:prop, value)
 */
public APISetClientScore(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, false);
    
    new ToolsScoreProps:prop = ToolsScoreProps:GetNativeCell(2);
    new bool:value = bool:GetNativeCell(3);
    
    // Set the client's score property value.
    ToolsSetClientScore(client, prop, value);
}

/**
 * Native call function (ZR_GetClientScore)
 * Get a client's score or deaths.
 * 
 * ToolsGetClientScore(client, ToolsScoreProps:prop)
 */
public APIGetClientScore(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, false);
    
    new ToolsScoreProps:prop = ToolsScoreProps:GetNativeCell(2);
    
    // Get the client's score property value.
    return ToolsGetClientScore(client, prop);
}

/**
 * Native call function (ZR_SetClientAlpha)
 * Set a client's (not entity) alpha value.
 * 
 * ToolsSetClientAlpha(client, alpha)
 */
public APISetClientAlpha(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    new alpha = GetNativeCell(2);
    
    // Get the client's alpha.
    ToolsSetClientAlpha(client, alpha);
}

/**
 * Native call function (ZR_GetEntityAlpha)
 * Sets an entity's color. (This is the "get" version of SetEntityRenderColor)
 * 
 * ToolsGetEntityAlpha(client)
 */
public APIGetEntityAlpha(Handle:plugin, numParams)
{
    new client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client, true);
    
    // Get the client's alpha.
    return ToolsGetEntityAlpha(client);
}
