
/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ezini.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

typedef struct ini_key_list_t
{
    char *key;                  /*!< pointer to a a NULL terminated string
                                    containing key name for this entry */
    char *value;                /*!< pointer to a NULL terminated string
                                    containing key value for this entry Use
                                    ASCII strings to represent numbers */
    struct ini_key_list_t *next;/*!< pointer to the next key/value pair in
                                    in this section */

} ini_key_list_t;


typedef struct ini_section_list_t
{
    char *section;                      /*!< pointer to a NULL terminated string
                                            containing the section name */
    ini_key_list_t *members;            /*!< pointer to the list of all key/value
                                            pairs in this section */
    struct ini_section_list_t *next;    /*!< pointer to the next section in
                                            the list of entries */

} ini_section_list_t;


/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/* allocate */
static ini_key_list_t *NewKeyList(const char *key, const char *value);
static ini_section_list_t *NewSectionList(const char *section, const char *key,
    const char *value);

/* free */
static void FreeKeyList(ini_key_list_t *list);
static void FreeEntry(ini_entry_t *entry);

/* utilities */
static char *SkipWS(const char *str);
static char *DupStr(const char *src);
static char *GetLine(FILE *fp);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

int AddEntryToList(ini_entry_list_t *list, const char *section,
    const char *key, const char *value)
{
    ini_section_list_t *next;
    int result;

    /* handle empty list */
    if (NULL == *list)
    {
        /* add the first entry to the list */
        *list = NewSectionList(section, key, value);

        if (NULL == *list)
        {
            return -1;
        }

        return 0;
    }

    next = *list;
    result = 1;

    while (1)
    {
        result = strcmp(section, next->section);

        if (0 == result)
        {
            break;      /* match, insert here */
        }

        if (NULL == next->next)
        {
            break;      /* no match, create new section here */
        }

        next = next->next;
    }

    if (0 == result)
    {
        ini_key_list_t *member;

        member = next->members;

        while (1)
        {
            result = strcmp(key, member->key);

            if (0 == result)
            {
                break;      /* match, insert here */
            }

            if (NULL == member->next)
            {
                break;      /* no match, create new section here */
            }

            member = member->next;
        }

        if (0 == result)
        {
            /* key exists, change value */
            free(member->value);
            member->value = DupStr(value);

            if (NULL == member->value)
            {
                return -1;
            }
        }
        else
        {
            /* new key, add to list */
            member->next = NewKeyList(key, value);

            if (NULL == member->next)
            {
                return -1;
            }
        }
    }
    else
    {
        /* add the section to the list with this key and value */
        next->next = NewSectionList(section, key, value);
    }

    return 0;
}


void FreeList(ini_entry_list_t list)
{
    /* recurse to the end of the list and free everything on the way back */
    if (list->next != NULL)
    {
        FreeList(list->next);
    }

    if (list->section != NULL)
    {
        /* free the section name */
        free(list->section);
    }

    if (list->members != NULL)
    {
        FreeKeyList(list->members);
    }

    free(list);
}


int MakeINIFile(const char *iniFile, const ini_entry_list_t list)
{
    ini_entry_list_t section;
    ini_key_list_t *members;
    FILE *fp;

    if (NULL == list)
    {
        errno = EINVAL;
        return -1;
    }

    if (NULL == iniFile)
    {
        fp = stdout;
    }
    else
    {

        fp = fopen(iniFile, "w");

        if (NULL == fp)
        {
            return -1;
        }
    }

    section = list;

    while (section != NULL)
    {
        fprintf(fp, "[%s]\n", section->section);

        members = section->members;

        while (members != NULL)
        {
            fprintf(fp, "%s = %s\n", members->key, members->value);
            members = members->next;
        }

        fprintf(fp, "\n");
        section = section->next;
    }

    if (fp != stdout)
    {
        fclose(fp);
    }

    return 0;
}

