/*
*  C Implementation: fileops.c
*
* (Note: read_config_file() was made possible by studying the file prefs.c in gtkpod:
*  URL: http://www.gtkpod.org/
*  URL: http://gtkpod.sourceforge.net/
*  Copyright (C) 2002-2005 Jorg Schuler <jcsjcs at users sourceforge net>.
*  Licensed under GNU GPL v2.
*  This code is a nearly complete rewrite but I would like to express my thanks.)
*
*
* Description: File operations - together, fileops.h and fileops.c contain 
* all code involving disk operations.  The intention is to make it easier to
* port tuxmath to other operating systems, as code to read and write as 
* well as paths and file locations may be more OS-dependent.
*
* This file contains functions to read and write config files.
* The config file contains name-value pairs, one pair per line, to control
* settings for the behavior of Tuxmath.
*
* Code for loading program data from disk is now also found here.
* 
* Author: David Bruce <davidstuartbruce@gmail.com>, (C) 2006
*
* Copyright: See COPYING file that comes with this distribution (briefly, GNU GPL)
*
*/

#include <stdio.h>

//#include "config.h"

/* Tuxmath includes: */
#include "globals.h"
#include "fileops.h"
#include "setup.h"
#include "options.h"
#include "highscore.h"
#include "scandir.h"

/* OS includes - NOTE: these may not be very portable */
#include <dirent.h>  /* for opendir() */
#include <sys/stat.h>/* for mkdir() */
#include <unistd.h>  /* for getcwd() */
#include <sys/types.h> /* for umask() */

/* Standard C includes: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


/* Used by both write_pregame_summary() and */
/* write_postgame_summary() so defined with */
/* file scope:                              */
#ifdef BUILD_MINGW32
#define SUMMARY_EXTENSION ".txt"
#else
#define SUMMARY_EXTENSION ""
#endif 

static char* summary_filenames[NUM_SUMMARIES] = {
  "summary1" SUMMARY_EXTENSION,
  "summary2" SUMMARY_EXTENSION,
  "summary3" SUMMARY_EXTENSION,
  "summary4" SUMMARY_EXTENSION,
  "summary5" SUMMARY_EXTENSION,
  "summary6" SUMMARY_EXTENSION,
  "summary7" SUMMARY_EXTENSION,
  "summary8" SUMMARY_EXTENSION,
  "summary9" SUMMARY_EXTENSION,
  "summary10" SUMMARY_EXTENSION
};

/* local function prototypes: */
static int find_tuxmath_dir(void);
static int str_to_bool(const char* val);
static int read_config_file(FILE* fp, int file_type);
static int write_config_file(FILE* fp, int verbose);
static int is_lesson_file(const struct dirent *lfdirent);
static int read_goldstars(void);
static int read_lines_from_file(FILE *fp,char ***lines);
static int parse_option(const char* name, int val, int file_type);
static void dirname_up(char *dirname);
static char* get_user_name(void);
static char* get_file_name(char *fullpath);


/* Mingw does not have localtime_r(): */
/* (this replacement is Windows-specific, so also check for Win32) */
#ifndef HAVE_LOCALTIME_R
#ifdef WIN32
#define localtime_r( _clock, _result ) \
        ( *(_result) = *localtime( (_clock) ), \
          (_result) )
#endif
#endif


/*************************************************************************
Using Autoconf's "config.h", we include our portability replacements
for scandir() and alphasort() if necessary:
*************************************************************************/

#ifndef HAVE_SCANDIR
#include "scandir.h"
#endif /* end of scandir() replacements */

/* fix HOME on windows */
#ifdef BUILD_MINGW32
#include <windows.h>





 
/* STOLEN from tuxpaint */

/*
  Removes a single '\' or '/' from end of path 
*/
static char *remove_slash(char *path)
{
  int len = strlen(path);

  if (!len)
    return path;

  if (path[len-1] == '/' || path[len-1] == '\\')
    path[len-1] = 0;

  return path;
}

/*
  Read access to Windows Registry
*/
static HRESULT ReadRegistry(const char *key, const char *option, char *value, int size)
{
  LONG        res;
  HKEY        hKey = NULL;

  res = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
  if (res != ERROR_SUCCESS)
    goto err_exit;
  res = RegQueryValueEx(hKey, option, NULL, NULL, (LPBYTE)value, (LPDWORD)&size);
  if (res != ERROR_SUCCESS)
    goto err_exit;
  res = ERROR_SUCCESS;

err_exit:
  if (hKey) RegCloseKey(hKey);
  return HRESULT_FROM_WIN32(res);
}


/*
  Returns heap string containing default application data path.
  Creates suffix subdirectory (only one level).
  E.g. C:\Documents and Settings\jfp\Application Data\suffix
*/
char *GetDefaultSaveDir(const char *suffix)
{
  char          prefix[MAX_PATH];
  char          path[2*MAX_PATH];
  const char   *key    = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
  const char   *option = "AppData";
  HRESULT hr = S_OK;

  if (SUCCEEDED(hr = ReadRegistry(key, option, prefix, sizeof(prefix))))
  {
    remove_slash(prefix);
    snprintf(path, sizeof(path), "%s/%s", prefix, suffix);
    _mkdir(path);
    return strdup(path);
  }
  return strdup("userdata");
}


/* Windows XP: User/App Data/TuxMath/ */
/* Windows 98/ME: TuxMath install dir/userdata/Options */
#define OPTIONS_SUBDIR ""
#define OPTIONS_FILENAME "options.cfg"
#define HIGHSCORE_FILENAME "highscores.txt"
#define GOLDSTAR_FILENAME "goldstars.txt"
#define USER_MENU_ENTRIES_FILENAME "user_menu_entries.txt"
#define USER_LOGIN_QUESTIONS_FILENAME "user_login_questions.txt"
#else

