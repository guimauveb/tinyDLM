#include "../include/dialogs.hxx"

const std::string tinyDLM_label  = "tinyDLM";
const std::string workInProg    = "Work in progress";

const std::string tinyDLMWelcome = " tinyDLM - A tiny download manager ";
const std::string tinyDLMVer     = " version 1.1 ";
const std::string madeBy         = " by guimauve ";
const std::string tinyHelp       = " Press 'a' to add a new download. ";

/* Display keys and their associated functions at the bottom of the window */
const std::string msgNewUserOk  = "Thank you for using tinyDLM. Configuration files have been written\
                                   to ~/.tinyDLM/. You can change the settings by pressing 's'. Enjoy!";

const std::string msgNewUserErr = "There was an error when initializing tinyDLM configuration files.\
                                   Check syslog using the following command for more informations:\
                                   \nlog show --predicate \"process == 'tinyDLM'\"";

const std::string msgHelp               = " Press 'h' to show help ";
const std::string msgHelpMenu           = " Help Menu ";
const std::string msgHelpSettings       = " s           :Settings ";
const std::string msgHelpAdd            = " a           :Add a new download ";
const std::string msgHelpArrowKeys      = " Arrow keys  :Navigate the downloads menu ";
const std::string msgHelpReturn         = " Enter       :Show selected download details ";
const std::string msgHelpPause          = " p           :Pause selected download ";
const std::string msgHelpPauseAll       = " P           :Pause all downloads ";
const std::string msgHelpResume         = " r           :Resume selected download ";
const std::string msgHelpResumeAll      = " R           :Resume all downloads ";
const std::string msgHelpClear          = " c           :Clear inactive downloads ";
const std::string msgHelpKill           = " k           :Stop and delete selected download ";
const std::string msgHelpKillAll        = " K           :Stop and delete all downloads ";
const std::string msgHelpExit           = " F1          :Exit tinyDLM ";
const std::string msgHelpCloseWin       = " Press 'Return' to hide ";

const std::string msgClose  = " x: close ";
const std::string msgPause  = " p: pause ";
const std::string msgResume = " r: resume ";
const std::string msgKill   = " k: kill ";

const std::string msgSettings = " Settings ";

const std::string settingsLabel     = "Settings";
const std::string addNewLabel       = "Add a new download";
const std::string addURL            = " URL(s) ";
const std::string addSaveAs         = " Save as ";
const std::string msgStart          = " enter: start " ;
const std::string msgScheduleCtrl   = " ctrl^l: schedule ";
const std::string msgCloseCtrl      = " ctrl^x: close ";

const std::string msgInvalidURL     = " Enter a valid URL ";
const std::string msgInvalidFilename = " Enter a valid filename ";

//const std::string statusLabel = " active - 0 | download speed - 0 mo/s ";
const std::string hundredPer    = "100 %";
const std::string labelName     = " NAME ";
//const std::string labelURL = "URL";
const std::string labelProgress = "PROGRESS";
const std::string labelSpeed    = "SPEED";
const std::string labelETA      = "ETA";
const std::string labelStatus   = "STATUS";

const std::string statusStrDl = "Downloading";
const std::string statusStrPd = "  Paused   ";
const std::string statusStrCd = " Complete  ";
const std::string statusStrEr = "   Error   ";
const std::string statusStrPe = "  Pending  ";
const std::string statusStrUn = "  Unknown  ";

const std::string ERR_WIN_BEG       = "Unable to allocate memory for the '";
const std::string ERR_MAIN_WIN_0    = "download list";
const std::string ERR_MAIN_WIN_1    = "key actions";
const std::string ERR_MAIN_WIN_2    = "download status";
const std::string ERR_ADD_DL_WIN    = "add a download";
const std::string ERR_DETAILS_WIN   = "details";
const std::string ERR_PROGRESS_WIN  = "progress";
const std::string ERR_WIN_END       = "' window. Exiting.";
