using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Threading;
/*! \brief namespace of the application*/
namespace testnet
{
    /*! \class NativeMethods
     *  \brief This is a Wrapper for all KMApi methods
     *  
     */

    /*! \brief NativeMethods 
     *   A C# static wrapper class with kmapi functionality
     *   It has KMAPI commands, enums and consts
     *   in order to use it elseware just load KMApi.dll using LoadLibrary and call its static methods
     */
    static class NativeMethods
    {
        /*! \brief win Api Call LoadLibrary
        *   This call will load the specified library into application memory
        */
        const string SKernel = "kernel32";

        [DllImport(SKernel, CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true)]
        public static extern IntPtr LoadLibrary(string fileName);

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]

        [DllImport(SKernel, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool FreeLibrary(IntPtr hModule);

        [DllImport(SKernel)]
        public static extern IntPtr GetProcAddress(IntPtr hModule, string procname);

        [DllImport(SKernel, CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true)]
        public static extern uint GetModuleFileName(
          [In]IntPtr hModule,
          [Out]StringBuilder lpFilename,
          [In]uint nSize);

        ///  KMAPI methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /*! \brief All KMAPI commands
        *   commands exported from KMAPI.dll
        */

        /*! \brief  KMNewKMAPI: Creates a new KMAPI 
         * Returns: new KMApi
         */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr KMNewKMAPI();

        /*! \brief MCFreeKMApi frees the KMAPI object 
             * Parameter: the kmapi objec created by KMNewKMApi()
             */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MCFreeKMApi(IntPtr kmApiPtr);

        /*! \brief  SetCallback: Set the callback pointer.
         *  @Param: KMApi- the KMApi object created by KMNewKMAPI
             */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCallback(IntPtr pKmapi, OnMessage pCallback);

        /*! \brief  Connect: Connets a specific IP and port
         *  @Param: KMApi- the KMApi object created by KMNewKMAPI
         *  @Param: IP- a string with MCs IP
         *  @Param: Port - number with Port to connect to
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Connect(IntPtr pKmapi, string ip, int port);

        /*! \brief  ExecCommand: Execute a command and wait for response
         *  @Param: KMApi- the KMApi object created by KMNewKMAPI
         *  @Param: Command- a string with requested command
         *  @Param[out]: response a string buffer for retuened value from MC
         *  @param[in]:  size of response
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ExecCommand(IntPtr pKmapi, string command, StringBuilder Response, int responseLen);

        /*! \brief  SendCommand: Execute a command but dont wait for response
          *  @Param: KMApi- the KMApi object created by KMNewKMAPI
          *  @Param: Command- a string with requested command
         */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int SendCommand(IntPtr pKmapi, string command);

        /*! \brief  DownloadFile: Downloads a file from MC
         *  @Param: KMApi- the KMApi object created by KMNewKMAPI
         *  @Param:  sFilePath- destination to save
         *  @Param:  sFileName- name of file to download from MC
         *  
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DownloadFile(IntPtr pKmapi, string sFilePath, string sFileName);

        /*! \brief  UploadFile: Upload a file to MC
         *  @Param: KMApi- the KMApi object created by KMNewKMAPI
         *  @Param:  Filename- the path and name of file to upload from MC
         */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int UploadFile(IntPtr pKmapi, string Filename);

        /*! \brief  GetMCLastError: Retrieve an error message from 
         *   @Param: KMApi- the KMApi object created by KMNewKMAPI
         *  @Param[out]: errorCode- a number represents an error code - can be a KMResponse or a socket error id
         *  @Param[out]: sErrorMessage- a buffer for error message
         *  @Param: msgLength- sErrorMessage's buffer length 
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMCLastError(IntPtr pKmapi,/* out*/ ref int errorCode,/* out*/  StringBuilder sErrorMessage,/* out*/  int msgLength);

        /*! \brief  SetTimeout: Set timeout
         *   @Param:  KMApi- the KMApi object created by KMNewKMAPI
         *  @Param:   type- type of timeout to set (0 for connection timeout or 1 for execute timeout)
         *  @Param: timeout- timeout in ms
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetTimeout(IntPtr pKmapi, Int16 type /* 0=connect 1 = exec*/, Int16 timeout);

