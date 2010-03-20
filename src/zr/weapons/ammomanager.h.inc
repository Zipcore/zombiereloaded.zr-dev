/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ammomanager.inc
 *  Description:   Ammo profile manager headers.
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

#if defined _ammomanager_included
 #endinput
#endif
#define _ammomanager_included

/**
 * @section Number of managed slots. So far it's for primary and secondary
 *          weapons. The slot ID's should match WeaponsSlot.
 */
#define AMMO_MANAGED_SLOTS 2
/**
 * @endsection
 */

/**
 * @section Maximum number of ammo managers. There must be one manager per
 *          client and per slot that is managed.
 */
#define AMMO_MANAGER_MAX    (MAXPLAYERS + 1) * AMMO_MANAGED_SLOTS
/**
 * @endsection
 */

enum AmmoManager
{
    bool:AmmoManager_InUse,         /** Specifies whether the profile is used or not. */
    WeaponsSlot:AmmoManager_Slot,   /** What weapon slot that's managed. TODO: Change tag to use WepLib_Slots when it's imported. */
    AmmoManager_Clip,               /** Current weapon clip value. */
    AmmoManager_Reserve,            /** Current weapon reserve value. */
    AmmoManager_ProfileIndex,       /** Index to a ammo profile. */
    Handle:AmmoManager_Timer        /** Timer used for refilling, if enabled. */
}
