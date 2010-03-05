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

/**
 * @section Counter Strike: Source specific damage flags.
 */
#define DMG_CSS_FALL        (DMG_FALL)      // Client was damaged by falling.
#define DMG_CSS_BLAST       (DMG_BLAST)     // Client was damaged by explosion.
#define DMG_CSS_BURN        (DMG_DIRECT)    // Client was damaged by fire.
#define DMG_CSS_BULLET      (DMG_NEVERGIB)  // Client was shot or knifed.
#define DMG_CSS_HEADSHOT    (1 << 30)       // Client was shot in the head.

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
 * Array to keep track of normal/mother zombies.
 */
new bool:g_bDamageMotherZombie[MAXPLAYERS + 1];

/**
 * Hook commands related to damage here.
 * Note: This isn't OnCommandsHook because this depends on cvars.
 */
new bool:g_bSuicideCmdsHooked = false;
DamageLoad()
{
    if (g_bSuicideCmdsHooked)
    {
        return;
    }
    
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
    
    // Important:  If ZR can be unloaded some day, make sure to remove the listeners and set this to false.
    g_bSuicideCmdsHooked = true;
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
DamageClientInit(client)
{
    // Hook "TraceAttack" and "OnTakeDamage" on the client.
    SDKHook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKHook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    // Unhook "TraceAttack" and "OnTakeDamage" on the client.
    SDKUnhook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKUnhook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
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
public Action:DamageTraceAttack(client, &attacker, &inflictor, &Float:damage, &damagetype, &ammotype, hitbox, hitgroup)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return Plugin_Continue;
    }
    
    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return Plugin_Continue;
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
            return Plugin_Continue;
        }
        
        // Stop bullet from hurting client.
        return Plugin_Handled;
    }
    
    // Here we know that attacker and client are different teams.
    
    // If client is a human, then allow damage.
    if (InfectIsClientHuman(client))
    {
        // Allow damage.
        return Plugin_Continue;
    }
    
    // If hitgroups is disabled, hitgroups didn't load, or hitgroup damage control is disabled, then stop.
    new bool:hitgroupsenabled = GetConVarBool(g_hCvarsList[CVAR_HITGROUPS]);
    new bool:hitgroupsloaded = ConfigIsConfigLoaded(File_Hitgroups);
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!hitgroupsenabled || !hitgroupsloaded || !damagehitgroups)
    {
        // Allow damage.
        return Plugin_Continue;
    }
    
    // If damage is disabled for this hitgroup, then stop.
    new index = HitgroupToIndex(hitgroup);
    
    // If index can't be found, then allow damage.
    if (index == -1)
    {
        // Allow damage.
        return Plugin_Continue;
    }
    
    new bool:candamage = HitgroupsCanDamage(index);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return Plugin_Handled;
    }
    
    // Allow damage.
    return Plugin_Continue;
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
public Action:DamageOnTakeDamage(client, &attacker, &inflictor, &Float:damage, &damagetype)
{
    // Get classname of the inflictor.
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    
    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return Plugin_Continue;
    }
    
    new Action:result;
    
    // Forward this hook to another module an return (or not) what it wants.
    result = NapalmOnTakeDamage(client, damagetype);
    
    // If the napalm module want's to block the damage, then block it here.
    if (result == Plugin_Handled || result == Plugin_Stop)
    {
        return result;
    }
    
    // Client was shot or knifed.
    if (damagetype & DMG_CSS_BULLET)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }
        
        // Get zombie flag for each client.
        new bool:clientzombie = InfectIsClientInfected(client);
        new bool:attackerzombie = InfectIsClientInfected(attacker);
        
        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return Plugin_Continue;
        }
        
        // We know that clientzombie is the opposite of attacker zombie.
        
        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return Plugin_Continue;
        }
        
        // Client is about to be infected, re-add HP so they aren't killed by knife.
        new health = GetClientHealth(client);
        SetEntityHealth(client, health + RoundToNearest(damage));
        
        // Allow damage.
        return Plugin_Continue;
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_CSS_BLAST)
    {
        // If blast damage is blocked, then stop.
        new bool:damageblockblast = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]);
        if (!damageblockblast)
        {
            return Plugin_Continue;
        }
        
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }
        
        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return Plugin_Continue;
        }
        
        // Stop damage.
        return Plugin_Handled;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_CSS_FALL)
    {
        // If class has "nofalldamage" disabled, then allow damage.
        new bool:blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return Plugin_Continue;
        }
        
        // Stop damage.
        return Plugin_Handled;
    }
    
    // Allow damage.
    return Plugin_Continue;
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
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
            
            return Plugin_Handled;
        }
        
        // If client is a zombie, and suicide intercept is enabled for zombies, then block command.
        if (!g_bDamageMotherZombie[client] && suicidezombie)
        {
            // Tell client their command has been intercepted, and log.
            TranslationPrintToChat(client, "Damage suicide intercept");
            LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
            
            return Plugin_Handled;
        }
    }
    
    // If client is a human, and suicide intercept is enabled for humans, then block command.
    if (InfectIsClientHuman(client) && suicidehuman)
    {
        // Tell client their command has been intercepted, and log.
        TranslationPrintToChat(client, "Damage suicide intercept");
        LogEvent(false, LogTypeOld_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
        
        return Plugin_Handled;
    }
    
    // Allow command.
    return Plugin_Continue;
}
