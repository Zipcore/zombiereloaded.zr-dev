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

/**
 * Converts a feature type to a string.
 *
 * @param feature   Feature type to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
stock VolFeatureToString(VolumeFeatureTypes:feature, String:buffer[], maxlen)
{
    switch (shape)
    {
        case VolFeature_Anticamp: return Format(buffer, maxlen, "%t", "Vol Anticamp");
        case VolFeature_ClassEdit: return Format(buffer, maxlen, "%t", "Vol Classedit");
    }
    
    return 0;
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
 * Converts a shape type to a string.
 *
 * @param shape     Shape type to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolShapeToString(VolumeShapes:shape, String:buffer[], maxlen)
{
    switch (shape)
    {
        case VolShape_Cuboid: return Format(buffer, maxlen, "%t", "Vol Cuboid");
        case VolShape_Sphere: return Format(buffer, maxlen, "%t", "Vol Sphere");
    }
    
    return 0;
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
 * Converts a effect type to a string.
 *
 * @param effect    Effect type to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolEffectToString(VolumeEffects:effect, String:buffer[], maxlen)
{
    switch (effect)
    {
        case VolEffect_None: return Format(buffer, maxlen, "%t", "Vol Effect None");
        case VolEffect_Wireframe: return Format(buffer, maxlen, "%t", "Vol Effect Wireframe");
        case VolEffect_Smoke: return Format(buffer, maxlen, "%t", "Vol Effect Smoke");
    }
    
    return 0;
}

/**
 * Gets the effect color from a string (format: r,g,b).
 *
 * @param effect    String value.
 * @param colors    Output. Color array with 3 elements.
 * @return          True on success, false otherwise.
 */
bool:VolStringToEffectColor(const String:color[], colors[3])
{
    new String:buffer[3][3];
    ExplodeString(color, ",", buffer, 3, 3);
    
    new r = StringToInt(buffer[0]);
    new g = StringToInt(buffer[1]);
    new b = StringToInt(buffer[2]);
    
    // Validate
    if (r < 0 || r > 255 ||
        g < 0 || g > 255 ||
        b < 0 || g > 255)
    {
        return false;
    }
    
    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
    
    return true;
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
 * Converts a team type to a string.
 *
 * @param team      Team type to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolTeamToString(VolumeTeamFilters:team, String:buffer[], maxlen)
{
    switch (team)
    {
        case VolTeam_All: return Format(buffer, maxlen, "%t", "All");
        case VolTeam_Zombies: return Format(buffer, maxlen, "%t", "Classes Menu Zombies");
        case VolTeam_Humans: return Format(buffer, maxlen, "%t", "Classes Menu Humans");
    }
    
    return 0;
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

/**
 * Converts a conflict action type to a string.
 *
 * @param conflictAction    Conflict action type to convert.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of destination buffer.
 * @return                  Number of cells written.
 */
VolConflictToString(VolumeConflictActions:conflictAction, String:buffer[], maxlen)
{
    switch (conflictAction)
    {
        case VolConflict_Priority: return Format(buffer, maxlen, "%t", "Vol Conflict Priority");
        case VolConflict_Merge: return Format(buffer, maxlen, "%t", "Vol Conflict Merge");
    }
    
    return 0;
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

/**
 * Converts a fade type to a string.
 *
 * @param fade      Fade type to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of cells written.
 */
VolFadeToString(FadeTypes:fade, String:buffer[], maxlen)
{
    switch (fade)
    {
        case FadeType_None: return Format(buffer, maxlen, "%t", "Vol Fade None");
        case FadeType_LinearCenter: return Format(buffer, maxlen, "%t", "Vol Fade LinCenter");
        case FadeType_LinearDistance: return Format(buffer, maxlen, "%t", "Vol Fade LinDist");
    }
    
    return 0;
}


/*****************
 *               *
 *   ANTI-CAMP   *
 *               *
 *****************/

/**
 * Converts a action type to a string.
 *
 * @param actionType    Action type to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
VolAnticampActionToString(VolAnticampAction:actionType, String:buffer[], maxlen)
{
    switch (actionType)
    {
        case Anticamp_NoAction:
        {
            return Format(buffer, maxlen, "%t", "Vol No action");
        }
        case Anticamp_Damage:
        {
            return Format(buffer, maxlen, "%t", "Vol Damage");
        }
        case Anticamp_Slay:
        {
            return Format(buffer, maxlen, "%t", "Vol Slay");
        }
        case Anticamp_Drug:
        {
            return Format(buffer, maxlen, "%t", "Vol Drug");
        }
        case Anticamp_Ignite:
        {
            return Format(buffer, maxlen, "%t", "Vol Ignite");
        }
    }
    
    return 0;
}

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
 * Converts a warning type to a string.
 *
 * @param warningType   Warning type to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
VolAnticampWarningToString(VolAnticampWarningType:warningType, String:buffer[], maxlen)
{
    switch (warningType)
    {
        case Anticamp_NoWarning:
        {
            return Format(buffer, maxlen, "%t", "Vol No Warning");
        }
        case Anticamp_Chat:
        {
            return Format(buffer, maxlen, "%t", "Vol Chat");
        }
        case Anticamp_Center:
        {
            return Format(buffer, maxlen, "%t", "Vol Center");
        }
        case Anticamp_Menu:
        {
            return Format(buffer, maxlen, "%t", "Vol Panel");
        }
    }
    
    return 0;
}

/**
 * Gets the anti camp warning type from a string.
 *
 * @param warning   String value.
 * @return          Warning type, or Anticamp_InvalidWarning on error.
 */
VolAnticampWarningType:VolStringToAnticampWarning(const String:warning[])
{
    if (StrEqual(warning, "none", false))
    {
        return Anticamp_NoWarning;
    }
    else if (StrEqual(warning, "chat", false))
    {
        return Anticamp_Chat;
    }
    else if (StrEqual(warning, "center", false))
    {
        return Anticamp_Center;
    }
    else if (StrEqual(warning, "menu", false))
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
