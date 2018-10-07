#pragma once
/*! \file */
#include "generalDefines.h"

#ifdef _USRDLL
#define KMAPI_EXPORT __declspec(dllexport)  
#include "MCSocket.h"
#include "MCFileUtils.h"
class MCUtils;
class NetDetector;
class MCSocketWrapper;
#else
#define KMAPI_EXPORT __declspec(dllimport)  
#endif // USRDLL

typedef void(__stdcall MCCallback)(int, LPCSTR);

/*! \struct MCDevice
*  \brief MC device details
* This is a struct returned from GetDeviceList where each struct contains Soft MC details, such as name, ip, serialNumber
\param  char name[13]       : name of a specific device
\param char ip[17]          : MCs IP
\param char serialNumber[24]:  The serial number (unique id)
\param int isSomoneConnected:  1 if someone is connected to MC
\param int dIP              :  old, deprecated
*/
class KMAPI_EXPORT MCDevice {
public:
	char name[13];  ///< The name of the device 13 chars long
	char ip[17];    //<  its ip in a string 17 char long
	char serialNumber[24]; ///< serial number - string 24 chars
	int isSomoneConnected; ///< 1 if true
	int dIP ; ///< old, deprecated
};

/*! \class KMApi KMApi.h 
*  \brief The KMApi class for connecting and communicating with MC.\n
*  Use the KMApi class inside any other program.
*  KMApi supports a direct access via the KMAPI class. KMApi Dll, also, contains a set of wrapper functions.
*/
class KMAPI_EXPORT KMApi {
public:
	/*! \fn void SetCallback( void* callback )
	*  \brief Sets a pointer to a callback function.
	*  Assign the callback function to receive async messages such as status and error notifications\n
	*  \param callback: A pointer to a callback function:
	*  Callback function should be in the form of \n\code void callback(int, LPCSTR) \endcode
	* 
	* Callback definition in C++
	\code
	// My callback function definition
	void CALLBACK MyCallback(int frameType, LPCSTR csMessage)
	{
		CString msg;
		switch (frameType) {
			case ERROR_FRAME:
			case ASYNC_FRAME:
			case ASYNC2_FRAME:
			case ASYNC_OTHER:
			case STATE_FRAME:
				msg.Format("Async message Type: %d, Message: '%s'",frameType,csMessage);
			break;

			case ERR_ON_CLOSE: // event sent to the client from Dll saying connection is closed
				// Handle close event and free object
				MCFreeKMApi(pKmapi);
				pKmapi = NULL;
			break;

			default:
				msg.Format(_T("Lost message: %d %s"),frameType,csMessage);
			break;

		}
		OutputDebugString(msg);
	}
	\endcode
	 You can assign the callback like this:
	\code
	SetCallback(MyCallback);
	\endcode
	Below is a full asynchronous example:
	\code
	KMApi* pKmapi = KMNewKMAPI();
	if (pKmapi != NULL)
	{
		// 10 seconds timeout for connect
		pKmapi->SetTimeout(KMTO_Connect, 10000);
		// 15s timeout for command execution
		pKmapi->SetTimeout(KMTO_Exec, 15000);

		// set the callback function
		m_pKmapi->SetCallback(MyCallback);

		// connect to IP 10.0.0.5 port 4001
		KMResponse iError = pKmapi->Connect("10.0.0.5", 4001);
		if (iError == KMRet_OK) // if connected
		{
		char sOutBuffer[2048];
		// execute a command to start script
			if ((iError = pKmapi->ExecCommand("LOAD myscript.prg", sOutBuffer, 2048)) != KMRet_OK)
			{
				ReportError(iError);
			}
		}
	
		//...
	}
	\endcode
	*/
	void SetCallback( void* callback );

	 /*! \fn KMResponse Connect(LPSTR ip, int port)
	 *  \brief Connect to a specific IP and port.\n
	 * The connect function tries to connect a specific IP:port.\n 
	 * It may return Timeout or connection refused.
	 * You can always check for last error and get detailed description from the socket.
	 *  \param ip: The IP string to connect to.
	 *  \param port: A port number.
	 *  \return ::KMResponse Return code.*/
	 KMResponse Connect(LPSTR ip, int port);

