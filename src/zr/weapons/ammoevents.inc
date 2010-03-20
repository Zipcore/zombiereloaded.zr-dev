/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ammoevents.inc
 *  Description:   Event handler for ammo profile manager.
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
 * Event callback. Weapon just finished reloading.
 *
 * @param client        Client that reloaded weapon.
 * @param weaponSlot    Weapon slot.
 */
AmmoOnWeaponReloaded(client, weaponsSlot:weaponSlot)
{
    // TODO: Set clip and clip size according to client's ammo profile.
}

/**
 * Event callback. Client attempted to fire a weapon with no bullets in clip.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
AmmoOnWeaponFireOnEmpty(client, weaponsSlot:weaponSlot)
{

}

/**
 * Event callback. Client switched weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot switched to.
 */
AmmoOnWeaponSwitch(client, WeaponsSlot:weaponSlot)
{

}

/**
 * Client just spawned.
 *
 * @param client    Client index.
 */
AmmoOnClientSpawn(client)
{
    // Get free ammo managers if they don't exist.
    AmmoAssignManagers();
    
    // Load profiles.
}

/**
 * Client died.
 *
 * @param client    Client index.
 */
AmmoOnClientDeath(client)
{
    
}

/**
 * Client got infected.
 *
 * @param client    Client index.
 */
AmmoOnClientZombie(client)
{
    // Forward to death event.
    AmmoOnClientDeath(client);
}

/**
 * Client was turned into a human (usually by admin command).
 *
 * @param client    Client index.
 */
AmmoOnClientHuman(client)
{
    // Forward to spawn event.
    AmmoOnClientSpawn(client);
}

/**
 * Round just ended.
 */
AmmoOnRoundEnd()
{

}

/**
 * Client disconnected.
 *
 * @param client    Client index.
 */
AmmoOnClientDisconnect(client)
{

}
