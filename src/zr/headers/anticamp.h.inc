/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           anticamp.h.inc
 *  Type:           Header
 *  Description:    Anti-camp headers.
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

#if defined _volfeatures_anticamp_included
 #endinput
#endif
#define _volfeatures_anticamp_included

/**
 * @section Default attribute values.
 */
#define ZR_ANTICAMP_DEF_IN_USE          false
#define ZR_ANTICAMP_DEF_ENABLED         false
#define ZR_ANTICAMP_DEF_NAME            ""
#define ZR_ANTICAMP_DEF_VOLUME_INDEX    -1
#define ZR_ANTICAMP_DEF_INTERVAL        1.0
#define ZR_ANTICAMP_DEF_ACTION          Anticamp_Damage
#define ZR_ANTICAMP_DEF_AMOUNT          5.0
#define ZR_ANTICAMP_DEF_WARNING         Anticamp_Chat
#define ZR_ANTICAMP_DEF_MESSAGE         ""
/**
 * @endsection
 */

/**
 * Maximum number of anti camp volumes.
 */
#define ZR_ANTICAMP_MAX ZR_VOLUMES_MAX

/**
 * Number of bytes reserved for message strings.
 */
#define ZR_ANTICAMP_MESSAGE_LEN 256

/**
 * Actions to do with players in anti-camp volumes.
 */
enum VolAnticampAction
{
    Anticamp_InvalidAction = -1,
    Anticamp_NoAction,      /** Do nothing but give a warning. */
    Anticamp_Damage,        /** Give damage to player. */
    Anticamp_Slay,          /** Slay player. */
    Anticamp_Drug,          /** Drug player. */
    Anticamp_Ignite         /** Ignite player. */
}

/**
 * Warning types.
 */
enum VolAnticampWarningType
{
    Anticamp_InvalidWarning = -1,
    Anticamp_NoWarning,     /** No warning messages. */
    Anticamp_Chat,          /** Print warning in chat area. */
    Anticamp_Center,        /** Print centered warning message. */
    Anticamp_Menu           /** Print a menu-like warning message with close option. */
}

/**
 * Data structure for a anti-camp volume.
 */
enum VolTypeAnticamp
{
    bool:Anticamp_InUse,                        /** Specifies if the data index is used or not. */
    bool:Anticamp_Enabled,                      /** Specifies if the feature is enabled or not. */
    String:Anticamp_Name[VOL_NAME_LEN],         /** Optional. Name of the anti-camp (alphanumeric). Cannot start with a number. */
    Anticamp_VolumeIndex,                       /** Which volume this anticamp is linked to. */
    Float:Anticamp_Interval,                    /** How often to trigger an action. */
    Handle:Anticamp_Timer,                      /** Action timer handle. */
    
    VolAnticampAction:Anticamp_Action,          /** What to do with players in anti-camp volumes */
    Float:Anticamp_Amount,                      /** Amount depending on action type. Usually time in seconds or damage amount. */
    
    VolAnticampWarningType:Anticamp_Warning,    /** How to warn the player. */
    String:Anticamp_Message[ZR_ANTICAMP_MESSAGE_LEN]    /** Override warning message. Max 256 characters. */    
}


/**
 * Anti-camp data.
 */
new AnticampData[ZR_VOLUMES_MAX][VolTypeAnticamp];

/**
 * Number of anti-camp features in use.
 */
new AnticampCount;