	 /*! \fn KMResponse ExecCommand( LPCSTR command, LPSTR response, int rspLength )
	 *  \brief Executes a command and waits for a response.\n
	 *  This method will execute selected command and will return a string with MCs response.\n
	 *  The function will return a timeout error if no response was receive before defined timeout.
	 *  See SetTimeout for details.
	 *  Calling this function will force KMApi to wait for acknowledge and prompt frames before it returns\n
	 
	 *  \param command: A utf8 string with command to run on Soft MC.
	 *  \param response: A buffer to copy response to.
	 *  \param rspLength: The length of the response buffer.
	 *  \return ::KMResponse: The return code.\n
	 *  May be an error from MC or any other error such a timeout, buffer too small\n
	 *  In all cases you want to call \ref GetLastError to understand what went wrong.\n\n
* \b Example: Run command in soft MC and present response.\n
	 * \code

KMApi* pKmapi = KMNewKMAPI();
if (pKmapi != NULL)
{
	// 10 seconds timeout for connect
	pKmapi->SetTimeout(KMTO_Connect, 10000); 
	// 15s timeout for command execution
	pKmapi->SetTimeout(KMTO_Exec, 15000); 

	// connect to IP 10.0.0.5 port 4001
	KMResponse iError = pKmapi->Connect("10.0.0.5", 4001);  
	if (iError == KMRet_OK) // if connected
	{
		char sOutBuffer[2048];
		// execute a command: sends '?ver' to Soft MC and wait for a response
		if ( pKmapi->ExecCommand("?ver", sOutBuffer, 2048) == KMRet_OK)
		{
			ShowMessage(sOutBuffer,"Version information");
		}
	}
	// Don't forget to free object
	MCFreeKMApi(pKmapi);
}
* \endcode */
	 KMResponse ExecCommand( LPCSTR command, LPSTR response, int rspLength );
	 
	 /*! \fn KMResponse InnerExecCommand( LPCSTR command, LPSTR response, int rspLength, bool isAuth )
	 *  \brief Same as ExecCommand but with  extra parameter (isAuth ) is for authentication purpose.\n
	 *  Do not use this method unless you want to do a manual authentication.\n
	 *  For authentication use \ref Authenticate()
	 *  For command execution use \ref ExecCommand\n
	 *  The function will return a timeout error if no response was received before defined timeout.\n
	 *  See KMApi::SetTimeout for details.\n
	 *  \param command: A utf8 string with command to run on Soft MC.
	 *  \param response: A buffer to copy response to.
	 *  \param rspLength: The length of the response buffer.
	 *  \param isAuth: Is this command an authentication frame.
	 *  \return ::KMResponse Return code.*/
	 KMResponse InnerExecCommand( LPCSTR command, LPSTR response, int rspLength, bool isAuth );
	 
	 /*! \fn KMResponse SendCommand(LPCSTR command, bool isAuth = false)
	 *  \brief Sends a command but doesn't to waits for a response.
	 *  The function send a command to Soft MC and returns.
	 *  \param command A utf8 string with command to run on Soft MC.
	 *  \param isAuth [Optional] Is this command an authentication frame (by default is false).
	 *  \return ::KMResponse Return code.*/
	 KMResponse SendCommand(LPCSTR command, bool isAuth = false);

	 /*! \fn KMResponse DownloadFile(LPCSTR sFilePath, LPCSTR sFileName)
	 *  \brief Downloads a file from Soft MC to a specific folder on PC.
	 *  \param sFilePath An existing folder name to download to.
	 *  \param sFileName A string with an existing filename to download from Soft MC.
	 *  \return ::KMResponse Return code.*/
	 KMResponse DownloadFile(LPCSTR sFilePath, LPCSTR sFileName);

