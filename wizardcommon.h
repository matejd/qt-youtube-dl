#ifndef WIZARDCOMMON_H
#define WIZARDCOMMON_H

const int PAGE_ID_INTRO   = 0;
const int PAGE_ID_FORMATS = 1;
const int PAGE_ID_FOLDER  = 2;

const char* const SETTING_BEST_FORMAT = "wizardIntroPage/bestFormat";
const char* const SETTING_SAME_FOLDER = "wizardIntroPage/sameFolder";
const char* const SETTING_DEST_FOLDER = "wizardFolderPage/destFolder";

const char* const FIELD_URL = "url";
const char* const FIELD_URL_REGISTER = "url*"; // Used when registering this field as *required*.
const char* const FIELD_FOLDER = "folder";
const char* const FIELD_FORMAT = "format";
const char* const FIELD_SAME_FOLDER = "sameFolder";

#endif
