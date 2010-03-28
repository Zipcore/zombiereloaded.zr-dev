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
stock AmmoOnWeaponReloaded(client, weaponsSlot:weaponSlot)
{
    // TODO: Set clip and clip size according to client's ammo profile.
}

/**
 * Event callback. Client fired his weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
stock AmmoOnWeaponFire(client, WeaponsSlot:weaponSlot)
{
    
}

/**
 * Event callback. Client attempted to fire a weapon with no bullets.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
stock AmmoOnWeaponFireOnEmpty(client, weaponsSlot:weaponSlot)
{
    // TODO: Do a virtual reload and apply the new values.
}

/**
 * Event callback. Client switched weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot switched to.
 */
stock AmmoOnWeaponSwitch(client, WeaponsSlot:weaponSlot)
{
    // TODO: Hook event using SDK Hooks.
}

/**
 * Client just spawned.
 *
 * @param client    Client index.
 */
AmmoOnClientSpawn(client)
{
    // Verify that the player is a live because the spawn event is also
    // triggered in the connection process.
    if (IsPlayerAlive(client))
    {
        // Attach managers to the client.
        AmmoManagerAttach(client);
    }
}

/**
 * Client died.
 *
 * @param client    Client index.
 */
AmmoOnClientDeath(client)
{
    // Detatch managers from the client.
    AmmoManagerDetach(client);
}

/**
 * Client got infected.
 *
 * @param client    Client index.
 */
AmmoOnClientInfected(client)
{
    // Detatch managers from the client.
    AmmoManagerDetach(client);
}

/**
 * Client was turned into a human (usually by admin command).
 *
 * @param client    Client index.
 */
AmmoOnClientHuman(client)
{
    // Attach managers to the client.
    AmmoManagerAttach(client);
}

/**
 * Round just ended.
 */
AmmoOnRoundEnd()
{
    // What to do here? Remove manager or just disable? Keep clip info?
    // Players might want to keep their current weapons and their states
    // if they survive.
    
    AmmoManagerDetachAll();
}

/**
 * Client disconnected.
 *
 * @param client    Client index.
 */
AmmoOnClientDisconnect(client)
{
    // Detatch managers from the client.
    AmmoManagerDetach(client);
}