# define get_home getenv("HOME")
#define OPTIONS_SUBDIR "/.tuxhistory"
#define OPTIONS_FILENAME "options"
#define HIGHSCORE_FILENAME "highscores"
#define GOLDSTAR_FILENAME "goldstars"
#define USER_MENU_ENTRIES_FILENAME "user_menu_entries"
#define USER_LOGIN_QUESTIONS_FILENAME "user_login_questions"

#endif


/* This functions keep and returns the user data directory application path */
/* FIXME?: currently the best way to test whether we're using the user's    */
/* home directory, or using a different path, is to test add_subdir (which  */
/* is 1 if we're using the user's ~/.tuxmath directory, 0 otherwise). Is    */
/* this a bad example of using 1 thing for 2 purposes? So far there are     */
/* no conflicts. */
static char* user_data_dir = NULL;
static int add_subdir = 1;
static char* high_scores_file_path = NULL;

/* A variable for storing the "current" config filename */
static char* last_config_file_name = NULL;

char *get_user_data_dir ()
{ 
  if (! user_data_dir)
#ifdef BUILD_MINGW32
     user_data_dir = GetDefaultSaveDir(PROGRAM_NAME);
#else
     user_data_dir = strdup(getenv("HOME"));
#endif

  return user_data_dir;  
}

/* This function sets the user data directory, and also sets a flag
   indicating that this should function as a .tuxmath directory, and
   thus doesn't need the subdir appended. */
void set_user_data_dir(const char *dirname)
{
  int len;

  if (user_data_dir != NULL)
    free(user_data_dir);   // clear the previous setting

  // Allocate space for the directory name. We do it with +2 because
  // we have to leave room for a possible addition of a "/"
  // terminator.
  user_data_dir = (char*) malloc((strlen(dirname)+2)*sizeof(char));
  if (user_data_dir == NULL) {
    fprintf(stderr,"Error: insufficient memory for duplicating string %s.\n",dirname);
    exit(EXIT_FAILURE);
  }
  strcpy(user_data_dir,dirname);

  // Check to see that dirname is properly terminated
  len = strlen(user_data_dir);
  if (user_data_dir[len-1] != '/')
    strcat(user_data_dir,"/");

  // If the user supplies a homedir, interpret it literally and don't
  // add .tuxmath
  add_subdir = 0;
}

/* This gets the user data directory including the .tuxmath, if applicable */
void get_user_data_dir_with_subdir(char *opt_path)
{
  strcpy(opt_path, get_user_data_dir());
  if (add_subdir)
    strcat(opt_path, OPTIONS_SUBDIR "/");
}
  
/* FIXME should have better file path (/etc or /usr/local/etc) and name */
int read_global_config_file(void)
{
  FILE* fp;
  fp = fopen(DATA_PREFIX "/missions/options", "r");
  if (fp)
  {
    read_config_file(fp, GLOBAL_CONFIG_FILE);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else
    return 0;
}

/* Attempts to read in user's config file - on a *nix system, */
/* something like: /home/laura/.tuxmath/options               */
int read_user_config_file(void)
{
  FILE* fp;
  char opt_path[PATH_MAX];

  /* find $HOME and tack on file name: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, OPTIONS_FILENAME);

  DEBUGMSG(debug_fileops, "In read_user_config_file() full path to config file is: = %s\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    read_config_file(fp, USER_CONFIG_FILE);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else  /* could not open config file: */
  {
    return 0;
  }
}

/* Looks for matching file in various locations:        */
/*   1. Current working directory                       */
/*   2. As an absolute path filename                    */
/*   3. In tuxmath's missions directory.                */
/*   4. In missions/lessons directory.                  */
/*   5. In missions/arcade directory.                   */
/*   6. In user's own .tuxmath directory                */
/* FIXME redundant code - figure out way to iterate through above */
int read_named_config_file(const char* fn)
{
  FILE* fp;
  char opt_path[PATH_MAX];
  /* Make compiler happy: */
  const char* filename = (const char*)fn;

  if (last_config_file_name != NULL)
    free(last_config_file_name);
  last_config_file_name = strdup(filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() filename is: = %s\n", filename);

  /* First look in current working directory:  */
  getcwd(opt_path, PATH_MAX); /* get current working directory */
  /* add separating '/' unless cwd is '/' : */
  if (0 != strcmp("/", opt_path)) 
  {
    strcat(opt_path, "/");
  }
  strcat(opt_path, filename); /* tack on filename              */

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (cwd)\n", opt_path);

  fp = fopen(opt_path, "r");  /* try to open file */
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* try matching filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }


  /* Next try matching filename as absolute:      */
  /* Supply leading '/' if not already there:   */
  if (0 == strncmp ("/", filename, 1))
  { 
    strcpy(opt_path, filename);
  }
  else
  {
    strcpy(opt_path, "/");
    strcat(opt_path, filename);
  }

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (abs)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }


  /* Next look in missions folder:      */
  strcpy(opt_path, DATA_PREFIX);
  strcat(opt_path, "/missions/");
  strcat(opt_path, filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (missions)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }  

  /* Next look in missions/lessons folder (for prepared "lessons curriculum"):      */
  strcpy(opt_path, DATA_PREFIX);
  strcat(opt_path, "/missions/lessons/");
  strcat(opt_path, filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (missions/lessons)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }  

  /* Next look in missions/arcade folder (for high score competition):      */
  strcpy(opt_path, DATA_PREFIX);
  strcat(opt_path, "/missions/arcade/");
  strcat(opt_path, filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (missions/arcade)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }  

  /* Look in user's hidden .tuxmath directory  */
  /* find $HOME and tack on file name: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (.tuxmath)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }


  /* Look in user's home directory  */
  /* find $HOME and tack on file name: */
  strcpy(opt_path, get_user_data_dir());
  strcat(opt_path, "/");
  strcat(opt_path, filename);

  DEBUGMSG(debug_fileops, "In read_named_config_file() checking for %s (home)\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    DEBUGMSG(debug_fileops, "Found %s\n", opt_path);

    if (read_config_file(fp, USER_CONFIG_FILE))
    {
      fclose(fp);
      fp = NULL;
      return 1;
    }
    else /* keep trying to match filename elsewhere */
    {
      fclose(fp);
      fp = NULL;
    }
  }

  /* Could not find file (or read it if found) in any location: */
  DEBUGMSG(debug_fileops, "read_named_config_file() could not find/read: %s\n", opt_path);
  return 0;
}