	 /*! \fn KMResponse UploadFile(LPCSTR Filename)
	 *  \brief Uploads a file to Soft MC.
	 *  \param Filename: A full path (a utf8 string) of the file to upload to Soft MC.
	 *  \return ::KMResponse Return code.*/
	 KMResponse UploadFile(LPCSTR Filename);

	 /*! \fn  KMResponse GetLastError( out int& errorCode,out  LPSTR sErrorMessage,out  int msgLength)
	 *  \brief Gets last error from socket or from soft MC.
	 *  Gets the last error description.\n Use this function after getting an error code from KMApi commands.\n
	 *  The method may return error text from soft MC or an error description from socket (last socket error)\n
	 *  \param errorCode: (Out value) the error code 
	 *  \param sErrorMessage: (Out value) a buffer to fill error text from socket or from soft MC
	 *  \param msgLength: buffer size
	 *  \return ::KMResponse Return code. Buffer Too Small or OK\n
	 * \b Example Getting last error
	\code
	if (pKmapi != NULL)
	{
			char sOutBuffer[2048];
			// execute a command: sends '?ver' to Soft MC and wait for a response
			if (( iRetCode = pKmapi->ExecCommand("?ver", sOutBuffer, 2048)) == KMRet_OK)
			{
				ShowMessage(sOutBuffer,"Version information");
			}else
			{
			  char sErrorMessage[10000];
			  iRetCode = pKmapi->GetLastError(iErrCode,sErrorMessage, 10000);
			  if (iRetCode == KMRet_OK)
				ShowError(iErrCode,sErrorMessage); // show error message with error code (KMResponse) and a text message
			  else
			   //.. try with a bigger buffer
			}
	}
	\endcode
	 */
	 KMResponse GetLastError(/* out*/ int& errorCode,/* out*/  LPSTR sErrorMessage,  int msgLength);

	 /*! \fn void SetTimeout(KMTimeout type, DWORD timeout)
	 *  \brief Sets the timeout to connect and for the execute commands.\n
	 *   Set timeout before connecting otherwise it'll use default timeouts
	 *  \param KMTimeout : The type of timeout you wish to set (see KMResponse for list of types)
	 *  \param timeout: timeout in milliseconds 
	 *  \return void\n
\b Example: Setting and using timeout.\n
\Note: This example is for explanation purpose only.\n
In general you want to call \ref GetLastError no matter what error it was.\n
\code
	...
	if (pKmapi != NULL)
	{
		// 10 seconds timeout for connection
		pKmapi->SetTimeout(KMTO_Connect, 10000);

		// 15s timeout for command execution
		pKmapi->SetTimeout(KMTO_Exec, 15000);

		// connect to IP 10.0.0.5 port 4001
		KMResponse iError = pKmapi->Connect("10.0.0.5", 4001);
		if (iError == KMRet_OK) // if connected
		{
			char sOutBuffer[2048];
			// execute a command to start script
			if ((iError = pKmapi->ExecCommand("?sys.info", sOutBuffer, 2048)) != KMRet_OK)
			{
  			  ...
			} else 
				if (iError == KMRet_TIMOUT)
					OutputDebugString("Timeout: MC did not respond to command in 15 seconds");
		} 
		else 
			if (iError == KMRet_TIMOUT)
				OutputDebugString("Timeout: MC did not respond to connection request in 10 seconds");
	}
\endcode
	 
	 */

	 void SetTimeout(KMTimeout type, DWORD timeout);

