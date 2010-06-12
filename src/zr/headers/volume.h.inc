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
 * @section Default attribute values.
 */
#define ZR_VOL_DEFAULT_ENABLED          false
#define ZR_VOL_DEFAULT_IN_USE           false
#define ZR_VOL_DEFAULT_NAME             ""
#define ZR_VOL_DEFAULT_FEATURE_TYPES    INVALID_HANDLE
#define ZR_VOL_DEFAULT_FEATURE_INDEXES  INVALID_HANDLE
#define ZR_VOL_DEFAULT_SHAPE            VolShape_Cuboid
#define ZR_VOL_DEFAULT_SHAPE_DATA       -1
#define ZR_VOL_DEFAULT_EFFECT           VolEffect_None
#define ZR_VOL_DEFAULT_EFFECT_COLOR     {255, 255, 255}
#define ZR_VOL_DEFAULT_TEAM_FILTER      VolTeam_Humans
#define ZR_VOL_DEFAULT_TRIGGER_DELAY    0.0
#define ZR_VOL_DEFAULT_PRIORITY         0
#define ZR_VOL_DEFAULT_CONFLICT_ACTION  VolConflict_Priority
/**
 * @endsection
 */

/**
 * Total volumes that can be created in a map.
 */
#define ZR_VOLUMES_MAX 32

/**
 * Represents a volume.
 */
enum VolumeAttributes
{
    /* General */
    bool:Vol_Enabled,                   /** Volume state. */
    bool:Vol_InUse,                     /** Marks if the volume is used. */
    String:Vol_Name[VOL_NAME_LEN],      /** Optional. Name of the volume (alphanumeric). Cannot start with a number. */
    VolumeShapes:Vol_Shape,             /** Shape type. */
    Vol_ShapeData,                      /** Index in remote shape array, depending on shape type. */
    
    /* Features */
    Handle:Vol_FeatureTypes,            /** List of feature types linked to the volume. */
    Handle:Vol_FeatureIndexes,          /** List the feature indexes. */
    
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
    VolShape_Cuboid,         /** Rectangular volume */
    VolShape_Sphere          /** Ball */
}

/**
 * Effects that can be applied on a volume. (Currently no effects implemented.)
 */
enum VolumeEffects
{
    VolEffect_Invalid = -1,
    VolEffect_None,
    VolEffect_Wireframe,
    VolEffect_Smoke
}

/**
 * Available team filter settings.
 */
enum VolumeTeamFilters
{
    VolTeam_Invalid = -1,
    VolTeam_All,
    VolTeam_Humans,
    VolTeam_Zombies
}

/**
 * Conflict actions. What to do with overlapping volumes of same type.
 */
enum VolumeConflictActions
{
    VolConflict_Invalid = -1,
    VolConflict_Priority,       /** Use the volume with highest priority, ignore others. */
    VolConflict_Merge           /** Try to merge volume settings/attributes, or use priority if there's a merge conflict. */
}
