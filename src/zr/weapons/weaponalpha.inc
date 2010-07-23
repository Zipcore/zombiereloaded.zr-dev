/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weaponalpha.inc
 *  Type:          Core 
 *  Description:   Weapon alpha functions, and alpha updating on drop/pickup.
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
 * Default alpha on any CS:S weapon.
 */
#define WEAPONALPHA_DEFAULT_VALUE 255

/**
 * Client has just picked up a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.   
 */
WeaponAlphaEquipPost(client, weapon)
{
    // Get client's current alpha.
    new alpha = ToolsGetEntityAlpha(client);
    
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(weapon, alpha);
}

/**
 * Called when a client drops their weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.
 */
WeaponAlphaDrop(weapon)
{
    // If weapon isn't a valid entity, then stop.
    if (weapon < MaxClients)
    {
        return;
    }
    
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(weapon, WEAPONALPHA_DEFAULT_VALUE);
}

/**
 * A client's alpha has been changed by the plugin.
 * 
 * @param client    The client index.
 */
WeaponAlphaOnClientAlphaChanged(client, alpha)
{
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(client, alpha);
}

/**
 * Set's the alpha on a client's weapons.
 * 
 * @param entity    If client index is given, alpha will be set on all their weapons.
 *                  If a non-client index is given, alpha will be set on given entity.
 * @param alpha     The alpha to set the weapons to.
 */
WeaponAlphaApplyWeaponAlpha(entity, alpha)
{
    if (entity > MaxClients)
    {
        // Turn rendermode on, on the weapon.
        SetEntityRenderMode(entity, RENDER_TRANSALPHA);
        
        // Set alpha value on the weapon.
        SetEntityRenderColor(entity, _, _, _, alpha);
        
        // Entity alpha has been set, so stop.
        return;
    }
    
    // Get client's list of weapons.
    new weapons[WepLib_Slots];
    WeaponsGetClientWeapons(entity, weapons);
    
    // Loop through array slots and set alpha.
    // x = weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        // If weapon is invalid, then stop.
        if (weapons[x] == -1)
        {
            continue;
        }
        
        // Turn rendermode on, on the weapon.
        SetEntityRenderMode(weapons[x], RENDER_TRANSALPHA);
        
        // Set alpha value on the weapon.
        SetEntityRenderColor(weapons[x], _, _, _, alpha);
    }
}
