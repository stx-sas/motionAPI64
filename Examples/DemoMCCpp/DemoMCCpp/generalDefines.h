#pragma once

/*! \enum KMResponse 
*  \brief a response code for all KMApi functions
*/
typedef enum {
	KMRet_OK = 0,           ///<  All is OK: returned when commands succeeds
	KMRet_CONNECTED,        ///<  connected to Soft MC
	KMRet_ASYNC,            ///<  Async message received
	// ----------------------   error messages -------------------------------------------
	KMErr_GENERAL,          ///<  General error use get last error for details
	KMErr_TIMEOUT,          ///<  Timeout waiting for response / connection
	KMErr_BAD_KMApiID,      ///<  Invalid KMApi ID received (only in C function)
	KMErr_FAIL_EXEC,        ///<  Error executing command 
	KMErr_UNKNOWN_MESSAGE,  ///<  Unknown message received
	KMErr_FAIL_CREATE_FILE, ///<  Error creating New File when loading file
	KMErr_FAIL_OPEN_FILE,   ///<  Cannot Open File either is locked or used by other process
	KMErr_FILE_NOT_FOUND,   ///<  File not found check file name again
	KMErr_CANNOT_CONNECT,   ///<  Cannot connect - connection refused or host not found
	KMErr_INVALID_FORMAT,   ///<  Invalid format - Frame CRC is wrong or invalid Frame
	KMErr_BUFFER_TOO_SMALL, ///<  The provided buffer is too small for response
	KMErr_FOLDER_NOT_FOUND, ///<  The folder was not found (cannot copy file to)
	KMErr_TRUNCATED_FRAME,  ///<  Frame is truncated (will wait for rest of the frame until timeout) therefore you will get Timeout instead of truncated
	KMErr_MC_ERR,           ///<  MC returned an error use GetLastError to get string
	KMErr_RETRIEVE          ///<  cannot retrieve error message (returned from GetLastError)
} KMResponse;
/*! \enum KMResponse
*  \brief a response code for all KMApi functions
*/


typedef enum {
	KMTO_Connect,  ///<  Connect timeout type.
	KMTO_Exec      ///<  Execute command timeout type.
} KMTimeout; 


//! ERR_ON_CLOSE:( = 100 ) special error event sent to the callback when connection is dropped.
#define ERR_ON_CLOSE 100


static char* KMResponsesNames[] = {
	"KMRet_OK",                      
	"KMRet_CONNECTED",               
	"KMRet_ASYNC",
	// ----------------------   error messages -------------------------------------------
	"KMErr_GENERAL",
	"KMErr_TIMEOUT", "KMErr_BAD_KMApiID", "KMErr_FAIL_EXEC", "KMErr_UNKNOWN_MESSAGE",
	"KMErr_FAIL_CREATE_FILE", "KMErr_FAIL_OPEN_FILE", "KMErr_FILE_NOT_FOUND", "KMErr_CANNOT_CONNECT",
	"KMErr_INVALID_FORMAT", "KMErr_BUFFER_TOO_SMALL", "KMErr_FOLDER_NOT_FOUND",
	"KMErr_TRUNCATED_FRAME", "KMErr_MC_ERR", "KMErr_RETRIEVE"
};

enum FrameTypeEnum
{
	INVALID_FRAME = 0, ///<  Invalid frame
	ACKNOWLEDGE_FRAME, ///< sent from mc to acknowledge frame is in process (internal)
	DATA_FRAME, ///<  a frame of date.(internal)
	BINARY_FRAME, ///<  binary data frame (internal).
	CONTROL_FRAME, // control frame (internal)
	PROMPT_FRAME, ///< a prompt is sent at the end of every MC response (internal). 
	ERROR_FRAME, ///< sent when last command result an error with error details (internal).
	//  asynchronous messages:
	ASYNC_FRAME, ///<  this frame is sent when an asynchronous error frame is received from SoftMC. You should expect it on your callback function. 
	ASYNC2_FRAME, ///< same as ASYNC_FRAME but contains data such a program log message or status. You should expect it on your callback function.
	ASYNC_OTHER,  //< dummy.
	AUTHENTICATE_FRAME,  ///< sent during authentication process 
	STATE_FRAME  ///< SoftMC "state change" message
};

/// @cond TEST  
static UINT8 FrameTypeBytes[] = {
	0,        ///< INVALID_FRAME
	'A',      ///< ACKNOWLEDGE_FRAME
	'D',      ///< DATA_FRAME
	'B',      ///< BINARY_FRAME
	'C',      ///< CONTROL_FRAME 
	'P',      ///< PROMPT_FRAME
	'E',      ///< ERROR_FRAME
	(UINT8)0xC5,     ///< ASYNC_FRAME- Async error frame 
	(UINT8)0xC4,     ///< ASYNC2_FRAME  - async data frame
	(UINT8)0xC4,     ///< ASYNC_OTHER (dummy)
	'Q',       ///< AUTHENTICATE_FRAME
	(UINT8)('S' | 0x80)      ///< STATE_frame
};


/// @endcond
