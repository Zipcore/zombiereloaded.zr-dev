/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           classeditmanager.inc
 *  Type:           Module
 *  Description:    Class editor managing API.
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
 * Creates a new class editor with the specified settings.
 *
 * @param mode          Optional. Mode to use.
 * @param classIndex    Optional. Class to replace if mode is 'name'.
 * @param classData     Optional. Class data to apply if mode is 'attributes'.
 * @param name          Optional. Name of this class editor.
 * @param classErrors   Optional output. Class error flags of invalid class
 *                      attributes.
 * @return              Class editor index, or negative on error.
 *                      -1 if no free class editors
 *                      -2 invalid class index
 *                      -3 invalid class attributes
 */
VolClassEditAdd(VolClassEditMode:mode = VOL_CLASSEDIT_DEF_MODE,
                classIndex = VOL_CLASSEDIT_DEF_CLASS_INDEX,
                classData[ClassEditableAttributes] = VolEmptyAttributes,
                String:name[] = VOL_CLASSEDIT_DEF_NAME,
                &classErrors = 0)
{
    // Validate class index if used.
    if (mode == ClassEditMode_Name && !ClassValidateIndex(classIndex))
    {
        return -2;
    }
    
    // Validate class data if used.
    if (mode == ClassEditMode_Attributes)
    {
        classErrors = ClassValidateEditableAttributes(classData);
        if (classErrors > 0)
        {
            return -3;
        }
    }
    
    // Get free class editor index.
    new classedit = VolClassEditGetFreeIndex();
    
    // Validate index.
    if (classedit < 0)
    {
        return -1;
    }
    
    // Apply class editor attributes.
    VolClassEditData[classedit][VolClassEdit_Mode] = mode;
    VolClassEditData[classedit][VolClassEdit_ClassIndex] = classIndex;
    VolClassEditClassAttributes[classedit] = classData;
    
    TrimString(name);
    strcopy(VolClassEditData[classedit][VolClassEdit_Name], VOL_NAME_LEN, name);
    
    return classedit;
}

/**
 * Removes the specified class editor.
 *
 * @param classedit     Class editor to remove.
 * @param recursive     Optional. Also remove the volume and its features.
 *                      Default is false.
 * @return              True if successful, false otherwise.
 */
bool:VolClassEditRemove(classedit, bool:recursive = false)
{
    // Validate index.
    if (!VolClassEditIsValid(classedit))
    {
        return false;
    }
    
    // Get the volume that use this class editor.
    new volumeIndex = VolClassEditData[classedit][VolClassEdit_VolumeIndex];
    if (volumeIndex >= 0)
    {
        // Check if recursive removal is enabled.
        if (recursive)
        {
            // Remove volume if valid.
            VolRemove(volumeIndex, recursive);
        }
        else
        {
            // Detach the class editor if attached.
            VolDetachFeature(volumeIndex, VolFeature_ClassEdit, classedit);
        }
    }
    
    // Remove the class editor.
    VolClassEditReset(classedit);
    
    return true;
}

/**
 * Removes all class editors.
 *
 * @param recursive     Optional. Also remove the volume and its features.
 *                      Default is false.
 * @return              True if successful, false otherwise.
 */
stock VolClassEditRemoveAll(bool:recursive = false)
{
    for (new classedit = 0; classedit < ZR_CLASEDIT_MAX; classedit++)
    {
        VolClassEditRemove(classedit, recursive);
    }
}

/**
 * Search for a class editor with the specified name.
 *
 * @param name              Name to search for.
 * @param caseSensitive     Optional. Use case sensitive search. Default is
 *                          false.
 * @return                  Class editor index if found, -1 otherwise.
 */
VolClassEditFind(const String:name[], bool:caseSensitive = false)
{
    for (new classedit = 0; classedit < ZR_CLASSEDIT_MAX; classedit++)
    {
        if (VolClassEditInUse(classedit))
        {
            if (StrEqual(VolClassEditData[classedit][VolClassEdit_Name], name, caseSensitive))
            {
                return classedit;
            }
        }
    }
    
    return -1;
}
