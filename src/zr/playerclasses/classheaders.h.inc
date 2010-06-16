/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          playerclasses.inc
 *  Type:          Core 
 *  Description:   Provides functions for managing classes.
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
 * Total number of classes that can be stored in each cache. A total of 48
 * classes should be enough. Too many classes will confuse players.
 */
#define ZR_CLASS_MAX 48

/**
 * @section Class cache types. Specifies what data array to use.
 */
#define ZR_CLASS_CACHE_ORIGINAL 0   /** Points to ClassData array. A cache that is never changed after loading. */
#define ZR_CLASS_CACHE_MODIFIED 1   /** Points to ClassDataCache array. Modified by admins or overrides. */
#define ZR_CLASS_CACHE_PLAYER   2   /** Points to ClassPlayerCache array. Current player attributes. */
/**
 * @endsection
 */

/**
 * Number of available class teams.
 */
#define ZR_CLASS_TEAMCOUNT      3

/**
 * @section Available class teams. The admin team is optional and not required
 * in class configs.
 */
#define ZR_CLASS_TEAM_ZOMBIES   0
#define ZR_CLASS_TEAM_HUMANS    1
#define ZR_CLASS_TEAM_ADMINS    2   /** Note: Will set you in a special mode where you don't really participates in the game, but just walk around. */
/**
 * @endsection
 */

/**
 * @section Damage immunity modes. The mode effects will vary depending on the
 * class team.
 */
#define ZR_CLASS_IMMUNITY_DISABLED  0   /** No immunity. */
#define ZR_CLASS_IMMUNITY_CONSTANT  1   /** Always immune. Should only be used in special cases like on admin classes. */
#define ZR_CLASS_IMMUNITY_TIMED     2   /** Immune to damage for n seconds. The time is specified in a class as immunity amount. */
/**
 * @endsection
 */

/**
 * @section Flags for special classes.
 */
#define ZR_CLASS_FLAG_ADMIN_ONLY        (1<<0)  /** Class is usable by admins only. */
#define ZR_CLASS_FLAG_MOTHER_ZOMBIE     (1<<1)  /** Class is usable by mother zombies only. */

/** A combination of special class flags. Used to exclude special classes. */
#define ZR_CLASS_SPECIALFLAGS           ZR_CLASS_FLAG_ADMIN_ONLY + ZR_CLASS_FLAG_MOTHER_ZOMBIE
/**
 * @endsection
 */

/**
 * @section Overall default class settings. Since this is a zombie plugin the
 * default values represent a zombie.
 */
#define ZR_CLASS_DEFAULT_ENABLED                "yes"
#define ZR_CLASS_DEFAULT_TEAM                   ZR_CLASS_TEAM_ZOMBIES
#define ZR_CLASS_DEFAULT_TEAM_DEFAULT           "yes"
#define ZR_CLASS_DEFAULT_FLAGS                  0
#define ZR_CLASS_DEFAULT_GROUP                  ""
#define ZR_CLASS_DEFAULT_NAME                   "classic"
#define ZR_CLASS_DEFAULT_DESCRIPTION            "Need brains!!! Arrrrggghh!"
#define ZR_CLASS_DEFAULT_MODEL_PATH             "models/player/zh/zh_zombie003.mdl"
#define ZR_CLASS_DEFAULT_ALPHA_INITIAL          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGED          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGE           0
#define ZR_CLASS_DEFAULT_OVERLAY_PATH           "overlays/zr/zvision"
#define ZR_CLASS_DEFAULT_NVGS                   "no"
#define ZR_CLASS_DEFAULT_FOV                    90
#define ZR_CLASS_DEFAULT_HAS_NAPALM             "no"
#define ZR_CLASS_DEFAULT_NAPALM_TIME            10.0
#define ZR_CLASS_DEFAULT_IMMUNITY_MODE          ZR_CLASS_IMMUNITY_DISABLED
#define ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT        0.0
#define ZR_CLASS_DEFAULT_NO_FALL_DAMAGE         "yes"
#define ZR_CLASS_DEFAULT_WEAPON_PROFILE         "default"
#define ZR_CLASS_DEFAULT_HEALTH                 6000
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL  0.0
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT    2
#define ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN     800
#define ZR_CLASS_DEFAULT_KILL_BONUS             2
#define ZR_CLASS_DEFAULT_SPEED                  75.0
#define ZR_CLASS_DEFAULT_KNOCKBACK              2.0
#define ZR_CLASS_DEFAULT_JUMP_HEIGHT            10.0
#define ZR_CLASS_DEFAULT_JUMP_DISTANCE          0.2
/**
 * @endsection
 */

