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
stock AmmoOnWeaponReloaded(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // TODO: How to get a event hook when the weapon is just _done_ reloading?
    
    // Simulate reload event on ammo manager.
    AmmoManagerReload(ammoManager);
    
    // Apply the new values to the active weapon.
    AmmoManagerApply(client, ammoManager);
}

/**
 * Event callback. Client fired his weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
AmmoOnWeaponFire(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // Get the ammo manager for this slot.
    new ammoManager = AmmoManagerIndex[client][weaponSlot];
    
    // Simulate fire even on ammo manager if valid index.
    if (AmmoIsValidManager(ammoManager))
    {
        AmmoManagerFire(ammoManager);
    }
}

/**
 * Event callback. Client attempted to fire a weapon with no bullets.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
AmmoOnWeaponFireOnEmpty(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // Get the ammo manager for this slot.
    new ammoManager = AmmoManagerIndex[client][weaponSlot];
    
    // Simulate fire on empty event on ammo manager.
    AmmoManagerFireOnEmpty(ammoManager);
    
    // Apply the new values to the active weapon.
    AmmoManagerApply(client, ammoManager);
}

/**
 * Event callback. Client switched weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot switched to.
 */
stock AmmoOnWeaponSwitch(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // TODO: Hook event using SDK Hooks.
    //       Do the hook in base adapter so other modules can make use of the hook?
    
    // Get the ammo manager for this slot.
    new ammoManager = AmmoManagerIndex[client][weaponSlot];
    
    // Apply the new values to the active weapon.
    AmmoManagerApply(client, ammoManager);
}

/**
 * Event callback. Client picked up a weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
stock AmmoOnWeaponPickup(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // Since a ammo manager may not exist for this slot, the AmmoGetManager
    // function is used to create and assign a new one.
    AmmoGetManager(client, slot);
    
    // Get ammo profile.
    new ammoProfile = AmmoProfileIndex[client][slot];
    
    // Attach the ammo manager for this slot.
    AmmoManagerAttachSlot(client, slot, ammoProfile)
    
    // TODO: Apply weapon's clip value in ammo manager, or fill clip?
}

/**
 * Event callback. Client dropped a weapon.
 *
 * @param client        Client index.
 * @param weaponSlot    Weapon slot.
 */
stock AmmoOnWeaponDrop(client, WepLib_Slots:weaponSlot)
{
    // Check if the slot is supported by the manager.
    if (!AmmoIsSlotManaged(weaponSlot))
    {
        // Not supported, ignore event.
        return;
    }
    
    // Detach manager.
    AmmoManagerDetachSlot(client, weaponSlot);
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
        // Assign ammo managers.
        AmmoAssignManagers(client);
        
        // Attach ammo managers to the client.
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