int AddEntryToFile(const char *iniFile, const ini_entry_list_t list)
{
    ini_entry_t entry;
    ini_entry_list_t merged;
    ini_entry_list_t here;
    int result;
    FILE *fp;

    if (NULL == iniFile)
    {
        errno = EINVAL;
        return -1;
    }

    if (NULL == list)
    {
        errno = EINVAL;
        return -1;
    }

    fp = fopen(iniFile, "r");

    if (NULL == fp)
    {
        return -1;
    }

    merged = NULL;
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into an entry list */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        AddEntryToList(&merged, entry.section, entry.key, entry.value);
    }

    fclose(fp);

    if (result < 0)
    {
        FreeList(merged);
        return -1;
    }

    /* add entries passed into this function to entries from INI file */
    here = list;

    while (here != NULL)
    {
        ini_key_list_t *members;

        members = here->members;

        while (members != NULL)
        {
            AddEntryToList(&merged, here->section, members->key,
                members->value);
            members = members->next;
        }

        here = here->next;
    }

    /* re-write INI file from merged entry list */
    result = MakeINIFile(iniFile, merged);
    FreeList(merged);

    return result;
}

int DeleteEntryFromFile(const char *iniFile, const char *section,
    const char *key)
{
    ini_entry_t entry;
    ini_entry_list_t list;
    int result;
    FILE *fp;

    if (NULL == iniFile)
    {
        errno = EINVAL;
        return -1;
    }

    if (NULL == section)
    {
        errno = EINVAL;
        return -1;
    }

    if (NULL == key)
    {
        errno = EINVAL;
        return -1;
    }

    fp = fopen(iniFile, "r");

    if (NULL == fp)
    {
        return -1;
    }

    list = NULL;
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        if (0 != strcmp(entry.section, section))
        {
            /* this isn't one we're supposed to delete */
            AddEntryToList(&list, entry.section, entry.key, entry.value);
        }
        else if (0 != strcmp(entry.key, key))
        {
            /* this isn't one we're supposed to delete */
            AddEntryToList(&list, entry.section, entry.key, entry.value);
        }
    }

    fclose(fp);

    if (result < 0)
    {
        FreeList(list);
        return -1;
    }

    result = MakeINIFile(iniFile, list);
    FreeList(list);

    return result;
}

int GetEntryFromFile(FILE *iniFile, ini_entry_t *entry)
{
    char *line;
    char *ptr;

    if (NULL == iniFile)
    {
        errno = EINVAL;
        return -1;
    }

    if (NULL == entry)
    {
        errno = EINVAL;
        return -1;
    }

    /* handle section names, comments, and blank lines */
    while ((line = GetLine(iniFile)) != NULL)
    {
        /* skip leading spaces and blank lines */
        ptr = SkipWS(line);

        /* skip blank lines and lines starting with ';' or '#' */
        if (*ptr == '\0' || *ptr == ';' || *ptr == '#')
        {
            free(line);
            continue;
        }
        else if (*ptr == '[')
        {
            /* possible new section */
            char *end;

            end = strchr(ptr, ']');

            if (NULL == end)
            {
                FreeEntry(entry);
                errno = EILSEQ;
                return -1;
            }

            /* we have the full string for a new section, trim white space */
            ptr = SkipWS(ptr + 1);

            while (isspace(*(end - 1)))
            {
                end--;
            }

            *end = '\0';

            free(entry->section);
            entry->section = DupStr(ptr);
            free(line);
        }
        else
        {
            /* this line should be key = value */
            break;
        }
    }

    /* we either have a non-section line or nothing left to get */
    if (NULL == line)
    {
        /* nothing left to get */
        FreeEntry(entry);
        return 0;
    }

    /* the only other allowable lines are of the form key = value */
    free(entry->key);       /* free old key */
    entry->key = ptr;
    ptr++;

    while (*ptr != '=')
    {
        if (*ptr == '\0')
        {
            /* didn't find '=' */
            entry->key = NULL;
            FreeEntry(entry);
            free(line);
            errno = EILSEQ;
            return -1;
        }

        ptr++;
    }

    /* we found the '=' separating key and value trim white space */
    free(entry->value);     /* free old value */
    entry->value = ptr + 1;
    ptr--;

    while (isspace(*ptr))
    {
        ptr--;
    }

    *(ptr + 1) = '\0';
    entry->key = DupStr(entry->key);

    ptr = entry->value;

    /* now skip white space after '=' */
    while (*ptr == ' ' || *ptr =='\t')
    {
        if (*ptr == '\0')
        {
            FreeEntry(entry);
            free(line);
            errno = EILSEQ;
            return -1;
        }

        ptr++;
    }

    /* we found the start of value, trim trailing white space */
    entry->value = ptr;
    ptr = entry->value + strlen(entry->value) - 1;

    while (isspace(*ptr))
    {
        ptr--;
    }

    *(ptr + 1) = '\0';
    entry->value = DupStr(entry->value);

    free(line);
    return 1;
}