/**
 * @section Attribute limit values. Used when validating.
 */
#define ZR_CLASS_TEAM_MIN                   0
#define ZR_CLASS_TEAM_MAX                   2
#define ZR_CLASS_FLAGS_MIN                  0
#define ZR_CLASS_FLAGS_MAX                  3
#define ZR_CLASS_NAME_MIN                   1
#define ZR_CLASS_DESCRIPTION_MIN            1
/** Model path is checked for existence. */
#define ZR_CLASS_ALPHA_INITIAL_MIN          0
#define ZR_CLASS_ALPHA_INITIAL_MAX          255
#define ZR_CLASS_ALPHA_DAMAGED_MIN          0
#define ZR_CLASS_ALPHA_DAMAGED_MAX          255
#define ZR_CLASS_ALPHA_DAMAGE_MIN           0
#define ZR_CLASS_ALPHA_DAMAGE_MAX           20000
/** Overlay path is optional, and file is checked for existence if specified. */
#define ZR_CLASS_FOV_MIN                    15
#define ZR_CLASS_FOV_MAX                    165
#define ZR_CLASS_NAPALM_TIME_MIN            0.0
#define ZR_CLASS_NAPALM_TIME_MAX            600.0
/** Weapon profile is checked for existence. */
#define ZR_CLASS_HEALTH_MIN                 1
#define ZR_CLASS_HEALTH_MAX                 20000
#define ZR_CLASS_REGEN_INTERVAL_MIN         0.0
#define ZR_CLASS_REGEN_INTERVAL_MAX         900.0
#define ZR_CLASS_REGEN_AMOUNT_MIN           0
#define ZR_CLASS_REGEN_AMOUNT_MAX           10000
#define ZR_CLASS_HEALTH_INFECT_GAIN_MIN     0
#define ZR_CLASS_HEALTH_INFECT_GAIN_MAX     20000
#define ZR_CLASS_KILL_BONUS_MIN             0
#define ZR_CLASS_KILL_BONUS_MAX             16
#define ZR_CLASS_SPEED_LMV_MIN              10.0
#define ZR_CLASS_SPEED_LMV_MAX              2000.0
#define ZR_CLASS_SPEED_PROP_MIN             -150.0
#define ZR_CLASS_SPEED_PROP_MAX             300.0   /** Also limited by sv_maxspeed, cl_forwardspeed, cl_backspeed and cl_sidespeed. */
#define ZR_CLASS_KNOCKBACK_MIN              -30.0
#define ZR_CLASS_KNOCKBACK_MAX              30.0
#define ZR_CLASS_KNOCKBACK_IGNORE           -31.0   /** Used by class editor volumetric feature. */
#define ZR_CLASS_JUMP_HEIGHT_MIN            0.0
#define ZR_CLASS_JUMP_HEIGHT_MAX            5.0
#define ZR_CLASS_JUMP_DISTANCE_MIN          0.0
#define ZR_CLASS_JUMP_DISTANCE_MAX          5.0
/**
 * @endsection
 */

/**
 * @section Class attribute flags.
 */
