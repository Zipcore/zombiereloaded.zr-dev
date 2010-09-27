/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infection.zrc.inc
 *  Type:          Module
 *  Description:   Handles infection for ZRClassic.
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
 * The weapon classname to fire player_death with.
 * Default: "zombie_claws_of_death" for compatibility.
 */
#define ZOMBIE_WEAPON_CLASSNAME "zombie_claws_of_death"

/**
 * This module's identifier.
 */
new Module:g_moduleZRCInfect;

/**
 * Function for outside files to use to return the module's identifier.
 */
stock Module:ZRCInfect_GetIdentifier() { return g_moduleZRCInfect; }

/**
 * Cvar handles.
 */
new Handle:g_hCvarMZombieMinTime;
new Handle:g_hCvarMZombieMaxTime;
new Handle:g_hCvarMZombieRatio;
new Handle:g_hCvarMZombieBlockConsecutive;

/**
 * Variable/function set to check/set if zombies are present.
 */
new bool:g_bZombiesPresent;
stock bool:ZRCInfect_AreZombiesPresent() { return g_bZombiesPresent; }
stock ZRCInfect_SetZombiesPresent(bool:zombiespresent) { g_bZombiesPresent = zombiespresent; }

/**
 * Timer handles for mother zombie selection and round timer.
 */
new Handle:g_hMotherInfect;
new Handle:g_hMotherInfectCountdown;

/**
 * Keeps track of who the mother zombies are for the current round.
 */
new bool:g_bMotherZombie[MAXPLAYERS + 1];

/**
 * Keeps track of if a client was mother zombie in the previous round.
 * See cvar g_hCvarMZombieBlockConsecutive.
 */
new bool:g_bWasMotherZombie[MAXPLAYERS + 1];

/**
 * Register this module.
 */
ZRCInfect_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "ZRC Infection Handler");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "zrcinfection");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Handles infection for ZRClassic gameplay.");
    moduledata[ModuleData_Dependencies][0] = ZRC_GetIdentifier();
    moduledata[ModuleData_Dependencies][1] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleZRCInfect = ModuleMgr_Register(moduledata);
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_OnEventsRegister",         "ZRCInfect_OnEventsRegister");
}

/**
 * Register all events here.
 */
public ZRCInfect_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_OnMapStart",               "ZRCInfect_OnMapStart");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_OnMapEnd",                 "ZRCInfect_OnMapEnd");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_OnClientPutInServer",      "ZRCInfect_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_OnClientDisconnect",       "ZRCInfect_OnClientDisconnect");
    
    #if defined PROJECT_GAME_CSS
    
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_RoundStart",               "ZRCInfect_RoundStart");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_RoundFreezeEnd",           "ZRCInfect_RoundFreezeEnd");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_RoundEnd",                 "ZRCInfect_RoundEnd");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_PlayerSpawn",              "ZRCInfect_PlayerSpawn");
    EventMgr_RegisterEvent(g_moduleZRCInfect, "Event_PlayerHurt",               "ZRCInfect_PlayerHurt");
    
    #endif
}

/**
 * Plugin is loading.
 */
ZRCInfect_OnPluginStart()
{
    // Register the module.
    ZRCInfect_Register();
    
    // Create cvars.
    g_hCvarMZombieMinTime =             CreateConVar("zrc_mzombie_min_time", "1.0", "Minimum time from the start of the round until picking the mother zombie(s).");
    g_hCvarMZombieMaxTime =             CreateConVar("zrc_mzombie_max_time", "1.0", "Maximum time from the start of the round until picking the mother zombie(s).");
    g_hCvarMZombieRatio =               CreateConVar("zrc_mzombie_ratio", "5", "Number of humans to infect as mother zombies in proportion to number of humans on the server. ['0' = Always one mother zombie]");
    g_hCvarMZombieBlockConsecutive =    CreateConVar("zrc_mzombie_block_consecutive", "1", "Prevent a player from being chosen as mother zombie two rounds in a row.");
    
    // Create commands.
    RegConsoleCmd("zrc_infect", ZRCInfect_InfectCommand, "Turn a player into a zombie. Usage: zr_infect <#userid|name> [respawn - 1/0]");
    RegConsoleCmd("zrc_human", ZRCInfect_HumanCommand, "Turn a player into a human. Usage: zr_human <#userid|name> [respawn - 1/0] [protect - 1/0]");
}