static ini_key_list_t *NewKeyList(const char *key, const char *value)
{
    ini_key_list_t *item;

    item = (ini_key_list_t *)malloc(sizeof(ini_key_list_t));

    if (NULL == item)
    {
        return NULL;
    }

    /* allocation succeeded copy key and value */
    item->next = NULL;

    item->key = DupStr(key);

    if (NULL == item->key)
    {
        free(item);
        return NULL;
    }

    item->value = DupStr(value);

    if (NULL == item->value)
    {
        free(item->key);
        free(item);
        return NULL;
    }

    return item;
}

static ini_section_list_t *NewSectionList(const char *section, const char *key,
    const char *value)
{
    ini_section_list_t *item;

    item = (ini_section_list_t *)malloc(sizeof(ini_section_list_t));

    if (NULL == item)
    {
        return NULL;
    }

    /* now populate item */
    item->next = NULL;
    item->section = DupStr(section);

    if (NULL == item->section)
    {
        free(item);
        return NULL;
    }

    /* start a member list with the current key and value */
    item->members = NewKeyList(key, value);

    if (NULL == item->members)
    {
        free(item->section);
        free(item);
        return NULL;
    }

    return item;
}

static void FreeKeyList(ini_key_list_t *list)
{
    /* recurse to the end of the list and free everything on the way back */
    if (list->next != NULL)
    {
        FreeKeyList(list->next);
    }

    free(list->key);
    free(list->value);
    free(list);
}

static void FreeEntry(ini_entry_t *entry)
{
    free(entry->section);
    free(entry->key);
    free(entry->value);

    entry->section = NULL;
    entry->key = NULL;
    entry->value = NULL;
}

static char *SkipWS(const char *str)
{
    char *c;

    c = (char *)str;
    while(isspace(*c))
    {
        c++;
    }

    return c;
}

static char *DupStr(const char *src)
{
    char *dest;

    if (NULL == src)
    {
        return NULL;
    }

    dest = (char *)malloc(strlen(src) + 1);

    if (NULL != dest)
    {
        strcpy(dest, src);
    }

    return dest;
}

static char *GetLine(FILE *fp)
{
    char *line;         /* string to read line into */
    char *next;         /* where to write the next characters into */
    const size_t chunkSize = 32;
    size_t lineSize;

    if ((NULL == fp) || feof(fp))
    {
        return NULL;
    }

    lineSize = chunkSize;
    line = (char *)malloc(lineSize * sizeof(char));

    if (NULL == line)
    {
        /* allocation failed */
        return NULL;
    }

    line[0] = '\0';
    next = line;

    while (NULL != fgets(next, lineSize - strlen(line), fp))
    {
        if ('\n' == line[strlen(line) - 1])
        {
            /* we got to the EOL strip off the trailing '\n' and exit */
            line[strlen(line) - 1] = '\0';
            break;
        }
        else
        {
            /* there's still more on this line */
            lineSize += chunkSize;
            line = (char *)realloc(line, lineSize);

            if (NULL == line)
            {
                return NULL;
            }

            next = line + strlen(line);
        }
    }

    return line;
}
