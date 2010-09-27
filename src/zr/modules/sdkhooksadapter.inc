/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          sdkhooksadapater.inc
 *  Type:          Module
 *  Description:   Establishes hooks via sdkhooks extension and forwards them as project events.
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

#include <sdkhooks>

/**
 * This module's identifier.
 */
new Module:g_moduleSDKHAdapt;

/**
 * Event ID variables.
 */
new ProjectEvent:g_EvTraceAttack;
new ProjectEvent:g_EvOnTakeDamage;
new ProjectEvent:g_EvWeaponCanUse;
new ProjectEvent:g_EvWeaponEquip;
new ProjectEvent:g_EvWeaponDrop;

/**
 * Register this module.
 */
SDKHAdapt_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "SDKHooks Adapter");
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "sdkhooksadapter");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Establishes hooks via sdkhooks extension and forwards them as project events.");
    moduledata[ModuleData_Dependencies][0] = INVALID_MODULE;
    
    // Send this array of data to the module manager.
    g_moduleSDKHAdapt = ModuleMgr_Register(moduledata);
    
    // Create custom events.
    g_EvTraceAttack =       EventMgr_CreateEvent("Hook_TraceAttack");
    g_EvOnTakeDamage =      EventMgr_CreateEvent("Hook_OnTakeDamage");
    g_EvWeaponCanUse =      EventMgr_CreateEvent("Hook_WeaponCanUse");
    g_EvWeaponEquip =       EventMgr_CreateEvent("Hook_WeaponEquip");
    g_EvWeaponDrop =        EventMgr_CreateEvent("Hook_WeaponDrop");
    
    // Register the OnEventsRegister event to register all events in it.
    EventMgr_RegisterEvent(g_moduleSDKHAdapt, "Event_OnEventsRegister",      "SDKHAdapt_OnEventsRegister");
}

/**
 * Register all events here.
 */
public SDKHAdapt_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleSDKHAdapt, "Event_OnMyModuleEnable",         "SDKHAdapt_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_moduleSDKHAdapt, "Event_OnMyModuleDisable",        "SDKHAdapt_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleSDKHAdapt, "Event_OnClientPutInServer",      "SDKHAdapt_OnClientPutInServer");
    EventMgr_RegisterEvent(g_moduleSDKHAdapt, "Event_OnClientDisconnect",       "SDKHAdapt_OnClientDisconnect");
}

/**
 * Plugin is loading.
 */
SDKHAdapt_OnPluginStart()
{
    // Register the module.
    SDKHAdapt_Register();
}

/**
 * The module that hooked this event callback has been enabled.
 * 
 * @return      Return Plugin_Handled to stop load, and Plugin_Continue to allow it.
 */
public SDKHAdapt_OnMyModuleEnable()
{
    // Hook all clients in the server.
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        SDKHook(client, SDKHook_TraceAttack, SDKHAdapt_TraceAttack);
        SDKHook(client, SDKHook_OnTakeDamage, SDKHAdapt_OnTakeDamage);
        SDKHook(client, SDKHook_WeaponCanUse, SDKHAdapt_WeaponCanUse);
        SDKHook(client, SDKHook_WeaponEquip, SDKHAdapt_WeaponEquip);
        SDKHook(client, SDKHook_WeaponDrop, SDKHAdapt_WeaponDrop);
    }
}

/**
 * The module that hooked this event callback has been disabled.
 */