	 /*! \fn KMResponse DetectDevices( DWORD timeout );
	 *  \brief Detects devices and waits for all responses received within a timeout.
	 *  This function will send a UDP message to all local networks and will returns after timeout.\n
	 *  It returns KMRet_OK if succeeds. You may want to call this function only when you want to refresh stored device list.\n
	 *  You may call GetDeviceList or GetDevicesDescriptor to read list.\n
	 *  See example below
	* \code
	iError = m_pKmapi->DetectDevices(200);  // wait 200 ms for all responses
	if (iError == KMRet_OK)
	{
	MCDevice* MCDevices = NULL;
	MCDevices = m_pKmapi->GetDeviceList();
	if (MCDevices != NULL)
		while ( strlen(MCDevices[j].ip) > 0) { // if ip is "" it means end of list 
		   CString csName = MCDevices[j].name;
		   CString csIPAddress = MCDevices[j].ip;
		   CString csSerialNumber = MCDevices[j].serialNumber;
		   CString csMessage; csMessage.Format("found %s, IP: %s, serial:%s",
																	MCDevices[j].name,
																	MCDevices[j].ip,
																	MCDevices[j].serialNumber);

		ShowMessage(csMessage);

	}
	* \endcode

	*  \param timeout: how long to wait for MC's responses
	*  \return ::KMResponse the return code.*/
	 KMResponse DetectDevices( DWORD timeout );
	 
	 /*! \fn MCDevice * GetDeviceList();
	 *  \brief Get a list of last detected devices.\n
	 *  Call this method after calling DetectDevices to get a list of detected device.\n
	 *  you may treat return value as an array of structs, or just move a pointer over the return buffer.\n
	 *  \return MCDevice* Returns a list of MCDevice struct.\n
	 *  List ends with MCDevice object with an empty ip (ip == "")\n
	 *  See DetectDevices for example
	 */
	 MCDevice * GetDeviceList();
	 
	 /*! \fn char* GetDevicesDescriptor( BOOL bUpdate )
	 *  \brief Get a list of devices in a CSV format.
	 *   Returns a string in a comma-separated-string\n
	 *												e.g.\n
	 *													MyMC1, 10.0.0.11, S16C-123\n
	 *		   											Mc2nd, 10.0.0.15, S16C-111\n
	 *  \param bUpdate: true to update list before returns
	 *  \return A string with all devices in a csv format.\n
	 *   The return string is in the following format: "Name , IP , Serial \n"... 
	 */
	 char* GetDevicesDescriptor( BOOL bUpdate );

	 /*! \fn KMResponse Authenticate(const char * password)
	 *  \brief Performs an authentication process.\n
	 *  If Soft MC requires authentication, you may do it using this function.\n
	 *  In order to authenticate, call this method with the right password as string.\n
	 *  \param Password: The password (utf8 string) to authenticate with MC.
	 *  \return ::KMResponse Return code.
	 *   Note: MC does not return error in case of a wrong password*/
	 KMResponse Authenticate( const char * password );

	 /*! \fn void CallCallback( int fType, LPCSTR message );
	 *  \brief Called when a new message (async Frame) is ready.
	 *  Method is called when a new async message is available.
	 *  The method will call the callback function defined in SetCallback
	 *  You do not want to call it yourself unless you want to simulate a callback to callback function.
	 *  \param fType: Frame type id (FrameTypeEnum) : The type of frame or a ERR_ON_CLOSE when connection is unexpectedly dropped.
	 *  \param message: the text message to send to callback.
	 *  \return ::KMResponse Return code.*/
	 void CallCallback( int fType, LPCSTR message );

	 /*! \fn KMApi();
	 *  \brief Constructor: INTERNAL: DO NOT USE! use * \ref KMNewKMAPI() instead.
	 * Do not use this constructor use KMNewKMAPI() to get a new KMApi object*/
	 KMApi();
	 /*! \fn ~KMApi();
	 *  \brief Destructor: INTERNAL: DO NOT USE! Use MCFreeKMApi() instead.
	 * Do not use this destructor use MCFreeKMApi( kmapi ) to free object*/
	 ~KMApi();

	 

protected:
	
private:
	char m_sIP[256];
	int    m_port;

#ifdef _USRDLL
	MCSocketWrapper* m_pSocketW = NULL;