#define ZR_CLASS_ENABLED                (1<<0)
#define ZR_CLASS_TEAM                   (1<<1)
#define ZR_CLASS_TEAM_DEFAULT           (1<<2)
#define ZR_CLASS_FLAGS                  (1<<3)
#define ZR_CLASS_GROUP                  (1<<4)
#define ZR_CLASS_NAME                   (1<<5)
#define ZR_CLASS_DESCRIPTION            (1<<6)
#define ZR_CLASS_MODEL_PATH             (1<<7)
#define ZR_CLASS_ALPHA_INITIAL          (1<<8)
#define ZR_CLASS_ALPHA_DAMAGED          (1<<9)
#define ZR_CLASS_ALPHA_DAMAGE           (1<<10)
#define ZR_CLASS_OVERLAY_PATH           (1<<11)
#define ZR_CLASS_NVGS                   (1<<12)
#define ZR_CLASS_FOV                    (1<<13)
#define ZR_CLASS_HAS_NAPALM             (1<<14)
#define ZR_CLASS_NAPALM_TIME            (1<<15)
#define ZR_CLASS_IMMUNITY_MODE          (1<<16)
#define ZR_CLASS_IMMUNITY_AMOUNT        (1<<17)
#define ZR_CLASS_NO_FALL_DAMAGE         (1<<18)
#define ZR_CLASS_WEAPON_PROFILE         (1<<19)
#define ZR_CLASS_HEALTH                 (1<<20)
#define ZR_CLASS_HEALTH_REGEN_INTERVAL  (1<<21)
#define ZR_CLASS_HEALTH_REGEN_AMOUNT    (1<<22)
#define ZR_CLASS_HEALTH_INFECT_GAIN     (1<<23)
#define ZR_CLASS_KILL_BONUS             (1<<24)
#define ZR_CLASS_SPEED                  (1<<25)
#define ZR_CLASS_KNOCKBACK              (1<<26)
#define ZR_CLASS_JUMP_HEIGHT            (1<<27)
#define ZR_CLASS_JUMP_DISTANCE          (1<<28)
/**
 * @endsection
 */

/**
 * Generic player attributes.
 *
 * Stuff that must be updated when new attributes are added:
 * ZR_CLASS_DEFAULT_... define
 * ZR_CLASS_..._MAX/MIN defines
 * ZR_CLASS_... define (place in same order as listed in ClassAttributes, bump bit numbers + update numbers in docs)
 * ClassLoad
 * ClassReloadDataCache
 * ClassReloadPlayerCache
 * ClassDumpData
 * attributes.inc - Add new Get-function
 * ClassAttributeNameToFlag
 * ClassGetAttributeType
 * ClassValidateAttributes
 * ClassModify* in classcommands.inc
 * Update docs with detailed attribute description
 */
enum ClassAttributes
{
    /* General */
    bool:Class_Enabled,
    Class_Team,
    bool:Class_TeamDefault,
    Class_Flags,
    String:Class_Group[64],
    
    String:Class_Name[64],
    String:Class_Description[256],
    
    /* Model */
    String:Class_ModelPath[PLATFORM_MAX_PATH],
    Class_AlphaInitial,
    Class_AlphaDamaged,
    Class_AlphaDamage,
    
    /* Hud */
    String:Class_OverlayPath[PLATFORM_MAX_PATH],
    bool:Class_Nvgs,
    Class_Fov,
    
    /* Effects */
    bool:Class_HasNapalm,
    Float:Class_NapalmTime,
    
    /* Player behaviour */
    Class_ImmunityMode,
    Float:Class_ImmunityAmount,
    bool:Class_NoFallDamage,
    Class_WeaponProfile,
    
    Class_Health,
    Float:Class_HealthRegenInterval,
    Class_HealthRegenAmount,
    Class_HealthInfectGain,
    Class_KillBonus,
    
    Float:Class_Speed,          // This is a offset value.
    Float:Class_KnockBack,
    Float:Class_JumpHeight,
    Float:Class_JumpDistance
}