/* NOTE the cast to "const char*" just prevents compiler from complaining */
static int is_lesson_file(const struct dirent *lfdirent)
{
  return (0 == strncasecmp((const char*)&(lfdirent->d_name), "lesson", 6));
  /* FIXME Should somehow test each file to see if it is a tuxmath config file */
}



int parse_lesson_file_directory(void)
{
  char lesson_path[PATH_MAX];             //Path to lesson directory
  char* fgets_return_val;
  char name_buf[NAME_BUF_SIZE];
  int nchars;

  struct dirent **lesson_list_dirents = NULL;
  FILE* tempFile = NULL;

  int i = 0;
  int lessonIterator = 0;  //Iterator over matching files in lesson dir
  int length = 0;
  int lessons = 0;         //Iterator over accepted (& parsed) lesson files

  num_lessons = 0;

  /* find the directory containing the lesson files:  */
  nchars = snprintf(lesson_path, PATH_MAX, "%s/missions/lessons", DATA_PREFIX);
  if (nchars < 0 || nchars >= PATH_MAX) {
    perror("formatting lesson directory");
    return 0;
  }

  DEBUGMSG(debug_fileops, "lesson_path is: %s\n", lesson_path);

  /* Believe we now have complete scandir() for all platforms :) */
  num_lessons = scandir(lesson_path, &lesson_list_dirents, is_lesson_file, alphasort);

  DEBUGMSG(debug_fileops, "num_lessons is: %d\n", num_lessons);

  if (num_lessons < 0) {
    perror("scanning lesson directory");
    num_lessons = 0;
    return 0;
  }

  /* Allocate storage for lesson list */
  lesson_list_titles = (char**) malloc(num_lessons * sizeof(char*));
  lesson_list_filenames = (char**) malloc(num_lessons * sizeof(char*));
  if (lesson_list_titles == NULL || lesson_list_filenames == NULL) {
    perror("allocating memory for lesson list");
    return 0;
  }
  for (lessonIterator = 0; lessonIterator < num_lessons; lessonIterator++) {
    lesson_list_titles[lessonIterator] = (char*) malloc(NAME_BUF_SIZE * sizeof(char));
    lesson_list_filenames[lessonIterator] = (char*) malloc(NAME_BUF_SIZE * sizeof(char));
    if (lesson_list_titles[lessonIterator] == NULL || lesson_list_filenames[lessonIterator] == NULL) {
      perror("allocating memory for lesson filenames or titles");
      return 0;
    }
  }

  /* lessonIterator indexes the direntries, lessons indexes */
  /* the correctly-parsed files.  If successful in parsing, */
  /* lessons gets incremented. In case of problems, we can  */
  /* just continue onto the next entry without incrementing */
  /* lessons, and the bad entry will get overwritten by the */
  /* next one (or simply never used, if it was the last).   */
  for (lessonIterator = 0, lessons = 0; lessonIterator < num_lessons; lessonIterator++) {
    /* Copy over the filename (as a full pathname) */
    nchars = snprintf(lesson_list_filenames[lessons], NAME_BUF_SIZE, "%s/%s", lesson_path, lesson_list_dirents[lessonIterator]->d_name);
    if (nchars < 0 || nchars >= NAME_BUF_SIZE)
      continue;

    DEBUGMSG(debug_fileops, "Found lesson file %d:\t%s\n", lessons, lesson_list_filenames[lessons]);

    /* load the name for the lesson from the file ... (1st line) */
    tempFile = fopen(lesson_list_filenames[lessons], "r");
    if (tempFile==NULL)
    {
      continue;
    }
    fgets_return_val = fgets(name_buf, NAME_BUF_SIZE, tempFile);
    if (fgets_return_val == NULL) {
      continue;
    }
    fclose(tempFile);


    /* check to see if it has a \r at the end of it (dos format!) */
    length = strlen(name_buf);
    while (length>0 && (name_buf[length - 1] == '\r' || name_buf[length - 1] == '\n')) {
      name_buf[length - 1] = '\0';
      length--;
    }

    /* Go past leading '#', ';', or whitespace: */
    /* NOTE getting i to the correct value on exit is the main goal of the loop */
    for (  i = 0;
           ((name_buf[i] == '#') ||
           (name_buf[i] == ';') ||
           isspace(name_buf[i])) &&
           (i < NAME_BUF_SIZE);
           i++  )
    {
      length--;
    }
    /* Now copy the rest of the first line into the list: */
    /* Note that "length + 1" is needed so that the final \0 is copied! */
    memmove(lesson_list_titles[lessons], &name_buf[i], length + 1); 


    /* Increment the iterator for correctly-parsed lesson files */
    lessons++;
  }
  /* Now free the individual dirents. We do this on a second pass */
  /* because of the "continue" approach used to error handling.   */
  for (lessonIterator = 0; lessonIterator < num_lessons; lessonIterator++)
    free(lesson_list_dirents[lessonIterator]);
  free(lesson_list_dirents);

  /* In case we didn't keep all of them, revise our count of how */
  /* many there are */
  num_lessons = lessons;

  /* Now we check to see which lessons have been previously completed */
  /* so we can display the Gold Stars: */
  /* Allocate storage for lesson list */

  /* prevent memory leak in case we called this already and */
  /* free the list:                                         */
  /* Now read file to see what lessons have been previously completed: */

  return (num_lessons > 0);  /* Success! */
}


