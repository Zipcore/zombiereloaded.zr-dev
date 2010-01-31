/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          downloads.inc
 *  Type:          Core
 *  Description:   Download validation.
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
 * Array that stores a list of downloadable files.
 */
new Handle:arrayDownloads = INVALID_HANDLE;

/**
 * Prepare all model/download data.
 */
DownloadsLoad()
{
    // Register config file.
    ConfigRegisterConfig(File_Downloads, Structure_List, CONFIG_FILE_ALIAS_DOWNLOADS);
    
    // Get downloads file path.
    decl String:pathdownloads[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_DOWNLOADS, pathdownloads);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Downloads, "Config Validation", "Missing downloads file: \"%s\"", pathdownloads);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Downloads, pathdownloads);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Downloads, arrayDownloads, PLATFORM_MAX_PATH);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Downloads, "Config Validation", "Unexpected error encountered loading: %s", pathdownloads);
    }
    
    new downloadcount;
    new downloadvalidcount;
    
    decl String:downloadpath[PLATFORM_MAX_PATH];
    decl String:filepath[PLATFORM_MAX_PATH];
    decl String:filename[32];
    new bool:has_wildcard; // default: false
    new Handle:downloaddir;
    decl String:iter_filename[32];
    new FileType:filetype;
    decl String:fullpath[PLATFORM_MAX_PATH];
    
    new downloads = downloadcount = GetArraySize(arrayDownloads);
    
    // x = download array index.
    for (new x = 0; x < downloads; x++)
    {
        // Get download path
        GetArrayString(arrayDownloads, x, downloadpath, sizeof(downloadpath));
        
        // If file exists, then add to the downloads table and increment valid count.
        if (FileExists(downloadpath))
        {
            // Increment downloadvalidcount
            downloadvalidcount++;
            
            // Precache model file and add to downloads table.
            AddFileToDownloadsTable(downloadpath);
        }
        else
        {
            // Break this filepath up into a path and the file it refers to.
            DownloadsGetFilePathParts(FileType_Directory, downloadpath, filepath, sizeof(filepath), filename, sizeof(filename));
            
            // Path is either invalid, has a wildcard, or just a folder to add all files.
            
            // If the filename has no wildcard in it, then it's either invalid or a folder reference.  So we can safely copy downloadpath over the filepath variable.
            if (!DownloadsWildcardContains(filename))
            {
                strcopy(filepath, sizeof(filepath), downloadpath);
            }
            else
            {
                has_wildcard = true;
            }
            
            // Check if the path is valid, and continue parsing if it is.
            downloaddir = OpenDirectory(filepath);
            if (downloaddir != INVALID_HANDLE)
            {
                // Iterate through every file in the folder, ignoring folder names. (no recursive folder support)
                while (ReadDirEntry(downloaddir, iter_filename, sizeof(iter_filename), filetype))
                {
                    // If the entry isn't a file, then stop.
                    if (filetype != FileType_File)
                    {
                        continue;
                    }
                    
                    // If the filename has a wildcard in it, then check if the file meets the condition.
                    if (has_wildcard)
                    {
                        new bool:match = DownloadsWildcardParse(filename, iter_filename);
                        if (!match)
                        {
                            continue;
                        }
                    }
                    
                    // Format the filepath.
                    if (filepath[strlen(filepath) - 1] == '/')
                    {
                        Format(fullpath, sizeof(fullpath), "%s%s", filepath, iter_filename);
                    }
                    else
                    {
                        Format(fullpath, sizeof(fullpath), "%s/%s", filepath, iter_filename);
                    }
                    
                    // Verify that the file exists.
                    if (!FileExists(fullpath))
                    {
                        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Downloads, "Config Validation", "File iteration failure: %s  Contact plugin support.", fullpath);
                        continue;
                    }
                    
                    // Increment downloadvalidcount
                    downloadvalidcount++;
                    
                    // Precache model file and add to downloads table.
                    AddFileToDownloadsTable(fullpath);
                }
            }
            else
            {
                // Remove client from array.
                RemoveFromArray(arrayDownloads, x);
                
                // Subtract one from count.
                downloads--;
                
                // Backtrack one index, because we deleted it out from under the loop.
                x--;
                
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Downloads, "Config Validation", "Invalid path: \"%s\"", filepath);
            }
        }
    }
    
    // Log model validation info.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Downloads, "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", downloadcount, downloadvalidcount, downloadcount - downloadvalidcount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_Downloads, true);
    ConfigSetConfigReloadFunc(File_Downloads, GetFunctionByName(GetMyHandle(), "DownloadsOnConfigReload"));
    ConfigSetConfigHandle(File_Downloads, arrayDownloads);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public DownloadsOnConfigReload(ConfigFile:config)
{
    // Reload download config.
    DownloadsLoad();
}

