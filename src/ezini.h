#ifndef __EZINI_H
#define __EZINI_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

typedef struct
{
    char *section;      /*!< pointer to a NULL terminated string containing the
                            section name */
    char *key;          /*!< pointer to a a NULL terminated string containing
                            the key name */
    char *value;        /*!< pointer to a NULL terminated string with entry
                            value.  Use ASCII strings to represent numbers */
} ini_entry_t;

typedef struct ini_section_list_t* ini_entry_list_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/* add (section, key, value) to a list of INI entries */
int AddEntryToList(ini_entry_list_t *list, const char *section,
    const char *key, const char *value);

/* free all of the entries in an entry list */
void FreeList(ini_entry_list_t list);

/* create/add entries to an INI file from a sorted entry list */
int MakeINIFile(const char *iniFile, const ini_entry_list_t list);
int AddEntryToFile(const char *iniFile, const ini_entry_list_t list);

/* remove a single entry from an INI file */
int DeleteEntryFromFile(const char *iniFile, const char *section,
    const char *key);

/***************************************************************************
* get the next entry in INI file.
* returns:  1 if an entry is found
*           0 if there are no more entries
*           -1 on error
***************************************************************************/
int GetEntryFromFile(FILE *iniFile, ini_entry_t *entry);

#ifdef __cplusplus
}
#endif

#endif  /* ndef __EZINI_H */
