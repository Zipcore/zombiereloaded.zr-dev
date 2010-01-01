/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ammoprofiles.h.inc
 *  Description:   Ammo profile headers.
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

#if defined _ammoprofiles_included
 #endinput
#endif
#define _ammoprofiles_included

/**
 * Profile storage capacity.
 */
#define AMMO_PROFILES_MAX 32

/**
 * @section Attribute limits.
 */
#define AMMO_NAME_MIN               1
#define AMMO_CLIP_SIZE_MIN          -1
#define AMMO_CLIP_SIZE_MAX          5000
#define AMMO_CLIP_RESERVE_MIN       -1
#define AMMO_CLIP_RESERVE_MAX       16000
#define AMMO_CLIP_INTERVAL_MIN      0.0
#define AMMO_CLIP_INTERVAL_MAX      600.0
#define AMMO_CLIP_AMOUNT_MIN        0
#define AMMO_CLIP_AMOUNT_MAX        AMMO_CLIP_SIZE_MAX
#define AMMO_CLIP_MAX_MIN           0
#define AMMO_CLIP_MAX_MAX           AMMO_CLIP_RESERVE_MAX
/**
 * @endsection
 */

/**
 * @section Default profile settings.
 */
#define AMMO_DEFAULT_NAME           "default"
#define AMMO_DEFAULT_MODE           "disabled"
#define AMMO_DEFAULT_CLIP_SIZE      AMMO_CLIP_SIZE_MIN
#define AMMO_DEFAULT_CLIP_RESERVE   AMMO_CLIP_RESERVE_MIN
#define AMMO_DEFAULT_INTERVAL       AMMO_CLIP_INTERVAL_MIN
#define AMMO_DEFAULT_AMOUNT         AMMO_CLIP_AMOUNT_MIN
#define AMMO_DEFAULT_MAX            AMMO_MAX_MAX
/**
 * @endsection
 */

/**
 * Ammo modes.
 */
enum AmmoMode
{
    AmmoMode_Invalid = -1,  /** Invalid mode. */
    AmmoMode_Disabled,      /** Do nothing applying clip settings. */
    AmmoMode_Unlimited,     /** Remove need for reloading weapon. Simulates unlimited clip size. */
    AmmoMode_FillClip,      /** Give "amount" bullets to clip every "interval" second as long it's below "max". */
    AmmoMode_FillReserve    /** Same as AmmoMode_FillClip, but on clip reserve. */
}

/**
 * Ammo profile attributes.
 */
enum AmmoProfile
{
    String:AmmoProfile_Name[32],
    AmmoMode:AmmoProfile_Mode,
    AmmoProfile_ClipSize,
    AmmoProfile_ClipReserve,
    Float:AmmoProfile_Interval,
    AmmoProfile_Amount,
    AmmoProfile_Max
}
