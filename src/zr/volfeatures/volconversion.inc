/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volconversion.inc
 *  Type:           Module 
 *  Description:    Functions for converting various attributes.
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
 * Gets the feature type from a string.
 *
 * @param feature   String value.
 * @return          Feature type, or VolFeature_Invalid on error.
 */
VolumeFeatureTypes:VolStringToFeature(const String:feature[])
{
    if (StrEqual(feature, "anticamp", false))
    {
        return VolFeature_Anticamp;
    }
    else if (StrEqual(feature, "classedit", false))
    {
        return VolFeature_ClassEdit;
    }
    else
    {
        return VolFeature_Invalid;
    }
}


/**************
 *            *
 *   VOLUME   *
 *            *
 **************/

/**
 * Gets the shape type from a string.
 *
 * @param shape     String value.
 * @return          Shape type, or VolShape_Invalid on error.
 */
VolumeShapes:VolStringToShape(const String:shape[])
{
    if (StrEqual(shape, "cuboid", false))
    {
        return VolShape_Cuboid;
    }
    else if (StrEqual(shape, "sphere", false))
    {
        return VolShape_Sphere;
    }
    else
    {
        return VolShape_Invalid;
    }
}

/**
 * Gets the effect type from a string.
 *
 * @param effect    String value.
 * @return          Effect type, or VolEffect_Invalid on error.
 */
VolumeEffects:VolStringToEffect(const String:effect[])
{
    if (StrEqual(effect, "none", false))
    {
        return VolEffect_None;
    }
    else if (StrEqual(effect, "wireframe", false))
    {
        return VolEffect_Wireframe;
    }
    else if (StrEqual(effect, "smoke", false))
    {
        return VolEffect_Smoke;
    }
    else
    {
        return VolEffect_Invalid;
    }
}

/**
 * Gets the team type from a string.
 *
 * @param team  String value.
 * @return      Team type, or VolTeam_Invalid on error.
 */
VolumeTeamFilters:VolStringToTeam(const String:team[])
{
    if (StrEqual(team, "all", false))
    {
        return VolTeam_All;
    }
    else if (StrEqual(team, "zombies", false))
    {
        return VolTeam_Zombies;
    }
    else if (StrEqual(team, "humans", false))
    {
        return VolTeam_Humans;
    }
    else
    {
        return VolTeam_Invalid;
    }
}

/**
 * Gets the conflict action type from a string.
 *
 * @param conflictAction    String value.
 * @return                  Conflict action type, or VolConflict_Invalid on error.
 */
VolumeConflictActions:VolStringToConflictAction(const String:conflictAction[])
{
    if (StrEqual(conflictAction, "priority", false))
    {
        return VolConflict_Priority;
    }
    else if (StrEqual(conflictAction, "merge", false))
    {
        return VolConflict_Merge;
    }
    else
    {
        return VolConflict_Invalid;
    }
}


/**************
 *            *
 *   SPHERE   *
 *            *
 **************/

/**
 * Gets the fade type from a string.
 *
 * @param fadeType  String value.
 * @return          Fade type, or FadeType_Invalid on error.
 */
FadeTypes:VolStringToFadeType(const String:fadeType[])
{
    if (StrEqual(fadeType, "none", false))
    {
        return FadeType_None;
    }
    else if (StrEqual(fadeType, "linearcenter", false))
    {
        return FadeType_LinearCenter;
    }
    else if (StrEqual(fadeType, "lineardistance", false))
    {
        return FadeType_LinearDistance;
    }
    else
    {
        return FadeType_Invalid;
    }
}


/*****************
 *               *
 *   ANTI-CAMP   *
 *               *
 *****************/

/**
 * Gets the anti camp action type from a string.
 *
 * @param action    String value.
 * @return          Anticamp action, or Anticamp_InvalidAction on error.
 */
VolAnticampAction:VolStringToAnticampAction(const String:action[])
{
    if (StrEqual(action, "none", false))
    {
        return Anticamp_NoAction;
    }
    else if (StrEqual(action, "damage", false))
    {
        return Anticamp_Damage;
    }
    else if (StrEqual(action, "slay", false))
    {
        return Anticamp_Slay;
    }
    else if (StrEqual(action, "drug", false))
    {
        return Anticamp_Drug;
    }
    else if (StrEqual(action, "ignite", false))
    {
        return Anticamp_Ignite;
    }
    else
    {
        return Anticamp_InvalidAction;
    }
}

/**
 * Gets the anti camp warning type from a string.
 *
 * @param warning   String value.
 * @return          Warning type, or Anticamp_InvalidWarning on error.
 */
VolAnticampWarningType:VolStringToAnticampWarning(const String:warning[])
{
if (StrEqual(action, "none", false))
    {
        return Anticamp_NoWarning;
    }
    else if (StrEqual(action, "chat", false))
    {
        return Anticamp_Chat;
    }
    else if (StrEqual(action, "center", false))
    {
        return Anticamp_Center;
    }
    else if (StrEqual(action, "menu", false))
    {
        return Anticamp_Menu;
    }
    else
    {
        return Anticamp_InvalidWarning;
    }
}


/********************
 *                  *
 *   CLASS EDITOR   *
 *                  *
 ********************/

/**
 * Gets the class edit mode type from a string.
 *
 * @param mode  String value.
 * @return      Mode type, or ClassEditMode_Invalid on error.
 */
VolClassEditMode:VolStringToClassEditMode(const String:mode[])
{
    if (StrEqual(mode, "name", false))
    {
        return ClassEditMode_Name;
    }
    else if (StrEqual(mode, "attributes", false))
    {
        return ClassEditMode_Attributes;
    }
    else
    {
        return ClassEditMode_Invalid;
    }
}
