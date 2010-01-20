/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weaponprofiles.h.inc
 *  Description:   Weapon profile headers.
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

#if defined _weaponprofileheaders_included
 #endinput
#endif
#define _weaponprofileheaders_included

/**
 * Profile storage capacity.  By default this matches the max number of classes.
 */
#define WEAPON_PROFILES_MAX 48

/**
 * Weapon profile attributes.
 */
enum WeaponProfile
{
    String:WeaponProfile_Name[32],                  /** Profile name. */
    WeaponProfile_PriAmmoProfile,                   /** Ammo profile for primary weapon. */
    WeaponProfile_SecAmmoProfile,                   /** Ammo profile for secondary weapon. */
    WeaponProfile_RestrictProfile,                  /** Weapon restriction profile. */
    WeaponProfile_PriAmmoManager[MAXPLAYERS + 1],   /** Index to ammo managers for the primary weapon, per player. */
    WeaponProfile_SecAmmoManager[MAXPLAYERS + 1],   /** Index to ammo managers for the secondary weapon, per player. */
    
    // Note: We might not need a restrict manager because it's static data.
    //WeaponProfile_RestrictManager[MAXPLAYERS + 1]   /** Index to a weapon restriction manager. */
}
