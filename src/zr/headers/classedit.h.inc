/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           classedit.h.inc
 *  Type:           Header
 *  Description:    Class editor headers.
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

#if defined _volfeatures_classedit_included
 #endinput
#endif
#define _volfeatures_classedit_included

#define VOL_CLASSNAME_SIZE  64

/**
 * Maximum number of class editors.
 */
#define ZR_CLASSEDIT_MAX ZR_VOLUMES_MAX

/**
 * @section Default attribute values.
 */
#define VOL_CLASSEDIT_DEF_IN_USE        false
#define VOL_CLASSEDIT_DEF_NAME          ""
#define VOL_CLASSEDIT_DEF_VOLUME_INDEX  -1
#define VOL_CLASSEDIT_DEF_MODE          ClassEditMode_Attributes
#define VOL_CLASSEDIT_DEF_CLASS_INDEX   -1
/**
 * @endsection
 */

/**
 * Empty attribute structure with all settings set to be ignored.
 */
new VolEmptyAttributes[ClassEditableAttributes] = {
    -1,                             /** AlphaInitial */
    -1,                             /** AlphaDamaged */
    -1,                             /** AlphaDamage */
    
    "nochange",                     /** OverlayPath */
    -1,                             /** Nvgs */
    -1,                             /** Fov */
    
    -1,                             /** HasNapalm */
    -1.0,                           /** NapalmTime */
    -1,                             /** ImmunityMode */
    -1.0,                           /** ImmunityAmount */
    -1,                             /** NoFallDamage */
    -1,                             /** WeaponProfile */
    
    -1.0,                           /** RegenInterval */
    -1,                             /** RegenAmount */
    -1,                             /** InfectGain */
    -1,                             /** KillBonus */
    
    -1.0,                           /** Speed */
    ZR_CLASS_KNOCKBACK_IGNORE,      /** KnockBack */
    -1.0,                           /** JumpHeight */
    -1.0,                           /** JumpDistance */
};

/**
 * List of class edit modes.
 */
enum VolClassEditMode
{
    ClassEditMode_Invalid = -1,
    ClassEditMode_Name,         /** Change whole player class. */
    ClassEditMode_Attributes    /** Override class attributes. */
}

/**
 * Data structure for class editor.
 */
enum VolTypeClassEdit
{
    bool:VolClassEdit_InUse,
    String:VolClassEdit_Name[VOL_NAME_LEN],
    VolClassEdit_VolumeIndex,
    
    VolClassEditMode:VolClassEdit_Mode,
    VolClassEdit_ClassIndex
}

/**
 * Class editor data.
 */
new VolClassEditData[ZR_CLASSEDIT_MAX][VolTypeClassEdit];

/**
 * Data storage for editable class attributes.
 */
new VolClassEditClassAttributes[ZR_CLASSEDIT_MAX][ClassEditableAttributes];

/**
 * Number of class editors in use.
 */
new ClassEditCount;
