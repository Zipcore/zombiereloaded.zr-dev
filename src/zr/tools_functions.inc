/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools_functions.inc
 *  Type:          Core
 *  Description:   API for offsets/signatures exposed in tools.inc
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
 * NVG properties on a client.
 */
enum ToolsNVGProps
{
    NVGProp_Owner,
    NVGProp_Enabled
}

/**
 * Score properties on a client.
 */
enum ToolsScoreProps
{
    ScoreProp_Kills,
    ScoreProp_Deaths
}

/**
 * Set a client's velocity.
 * @param client        The client index.
 * @param vecVelocity   Array to store vector in, or velocity to set on client.
 * @param retrieve      True to get client's velocity, false to set it.
 * @param stack         If modifying velocity, then true will stack new velocity onto the client's
 *                      current velocity, false will reset it.
 */
stock ToolsSetClientVelocity(client, const Float:vecVelocity[3], bool:stack = true)
{
    // Copy input velocity to new variable.
    new Float:vecTrueVelocity[3];
    for (new x = 0; x < 3; x++)
    {
        vecTrueVelocity[x] = vecVelocity[x];
    }
        
    // If stack is true, then add client's velocity.
    if (stack)
    {
        // Get client's velocity.
        new Float:vecClientVelocity[3];
        
        // x = vector component.
        for (new x = 0; x < 3; x++)
        {
            vecClientVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
        }
        
        // Add the vectors and overwrite to update the "true" velocity.
        AddVectors(vecClientVelocity, vecVelocity, vecTrueVelocity);
    }
    
    // Apply velocity on client.
    TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecTrueVelocity);
}

/**
 * Set a client's velocity.
 * @param client        The client index.
 * @param vecVelocity   Array to store vector in, or velocity to set on client.
 * @param retrieve      True to get client's velocity, false to set it.
 * @param stack         If modifying velocity, then true will stack new velocity onto the client's
 *                      current velocity, false will reset it.
 */
stock ToolsGetClientVelocity(client, Float:vecVelocity[3])
{
    // x = vector component.
    for (new x = 0; x < 3; x++)
    {
        vecVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
    }
}

/**
 * Set a client's lagged movement value.
 * @param client    The client index.
 * @param value     LMV value. (1.0 = default, 2.0 = double)
 */
stock ToolsSetClientLMV(client, Float:fLMV)
{
    // Set lagged movement value of client.
    SetEntDataFloat(client, g_iToolsLMV, fLMV / 300.0, true);
}

/**
 * Get a client's lagged movement value.
 * @param client    The client index.
 *  
 * @return          LMV value. (1.0 = default, 2.0 = double)
 */
stock Float:ToolsGetClientLMV(client)
{
    // Set lagged movement value of client.
    return GetEntDataFloat(client, g_iToolsLMV);
}

/**
 * Set nightvision properties on a client.
 * @param client    The client index.
 * @param prop      The prop to set: See ToolsNVGProps (top of file) 
 * @param value     New value of the property of nightvision.
 */
stock ToolsSetClientNVG(client, ToolsNVGProps:prop, bool:value)
{
    if (prop == NVGProp_Owner)
    {
        SetEntData(client, g_iToolsHasNightVision, value, 1, true);
    }
    else if (prop == NVGProp_Enabled)
    {
        SetEntData(client, g_iToolsNightVisionOn, value, 1, true);
    }
}

/**
 * Get nightvision properties on a client.
 * @param client    The client index.
 * @param prop      The prop to get: See ToolsNVGProps (top of file)
 * @return          The value of the property of nightvision on client.
 */
stock bool:ToolsGetClientNVG(client, ToolsNVGProps:prop)
{
    if (prop == NVGProp_Owner)
    {
        return bool:GetEntData(client, g_iToolsHasNightVision, 1);
    }
    else if (prop == NVGProp_Enabled)
    {
        return bool:GetEntData(client, g_iToolsNightVisionOn, 1);
    }
    
    // Neither case was given.
    return false;
}

/**
 * Set a client's default field of vision.
 * @param client    The client index.
 * @param FOV       The field of vision of the client.
 */
stock ToolsSetClientDefaultFOV(client, FOV)
{
    SetEntData(client, g_iToolsFOV, FOV, 1, true);
}

/**
 * Set a client's score or deaths.
 * 
 * @param client    The client index.
 * @param score     True to look at score, false to look at deaths.  
 * @param apply     True to set scores or death, false to get. 
 * @param value     The value of the client's score or deaths.
 * @return          The score or death count of the client, -1 if setting.
 */
stock ToolsSetClientScore(client, ToolsScoreProps:prop, value)
{
    // If score is true, then set client's score.
    if (prop == ScoreProp_Kills)
    {
        SetEntProp(client, Prop_Data, "m_iFrags", value);
    }
    // Set client's deaths.
    else if (prop == ScoreProp_Deaths)
    {
        SetEntProp(client, Prop_Data, "m_iDeaths", value);
    }
    
    // Neither case was given.
    return -1;
}

/**
 * Get a client's score or deaths.
 * 
 * @param client    The client index.
 * @param score     True to look at score, false to look at deaths.  
 * @param apply     True to set scores or death, false to get. 
 * @param value     The value of the client's score or deaths.
 * @return          The score or death count of the client, -1 if setting.
 */
stock ToolsGetClientScore(client, ToolsScoreProps:prop)
{
    if (prop == ScoreProp_Kills)
    {
        // If score is true, then return client's score.
        return GetEntProp(client, Prop_Data, "m_iFrags");
    }
    // Return client's deaths.
    else if (prop == ScoreProp_Deaths)
    {
        return GetEntProp(client, Prop_Data, "m_iDeaths");
    }
}

/**
 * Set a client's alpha value.
 * 
 * @param client    The client index.
 * @param alpha     The alpha value to set client's alpha to. (0-255)
 * @param weapons   Apply alpha to all client's weapons.
 */
stock ToolsSetClientAlpha(client, alpha)
{
    // Turn rendermode on, on the client.
    SetEntityRenderMode(client, RENDER_TRANSALPHA);
    
    // Set alpha value on the client.
    SetEntityRenderColor(client, _, _, _, alpha);
    
    // Forward event to modules.
    WeaponAlphaOnClientAlphaChanged(client, alpha);
}

/**
 * Sets an entity's color. (This is the "get" version of SetEntityRenderColor)
 *
 * @param client	The client index.
 * @return          The alpha value of the client. (0-255)
 */
stock ToolsGetEntityAlpha(entity)
{
	static bool:gotconfig = false;
	static String:prop[32];
	
	if (!gotconfig)
	{
		new Handle:gc = LoadGameConfigFile("core.games");
		new bool:exists = GameConfGetKeyValue(gc, "m_clrRender", prop, sizeof(prop));
		CloseHandle(gc);
		
		if (!exists)
		{
			strcopy(prop, sizeof(prop), "m_clrRender");
		}
		
		gotconfig = true;
	}
	
	new offset = GetEntSendPropOffs(entity, prop);
	
	return GetEntData(entity, offset + 3, 1);
}