	MCCallback* m_pCallback = NULL;
	MCUtils* m_pUtils = NULL;
	MCFileUtils* m_pFile = NULL;
	NetDetector* m_netDetector = NULL;
	BOOL m_isWaitingForQuery = FALSE;
	BOOL m_isCreated = FALSE;
	UINT32 m_TIMEOUTCommand = 5000;// default is 5 sec
	KMResponse m_lastRespCode;
	CString m_sLastErrorMessage;
	int m_lastErrorCode = 0;
#endif // USRDLL

};


/*! \fn KMApi* KMNewKMAPI()
*  \brief Creates and returns a new KMApi object.
*  Use this method to create a new KMApi object (do not create it directly)
* you may use the class directly:
* \code
KMApi* kmapiObj = KMNewKMAPI();
if ( kmapiObj != NULL)
{
	connectRet = kmapiObj->Connect( ip, port );
	...
	MCFreeKMApi(kmapiObj);
 }
* \endcode
or Like this (indirect)
* \code
KMApi* kmapiObj = KMNewKMAPI();
if ( kmapiObj != NULL)
{
	connectRet = Connect(kmapiObj, ip, port );
	...
	MCFreeKMApi(kmapiObj);
}
* \endcode
* Note: see MCFreeKMApi.
*  \return KMApi new Object.*/
extern "C"  KMAPI_EXPORT  KMApi* KMNewKMAPI();


// free KMApi object
/*! \fn MCFreeKMApi( KMApi* pKmapi );
*  \brief Releases KMApi object.
*  Use this method to release KMApi object and free memory.
*  Here is an example of creating, using and freeing kmapi object:
* \code
KMApi* kmapiObj = KMNewKMAPI();
if ( kmapiObj != NULL)
{
  //use kmapiObj...
  ...

  // release memory
  MCFreeKMApi(kmapiObj);
  // now kmapiObj is invalid.
}
* \endcode
* Note: see KMNewKMApi.
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \return void*/
extern "C" KMAPI_EXPORT void MCFreeKMApi( KMApi* pKmapi );

/*! \fn void SetCallback(KMApi* pKmapi, MCCallback * pCallback);
*  \brief Sets a pointer to a callback function.\n
*   Assign the callback function to receive async messages and other error status notifications.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param pCallback a pointer to a callback function:
*  callback function should be in the form of \c void callback(int, LPCSTR)*/
extern "C" KMAPI_EXPORT void SetCallback(KMApi* pKmapi, MCCallback * pCallback);

/*! \fn KMResponse Connect(KMApi* pKmapi, LPCSTR ip, int port)
*  \brief Connect to a specific IP and port.\n
* the connect function tries to connect a specific IP:port. If it succeeds, return value is KMRet_OK.\n
* It may return Timeout or connection refused.\n
* On error, you should always call \ref GetMCLastError and get detailed description from the socket.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param ip The IP string to connect to.
*  \param port A port number.
*  \return ::KMResponse Return code.*/
extern "C" KMAPI_EXPORT KMResponse Connect(KMApi* pKmapi, LPCSTR ip, int port);

/*! \fn KMResponse ExecCommand(KMApi* pKmapi, LPCSTR command, LPSTR response, int rspLength)
*  \brief Executes command and waits for response\n
*  This method will execute a command and will return a string with the response from MC or an error code.\n
*  the function will return a timeout error if no response was receive before defined timeout\n
*  see \c SetTimeout for details.\n
*  Calling this function will force KMApi to wait for acknowledge and prompt frames before it returns \n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param command a utf8 string with command to run on Soft MC.
*  \param response a buffer to copy response to.
*  \param rspLength The length of the response buffer.
*  \return ::KMResponse Return code.\n May be an error from MC or any other error such a timeout, buffer too small\n
*  In all cases you want to call \ref GetLastError to understand what went wrong
*/
extern "C" KMAPI_EXPORT KMResponse ExecCommand(KMApi* pKmapi, LPCSTR command, LPSTR response, int rspLength);

/*! \fn SendCommand(KMApi* pKmapi, LPCSTR command)
*  \brief Sends a command but doesn't to wait for response.\n
*  The function send a command on Soft MC and returns.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param command a utf8 string with command to run on Soft MC.
*  \return ::KMResponse Return code.*/
extern "C" KMAPI_EXPORT KMResponse SendCommand(KMApi* pKmapi, LPCSTR command);