/**
 * Structure of class attributes that are allowed to be modified directly,
 * while the player is alive.
 *
 * Note: This structure is also used as a mask to tell if a individual
 *       attribute should be ignored or not. Negative valueas usually indicate
 *       ignored attributes. Booleans are now ints so they can be negative.
 *       Strings have reserved keywords like "nochange" that indicate a ignored
 *       attribute.
 */
enum ClassEditableAttributes
{
    /* Model */
    ClassEdit_AlphaInitial = 0,
    ClassEdit_AlphaDamaged,
    ClassEdit_AlphaDamage,
    
    /* Hud */
    String:ClassEdit_OverlayPath[PLATFORM_MAX_PATH],
    ClassEdit_Nvgs,
    ClassEdit_Fov,
    
    /* Effects */
    ClassEdit_HasNapalm,
    Float:ClassEdit_NapalmTime,
    
    /* Player behaviour */
    ClassEdit_ImmunityMode,
    Float:ClassEdit_ImmunityAmount,
    
    ClassEdit_NoFallDamage,
    ClassEdit_WeaponProfile,
    Float:ClassEdit_RegenInterval,
    ClassEdit_RegenAmount,
    ClassEdit_InfectGain,
    ClassEdit_KillBonus,
    
    Float:ClassEdit_Speed,
    Float:ClassEdit_KnockBack,
    Float:ClassEdit_JumpHeight,
    Float:ClassEdit_JumpDistance
}

/**
 * Class attributes that support multipliers.
 */
enum ClassMultipliers
{
    ClassM_Invalid = 0,
    Float:ClassM_NapalmTime,
    Float:ClassM_Health,
    Float:ClassM_HealthRegenInterval,
    Float:ClassM_HealthRegenAmount,
    Float:ClassM_HealthInfectGain,
    Float:ClassM_Speed,
    Float:ClassM_Knockback,
    Float:ClassM_JumpHeight,
    Float:ClassM_JumpDistance
}

/**
 * Available class teams, used to specify targets.
 */
enum ClassTeams
{
    ClassTeam_Zombies = 0,
    ClassTeam_Humans,
    ClassTeam_Admins,
    ClassTeam_All
}

/**
 * Data types used in class attributes.
 */
enum ClassDataTypes
{
    ClassDataType_InvalidType,  /** Invalid type */
    ClassDataType_Boolean,      /** Boolean value */
    ClassDataType_Integer,      /** Integer value */
    ClassDataType_Float,        /** Floating point value */
    ClassDataType_String        /** String value */
}

/**
 * Structure for class filter settings passed to various functions.
 */
enum ClassFilter
{
    bool:ClassFilter_IgnoreEnabled,     /** Ignore whether the class is disabled or not. */
    ClassFilter_RequireFlags,           /** Flags the classes must have set. */
    ClassFilter_DenyFlags,              /** Flags the classes cannot have set. */
    ClassFilter_Client                  /** The client to check for class group permissions. Use 0 to ignore group filter and negative to exclude classes with groups set. */
}

/**
 * Specifies how class selections should be applied on a client when connecting.
 */
enum ClassLoadMode
{
    ClassLoadMode_Normal,   /** Default loading mode */
    ClassLoadMode_Preload,  /** Preload a client's class selections with config defaults. Used before they are fully connected. Cookies and admin-only classes will be ignored. */
    ClassLoadMode_Postload  /** Lode a client's class selections from cookies if enabled, but mark them as next spawn class if the client already have spawned. */
}

/**
 * Speed methods for applying player speed.
 */
enum ClassSpeedMethods
{
    ClassSpeed_Invalid = -1,
    ClassSpeed_LMV,             /** Modifies lagged movement value. */
    ClassSpeed_Prop,            /** Modifies players' max speed property(m_flMaxspeed). */
    ClassSpeed_CommandHook      /** Modifies speed value sent in commands by the client. */
}
