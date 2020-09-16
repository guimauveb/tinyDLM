#include "../include/dialogs.hpp"

const std::string liteDL_label  = "tinyDLM v1.0";
const std::string workInProg    = "Work in progress";

const std::string tinyDLMWelcome = " tinyDLM - A tiny downloader manager ";
const std::string tinyDLMVer     = " v1.0 ";
const std::string tinyHelp       = " Press 'a' to add a new download. ";

/* Display keys and their associated functions at the bottom of the window */
//const std::string msgKeyInfoP  = " a: add  p: pause  c: clear inactive  k: kill all " ;
//const std::string msgKeyInfoR  = " a: add  r: resume c: clear inactive  k: kill all " ;
const std::string msgHelp               = " Press 'h' to show help ";
const std::string msgHelpMenu           = " Help Menu ";
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
const std::string msgHelpCloseWin       = " Press 'Return' key to hide ";

const std::string msgClose  = " x: close ";
const std::string msgPause  = " p: pause ";
const std::string msgResume = " r: resume ";
const std::string msgKill   = " k: kill ";

const std::string addNewLabel       = "Add a new download";
const std::string addURL            = " URL ";
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

/* TODO - make it selectionable by the user */
/* TODO - move to the designated function */
const std::string dlFolder = "downloads/";

const std::string ERR_WIN_BEG = "Unable to allocate memory for the '";
const std::string ERR_MAIN_WIN_0 = "download list";
const std::string ERR_MAIN_WIN_1 = "key actions";
const std::string ERR_MAIN_WIN_2 = "download status";
const std::string ERR_ADD_DL_WIN = "add a download";
const std::string ERR_DETAILS_WIN = "details";
const std::string ERR_PROGRESS_WIN = "progress";
const std::string ERR_WIN_END = "' window. Exiting.";
