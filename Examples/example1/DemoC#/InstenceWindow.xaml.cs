using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
/*! \brief namespace of the application*/
namespace testnet
{
    /// 
    /// Interaction logic for MainWindow.xaml
    /// 
    public partial class InstenceWindow
        : Window
    {
        // callback pointer 
        NativeMethods.OnMessage callback;
        // KMAPI
        IntPtr kmapi;
        bool propmtRecived;
        const int MSG_LEN = 1024 * 4;
        internal InstenceWindow(ParametersObject parametersObject )
        {
            InitializeComponent();
            if (parametersObject == null)
                return;
            this.Title = "Instence" + parametersObject.index;

            // The instence use the kmapi created in the main window, but it could use code below to create fresh version of the DLL
            // initalize KMAPI
            // you must load the KMAPI.dll in order to call methods
            //IntPtr pDll = NativeMethods.LoadLibrary(@"KMApi.dll");

            //if (pDll == IntPtr.Zero)
            //{
            //    throw new Win32Exception(Marshal.GetLastWin32Error());
            //}

            //// ---------- creates a New KMApi object ---------------
            //kmapi = NativeMethods.KMNewKMAPI();
            //if (kmapi == IntPtr.Zero)
            //{
            //    throw new Win32Exception(Marshal.GetLastWin32Error());
            //}

            kmapi = parametersObject.kmapi;

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
        }


    }

}
