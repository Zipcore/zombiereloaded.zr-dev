/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          voice.inc
 *  Type:          Module
 *  Description:   Alter listening/speaking states of humans/zombies.
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
 * 
 * Known Issues:
 * - Using sm_(un)mute might cause temporary listening "problems."  The clients normal CS:S listening
 * flags will be restored.  Zombie:Reloaded should fix it either during the round, and if not then on spawn.
 * Since there is no forward telling me when the client is either muted or unmuted, the plugin just will ignore
 * muted clients until unmuted.
 */

/**
 * The current value of the voice cvar.
 */
#define VOICE_ZR_VOICE GetConVarBool(g_hCvarsList[CVAR_VOICE])

/**
 * The current value of the cvar sv_alltalk.
 */
#define VOICE_SV_ALLTALK GetConVarBool(FindConVar("sv_alltalk"))

/**
 * Hook voice-sensitive cvar changes.
 * 
 * @param unhook    If true, cvars will be unhooked, false to hook cvars.
 */
VoiceCvarsHook(bool:unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook cvars.
        UnhookConVarChange(FindConVar("sv_alltalk"), VoiceCvarsHookVoice);
        UnhookConVarChange(g_hCvarsList[CVAR_VOICE], VoiceCvarsHookVoice);
        UnhookConVarChange(g_hCvarsList[CVAR_VOICE_ZOMBIES_MUTE], VoiceCvarsHookVoice);
        
        // Stop after unhooking cvars.
        return;
    }
    
    // Hook cvars.
    HookConVarChange(FindConVar("sv_alltalk"), VoiceCvarsHookVoice);
    HookConVarChange(g_hCvarsList[CVAR_VOICE], VoiceCvarsHookVoice);
    HookConVarChange(g_hCvarsList[CVAR_VOICE_ZOMBIES_MUTE], VoiceCvarsHookVoice);
}

/**
 * Cvar hook callback (zr_voice, zr_voice_zombies_mute)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VoiceCvarsHookVoice(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // Reset all voice settings.
    VoiceReset();
}

/**
 * The round is ending.
 */