        /*! \brief  DetectDevices: Detect devices in local networks
         *   @Param:  KMApi- the KMApi object created by KMNewKMAPI
         *  @Param: timeout- time to wait for responses in ms
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DetectDevices(IntPtr pKmapi, Int16 Timout);

        /*! \brief  GetDeviceList: Get a list of devices-
         *   @Param:  KMApi- the KMApi object created by KMNewKMAPI
         *   @return: returns a pointer of the list memoery
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetDeviceList(IntPtr pKmapi);

        /*! \brief  Authenticate: performe authentication using a password
         *  @Param:  KMApi- the KMApi object created by KMNewKMAPI
         *  @Param: password - the string with the password
        */
        [DllImport(@"KMApi.dll", CharSet = CharSet.Ansi, BestFitMapping = false, SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Authenticate(IntPtr pKmapi, string password);


        /*! \brief  The callback definition
         * This method will be called on each async frame, error messages and progress status ( ack/ prompt )
         *  @Param: value- frame type or error code (such as unexpected disconnnection)
         *  @Param: message- the text describing value or async message
        */
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate void OnMessage(int value, string message);


        /*! \brief  enums and static lists
        * KMAPI response codes
        * This is an enum with all return codes from methods
        * response can be a sucess or error code
        * Allmost all functions return such a response 
        */
        public enum KMResponse
        {
            // ----------------------- success messages -----------------------------------------------
            KMRet_OK = 0,
            KMRet_CONNECTED,  KMRet_ASYNC,
            // ----------------------   error messages -------------------------------------------
            KMErr_GENERAL,
            KMErr_TIMEOUT, KMErr_BAD_KMApiID, KMErr_FAIL_EXEC, KMErr_UNKNOWN_MESSAGE,
            KMErr_FAIL_CREATE_FILE, KMErr_FAIL_OPEN_FILE, KMErr_FILE_NOT_FOUND, KMErr_CANNOT_CONNECT,
            KMErr_INVALID_FORMAT, KMErr_BUFFER_TOO_SMALL, KMErr_FOLDER_NOT_FOUND,
            KMErr_TRUNCATED_FRAME, KMErr_MC_ERR, KMErr_RETRIEVE
        };

        public enum KMTimeout
        {
            KMTO_Connect,
            KMTO_Exec
        };

        /*! \brief  frame type enums for MC frames
         * You might recive them on the callback (1st parameter) 
         */
        public enum FrameTypeEnum
        {
            INVALID_FRAME = 0,
            ACKNOWLEDGE_FRAME,
            DATA_FRAME,
            BINARY_FRAME,
            CONTROL_FRAME,
            PROMPT_FRAME,
            ERROR_FRAME,
            //  asyncronius messages
            ASYNC_FRAME,
            ASYNC2_FRAME,
            ASYNC_OTHER,
            AUTHENTICATE_FRAME,
            STATE_FRAME
        };

        /// \brief  error sent to the callback whe error is closed unexpectedly 
        public static int ERR_ON_CLOSE = 100;

        /*! \brief  List of error messages 
        /* Note: You may want to chenge text into a clear error description
         */
        public static string[] KMResponsesNames = {
            "KMRet_OK",
            "KMRet_CONNECTED", "KMRet_ASYNC",
	        // ----------------------   error messages -------------------------------------------
	        "KMErr_GENERAL",
            "KMErr_TIMEOUT", "KMErr_BAD_KMApiID", "KMErr_FAIL_EXEC", "KMErr_UNKNOWN_MESSAGE",
            "KMErr_FAIL_CREATE_FILE", "KMErr_FAIL_OPEN_FILE", "KMErr_FILE_NOT_FOUND", "KMErr_CANNOT_CONNECT",
            "KMErr_INVALID_FORMAT", "KMErr_BUFFER_TOO_SMALL", "KMErr_FOLDER_NOT_FOUND",
            "KMErr_TRUNCATED_FRAME", "KMErr_MC_ERR", "KMErr_RETRIEVE"
        };
    }

    /// 
    /// Interaction logic for MainWindow.xaml
    /// 
    public partial class MainWindow : Window
    {
        // callback pointer 
        NativeMethods.OnMessage callback;
        // KMAPI
        IntPtr kmapi;
        bool propmtRecived;
        const int MSG_LEN = 1024 * 4;
        public MainWindow()
        {
            InitializeComponent();


            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            string productVersion = assembly.GetName().Version.ToString();
            lblProgVer.Content = "Version: " + productVersion;


            // initalize KMAPI
            // you must load the KMAPI.dll in order to call methods
            IntPtr pDll = NativeMethods.LoadLibrary(@"KMApi.dll");
            if (pDll == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            // ---------- creates a New KMApi object ---------------
            kmapi = NativeMethods.KMNewKMAPI();
            if (kmapi == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            // point the callback to a local method

            // 1: define the callback
            callback = (value, message) =>
            {
                OnCallback(value, message);
            };

            // 2: set the callback (tell KMApi.dll it should call this method)
            NativeMethods.SetCallback(kmapi, callback);
        }

        /*! \brief OnCallback: the clllback called from KMApi.dll
         *param FrameType: the frame type enum or an error (disconnection)
         *param message : text that comes with frame
         */
        private void OnCallback(int FrameType, string message)
        {
            // in case of disconnection error 
            if (FrameType == NativeMethods.ERR_ON_CLOSE)
            {
                MessageBox.Show("MC was disconnected unexpectedly");
                // handle it ...
                // free old one
                NativeMethods.MCFreeKMApi(kmapi);
                // create new one
                kmapi = NativeMethods.KMNewKMAPI();
                return;
            }

            // switch by frame type enum
            switch ((NativeMethods.FrameTypeEnum)FrameType)
            {
                case NativeMethods.FrameTypeEnum.INVALID_FRAME:
                    AddToLogTextBox("Invalid Frame:" + message);
                    break;
                case NativeMethods.FrameTypeEnum.ACKNOWLEDGE_FRAME:
                    // ...
                    break;
                case NativeMethods.FrameTypeEnum.DATA_FRAME:
                    AddToLogTextBox(message);
                    break;
                case NativeMethods.FrameTypeEnum.BINARY_FRAME:
                    break;
                case NativeMethods.FrameTypeEnum.CONTROL_FRAME:
                    break;
                case NativeMethods.FrameTypeEnum.PROMPT_FRAME:
                    propmtRecived = true;
                    break;
                case NativeMethods.FrameTypeEnum.ERROR_FRAME:
                    break;

                //  asyncronius messages
                case NativeMethods.FrameTypeEnum.ASYNC_FRAME:
                case NativeMethods.FrameTypeEnum.ASYNC2_FRAME:
                case NativeMethods.FrameTypeEnum.ASYNC_OTHER:
                    AsyncAddToLog(string.Format("ASYNC: {0}\n", message));
                    break;
                case NativeMethods.FrameTypeEnum.STATE_FRAME:
                    AsyncAddToLog(string.Format("ASYNC STATE: {0}\n", message));
                    break;
                default:
                    break;
            }
        }


        //! connect to MC
        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                // connect to a specific IP / port 
                int ret = NativeMethods.Connect(kmapi, edtIP.Text, Convert.ToInt16(edtPort.Text));
                if (!ProcessResult(ret))
                {
                    return;
                }
                AddToLogTextBox("Connected\n");
            }
        }

        //execute a command and wait for response
        private void btnExec_Click(object sender, RoutedEventArgs e)
        {
            executeOneTime();
        }

        private void executeOneTime()
        {
            //Ensure thread safe
            if (!this.Dispatcher.CheckAccess())
            {
                this.Dispatcher.Invoke((Action)delegate
                {
                    executeOneTime();
                });
                return;
            }
            if (kmapi != IntPtr.Zero)
            {
                int buffsize = MSG_LEN;
                StringBuilder result = new StringBuilder(buffsize);
                propmtRecived = false;
                //execute a command and wait for response
                int ret = NativeMethods.ExecCommand(kmapi, edtCommand.Text, result, buffsize);

                if (!ProcessResult(ret))
                    return;

                AddToLogTextBox(result.ToString());
                if (propmtRecived)
                    AddToLogTextBox("-->");

            }
        }

        // just send a command and wait for nothing
        private void btnSend_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                int ret = NativeMethods.SendCommand(kmapi, edtCommand.Text);
                ProcessResult(ret);
            }

        }

