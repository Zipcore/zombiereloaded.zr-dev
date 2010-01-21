/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          damage.inc
 *  Type:          Core
 *  Description:   Modify damage stuff here.
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

#if defined USE_SDKHOOKS
    /**
     * @section Counter Strike: Source specific damage flags.
     */
    #define DMG_CSS_FALL        (DMG_FALL)      // Client was damaged by falling.
    #define DMG_CSS_BLAST       (DMG_BLAST)     // Client was damaged by explosion.
    #define DMG_CSS_BURN        (DMG_DIRECT)    // Client was damaged by fire.
    #define DMG_CSS_BULLET      (DMG_NEVERGIB)  // Client was shot or knifed.
    #define DMG_CSS_HEADSHOT    (1 << 30)       // Client was shot in the head.
    /**
     * @endsection
     */
#endif

/**
 * @section Prop damage defines (see cvars zr_props_block_bullets_ignore or zr_props_block_blast_ignore)
 */
#define DAMAGE_PROPS_MAX_CLASSNAMES 15
#define DAMAGE_PROPS_MAX_LENGTH 32
/**
 * @endsection
 */

/**
 * @section Suicide intercept defines.
 */
#define DAMAGE_SUICIDE_MAX_CMDS 10
#define DAMAGE_SUICIDE_MAX_LENGTH 16
/**
 * @endsection
 */

/**
 * Prop damage types.
 */
enum DamagePropTypes
{
    DamageProp_Bullets,
    DamageProp_Blast
}

/**
 * Array to store TraceAttack HookIDs.
 */
new g_iDamageTraceAttackHookID[MAXPLAYERS + 1] = {-1, ...};

/**
 * Array to store OnTakeDamage HookIDs.
 */
new g_iDamageOnTakeDamageHookID[MAXPLAYERS + 1] = {-1, ...};

/**
 * Array to keep track of normal/mother zombies.
 */
new bool:g_bDamageMotherZombie[MAXPLAYERS + 1];

/**
 * Hook commands related to damage here.
 */