VoiceOnRoundEnd()
{
    // If voice module is disabled or sv_alltalk is enabled, then stop.
    if (!VOICE_ZR_VOICE || VOICE_SV_ALLTALK)
    {
        return;
    }
    
    // If the round end alltalk cvar is disabled, then stop.
    new bool:roundend_alltalk = GetConVarBool(g_hCvarsList[CVAR_VOICE_ROUNDEND_ALLTALK]);
    if (!roundend_alltalk)
    {
        return;
    }
    
    // Allow everyone to listen/speak with each other.
    VoiceAllTalk();
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
VoiceOnClientSpawn(client)
{
    // If voice module is disabled or sv_alltalk is enabled, then stop.
    if (!VOICE_ZR_VOICE || VOICE_SV_ALLTALK)
    {
        return;
    }
    
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
VoiceOnClientInfected(client)
{
    // If voice module is disabled or sv_alltalk is enabled, then stop.
    if (!VOICE_ZR_VOICE || VOICE_SV_ALLTALK)
    {
        return;
    }
    
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Client has been turned back human.
 * 
 * @param client    The client index.
 */
VoiceOnClientHuman(client)
{
    // If voice module is disabled or sv_alltalk is enabled, then stop.
    if (!VOICE_ZR_VOICE || VOICE_SV_ALLTALK)
    {
        return;
    }
    
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Set the receiver ability to listen to the sender.
 * Note:  This function is from sdktools_voice, it fails if iSender is muted.
 *
 * @param iReceiver		The listener index.
 * @param iSender		The sender index.
 * @param override      The listen override (see sdktools_voice.inc enum ListenOverride)
 * @return			    True if successful otherwise false.
 */
stock bool:VoiceSetClientOverride(iReceiver, iSender, ListenOverride:override)
{
    // If the sender is muted, then return false.
    if (VoiceIsClientMuted(iSender))
    {
        return false;
    }
    
    // Set the new override if the override isn't the same.
    if (GetListenOverride(iReceiver, iSender) != override)
    {
        // Debug stuff
        /*switch (override)
        {
            case Listen_Yes:
            {
                PrintToChat(iReceiver, "You can now hear %N", iSender);
            }
            case Listen_No:
            {
                PrintToChat(iReceiver, "You can't hear %N now", iSender);
            }
            case Listen_Default:
            {
                PrintToChat(iReceiver, "Default between you and %N", iSender);
            }
        }*/
        
        SetListenOverride(iReceiver, iSender, override);
    }
    
    return true;
}

/**
 * Set which team the client is allowed to listen/speak with.
 * 
 * @param client    The client index.
 * @param zombie    True to permit verbal communication to zombies only, false for humans only.
 * @param mute      If this is true, the client isn't allowed to listen or hear anybody. (Implemented for zr_voice_zombies_mute) 
 */
stock VoiceSetClientTeam(client, bool:zombie, bool:mute = false)
{
    // x = Client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If sender isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // No need to alter listening/speaking flags between one client.
        if (client == x)
        {
            continue;
        }
        
        // Client can only listen/speak if the sender is on their team.
        new ListenOverride:override = (zombie == InfectIsClientInfected(x) && !mute) ? Listen_Yes : Listen_No;
        
        // (Dis)allow clients to listen/speak with each other, don't touch if the sender is muted.
        VoiceSetClientOverride(client, x, override);
        VoiceSetClientOverride(x, client, override);
    }
}

/**
 * Update a client's listening/speaking status.
 * 
 * @param client    The client index.
 */
stock VoiceUpdateClient(client)
{
    new bool:zombies_mute = GetConVarBool(g_hCvarsList[CVAR_VOICE_ZOMBIES_MUTE]);
    new bool:infected = InfectIsClientInfected(client);
    
    // Set the client's listening/speaking status to their current team, or if the client is a zombie and zombie mute is enabled, then mute.
    VoiceSetClientTeam(client, infected, (zombies_mute && infected));
}

/**
 * This function returns if the client is muted.
 * 
 * @param client    The client index.
 * @return          True if the client is muted, false if not.
 */
stock bool:VoiceIsClientMuted(client)
{
    // Return true if the mute flag isn't on the client.
    return bool:(GetClientListeningFlags(client) & VOICE_MUTED);
}

/**
 * Allow all clients to listen and speak with each other.
 */
stock VoiceAllTalk()
{
    // x = Receiver index.
    // y = Sender index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If receiver isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        for (new y = 1; y <= MaxClients; y++)
        {
            // If sender isn't in-game, then stop.
            if (!IsClientInGame(y))
            {
                continue;
            }
            
            // No need to alter listening/speaking flags between one client.
            if (x == y)
            {
                continue;
            }
            
            // Receiver (x) can now hear the sender (y), only if sender isn't muted.
            VoiceSetClientOverride(x, y, Listen_Yes);
        }
    }
}

/**
 * Reset voice listening/speaking permissions on all clients.
 * 
 * @param humanzombie   If true, clients will be updated to only hear clients on their team (human/zombie, not T/CT), false will undo overrides.
 */
stock VoiceReset()
{
    // Cache it here, so we don't retrieve the same value several times.
    new bool:voice = VOICE_ZR_VOICE;
    
    // x = Client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If voice module is enabled, and alltalk is off, then update client with human/zombie rules.
        if (voice && !VOICE_SV_ALLTALK)
        {
            VoiceUpdateClient(x);
            continue;
        }
        
        for (new y = 1; y <= MaxClients; y++)
        {
            // If sender isn't in-game, then stop.
            if (!IsClientInGame(y))
            {
                continue;
            }
            
            // No need to alter listening/speaking flags between one client.
            if (x == y)
            {
                continue;
            }
            
            // Undo the override.
            VoiceSetClientOverride(x, y, Listen_Default);
        }
    }
}