        // authenticate MC
        private void btnAuthenticate_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                // Authenticate using api
                // Password in this example is 'Hello'
                int ret = NativeMethods.Authenticate(kmapi, "Hello");
                ProcessResult(ret);
            }
        }

        private void btnDetect_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                // detect devices in local network wait half a second for responses
                NativeMethods.DetectDevices(kmapi, 500);
                IntPtr builder = NativeMethods.GetDeviceList(kmapi);
                while (true)
                {
                    string name = Marshal.PtrToStringAnsi(builder);
                    builder += 13;
                    string ip = Marshal.PtrToStringAnsi(builder);
                    if (ip == "")
                    {
                        AddToLogTextBox("End of device list\n");
                        break;
                    }
                        
                    builder += 17;
                    string serialNumber = Marshal.PtrToStringAnsi(builder);
                    builder += 34;
                    string message = new string((char)0, 0);
                    AddToLogTextBox(string.Format("Detected device Name:'{0}', IP:{1}, Serial:{2}\n", name, ip, serialNumber));
                }
            }
        }

        private void btnLastError_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                int messaegLength = MSG_LEN;
                StringBuilder message = new StringBuilder(messaegLength);
                int errId = 0;
                NativeMethods.GetMCLastError(kmapi, ref errId, message, messaegLength);
                ProcessResult(errId);
            }
        }


        private void btnUpload_Click(object sender, RoutedEventArgs e)
        {
            if (kmapi != IntPtr.Zero)
            {
                OpenFileDialog openFileDialog = new OpenFileDialog();
                if (openFileDialog.ShowDialog() == true)
                {
                    string filename = openFileDialog.FileName;
                    int ret = NativeMethods.UploadFile(kmapi, filename);
                    if (ProcessResult(ret))
                        AddToLogTextBox("File uploaded successfully.\n");
                }
            }
        }

        private void btnClear_Click(object sender, RoutedEventArgs e)
        {
            edtlog.Text = "";
        }

        private void btnDisconnect_Click(object sender, RoutedEventArgs e)
        {
            // just drop old  one
            if (kmapi != IntPtr.Zero)
                NativeMethods.MCFreeKMApi(kmapi);
            kmapi = NativeMethods.KMNewKMAPI();
        }

        // inner error prccessing method
        ////////////////////////////////////////////////////////////////////////////////////
        private bool ProcessResult(int ret)
        {
            if (ret < (int)NativeMethods.KMResponse.KMErr_GENERAL)
                return true;

            // it's an error
            int messaegLength = MSG_LEN;
            StringBuilder message = new StringBuilder(messaegLength);
            int errId = -1;

            int response = NativeMethods.GetMCLastError(kmapi, ref errId, message, messaegLength);

            if (ret == (int)NativeMethods.KMResponse.KMErr_MC_ERR)
            {
                AddToLogTextBox(string.Format("Error: {0} ({1}) \n{2}\n", NativeMethods.KMResponsesNames[ret], errId, message));
            }
            else
            {
                AddToLogTextBox(string.Format("Error: {0} ({1}) \n{2}\n", NativeMethods.KMResponsesNames[response], errId, message));
            }

            return false;
        }

        private void AsyncAddToLog(string msg)
        {
            Dispatcher.BeginInvoke(new Action(() => AddToLogTextBox(msg)));
        }
        private void AddToLogTextBox(string msg)
        {
            edtlog.Text += msg;
            edtlog.ScrollToEnd();
        }

        int InstenceCounter = 1;
        private void btnRunThread_Click(object sender, RoutedEventArgs e)
        {
            bool sts = runInstenceInThread(InstenceCounter++);
        }

        private bool runInstenceInThread(int idx)
        {
            ParametersObject parametersObject = new ParametersObject(kmapi,idx);

            Thread  InstenceThread = new Thread (showInstencThread);
            InstenceThread.Name = "Instence-" + idx;
            InstenceThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            InstenceThread.IsBackground = true;
            InstenceThread.SetApartmentState(ApartmentState.STA);
            InstenceThread.Start(parametersObject);
            return true;
        }

        private void showInstencThread(Object paramsObj)
        {
            
            ParametersObject parametersObject = null; 
            try { parametersObject = (ParametersObject)paramsObj; } catch { }

            InstenceWindow iWindow = new InstenceWindow(parametersObject);
            iWindow.ShowDialog();
        }

        bool stopLoop = true;
        int loopDelyTime = 100;
        private void BtnContinueExec_Click(object sender, RoutedEventArgs e)
        {
            stopLoop = false;
            runInThread();
        }

        private void runInThread()
        {
             
            Thread runContThread = new Thread(runContinue); // Do the calculation to find application cycle
            runContThread.Name = "runContinue";
            //ProcessInfoThread.Priority = ThreadPriority.AboveNormal;
            runContThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            runContThread.IsBackground = true; //Mar 2 2016: Ensure thread stop on exit.
            runContThread.Start();

        }

        private void runContinue()
        {
            while (!stopLoop)
            {
                executeOneTime();
                Thread.Sleep(loopDelyTime);
            }
        }

        private void BtnStop_Click(object sender, RoutedEventArgs e)
        {
            stopLoop = true;
        }
    }


    internal class ParametersObject
    {
        // KMAPI
        internal IntPtr kmapi;
        internal int index = 11;
        internal ParametersObject(IntPtr _kmapi, int _index)
        {
            kmapi = _kmapi;
            index = _index;

        }
    }
}