/**
 * The map has started.
 */
public ZRCInfect_OnMapStart()
{
    // Stop timers.
    Util_CloseHandle(g_hMotherInfect);
    Util_CloseHandle(g_hMotherInfectCountdown);
}

/**
 * The map has ended.
 */
public ZRCInfect_OnMapEnd()
{
    // Stop timers.
    Util_CloseHandle(g_hMotherInfect);
    Util_CloseHandle(g_hMotherInfectCountdown);
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public ZRCInfect_OnClientPutInServer(client)
{
    g_bMotherZombie[client] = false;
    g_bWasMotherZombie[client] = false;
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public ZRCInfect_OnClientDisconnect(client)
{
    // Infect a new zombie if the last one leaves.
    if (TLib_IsClientZombie(client))
    {
        // If there are no more zombies then pick a new one.
        if (TLib_CountTeam(VTeam_Zombie, true) <= 1)
        {
            new newzombie = ZRCInfect_MotherZombie(1);
            if (newzombie != -1)
            {
                TransMgr_PrintText(newzombie, MsgFormat_Plugin, MsgType_Chat, _, _, "ZRC infect disconnect");
            }
        }
    }
}

/**
 * Round has started.
 */
public ZRCInfect_RoundStart()
{
    // Stop timers.
    Util_CloseHandle(g_hMotherInfect);
    Util_CloseHandle(g_hMotherInfectCountdown);
    
    // Fresh round, no zombies.
    g_bZombiesPresent = false;
}

/**
 * Pre-round freezetime has finished.
 */
public ZRCInfect_RoundFreezeEnd()
{
    // Stop timers.
    Util_CloseHandle(g_hMotherInfect);
    Util_CloseHandle(g_hMotherInfectCountdown);
    
    if (!g_bZombiesPresent)
    {
        // Get min and max times and pick a random time inbetween.
        new Float:mintime = GetConVarFloat(g_hCvarMZombieMinTime);
        new Float:maxtime = GetConVarFloat(g_hCvarMZombieMaxTime);
        new randomtime = RoundToNearest(GetRandomFloat(mintime, maxtime));
        
        g_hMotherInfect = CreateTimer(float(randomtime), ZRCInfect_InitMZombieTimer, false, TIMER_FLAG_NO_MAPCHANGE);
    }
}

/**
 * Round has ended.
 */
public ZRCInfect_RoundEnd()
{
    // Stop timers.
    Util_CloseHandle(g_hMotherInfect);
    Util_CloseHandle(g_hMotherInfectCountdown);
    
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        // Only reset clients that are in the game to human.  Ignore unassigned/spectators.
        if (TLib_IsClientPlaying(client))
            TLib_SetClientTeam(client, VTeam_Human);
        
        // Keep track of who was mother zombie this round for next round.
        g_bWasMotherZombie[client] = g_bMotherZombie[client];
        g_bMotherZombie[client] = false;
    }
    
    // No more zombies.
    g_bZombiesPresent = false;
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public ZRCInfect_PlayerSpawn(client)
{
}


/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public ZRCInfect_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    // Check if the victim and attacker are valid clients.
    if (!Util_IsClientInGame(victim) || !Util_IsClientInGame(attacker))
        return;
    
    // Check if a zombie is attacking a human.
    if (TLib_IsClientZombie(attacker) && TLib_IsClientHuman(victim))
    {
        #if defined PROJECT_GAME_CSS
            if (StrEqual(weapon, "knife"))
            {
                ZRCInfect_HumanToZombie(victim, attacker, false);
            }
        #endif
    }
}

/**
 * Turn a human into a zombie.
 * 
 * @param client    The client being infected.
 * @param attacker  The client doing the infecting.
 * @param mzombie   True to make a mother zombie, false for normal zombie.
 */
ZRCInfect_HumanToZombie(client, attacker = 0, bool:mzombie)
{
    PrintToChatAll("INFECT %N infected %N", attacker, client);
    TLib_SetClientTeam(client, VTeam_Zombie);
    
    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, _, _, "ZRC human to zombie");
    
    #if defined PROJECT_GAME_CSS
        // If this is the first zombie of the round then update the client teams.
        if (!g_bZombiesPresent)
        {
            TLib_UpdateClientTeams();
        }
    #endif
    
    // Definitely zombies present now.
    g_bZombiesPresent = true;
    
    if (mzombie)
    {
        g_bMotherZombie[client] = true;
    }
    
    #if defined PROJECT_GAME_CSS
        // Only fire the event if there is an attacker.
        if (attacker != 0)
        {
            new Handle:hEventPlayerDeath = CreateEvent("player_death", true);
            SetEventInt(hEventPlayerDeath, "userid", GetClientUserId(client));
            SetEventInt(hEventPlayerDeath, "attacker", GetClientUserId(attacker));
            SetEventString(hEventPlayerDeath, "weapon", ZOMBIE_WEAPON_CLASSNAME);
            SetEventBool(hEventPlayerDeath, "headshot", false);
            FireEvent(hEventPlayerDeath);
        }
    #endif
    
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = client;
    eventdata[1][0] = attacker;
    eventdata[2][0] = mzombie;
    
    EventMgr_Forward(g_EvOnClientInfected, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Turn a zombie into a human.
 * 
 * @param client    The zombie client to turn human.
 */
ZRCInfect_ZombieToHuman(client)
{
    TLib_SetClientTeam(client, VTeam_Human);
    
    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, _, _, "ZRC zombie to human");
    
    // If client was a mother zombie then set to false.
    g_bMotherZombie[client] = false;
    
    // Forward event to all modules.
    new any:eventdata[1][1];
    
    eventdata[0][0] = client;
    
    EventMgr_Forward(g_EvOnClientHuman, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Choose a random mother zombie.
 * 
 * @param override_count    If this is set to something > 0 then this is how many mother zombies will be spawned.
 * 
 * @return                  The first mother zombie infected, -1 if none were infected.
 */
ZRCInfect_MotherZombie(override_count = 0)
{
    // Create eligible player list.
    new Handle:adtClients;
    new count = Util_BuildClientList(adtClients, UTILS_FILTER_TEAM | UTILS_FILTER_ALIVE | UTILS_FILTER_HUMAN);
    
    // If there are no clients that can become a zombie then stop.
    if (count == 0)
    {
        CloseHandle(adtClients);
        return -1;
    }
    
    // Prune the list further.
    new client;
    new bool:blockconsinfect = GetConVarBool(g_hCvarMZombieBlockConsecutive);
    for (new cindex = 0; cindex < count; cindex++)
    {
        // Don't pick a mother zombie if there is only 1 client left at this point.
        if (count <= 1)
            break;
        
        // Get client stored in array index.
        client = GetArrayCell(adtClients, cindex);
        
        // Prune out clients that were mother zombies last round to prevent double-infection.
        if (blockconsinfect && g_bWasMotherZombie[client])
        {
            // Prune, subtract 1 from count, and backtrack one index.
            RemoveFromArray(adtClients, cindex); count--; cindex--;
        }
    }
    
    // Determine how many mother zombies to spawn.
    new mzombiecount;
    if (override_count > 0)
    {
        mzombiecount = override_count;
    }
    else
    {
        // Ratio of mother zombies to humans.
        new ratio = GetConVarInt(g_hCvarMZombieRatio);
        
        // Count living clients on the human team.
        new humancount = TLib_CountTeam(VTeam_Human, true);
        
        // Calculate mother zombie count.
        mzombiecount = (ratio > 0) ? RoundToNearest(float(humancount) / float(ratio)) : 1;
        
        // If mothercount is 0, then set to 1.
        if (mzombiecount == 0)
            mzombiecount = 1;
    }
    
    new randindex;
    new randclient;
    new firstinfected = -1;
    for (new cindex = 0; cindex < mzombiecount; cindex++)
    {
        // Should always leave at least one human and stop if there are no more clients left.
        if (TLib_CountTeam(VTeam_Human, true) <= 1 || count <= 0)
            break;
        
        // Get a random client from the array, infect, and remove from list.
        randindex = GetRandomInt(0, count - 1);
        randclient = GetArrayCell(adtClients, randindex);
        ZRCInfect_HumanToZombie(randclient, _, true);
        RemoveFromArray(adtClients, randindex); count--;
        
        // Store first infected client.
        if (cindex == 0)
            firstinfected = randclient;
    }
    
    CloseHandle(adtClients);
    
    return firstinfected;
}

/**
 * Timer callback.
 * Choose a mother zombie. 
 * 
 * @param timer     The timer handle.
 */
public Action:ZRCInfect_InitMZombieTimer(Handle:timer)
{
    g_hMotherInfect = INVALID_HANDLE;
    
    // Pick a random mother zombie.
    ZRCInfect_MotherZombie();
}

/**
 * Command callback: zr_infect
 * Turn a player into a zombie.
 * 
 * @param client    The client index. 
 * @param argc      The number of arguments that the server sent with the command.
 */
public Action:ZRCInfect_InfectCommand(client, argc)
{
    // Check if the this core module is disabled, if so then don't do anything with it.
    if (ModuleMgr_IsDisabled(g_moduleZRCInfect))
    {
        return Plugin_Continue;
    }
    
    // Check if the client has permission to use this.
    if (!AccessMgr_HasAccess(client, g_moduleZRCInfect))
    {
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "No access to command");
        return Plugin_Handled;
    }
    
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command infect syntax");
        return Plugin_Handled;
    }
    
    decl String:target[MAX_NAME_LENGTH], String:targetname[MAX_NAME_LENGTH];
    new targets[MAXPLAYERS], bool:tn_is_ml, result;
    
    // Get targetname.
    GetCmdArg(1, target, sizeof(target));
    
    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE, targetname, sizeof(targetname), tn_is_ml);
        
    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        TransMgr_ReplyToTargetError(client, result);
        return Plugin_Handled;
    }
    
    // Check optional parameter.
    decl String:strRespawn[4];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));
    new bool:bRespawn = bool:StringToInt(strRespawn);
    
    for (new tindex = 0; tindex < result; tindex++)
    {
        // Can't infect a zombie.
        if (TLib_IsClientHuman(targets[tindex]))
        {
            // If a human is infected, cancel the timer that infects a mother zombie.
            if (g_bZombiesPresent)
            {
                ZRCInfect_HumanToZombie(targets[tindex], _, false);
                if (result == 1)
                {
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command infect successful", targets[tindex]);
                }
            }
            else
            {
                Util_CloseHandle(g_hMotherInfect);
                ZRCInfect_HumanToZombie(targets[tindex], _, true);
                if (result == 1)
                {
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command infect mother successful", targets[tindex]);
                }
            }
        }
        else
        {
            if (result == 1)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command infect unsuccessful", targets[tindex]);
            }
        }
    }
    
    return Plugin_Handled;
}