/**
 * Multi-purposed:  Can either separate a filename into it's raw name and ext
 *                  or separate the filename refered to at the end of the path.
 * 
 * @param filetype  FileType_File does the former in the description,
 *                  FileType_Directory does the latter.
 * @param file      Either the filename or filepath to break apart.
 * @param part1     The output of either the raw filename or the raw path to a file.
 * @param maxlen    The maximum length of the part1 output string.
 * @param part2     The output of either the file extension or the file at the end of the path.
 * @param maxlen2   The maximum length of the part2 output string.
 */
stock DownloadsGetFilePathParts(FileType:filetype, const String:file[], String:part1[], maxlen, String:part2[], maxlen2)
{
    // Set the correct point to explode the string apart from.
    decl String:explodepoint[4];
    if (filetype == FileType_File)
    {
        strcopy(explodepoint, sizeof(explodepoint), ".");
    }
    else if (filetype == FileType_Directory)
    {
        if (StrContains(file, "/"))
        {
            strcopy(explodepoint, sizeof(explodepoint), "/");
        }
        // This isn't a path, just a filename, so copy it to the part2 output string and nullify part1.
        else
        {
            part1[0] = '\0';
            strcopy(part2, maxlen2, file);
            return;
        }
    }
    
    // Explode the filename at the explode point.
    new String:parts[15][32]; // Only 14 parts allowed, which should be plenty for a file name/path.
    new partcount = ExplodeString(file, explodepoint, parts, sizeof(parts), sizeof(parts[]));
    
    // Copy the last part into the output string.
    if (partcount >= 2)
    {
        strcopy(part2, maxlen2, parts[partcount - 1]);
        parts[partcount - 1][0] = '\0';
    }
    else
    {
        part2[0] = '\0';
    }
    
    // Now that the extension has been copied and removed, implode this back together for the raw name.
    ImplodeStrings(parts, (partcount >= 2) ? partcount - 1: partcount, explodepoint, part1, maxlen);
}

/**
 * Parses a given input to see if it meets the condition specified keeping the wildcard character in consideration.
 * 
 * @param condition     The form the input needs to match. (ex. condition: filename.*  input: filename.ext = pass)
 * @param input         The input to check against the condition.
 * @param wildcard      The wildcard character. (see below for max number of wildcards)
 * 
 * @return              True if the input passes the given condition, false if not.
 */
stock bool:DownloadsWildcardParse(const String:condition[], const String:input[])
{
    // Prepare the regular expression.
    decl String:regex[64];
    DownloadsWildcardPrepRegex(condition, regex, sizeof(regex));
    
    return (SimpleRegexMatch(input, regex, PCRE_CASELESS) > 0);
}

/**
 * This stock simply turns all non-wildcard related metacharacters into literal characters by putting "\" in front of it.
 * And replace simple wildcard formatting with the regex format.
 * 
 * @param input     The input string to be modified.
 * @param regex     The regular expression to prepare.
 * @param maxlen    The maximum length of the output string.
 */
stock DownloadsWildcardPrepRegex(const String:input[], String:regex[], maxlen)
{
    strcopy(regex, maxlen, input);
    
    // Replace unneeded metacharacters with literals.
    ReplaceString(regex, maxlen, "(", "\\(");
    ReplaceString(regex, maxlen, ")", "\\)");
    ReplaceString(regex, maxlen, "{", "\\{");
    ReplaceString(regex, maxlen, "}", "\\}");
    ReplaceString(regex, maxlen, "$", "\\$");
    ReplaceString(regex, maxlen, "+", "\\+");
    ReplaceString(regex, maxlen, ".", "\\.");
    
    // Replace simple wildcard characters with regex wildcard formatting.
    ReplaceString(regex, maxlen, "*", ".*");
    ReplaceString(regex, maxlen, "?", ".");
    
    // Adding a '$' to the end of the line in regular expressions ensures the end matches exactly.
    // Without this, regex would match "The cat in the hat" with "The cat in the hat!!"
    // That would accept ".mdl.ztmp" when putting ".mdl".
    StrCat(regex, maxlen, "$");
}

/**
 * Checks if a string contains any wildcard characters.
 * 
 * @param input     The input string to be checked.
 */
stock DownloadsWildcardContains(const String:input[])
{
    return (StrContains(input, "*") ||
            StrContains(input, "?") ||
            StrContains(input, "[") ||
            StrContains(input, "]"));
}