/* Look for a completed lessons file in the user's homedir   */
/* and if found, pass the FILE* to read_goldstars_fp()       */
/* to actually read the data. The idea is to have TuxMath    */
/* keep track of what lessons the student has successfully   */
/* completed and display the "Gold Star" icon for those,     */
/* versus a grayed-out one for lessons remaining to be done. */
int read_goldstars(void)
{
  FILE* fp;
  char opt_path[PATH_MAX];

  /* find $HOME and tack on file name: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, GOLDSTAR_FILENAME);

  DEBUGMSG(debug_fileops, "In read_goldstars() full path to file is: = %s\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    read_goldstars_fp(fp);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else  /* could not open goldstar file: */
  {
    return 0;
  }
}


/* Write gold star list in user's homedir in format     */
/* compatible with read_goldstars() above.              */
int write_goldstars(void)
{
  char opt_path[PATH_MAX];
  FILE* fp;

  if (!find_tuxmath_dir())
  {
    fprintf(stderr, "\nCould not find or create tuxmath dir\n");
    return 0;
  }

  /* find $HOME and add rest of path to config file: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, GOLDSTAR_FILENAME);
}


/* Look for a highscore table file in the current user    */
/* data directory.  Return 1 if found, 0 if not.  This    */
/* is used for the multi-user login code, in deciding     */
/* where to put the highscore information.                */
int high_scores_found_in_user_dir(void)
{
  FILE* fp;
  char opt_path[PATH_MAX];

  /* find $HOME and tack on file name: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, HIGHSCORE_FILENAME);

  DEBUGMSG(debug_fileops, "In read_high_scores() full path to file is: = %s\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    fclose(fp);
    return 1;
  }
  else
    return 0;
}

/* Set the path to the high score file to the current     */
/* user data dir                                          */
void set_high_score_path(void)
{
  char opt_path[PATH_MAX];

  /* find $HOME and tack on file name: */
  get_user_data_dir_with_subdir(opt_path);

  // Free any previous allocation
  if (high_scores_file_path != NULL)
    free(high_scores_file_path);

  high_scores_file_path = strdup(opt_path);
}

/* Look for a high score table file in the user's homedir */
/* and if found, pass the FILE* to read_high_scores_fp() in */
/* highscore.c to actually read in scores. (A "global"    */
/* location might in theory be better, but most schools   */
/* run Windows with all students sharing a common login   */
/* that may not be able to write to "global" locations).  */
int read_high_scores(void)
{
  FILE* fp;
  char opt_path[PATH_MAX];

  /* find $HOME and tack on file name: */
  if (high_scores_file_path == NULL)
    get_user_data_dir_with_subdir(opt_path);
  else
    strncpy(opt_path,high_scores_file_path,PATH_MAX);
  strcat(opt_path, HIGHSCORE_FILENAME);

  DEBUGMSG(debug_fileops, "In read_high_scores() full path to file is: = %s\n", opt_path);

  fp = fopen(opt_path, "r");
  if (fp) /* file exists */
  {
    initialize_scores();  // clear any previous values
    read_high_scores_fp(fp);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else  /* could not open highscore file: */
  {
    return 0;
  }
}

/* On File Locking: With multiple users possibly updating the same
   high-scores table simultaneously, we have to be concerned with the
   possibility that the high score information might change between
   the time at which it was determined that the user gets a high
   score, and time at which the high score table actually gets
   written.  This is especially problematic if it takes kids a while
   to type in their name, and it's being assumed that the high scores
   table is valid over that entire time.

   As a first (easy) step, it's best to simply append new information
   to the high scores file, rather than re-writing the whole file; the
   read function can make sure that only the top scores are used.
   That way, the only time there would be trouble is if two appends
   start at exactly the same moment; and since the amount of
   information per line is small (and is thus written quickly) and
   updates are unlikely to be occurring on a
   millisecond-by-millisecond basis, it's pretty unlikely that
   problems will crop up.

   An even more robust alternative is to use real file locking.  One
   would need to design a cross-platform solution that also does
   sensible things (like, say, delete the lock if it's been held for
   more than 1s, so that locking doesn't block the application).  In
   researching this, the best approach seems to be:
   a) Open a second file - a lock file of a specific name - for read/write.
   b) If the lock file already contains your process ID, proceed
   c) If the lock file already contains a different process ID, deny
   d) If the lock file is new / empty write and flush your process ID
      to it, then go back to step (a)

   However, given that this information may not be "mission critical"
   (pun intended) and might be cleared on a somewhat regular basis
   anyway, it seems reasonable to just use the append strategy.
*/

/* Append a new high score to the high-scores file.       */
/* Using this approach is safer than writing the whole    */
/* high scores table if you're in an environment where    */
/* multiple users might be updating the table             */
/* simultaneously.                                        */
int append_high_score(int tableid,int score,char *player_name)
{
  char opt_path[PATH_MAX];
  FILE* fp;

  if (!find_tuxmath_dir())
  {
    fprintf(stderr, "\nCould not find or create tuxmath dir\n");
    return 0;
  }

  /* find $HOME and add rest of path to config file: */
  if (high_scores_file_path == NULL)
    get_user_data_dir_with_subdir(opt_path);
  else
    strncpy(opt_path,high_scores_file_path,PATH_MAX);
  strcat(opt_path, HIGHSCORE_FILENAME);

  DEBUGMSG(debug_fileops, "In write_high_scores() full path to file is: = %s\n", opt_path);

  fp = fopen(opt_path, "a");
  if (fp)
  {
    fprintf(fp,"%d\t%d\t%s\t\n",tableid,score,player_name);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else
    return 0;
}
  


/* Checks to see if the current homedir has a menu_entries file, and if */
/* so returns the names of the menu entries. This is used in cases      */
/* where users must select their login information. Returns the number  */
/* of menu entries (0 if there are none), and sets the input            */
/* argument to a malloc-ed array of names (sets to NULL if there are no */
/* choices to be made).  */
int read_user_menu_entries(char ***user_names)
{
  FILE *fp;
  int n_entries;
  char opt_path[PATH_MAX],menu_entries_file[PATH_MAX];

  // Look for a menu_entries file
  get_user_data_dir_with_subdir(opt_path);
  strncpy(menu_entries_file,opt_path,PATH_MAX);
  strncat(menu_entries_file,USER_MENU_ENTRIES_FILENAME,PATH_MAX-strlen(menu_entries_file));
  n_entries = 0;
  fp = fopen(menu_entries_file,"r");
  if (fp)
  {
    // There is a menu_entries file, read it
    n_entries = read_lines_from_file(fp,user_names);
    fclose(fp);
  }

  return n_entries;
}

/* Reads the user_login_questions file. The syntax is identical to
   read_user_menu_entries. */
int read_user_login_questions(char ***user_login_questions)
{
  FILE *fp;
  int n_entries;
  char opt_path[PATH_MAX],user_login_questions_file[PATH_MAX];

  // Look for a user_login_questions file
  get_user_data_dir_with_subdir(opt_path);
  strncpy(user_login_questions_file,opt_path,PATH_MAX);
  strncat(user_login_questions_file,USER_LOGIN_QUESTIONS_FILENAME,PATH_MAX-strlen(user_login_questions_file));
  n_entries = 0;
  fp = fopen(user_login_questions_file,"r");
  if (fp)
  {
    // There is a user_login_questions file, read it
    n_entries = read_lines_from_file(fp,user_login_questions);
    fclose(fp);
   }
 
  return n_entries;
}

void user_data_dirname_up(void)
{
  dirname_up(user_data_dir);
}

void user_data_dirname_down(char *subdir)
{
  DIR *dir;

  // The space for user_data_dir has to have sufficient memory
  // available for concatenating subdir and a possible final "/",
  // hence the +2s.
  if (user_data_dir != NULL) {
    user_data_dir = (char*) realloc(user_data_dir,(strlen(user_data_dir) + strlen(subdir) + 2)*sizeof(char));
    if (user_data_dir == NULL) {
      fprintf(stderr,"Error allocating memory in user_data_dirname_down.\n");
      exit(EXIT_FAILURE);
    }
    strcat(user_data_dir,subdir);
  }
  else {
    user_data_dir = (char*) malloc((strlen(subdir)+2)*sizeof(char));
    if (user_data_dir == NULL) {
      fprintf(stderr,"Error allocating memory in user_data_dirname_down.\n");
      exit(EXIT_FAILURE);
    }
    strcpy(user_data_dir,subdir);
  }
  strcat(user_data_dir,"/");
  dir = opendir(user_data_dir);
  if (dir == NULL) {
    printf("User data directory cannot be opened, there is a configuration error\n");
    printf("Continuing anyway without saving or loading individual settings.\n");
  }
  else {
    closedir(dir);
    // If we have multi-user logins, don't create restrictive
    // permissions on new or rewritten files
    umask(0x0);
  }
}


/***********************************************************
*                                                          *
*       "Private methods" with file scope only             *
*                                                          *
***********************************************************/


/* This function does the heavy lifting, so to speak:     */
/* Note that file_type simply indicates whether or not    */
/* to change admin-only settings such as per_user_config. */
/* FIXME return value only tells whether file pointer valid */
int read_config_file(FILE *fp, int file_type)
{
  char buf[PATH_MAX];
  char *parameter, *param_begin, *param_end, *value, *value_end;

  DEBUGMSG(debug_fileops, "Entering read_config_file()\n");

  /* get out if file pointer invalid: */
  if(!fp)
  {
    DEBUGMSG(debug_fileops, "config file pointer invalid!\n");
    DEBUGMSG(debug_fileops, "Leaving read_config_file()\n");

    fprintf(stderr, "config file pointer invalid!\n");
    return 0;
  }

  /* make sure we start at beginning: */
  rewind(fp);

  /* read in a line at a time: */
  while (fgets (buf, PATH_MAX, fp))
  { 
    /* "parameter" and "value" will contain the non-whitespace chars */
    /* before and after the '=' sign, respectively.  e.g.:           */
    /*                                                               */
    /* fullscreen = 0;                                               */
    /* parameter is "fullscreen"                                     */
    /* value is '0'                                                  */
    /*                                                               */

    /* ignore comment lines */
    if ((buf[0] == ';') || (buf[0] == '#'))
    {
      continue;
    }
 
    /* First find parameter string and make a copy: */
    /* start at beginning: */
    param_begin = buf;
    /* skip leading whitespace */
    while (isspace(*param_begin))
    {
      ++param_begin;
    }

    /* If this was a blank line, then we don't have to process any more */
    if (param_begin-buf >= strlen(buf))
      continue;

    /* now go from here to end of string, stopping at either */
    /* whitespace or '=':   */
    param_end = param_begin;
    while (!isspace(*param_end)
         && ('=' != (*param_end)))
    {
      ++param_end;
    }

    /* copy chars from start of non-whitespace up to '=': */
    //parameter = strndup(param_begin, (param_end - param_begin));

    /* Next three lines do same as strndup(), which may not be available: */
    parameter = malloc((sizeof(char) * (param_end - param_begin)) + 1);
    strncpy(parameter, param_begin, (param_end - param_begin));
    parameter[param_end - param_begin] = '\0';
 
   /* Now get value string: */
    /* set value to first '=' in line: */
    value = strchr(buf, '=');

    if (!value || (value == buf))
    {
      free(parameter);
      continue;
    }

    /* move past '=' sign: */
    ++value;

    /* skip leading whitespace */
    while (isspace(*value))
    { 
      ++value;
    }

    value_end = value;

    /* remove trailing whitespace or newline */
    while (!isspace(*value_end)
         && (0x0a != (*value_end))
         && (*value_end))
    {
      ++value_end;
    }
    /* terminate string here: */
    *value_end = 0;

    DEBUGMSG(debug_fileops, "parameter = '%s'\t, length = %zu\n", parameter, strlen(parameter));
    DEBUGMSG(debug_fileops, "value = '%s'\t, length = %zu\t, atoi() = %d\t, atof() = %.2f\n", value, strlen(value), atoi(value), atof(value));
    
    /* Now ready to handle each name/value pair! */
    
    /* Set general game_options struct (see tuxmath.h): */ 
//    if(0 == strcasecmp(parameter, "per_user_config"))
//    {
//      /* Only let administrator change this setting */
//      if (file_type == GLOBAL_CONFIG_FILE) 
//      {
//        int v = str_to_bool(value);
//        if (v != -1)
//          Opts_SetGlobalOpt(PER_USER_CONFIG, v);
//      }
//    }
//                                 
//    else if(0 == strcasecmp(parameter, "homedir"))
//    {
//      /* Only let administrator change this setting */
//      if (file_type == GLOBAL_CONFIG_FILE && user_data_dir == NULL)
//      {
//        /* Check to see whether the specified homedir exists */
//        dir = opendir(value);
//        if (dir == NULL)
//          fprintf(stderr,"homedir: %s is not a directory, or it could not be read\n", value);
//        else {
//          set_user_data_dir(value);  /* copy the homedir setting */
//          closedir(dir);
//        }
//      }
//    }
//
//    else if(0 == strcasecmp(parameter, "use_sound"))
//    {
//      int v = str_to_bool(value);
//      if (v != -1)
//        Opts_SetGlobalOpt(USE_SOUND, v);
//    }
//    else if(0 == strcasecmp(parameter, "menu_sound"))
//    {
//      int v = str_to_bool(value);
//      if (v != -1)
//        Opts_SetGlobalOpt(MENU_SOUND, v);
//    }
//
//    else if(0 == strcasecmp(parameter, "menu_music"))
//    {
//      int v = str_to_bool(value);
//      if (v != -1)
//        Opts_SetGlobalOpt(MENU_MUSIC, v);
//    }
//
//    else if(0 == strcasecmp(parameter, "fullscreen"))
//    {
//      int v = str_to_bool(value);
//      if (v != -1)
//        Opts_SetGlobalOpt(FULLSCREEN, v);
//    }
    //TODO herd these per-game options into their own "domain" as well
    if(0 == strcasecmp(parameter, "use_bkgd"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetUseBkgd(v);
    }

    else if(0 == strcasecmp(parameter, "demo_mode"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetDemoMode(v);
    }

    else if(0 == strcasecmp(parameter, "oper_override"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetOperOverride(v);
    }

    else if(0 == strcasecmp(parameter, "use_keypad"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetGlobalOpt(USE_KEYPAD, v);
    }

    else if(0 == strcasecmp(parameter, "allow_pause"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetAllowPause(v);
    }

    else if(0 == strcasecmp(parameter, "use_igloos"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetGlobalOpt(USE_IGLOOS, v);
    }

    else if(0 == strcasecmp(parameter, "bonus_comet_interval"))
    {
      Opts_SetBonusCometInterval(atoi(value));
    }

    else if(0 == strcasecmp(parameter, "bonus_speed_ratio"))
    {
      Opts_SetBonusSpeedRatio(atof(value));
    }

    else if(0 == strcasecmp(parameter, "save_summary"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetSaveSummary(v);
    }

    else if(0 == strcasecmp(parameter, "speed"))
    {
      Opts_SetSpeed(atof(value));
    }

    else if(0 == strcasecmp(parameter, "use_feedback"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetUseFeedback(v);
    }

    else if(0 == strcasecmp(parameter, "danger_level"))
    {
      Opts_SetDangerLevel(atof(value));
    }

    else if(0 == strcasecmp(parameter, "danger_level_speedup"))
    {
      Opts_SetDangerLevelSpeedup(atof(value));
    }

    else if(0 == strcasecmp(parameter, "danger_level_max"))
    {
      Opts_SetDangerLevelMax(atof(value));
    }

    else if(0 == strcasecmp(parameter, "city_explode_handicap"))
    {
      Opts_SetCityExplHandicap(atof(value));
    }

    else if(0 == strcasecmp(parameter, "allow_speedup"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetAllowSpeedup(v);
    }

    else if(0 == strcasecmp(parameter, "speedup_factor"))
    {
      Opts_SetSpeedupFactor(atof(value));
    }

    else if(0 == strcasecmp(parameter, "max_speed"))
    {
      Opts_SetMaxSpeed(atof(value));
    }

    else if(0 == strcasecmp(parameter, "slow_after_wrong"))
    {
      int v = str_to_bool(value);
      if (v != -1)
        Opts_SetSlowAfterWrong(v);
    }

    else if(0 == strcasecmp(parameter, "starting_comets"))
    {
      Opts_SetStartingComets(atoi(value));
    }          

    else if(0 == strcasecmp(parameter, "extra_comets_per_wave"))
    {
      Opts_SetExtraCometsPerWave(atoi(value));
    }

    else if(0 == strcasecmp(parameter, "max_comets"))
    {
      Opts_SetMaxComets(atoi(value));
    }
    
    else if (0 == strcasecmp(parameter, "keep_score"))
    {
      Opts_SetKeepScore(atoi(value) );
    }

    else //we're going to delegate the setting of options to their subsystems
    {
      int ival = str_to_bool(value); //see if it's a valid bool
      if (ival == -1) //guess not, must be an int
        ival = atoi(value);
      if (!parse_option(parameter, ival, file_type) )
        printf("Sorry, I couldn't set %s\n", parameter);
//        
//      if (file_type != GLOBAL_CONFIG_FILE)
//        MC_SetOp(parameter, ival); 
//      else
//      {
//        if(0 != strcasecmp(parameter, "homedir"))
//        {
//          Opts_SetGlobalOp(parameter, ival);
//        }
//        else //set homedir
//        {
//          if (user_data_dir == NULL)
//          {
//            /* Check to see whether the specified homedir exists */
//            dir = opendir(value);
//            if (dir == NULL)
//              fprintf(stderr,"homedir: %s is not a directory, or it could not be read\n", value);
//            else {
//              set_user_data_dir(value);  /* copy the homedir setting */
//              closedir(dir);
//            }
//          }
//        }
//      }
    }
    free(parameter);
  }
  //handle min > max by disallowing operation
    DEBUGMSG(debug_fileops, "After file read in:\n");
  DEBUGCODE(debug_fileops)
    write_config_file(stdout, 0);
  DEBUGMSG(debug_fileops, "Leaving read_config_file()\n");

  return 1;
}

/* determine which option class a name belongs to, and set it */
/* accordingly. Returns 1 on success, 0 on failure            */
static int parse_option(const char* name, int val, int file_type)
{
  int index = -1;
  
  
  if ((index = Opts_MapTextToIndex(name)) != -1) //is it a global opt?
  {
    if (file_type == GLOBAL_CONFIG_FILE)
      Opts_SetGlobalOpt(index, val);
  }
  else //no? oh well.
  {
    return 0;
  }
  
  return 1;
}


int write_user_config_file(void)
{
  char opt_path[PATH_MAX];
  FILE* fp;

  if (!find_tuxmath_dir())
  {
    fprintf(stderr, "\nCould not find or create tuxmath dir\n");
    return 0;
  }

  /* find $HOME and add rest of path to config file: */
  get_user_data_dir_with_subdir(opt_path);
  strcat(opt_path, OPTIONS_FILENAME);

  DEBUGMSG(debug_fileops, "In write_user_config_file() full path to config file is: = %s\n", opt_path);

  /* save settings: */
  fp = fopen(opt_path, "w");
  if (fp)
  {
    write_config_file(fp, 1);
    fclose(fp);
    fp = NULL;
    return 1;
  }
  else
    return 0;
}



/* this function writes the settings for all game options to a */
/* human-readable file.                                        */
int write_config_file(FILE *fp, int verbose)
{
  int i, vcommentsprimed = 0;
  DEBUGMSG(debug_fileops, "Leaving write_config_file()\n");

  return 1;
}

/* Checks to see if user's .tuxmath directory exists and, if not, tries  */
/* to create it. Returns 1 if .tuxmath dir found or successfully created */
static int find_tuxmath_dir(void)
{
  char opt_path[PATH_MAX];
  DIR* dir_ptr;

  /* find $HOME */
  get_user_data_dir_with_subdir(opt_path);

  DEBUGMSG(debug_fileops, "In find_tuxmath_dir() tuxmath dir is: = %s\n", opt_path);

  /* find out if directory exists - if not, create it: */
  dir_ptr = opendir(opt_path);
  if (dir_ptr)  /* don't leave DIR* open if it was already there */
  {
    DEBUGMSG(debug_fileops, "In find_tuxmath_dir() tuxmath dir opened OK\n");

    closedir(dir_ptr);
    return 1;
  }
  else /* need to create tuxmath config directory: */
  {
    FILE* fp;
    int status;

    if (!add_subdir)
      return 0;      // fail if the user specified a directory, but it doesn't exist

    /* if user's home has a _file_ named .tuxmath (as from previous version */
    /* of program), need to get rid of it or directory creation will fail:  */
    fp = fopen(opt_path, "r");
    if (fp)
    {
      DEBUGMSG(debug_fileops, "In find_tuxmath_dir() - removing old .tuxmath file\n");

      fclose(fp);
      remove(opt_path);
    }

    DEBUGMSG(debug_fileops, "In find_tuxmath_dir() - trying to create .tuxmath dir\n");

    //status = mkdir(opt_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

#ifndef BUILD_MINGW32
    status = mkdir(opt_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
    status = mkdir(opt_path);
#endif

    DEBUGMSG(debug_fileops, "In find_tuxmath_dir() - mkdir returned: %d\n", status);

    /* mkdir () returns 0 if successful */
    if (0 == status)
    {
      fprintf(stderr, "\nfind_tuxmath_dir() - $HOME" OPTIONS_SUBDIR " created\n");
      return 1;
    }
    else
    {
      fprintf(stderr, "\nfind_tuxmath_dir() - mkdir failed\n");
      return 0;
    }
  }
}



/* A utility function to read lines from a textfile.  Upon exit, it */
/* returns the # of lines successfully read, and sets the pointer   */
/* array so that (*lines)[i] is a pointer to the text on the ith    */
/* line.  Note this function also cleans up trailing whitespace,    */
/* and skips blank lines.                                           */
/* On entry, *lines must be NULL, as a sign that any previously     */
/* allocated memory has been freed.                                 */
static int read_lines_from_file(FILE *fp,char ***lines)
{
  char *fgets_return_val;
  char name_buf[NAME_BUF_SIZE];
  int n_entries;
  int length;

  n_entries = 0;
  if(*lines != NULL) {
    printf("Error: lines buffer was not NULL upon entry");
    exit(EXIT_FAILURE);
  }

  fgets_return_val = fgets(name_buf,NAME_BUF_SIZE,fp);
  while (fgets_return_val != NULL) {
    // Strip terminal whitespace and \r
    length = strlen(name_buf);
    while (length>0 && (name_buf[length - 1] == '\r' || name_buf[length - 1] == '\n'|| name_buf[length-1] == ' ' || name_buf[length-1] == '\t')) {
      name_buf[length - 1] = '\0';
      length--;
    }
    if (length == 0) {
      // If we get to a blank line, skip over it
      fgets_return_val = fgets(name_buf,NAME_BUF_SIZE,fp);
      continue;
    }
    n_entries++;
    *lines = (char**) realloc(*lines,n_entries*sizeof(char*));
    if (*lines == NULL) {
      // Memory allocation error
      printf("Error #1 allocating memory in read_lines_from_file\n");
      exit(EXIT_FAILURE);
    }
    // Copy the cleaned-up line to the list
    (*lines)[n_entries-1] = strdup(name_buf);
    if ((*lines)[n_entries-1] == NULL) {
      // Memory allocation error
      printf("Error #2 allocating memory in read_lines_from_file\n");
      exit(EXIT_FAILURE);
    }
    // Read the next line
    fgets_return_val = fgets(name_buf,NAME_BUF_SIZE,fp);
  }
  return n_entries;
}

/* A utility function to go up one level in a directory hierarchy */
static void dirname_up(char *dirname)
{
  int len;

  len = strlen(dirname);
  // Pass over all trailing "/"
  while (len > 0 && dirname[len-1] == '/')
    len--;

  // Now pass over all non-"/" characters at the end
  while (len > 0 && dirname[len-1] != '/')
    len--;
  
  // Terminate the string after that next-to-last "/"
  dirname[len] = '\0';
}

/* Identify user by the directory name. We don't want to use the */
/* whole path, just the name of the last subdirectory. */
static char* get_user_name(void)
{
  char filepath2[PATH_MAX];

  get_user_data_dir_with_subdir(filepath2);
  return get_file_name(filepath2);
}

/* Extract the last "field" in a full pathname */
static char* get_file_name(char *fullpath)
{
  char *file_name;

  file_name = &fullpath[strlen(fullpath)-1];
  /* Chop off trailing "/" */
  while (file_name > &fullpath[0] && *file_name == '/') {
    *file_name = '\0';
    file_name--;
  }
  /* Back up to the next "/" */
  while (file_name > &fullpath[0] && *file_name != '/')
    file_name--;

  return ++file_name;
}


/* Allows use of "true", "YES", T, etc. in text file for boolean values. */
/* Return value of -1 means value string is not recognized.              */
/* Now reject non-"boolish" ints to prevent int/bool ambiguity           */
static int str_to_bool(const char* val)
{
  char* ptr;

  /* Check for recognized boolean strings: */
  if ((0 == strcasecmp(val, "true"))
    ||(0 == strcasecmp(val, "t"))
    ||(0 == strcasecmp(val, "yes"))
    ||(0 == strcasecmp(val, "y"))
    ||(0 == strcasecmp(val, "1"))
    ||(0 == strcasecmp(val, "on")))
  {
    return 1;
  }

  if ((0 == strcasecmp(val, "false"))
    ||(0 == strcasecmp(val, "f"))
    ||(0 == strcasecmp(val, "no"))
    ||(0 == strcasecmp(val, "n"))
    ||(0 == strcasecmp(val, "0"))
  ||(0 == strcasecmp(val, "off")))
  {
    return 0;
  }  

  return -1;
  
  /* Return -1 if any chars are non-digits: */
  ptr = (char*)val;
  while (*ptr)
  {
    if (!isdigit(*ptr))
      return -1;
    ptr++;
  }

  /* If we get to here, val should be an integer. */
  
  if (atoi(val))
    return 1;
  else
    return 0;
}