DamageOnCommandsHook()
{
    // Create array to store cmds.
    new String:arrayCmds[DAMAGE_SUICIDE_MAX_CMDS][DAMAGE_SUICIDE_MAX_LENGTH];
    
    // Separate the list into an array.
    new cmdcount = CvarsListToArray(g_hCvarsList[CVAR_SUICIDE_CMDS], arrayCmds, DAMAGE_SUICIDE_MAX_CMDS, DAMAGE_SUICIDE_MAX_LENGTH, ",");
    
    // x = Array index.
    // arrayCmds[x] = suicide command.
    for (new x = 0; x <= cmdcount - 1; x++)
    {
        TrimString(arrayCmds[x]);
        AddCommandListener(DamageSuicideIntercept, arrayCmds[x]);
    }
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
DamageClientInit(client)
{
    // Hook damage callbacks.
    #if defined USE_SDKHOOKS
        SDKHook(client, SDKHook_TraceAttack, DamageTraceAttack);
        SDKHook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
        
        // Set dummy values so it think it's hooked.
        g_iDamageTraceAttackHookID[client] = 1;
        g_iDamageOnTakeDamageHookID[client] = 1;
    #else
        g_iDamageTraceAttackHookID[client] = ZRTools_HookTraceAttack(client, DamageTraceAttack);
        g_iDamageOnTakeDamageHookID[client] = ZRTools_HookOnTakeDamage(client, DamageOnTakeDamage);
    #endif
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    // Unhook damage callbacks, and reset variables.
    
    if (g_iDamageTraceAttackHookID[client] != -1)
    {
        #if defined USE_SDKHOOKS
            SDKUnhook(client, SDKHook_TraceAttack, DamageTraceAttack);
        #else
            ZRTools_UnhookTraceAttack(g_iDamageTraceAttackHookID[client]);
        #endif
        
        g_iDamageTraceAttackHookID[client] = -1;
    }
    
    if (g_iDamageOnTakeDamageHookID[client] != -1)
    {
        #if defined USE_SDKHOOKS
            SDKUnhook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
        #else
            ZRTools_UnhookOnTakeDamage(g_iDamageOnTakeDamageHookID[client]);
        #endif
        
        g_iDamageOnTakeDamageHookID[client] = -1;
    }
}

/**
 * A client was infected.
 * 
 * @param client        The client index.
 * @param motherinfect  True if the zombie is mother, false if not.
 */
DamageOnClientInfected(client, bool:motherinfect)
{
    // Update if client is a mother zombie or not.
    g_bDamageMotherZombie[client] = motherinfect;
}

// Prop Damage

/**
 * Called when an entity is created.
 *
 * @param entity    Entity index.
 * @param classname Class name.
 */
DamageOnEntityCreated(entity)
{
    // If the entity is a client, then stop.
    if (entity <= MaxClients)
    {
        return;
    }
    
    // Hook OnTakeDamage on the entity, to later possibly block it.
    SDKHook(entity, SDKHook_OnTakeDamage, DamagePropOnTakeDamage);
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 * 
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param damagetype    The type of damage inflicted.
 * @param ammotype      The ammo type of the attacker's weapon. 
 * @return              Return Plugin_Handled to stop the damage to client.
 *                      Plugin_Continue to allow damage to client.
 */
public Action:DamagePropOnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    // If bullet damage is being done, then handle it here.
    if (damagetype & DMG_CSS_BULLET)
    {
        // If the block bullets cvar is disabled, then allow damage.
        new bool:block_bullets = GetConVarBool(g_hCvarsList[CVAR_PROPS_BLOCK_BULLETS]);
        if (!block_bullets)
        {
            return Plugin_Continue;
        }
        
        // If the entity is ignored in the cvar, then allow damage.
        new bool:ignore = DamageIsEntityIgnored(victim, DamageProp_Bullets);
        if (ignore)
        {
            return Plugin_Continue;
        }
        
        // Block the damage.
        return Plugin_Handled;
    }
    // If blast damage is being done, then handle it here.
    else if (damagetype & DMG_BLAST)
    {
        // If the block blast cvar is enabled, then block.
        new bool:block_blast = GetConVarBool(g_hCvarsList[CVAR_PROPS_BLOCK_BLAST]);
        if (!block_blast)
        {
            return Plugin_Continue;
        }
        
        // If the entity is ignored in the cvar, then allow damage.
        new bool:ignore = DamageIsEntityIgnored(victim, DamageProp_Blast);
        if (ignore)
        {
            return Plugin_Continue;
        }
        
        // Block the damage.
        return Plugin_Handled;
    }
    
    // Allow damage.
    return Plugin_Continue;
}

/**
 * Checks if an entity is ignored in the prop_block_*_ignore cvars.
 * 
 * @param entity    The entity index.
 * @param type      The type to check. (see enum DamagePropTypes at the top of the file)
 * 
 * @return          True if the entity is ignored, false if not.
 */
stock bool:DamageIsEntityIgnored(entity, DamagePropTypes:type)
{
    // Get the classname of the entity.
    decl String:classname[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    
    // Create array to store classnames.
    new String:arrayClassnames[DAMAGE_PROPS_MAX_CLASSNAMES][DAMAGE_PROPS_MAX_LENGTH];
    
    // Separate the list into an array.
    new classnamecount;
    if (type == DamageProp_Bullets)
    {
        classnamecount = CvarsListToArray(g_hCvarsList[CVAR_PROPS_BLOCK_BULLETS_IGNORE], arrayClassnames, DAMAGE_PROPS_MAX_CLASSNAMES, DAMAGE_PROPS_MAX_LENGTH, ",");
    }
    else if (type == DamageProp_Blast)
    {
        classnamecount = CvarsListToArray(g_hCvarsList[CVAR_PROPS_BLOCK_BLAST_IGNORE], arrayClassnames, DAMAGE_PROPS_MAX_CLASSNAMES, DAMAGE_PROPS_MAX_LENGTH, ",");
    }
    
    // x = Array index.
    // arrayClassnames[x] = classname, or partial classname.
    for (new x = 0; x <= classnamecount - 1; x++)
    {
        TrimString(arrayClassnames[x]);
        if (StrContains(classname, arrayClassnames[x], false) > -1)
        {
            return true;
        }
    }
    
    // Didn't match any classnames.
    return false;
}

/**
 * Hook: TraceAttack
 * Called right before the bullet enters a client.
 * 
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param hitbox        The hitbox index.
 * @param hitgroup      The hitgroup index.  
 * @return              Return ZRTools_Handled to stop bullet from hitting client.
 *                      ZRTools_Continue to allow bullet to hit client.
 */
#if defined USE_SDKHOOKS
public Action:DamageTraceAttack(client, &attacker, &inflictor, &Float:damage, &damagetype, &ammotype, hitbox, hitgroup)
#else
public ZRTools_Action:DamageTraceAttack(client, inflictor, attacker, Float:damage, hitbox, hitgroup)
#endif
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return ACTION_CONTINUE;
    }
    
    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return ACTION_CONTINUE;
    }
    
    // Get zombie flag for each client.
    new bool:clientzombie = InfectIsClientInfected(client);
    new bool:attackerzombie = InfectIsClientInfected(attacker);
    
    // If the flags are the same on both clients, then stop.
    if (clientzombie == attackerzombie)
    {
        // If friendly fire is blocked, then allow damage.
        new bool:damageblockff = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]);
        if (!damageblockff)
        {
            return ACTION_CONTINUE;
        }
        
        // Stop bullet from hurting client.
        return ACTION_HANDLED;
    }
    
    // Here we know that attacker and client are different teams.
    
    // If client is a human, then allow damage.
    if (InfectIsClientHuman(client))
    {
        // Allow damage.
        return ACTION_CONTINUE;
    }
    
    // If hitgroups is disabled, hitgroups didn't load, or hitgroup damage control is disabled, then stop.
    new bool:hitgroupsenabled = GetConVarBool(g_hCvarsList[CVAR_HITGROUPS]);
    new bool:hitgroupsloaded = ConfigIsConfigLoaded(File_Hitgroups);
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!hitgroupsenabled || !hitgroupsloaded || !damagehitgroups)
    {
        // Allow damage.
        return ACTION_CONTINUE;
    }
    
    // If damage is disabled for this hitgroup, then stop.
    new index = HitgroupToIndex(hitgroup);
    
    // If index can't be found, then allow damage.
    if (index == -1)
    {
        // Allow damage.
        return ACTION_CONTINUE;
    }
    
    new bool:candamage = HitgroupsCanDamage(index);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return ACTION_HANDLED;
    }
    
    // Allow damage.
    return ACTION_CONTINUE;
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 * 
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param damagetype    The type of damage inflicted.
 * @param ammotype      The ammo type of the attacker's weapon. 
 * @return              Return ZRTools_Handled to stop the damage to client.
 *                      ZRTools_Continue to allow damage to client.
 */