public SDKHAdapt_OnMyModuleDisable()
{
    // Unhook all clients in the server.
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        SDKUnhook(client, SDKHook_TraceAttack, SDKHAdapt_TraceAttack);
        SDKUnhook(client, SDKHook_OnTakeDamage, SDKHAdapt_OnTakeDamage);
        SDKUnhook(client, SDKHook_WeaponCanUse, SDKHAdapt_WeaponCanUse);
        SDKUnhook(client, SDKHook_WeaponEquip, SDKHAdapt_WeaponEquip);
        SDKUnhook(client, SDKHook_WeaponDrop, SDKHAdapt_WeaponDrop);
    }
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public SDKHAdapt_OnClientPutInServer(client)
{
    SDKHook(client, SDKHook_TraceAttack, SDKHAdapt_TraceAttack);
    SDKHook(client, SDKHook_OnTakeDamage, SDKHAdapt_OnTakeDamage);
    SDKHook(client, SDKHook_WeaponCanUse, SDKHAdapt_WeaponCanUse);
    SDKHook(client, SDKHook_WeaponEquip, SDKHAdapt_WeaponEquip);
    SDKHook(client, SDKHook_WeaponDrop, SDKHAdapt_WeaponDrop);
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public SDKHAdapt_OnClientDisconnect(client)
{
    SDKUnhook(client, SDKHook_TraceAttack, SDKHAdapt_TraceAttack);
    SDKUnhook(client, SDKHook_OnTakeDamage, SDKHAdapt_OnTakeDamage);
    SDKUnhook(client, SDKHook_WeaponCanUse, SDKHAdapt_WeaponCanUse);
    SDKUnhook(client, SDKHook_WeaponEquip, SDKHAdapt_WeaponEquip);
    SDKUnhook(client, SDKHook_WeaponDrop, SDKHAdapt_WeaponDrop);
}

/**
 * Stop humans on opposite teams from killing each other.
 * 
 * @param victim        The client being attacked.
 * @param attacker      The client attacking the victim.
 * @param inflictor     The entity doing damage?
 * @param damage        The amount of damage inflicted.
 * @param damagetype    A bit indicating type of damage that was being inflicted.
 * @param ammotype      The type of ammo used.
 * @param hitbox        The hitbox being shot.
 * @param hitgroup      The hitgroup being shot.
 * 
 * @return              Hook action.  See include/core.inc.
 */
public Action:SDKHAdapt_TraceAttack(victim, &attacker, &inflictor, &Float:damage, &damagetype, &ammotype, hitbox, hitgroup)
{
    // Only forwarding victim, attacker, damage, and damagetype.
    
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_CellRef, DataType_FloatRef, DataType_CellRef};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = victim;
    eventdata[1][0] = attacker;
    eventdata[2][0] = damage;
    eventdata[3][0] = damagetype;
    
    new Action:result = EventMgr_Forward(g_EvTraceAttack, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
    
    // Copy event data back to this hook's variables, to let the engine use the new values.
    victim = eventdata[0][0];
    attacker = eventdata[1][0];
    damage = eventdata[2][0];
    damagetype = eventdata[3][0];
    
    return result;
}

/**
 * Stop damage if zombie is infecting a human.
 * 
 * @param victim        The client being attacked.
 * @param attacker      The client attacking the victim.
 * @param inflictor     The entity doing damage?
 * @param damage        The amount of damage inflicted.
 * @param damagetype    A bit indicating type of damage that was being inflicted.
 *  
 * @return              Hook action.  See include/core.inc.
 */
public Action:SDKHAdapt_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    // Only forwarding victim, attacker, damage, and damagetype.
    
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_CellRef, DataType_FloatRef, DataType_CellRef};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = victim;
    eventdata[1][0] = attacker;
    eventdata[2][0] = damage;
    eventdata[3][0] = damagetype;
    
    new Action:result = EventMgr_Forward(g_EvOnTakeDamage, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
    
    // Copy event data back to this hook's variables, to let the engine use the new values.
    victim = eventdata[0][0];
    attacker = eventdata[1][0];
    damage = eventdata[2][0];
    damagetype = eventdata[3][0];
    
    return result;
}

/**
 * Called when a client is trying to pick up a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.
 * 
 * @return          Hook action.  See include/core.inc.
 */
public Action:SDKHAdapt_WeaponCanUse(client, weapon)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = client;
    eventdata[1][0] = weapon;
    
    return EventMgr_Forward(g_EvWeaponCanUse, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Called when a client equips a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.
 * 
 * @return          Hook action.  See include/core.inc.
 */
public Action:SDKHAdapt_WeaponEquip(client, weapon)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = client;
    eventdata[1][0] = weapon;
    
    return EventMgr_Forward(g_EvWeaponEquip, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Called when a client drops a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.
 * 
 * @return          Hook action.  See include/core.inc.
 */
public Action:SDKHAdapt_WeaponDrop(client, weapon)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = client;
    eventdata[1][0] = weapon;
    
    return EventMgr_Forward(g_EvWeaponDrop, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}