/**
 * Command callback: zr_human
 * Turn a player into a human.
 * 
 * @param client    The client index. 
 * @param argc      The number of arguments that the server sent with the command.
 */
public Action:ZRCInfect_HumanCommand(client, argc)
{
    // Check if the this core module is disabled, if so then don't do anything with it.
    if (ModuleMgr_IsDisabled(g_moduleZRCInfect))
    {
        return Plugin_Continue;
    }
    
    // Check if the client has permission to use this.
    if (!AccessMgr_HasAccess(client, g_moduleZRCInfect))
    {
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "No access to command");
        return Plugin_Handled;
    }
    
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command infect syntax");
        return Plugin_Handled;
    }
    
    decl String:target[MAX_NAME_LENGTH], String:targetname[MAX_NAME_LENGTH];
    new targets[MAXPLAYERS], bool:tn_is_ml, result;
    
    // Get targetname.
    GetCmdArg(1, target, sizeof(target));
    
    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE, targetname, sizeof(targetname), tn_is_ml);
        
    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        TransMgr_ReplyToTargetError(client, result);
        return Plugin_Handled;
    }
    
    // Check optional parameters.
    decl String:strRespawn[4];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));
    new bool:bRespawn = bool:StringToInt(strRespawn);
    
    decl String:strProtect[4];
    GetCmdArg(3, strProtect, sizeof(strProtect));
    new bool:bProtect = bool:StringToInt(strProtect);
    
    for (new tindex = 0; tindex < result; tindex++)
    {
        // Can't infect a zombie.
        if (TLib_IsClientZombie(targets[tindex]))
        {
            ZRCInfect_ZombieToHuman(targets[tindex]);
            if (result == 1)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command human successful", targets[tindex]);
            }
        }
        else
        {
            if (result == 1)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, _, "ZRC command human unsuccessful", targets[tindex]);
            }
        }
    }
    
    return Plugin_Handled;
}