#if defined USE_SDKHOOKS
public Action:DamageOnTakeDamage(client, &attacker, &inflictor, &Float:damage, &damagetype)
#else
public ZRTools_Action:DamageOnTakeDamage(client, inflictor, attacker, Float:damage, damagetype, ammotype)
#endif
{
    // Get classname of the inflictor.
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    
    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return ACTION_CONTINUE;
    }
    
    new action;
    
    // Forward this hook to another module an return (or not) what it wants.
    action = NapalmOnTakeDamage(client, damagetype);
    
    // If the napalm module wants to return here, then return the int casted into the action type.
    if (action > -1)
    {
        #if defined USE_SDKHOOKS
            return Action:action;
        #else
            return ZRTools_Action:action;
        #endif
    }
    
    // Client was shot or knifed.
    if (damagetype & DMG_CSS_BULLET)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return ACTION_CONTINUE;
        }
        
        // Get zombie flag for each client.
        new bool:clientzombie = InfectIsClientInfected(client);
        new bool:attackerzombie = InfectIsClientInfected(attacker);
        
        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return ACTION_CONTINUE;
        }
        
        // We know that clientzombie is the opposite of attacker zombie.
        
        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return ACTION_CONTINUE;
        }
        
        // Client is about to be infected, re-add HP so they aren't killed by knife.
        new health = GetClientHealth(client);
        SetEntityHealth(client, health + RoundToNearest(damage));
        
        // Allow damage.
        return ACTION_CONTINUE;
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_CSS_BLAST)
    {
        // If blast damage is blocked, then stop.
        new bool:damageblockblast = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]);
        if (!damageblockblast)
        {
            return ACTION_CONTINUE;
        }
        
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return ACTION_CONTINUE;
        }
        
        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return ACTION_CONTINUE;
        }
        
        // Stop damage.
        return ACTION_HANDLED;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_CSS_FALL)
    {
        // If class has "nofalldamage" disabled, then allow damage.
        new bool:blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return ACTION_CONTINUE;
        }
        
        // Stop damage.
        return ACTION_HANDLED;
    }
    
    // Allow damage.
    return ACTION_CONTINUE;
}

