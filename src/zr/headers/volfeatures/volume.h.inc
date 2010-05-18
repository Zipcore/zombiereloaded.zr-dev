/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volume.h.inc
 *  Type:           Header
 *  Description:    Volume headers.
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

#if defined _volfeatures_volume_included
 #endinput
#endif
#define _volfeatures_volume_included

/**
 * Total volumes that can be created in a map.
 */
#define ZR_VOLUMES_MAX 32

/**
 * Represent a volume.
 */
enum VolumeAttributes
{
    /* General */
    bool:Vol_Enabled,                   /** Volume state. */
    bool:Vol_InUse,                     /** Marks if the volume is used. */
    VolumeShapes:Vol_Shape,             /** Shape type. */
    Vol_ShapeData,                      /** Index in remote shape array, depending on shape type. */

    /* Style */
    VolumeEffects:Vol_Effect,           /** Visual effect to apply on the volume. */
    Vol_EffectColor[3],                 /** Render color of the effect. RGB colors. */
    
    /* Behaviour */
    VolumeTeamFilters:Vol_TeamFilter,   /** Team filtering. Trigger by certain teams, or all. */
    Float:Vol_TriggerDelay,             /** Trigger delay. How many seconds players have to stay to trigger volume events. */
    Vol_Priority,                       /** Volume priority. */
    VolumeConflictActions:Vol_ConflictAction    /** What to do if features of same type overlap. */
}

/**
 * Available volume shapes.
 */
enum VolumeShapes
{
    VolShape_Invalid = -1,   /** Invalid shape (used by parser) */
    VolShape_Cubiod,         /** Rectangular volume */
    VolShape_Sphere          /** Ball */
}

/**
 * Effects that can be applied on a volume. (Currently no effects.)
 */
enum VolumeEffects
{
    VolEffect_None = 0,
    VolEffect_Wireframe,
    VolEffect_Smoke
}

/**
 * Available team filter settings.
 */
enum VolumeTeamFilters
{
    VolTeam_All = 0,
    VolTeam_Humans,
    VolTeam_Zombies
}

/**
 * Conflict actions. What to do with overlapping volumes of same type.
 */
enum VolumeConflictActions
{
    VolPriority = 0,    /** Use the volume with highest priority, ignore others. */
    VolMerge            /** Try to merge volume settings/attributes, or use priority if there's a merge conflict. */
}

/**
 * Volume data.
 */
new Volumes[ZR_VOLUMES_MAX][VolumeAttributes];

/**
 * Number of volumes in use.
 */
new VolumeCount;

/**
 * List of player locations. Updated by a timer.
 */
new Float:VolPlayerLoc[MAXPLAYERS + 1][3];

/**
 * Cache that specifies if a player is in a volume or not.
 */
new bool:VolPlayerInVolume[MAXPLAYERS + 1][ZR_VOLUMES_MAX];

/**
 * Counter for trigger delay.
 */
new Float:VolPlayerCountDown[MAXPLAYERS + 1][ZR_VOLUMES_MAX];

/**
 * The handle for a timer that updates player locations. This is the main timer
 * and any feature events can't be updated faster than this interval.
 *
 * Note: Some features may have its own timer for actions on players.
 */
new Handle:hVolUpdateTimer = INVALID_HANDLE;

/**
 * The handle for a timer that do count down on trigger delays.
 */
new Handle:hVolTriggerTimer = INVALID_HANDLE;

/**
 * Cached interval value for trigger timer.
 */
new Float:VolTriggerInterval;