/*! \fn KMResponse KMResponse DownloadFile( KMApi* pKmapi, LPCSTR sFilePath, LPCSTR sFileName );
*  \brief Downloads a file from Soft MC to a specific folder on the PC.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param sFilePath An existing folder name to download to
*  \param sFileName A string with an existing filename to download from Soft MC
*  \return ::KMResponse Return code.*/
extern "C" KMAPI_EXPORT KMResponse DownloadFile( KMApi* pKmapi, LPCSTR sFilePath, LPCSTR sFileName );

/*! \fn KMResponse UploadFile( KMApi* pKmapi, LPCSTR Filename );
*  \brief Uploads a file to Soft MC\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param Filename: a full path (a utf8 string) of the file to upload to Soft MC
*  \return ::KMResponse Return code.*/
extern "C" KMAPI_EXPORT KMResponse UploadFile( KMApi* pKmapi, LPCSTR Filename );

/*! \fn  KMResponse GetMCLastError( KMApi* pKmapi, int& errorCode,  LPSTR sErrorMessage,  int msgLength )
*  \brief Gets last error from socket or from soft MC\n
* Gets the last error id and text.\n
* Call this method after you receive an error code from any kmapai function\n
* The method may return error text from soft MC or an error description from socket (last socket error)\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()\n
*  \param errorCode: (Out value) The error code it may be KMRetCode or socket error type. (according to the error type)
*  \param sErrorMessage: a buffer to fill error text from socket or from soft MC
*  \param msgLength: buffer size
*  \return ::KMResponse: Return code. Buffer Too Small or OK*/
extern "C" KMAPI_EXPORT KMResponse GetMCLastError( KMApi* pKmapi,/* out*/ int& errorCode,/* out*/  LPSTR sErrorMessage, int msgLength );

/*! \fn void SetTimeout( KMTimeout type, DWORD timeout )
*  \brief Sets the timeout to connect and for the execute commands\n
*   Set timeout before connecting otherwise it'll use default timeouts\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param KMTimeout : The type of timeout you wish to set( see KMResponse for list of types )
*  \param timeout : Timeout in milliseconds
*  \return void */
extern "C" KMAPI_EXPORT void SetTimeout( KMApi* pKmapi, KMTimeout type, DWORD timeout );

/*! \fn KMResponse DetectDevices( KMApi* pKmapi, DWORD Timout )
*  \brief Detect device and wait for all to responses a specific timeout\n
*  This function will send a UDP message to all local networks\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param timeout: How long to wait for MC's responses
*  \return ::KMResponse Return code.*/
extern "C" KMAPI_EXPORT KMResponse DetectDevices( KMApi* pKmapi, DWORD Timout );


/*! \fn MCDevice * GetDeviceList( KMApi* pKmapi)
*  \brief Get a list of last detected devices\n
*  Call this method after calling DetectDevices to get a list of detected device.\n
*  You may treat return value as an array of structs, or just move a pointer over the return buffer.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \return returns a list of MCDevice struct.
*  list ends with a MCDevice object where ip == ""  */
extern "C" KMAPI_EXPORT MCDevice * GetDeviceList( KMApi* pKmapi);

/*! \fn KMResponse Authenticate( KMApi* pKmapi, const char * password )
*  \brief Performs an authentication process.\n
*  If Soft MC requires authentication, you may do it using this function.\n
*  In order to authenticate call this method with the right Password as string.\n
*  \param pKmapi: A pointer to KMApi object created by KMNewKMAPI()
*  \param password: The password (utf8 string) to authenticate with MC
*  \return ::KMResponse: Return code.\n
*   \bNote: MC does not return error in case of a wrong password*/
extern "C" KMAPI_EXPORT KMResponse Authenticate( KMApi* pKmapi, const char * password );


#ifndef  _USRDLL
 
#endif // ! _USRDLL