// Suicide Intercept

/**
 * Command listener. (See zr_suicide_cmds cvar)
 * Block command if plugin thinks they are trying to commit suicide.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments in command string.
 */     
public Action:DamageSuicideIntercept(client, const String:command[], argc)
{
    // If zombie hasn't spawned, and the cvar requires it, then stop.
    new bool:suicideafterinfect = GetConVarBool(g_hCvarsList[CVAR_SUICIDE_AFTER_INFECT]);
    if (suicideafterinfect && !g_bZombieSpawned)
    {
        return Plugin_Continue;
    }
    
    // If client isn't valid, then stop. (Stop console)
    if (!ZRIsClientValid(client))
    {
        return Plugin_Continue;
    }
    
    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return Plugin_Continue;
    }
    
    // Get cvar values for suicide interception.
    new bool:suicidezombie = GetConVarBool(g_hCvarsList[CVAR_SUICIDE_ZOMBIE]);
    new bool:suicidezombiemother = GetConVarBool(g_hCvarsList[CVAR_SUICIDE_MZOMBIE]);
    new bool:suicidehuman = GetConVarBool(g_hCvarsList[CVAR_SUICIDE_HUMAN]);
    
    // Check if client is a zombie.
    if (InfectIsClientInfected(client))
    {
        // If client is a mother zombie, and suicide intercept is enabled for mother zombies zombies, then block command.
        if (g_bDamageMotherZombie[client] && suicidezombiemother)
        {
            // Tell client their command has been intercepted, and log.
            TranslationPrintToChat(client, "Damage suicide intercept");
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
            
            return Plugin_Handled;
        }
        
        // If client is a zombie, and suicide intercept is enabled for zombies, then block command.
        if (!g_bDamageMotherZombie[client] && suicidezombie)
        {
            // Tell client their command has been intercepted, and log.
            TranslationPrintToChat(client, "Damage suicide intercept");
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
            
            return Plugin_Handled;
        }
    }
    
    // If client is a human, and suicide intercept is enabled for humans, then block command.
    if (InfectIsClientHuman(client) && suicidehuman)
    {
        // Tell client their command has been intercepted, and log.
        TranslationPrintToChat(client, "Damage suicide intercept");
        LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
        
        return Plugin_Handled;
    }
    
    // Allow command.
    return Plugin_Continue;
}
