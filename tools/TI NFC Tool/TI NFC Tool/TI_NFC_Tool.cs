//*************************************************************************
//   TI NFC Tool Version 1.8
//   Authors: E. Macias
//            R. Jacobi
//            Texas Instruments, Inc
//   Date:    March 29, 2016
//
// Copyright (c) 2016 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*************************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Security.Permissions;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Win32;

namespace TI_NFC_Tool
{
    public partial class TI_NFC_Tool : Form
    {
        // This is used to compare time between packets to measure throughput
        DateTime CurrentTime = DateTime.Now;

        // Used to keep track of the NDEF being received
        Int32 NDefLength = 0;

        // Commands NFC Host <-- NFC Controller
        enum eNFCControllerCommands : byte
        {
            // General NFC commands
            FW_VERSION_CMD = 0x01,
            NFC_CONFIGURATION = 0x02,
            NFC_MODE_LOST = 0x03,

            // P2P Commands
            P2P_TECH_DATA = 0x20,
            P2P_BITR_DATA = 0x21,
            P2P_RSSI_DATA = 0x22,
            P2P_MODE_DATA = 0x23,
            P2P_RX_SIZE_DATA = 0x24,
            P2P_NDEF_TOTAL_SIZE = 0x25,
            P2P_PAYLOAD_DATA = 0x26,

            // CE Commands
            CE_TECH_DATA = 0x40,
            CE_BITR_DATA = 0x41,
            CE_RSSI_DATA = 0x42,
            CE_TYPE_DATA = 0x43,
            CE_UID_DATA = 0x44,
            CE_FILE_STATUS = 0x45,

            // RW Commands
            RW_TECH_DATA = 0x60,
            RW_BITR_DATA = 0x61,
            RW_RSSI_DATA = 0x62,
            RW_TYPE_DATA = 0x63,
            RW_UID_DATA = 0x64,
            RW_PAYLOAD_DATA = 0x65,
            RW_STATUS_DATA = 0x66
        }

        // Commands NFC Host --> NFC Controller
        enum eNFCHostCommands : byte
        {
            // General NFC commands
            COMM_START = 0x80,
            COMM_END = 0x81,
            NFC_TEST_CONFIG = 0x82,
            TRF_SETTINGS = 0x83,

            // P2P Commands
            P2P_START_CMD = 0xA0,
            P2P_STOP_CMD = 0xA1,
            P2P_PUSH_PAYLOAD = 0xA2,

            // CE Commands
            CE_START_CMD = 0xC0,
            CE_STOP_CMD = 0xC1,
            CE_NDEF_CONFIG = 0xC2,

            // RW Commands
            RW_START_CMD = 0xE0,
            RW_STOP_CMD = 0xE1,
            RW_FORMAT_TAG = 0xE2,
            RW_WRITE_TAG = 0xE3
        }

        // RTD Types for the NDEF messages inside the Peer to Peer Tab
        enum eRTDTypes : byte
        {
            RTD_TEXT = 1,
            RTD_LONG_TEXT,
            RTD_URI,
            RTD_SMARTPOSTER,
            RTD_MIME
        };

        // URI Types 
        enum eNDEF_URIRecord_IDCode : byte
        {
            //! Nothing 
            UNABRIDGE = 0x00,
            //! 'http://www.' 
            HTTP_WWW = 0x01,
            //! 'https://www.' 
            HTTPS_WWW = 0x02,
            //! 'http://' 
            HTTP = 0x03,
            //! 'https://' 
            HTTPS = 0x04,
            //! 'tel:' 
            TEL = 0x05,
            //! 'mailto:' 
            MAILTO = 0x06,
            //! 'ftp://anonymous:anonymous@' 
            FTP_ANONYMOUS = 0x07,
            //! 'ftp://ftp.' 
            FTP_FTP = 0x08,
            //! 'ftps://' 
            FTPS = 0x09,
            //! 'sftp://' 
            SFTP = 0x0A,
            //! 'smb://' 
            SMB = 0x0B,
            //! 'nfs://' 
            NFS = 0x0C,
            //! 'ftp://' 
            FTP = 0x0D,
            //! 'dav://' 
            DAV = 0x0E,
            //! 'news:' 
            NEWS = 0x0F,
            //! 'telnet://' 
            TELNET = 0x10,
            //! 'imap:' 
            IMAP = 0x11,
            //! 'rtsp://' 
            RTSP = 0x12,
            //! 'urn:' 
            URN = 0x13,
            //! 'pop:' 
            POP = 0x14,
            //! 'sip:' 
            SIP = 0x15,
            //! 'sips:' 
            SIPS = 0x16,
            //! 'tftp:' 
            TFTP = 0x17,
            //! 'btspp://' 
            BTSPP = 0x18,
            //! 'btl2cap://' 
            BTL2CAP = 0x19,
            //! 'btgoep://' 
            BTGOEP = 0x1A,
            //! 'tcpobex://' 
            TCPOBEX = 0x1B,
            //! 'irdaobex://' 
            IRDAOBEX = 0x1C,
            //! 'file://' 
            FILE = 0x1D,
            //! 'urn:epc:id:' 
            URN_EPC_ID = 0x1E,
            //! 'urn:epc:tag:' 
            URN_EPC_TAG = 0x1F,
            //! 'urn:epc:pat:' 
            URN_EPC_PAT = 0x20,
            //! 'urn:epc:raw:' 
            URN_EPC_RAW = 0x21,
            //! 'urn:epc:' 
            URN_EPC = 0x22,
            //! 'urn:nfc:' 
            URN_NFC = 0x23,
            //! Values equal to and above this are reserved for future use (RFU)
            RFU = 0x24
        };

        // Buffer for incoming packets
        byte[] incomingByte = new byte[258];

        // Counter for bytes stored inside the incomingByte buffer by the parseSerialCommand()
        int bytesReceived = 0;

        // Length of the incoming packet received by the parseSerialCommand()
        int serialPacketLength = 0x00;

        // P2P Mode Status - 
        //      true (the NFC Controller is polling/listening), 
        //      false (transceiver is not listening or polling to activate NFC DEP)
        bool p2p_mode_enabled = false;

        // CE Mode Status - 
        //      true (the NFC Controller is emulating a tag), 
        //      false (transceiver is not activating NFC-A or NFC-B for ISO DEP)
        bool ce_mode_enabled = false;

        // RW Mode Status - 
        //      true (the NFC Controller is polling for tag(s), 
        //      false (transceiver is not polling for tags)
        bool rw_mode_enabled = false;

        // NFC Compliance Testing
        //      true (the NFC module is in NFC test mode
        //      false (the NFC module is not in NFC test mode)
        bool nfc_test_mode_enabled = false;

        // P2P Target Firmware Enabled Flag
        bool p2p_target_fw_enabled = false;

        // P2P Initiator Firmware Enabled Flag
        bool p2p_initiator_fw_enabled = false;

        // Card Emulation Firmware Enabled Flag
        bool card_emulation_fw_enabled = false;

        // Reader/Writer Firmware Enabled Flag
        bool reader_writer_fw_enabled = false;

        // Queue to receive data from the SerialPort
        Queue serialDataQueue = new Queue();

        // Mutex for the SendCommand() to ensure the tx buffer is not corrupted.
        Mutex transmitMutex = new Mutex();

        // Application path
        string TI_NFC_Tool_App_Path = Path.GetDirectoryName(Application.ExecutablePath);

        // Connection Timeout Error to detect invalid devices
        int timeOutCounter = -1;

        // Firmware Version Received Flag for initial timeout handling when a device is connected
        bool fw_version_flag;

        // Successful GUI connection flag
        bool gui_connect_flag = false;

        // Display hardware error message flag 
        bool display_error_flag = true;

        // Auto connect is in progress flag 
        bool auto_connect_flag = false;

        //int FileCounterVar = 0;

        // Peer to Peer Mask Bits
        const byte P2P_TARGET_FW_ENABLED = 0x01;
        const byte P2P_INITIATOR_FW_ENABLED = 0x02;
        const byte CARD_EMULATION_FW_ENABLED = 0x04;
        const byte READER_WRITER_FW_ENABLED = 0x08;

        const uint NFC_FW_ENABLE_INDEX = 3;
        const uint P2P_TARGET_INDEX = 4;
        const uint P2P_INITIATOR_INDEX = 5;
        const uint CARD_EMULATION_INDEX = 6;
        const uint READER_WRITER_INDEX = 7;
        const uint LISTEN_TIME_1_INDEX = 10;
        const uint LISTEN_TIME_2_INDEX = 11;
        const uint NFC_CERT_INDEX = 12;
        const uint P2P_CERT_INDEX = 13;
        const uint P2P_NFC_DEP_DID_INDEX = 14;
        const uint RW_OPTS_1_INDEX = 15;
        const uint RW_OPTS_2_INDEX = 16;
        const uint RW_ISO_DEP_DID_INDEX = 17;
        const uint TRF_AUTO_SDD_INDEX = 18;
        const uint TRF_EXT_AMP_INDEX = 19;
        const uint TRF_5V_SUPPLY_INDEX = 20;
        const uint TRF_VERSION_INDEX = 21;

        const byte P2P_TARGET_MODE_MASK = 0x01;
        const byte P2P_TARGET_PASS_106_MASK = 0x02;
        const byte P2P_TARGET_PASS_212_MASK = 0x04;
        const byte P2P_TARGET_PASS_424_MASK = 0x08;
        const byte P2P_TARGET_ACT_106_MASK = 0x10;
        const byte P2P_TARGET_ACT_212_MASK = 0x20;
        const byte P2P_TARGET_ACT_424_MASK = 0x40;

        const byte P2P_INITIATOR_MODE_MASK = 0x01;
        const byte P2P_INITIATOR_PASS_106_MASK = 0x02;
        const byte P2P_INITIATOR_PASS_212_MASK = 0x04;
        const byte P2P_INITIATOR_PASS_424_MASK = 0x08;
        const byte P2P_INITIATOR_ACT_106_MASK = 0x10;
        const byte P2P_INITIATOR_ACT_212_MASK = 0x20;
        const byte P2P_INITIATOR_ACT_424_MASK = 0x40;

        // Card Emulation Mask Bits
        const byte CE_TYPE4A_MASK = 0x01;
        const byte CE_TYPE4B_MASK = 0x02;

        // Reader/Writer Mask Bits
        const byte RW_NFC_A_ENABLE_MASK = 0x01;
        const byte RW_NFC_B_ENABLE_MASK = 0x02;
        const byte RW_NFC_F_ENABLE_MASK = 0x04;
        const byte RW_NFC_V_ENABLE_MASK = 0x08;

        const Int16 RW_NFC_A_106_MASK = 0x0001;
        const Int16 RW_NFC_A_212_MASK = 0x0002;
        const Int16 RW_NFC_A_424_MASK = 0x0004;
        const Int16 RW_NFC_A_848_MASK = 0x0008;
        const Int16 RW_NFC_B_106_MASK = 0x0010;
        const Int16 RW_NFC_B_212_MASK = 0x0020;
        const Int16 RW_NFC_B_424_MASK = 0x0040;
        const Int16 RW_NFC_B_848_MASK = 0x0080;
        const Int16 RW_NFC_F_212_MASK = 0x0100;
        const Int16 RW_NFC_F_424_MASK = 0x0200;
        const Int16 RW_NFC_V_6_7_MASK = 0x0400;
        const Int16 RW_NFC_V_26_7_MASK = 0x0800;

        //
        // Delay function for .NET 4.0 Framework
        //
        public static Task Delay(double milliseconds)
        {
            var tcs = new TaskCompletionSource<bool>();
            System.Timers.Timer timer = new System.Timers.Timer();
            timer.Elapsed += (obj, args) =>
            {
                tcs.TrySetResult(true);
            };
            timer.Interval = milliseconds;
            timer.AutoReset = false;
            timer.Start();
            return tcs.Task;
        }

        //
        // Form Initialization of all components
        //
        public TI_NFC_Tool()
        {
            InitializeComponent();
            // Define default Baud Rate
            baudRateBox.SelectedIndex = 3;
            comboBox_P2P_ATN_Retry.SelectedIndex = 0;
            comboBox_P2P_NACK_Retry.SelectedIndex = 0;
            comboBox_P2P_DID.SelectedIndex = 0;
            comboBox_RW_DSL_Retry.SelectedIndex = 0;
            comboBox_RW_ACK_Retry.SelectedIndex = 0;
            comboBox_RW_NACK_Retry.SelectedIndex = 0;
            comboBox_RW_WTX_Retry.SelectedIndex = 0;
            comboBox_RW_DID.SelectedIndex = 0;
            comboBox_TRF_Version.SelectedIndex = 0;
            textBox_RW_RTD_Text_Counter.Text = "Message Length: " + richTextBox_RW_RTD_Text.TextLength + "/246";
            textBox_CE_RTD_Text_Counter.Text = "Message Length: " + richTextBox_CE_RTD_Text.TextLength + "/246";
            textBox_P2P_RTD_Text_Counter.Text = "Message Length: " + richTextBox_P2P_RTD_Text.TextLength + "/235";
        }

        //------------------------------- Helper Functions START---------------------------------//

        //
        // Sends Command to the NFC Controller
        //
        private void SendCommand(Byte[] data,
                                eNFCHostCommands eNFCHostCmd,
                                int packet_length)
        {
            transmitMutex.WaitOne();

            Byte[] ui8Packet = new Byte[packet_length + 3];

            if (packet_length > 256)
                return;

            // SOF
            ui8Packet[0] = 0xFE;
            // Command Byte
            ui8Packet[1] = (byte)eNFCHostCmd;
            // Length 
            ui8Packet[2] = (byte)packet_length;

            if (packet_length > 0)
            {
                // Copy data Array into the ui8Packet
                Array.Copy(data, 0, ui8Packet, 3, packet_length);
            }

            if (serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Write(ui8Packet, 0, packet_length + 3);
                }
                catch
                {
                    gui_connect_flag = false;
                }
            }

            transmitMutex.ReleaseMutex();
        }

        //
        // When the TI NFC Tool Form Closes 
        //
        private void TI_NFC_Tool_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (serialPort1.IsOpen == true)
            {
                manualConnectButton_Click(null, null);
            }
        }

        //
        // Background timer - handles parsing the incoming commands
        //
        private void timerBackground_Tick(object sender, EventArgs e)
        {
            timerBackground.Stop();
            parseSerialCommand();
            timerBackground.Start();
            if (timeOutCounter > 0)
            {
                timeOutCounter--;
            }
            else if(timeOutCounter == 0)
            {
                timeOutCounter = -1;

                if (fw_version_flag == false)
                {
                    if (auto_connect_flag == false)
                    {
                        MessageBox.Show("Connection Failed." + Environment.NewLine + "Verify that the target device has NFCLink (Standalone) firmware loaded.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1);
                    }
                    // Disconnect
                    manualConnectComport();
                }
                else
                {
                    // Disable error message
                    display_error_flag = false;
                    // Mark that the GUI has made a successful connection
                    gui_connect_flag = true;
                }
            }
            if ((fw_version_flag == true) && (gui_connect_flag == false))
            {
                manualConnectComport();
            }
            else if ((fw_version_flag == true) && (gui_connect_flag == true))
            {
                comportBox.Enabled = false;
                baudRateBox.Enabled = false;
                hardwareSelectBox.Enabled = false;
            }
        }

        //
        // Process Incoming Commands from the NFC Controller
        //
        private void processCommand(eNFCControllerCommands eControllerCmd,
                                    Byte payloadStartIndex,
                                    Byte[] pui8Data,
                                    Byte ui8PayLoadLength)
        {
            Encoding ascii = Encoding.ASCII;

            switch (eControllerCmd)
            {
                case eNFCControllerCommands.FW_VERSION_CMD:
                   {
                        serialPortDataTextBox.AppendText("FW Version ");

                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);

                        serialPortDataTextBox.AppendText(asciiString);

                        serialPortDataTextBox.AppendText("\n");

                        // Print the FW Version
                        toolStripStatusLabelFWVer.Text = "FW Version " + asciiString;

                        toolStripStatusConnectionLabel.Text = "Connected";
                        toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.SpringGreen;

                       // FW Version Received
                        fw_version_flag = true;

                        break;
                    }
                case eNFCControllerCommands.NFC_CONFIGURATION:
                    {
                        if (ui8PayLoadLength >= 3)
                        {
                            // P2P Configuration
                            groupBox_P2P_Config.Enabled = false;

                            panel_P2P_Target_Support.Enabled = false;
                            panel_P2P_Initiator_Support.Enabled = false;

                            checkBox_Pas_Target_106.Checked = false;
                            checkBox_Pas_Target_212.Checked = false;
                            checkBox_Pas_Target_424.Checked = false;
                            checkBox_Act_Target_106.Checked = false;
                            checkBox_Act_Target_212.Checked = false;
                            checkBox_Act_Target_424.Checked = false;
                            checkBox_Pas_Initiator_106.Checked = false;
                            checkBox_Pas_Initiator_212.Checked = false;
                            checkBox_Pas_Initiator_424.Checked = false;
                            checkBox_Act_Initiator_106.Checked = false;
                            checkBox_Act_Initiator_212.Checked = false;
                            checkBox_Act_Initiator_424.Checked = false;

                            // CE Configuration
                            groupBox_CE_Config.Enabled = false;

                            panel_CE_Mode_Support.Enabled = false;

                            checkBox_CE_T4AT.Checked = false;
                            checkBox_CE_T4BT.Checked = false;

                            // RW Configuration
                            groupBox_RW_Config.Enabled = false;

                            panel_RW_Mode_Support.Enabled = false;

                            checkBox_RW_NFC_A.Checked = false;
                            checkBox_RW_NFC_B.Checked = false;
                            checkBox_RW_NFC_F.Checked = false;
                            checkBox_RW_ISO15693.Checked = false;

                            //TODO: Verify this behavior is working as expected

                            // Assume the code is running P2P
                            button_P2P_toggle.Text = "Stop P2P";
                            p2p_mode_enabled = true;

                            // Assume the code is running CE
                            button_CE_toggle.Text = "Stop CE";
                            ce_mode_enabled = true;

                            // Assume the code is running RW
                            button_RW_toggle.Text = "Stop RW";
                            rw_mode_enabled = true;

                            groupBox_NFC_Compliance_Config.Enabled = true;

                            //
                            // Target P2P Configuration
                            //
                            if ((pui8Data[NFC_FW_ENABLE_INDEX] & P2P_TARGET_FW_ENABLED) == P2P_TARGET_FW_ENABLED)
                            {
                                groupBox_P2P_Config.Enabled = true;
                                p2p_target_fw_enabled = true;

                                if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_MODE_MASK) != P2P_TARGET_MODE_MASK)
                                {
                                    button_P2P_toggle.Text = "Start P2P";
                                    panel_P2P_Target_Support.Enabled = true;
                                    p2p_mode_enabled = false;
                                }

                                if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_MODE_MASK) == P2P_TARGET_MODE_MASK)
                                {
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_PASS_106_MASK) == P2P_TARGET_PASS_106_MASK)
                                    {
                                        checkBox_Pas_Target_106.Checked = true;
                                    }
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_PASS_212_MASK) == P2P_TARGET_PASS_212_MASK)
                                    {
                                        checkBox_Pas_Target_212.Checked = true;
                                    }
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_PASS_424_MASK) == P2P_TARGET_PASS_424_MASK)
                                    {
                                        checkBox_Pas_Target_424.Checked = true;
                                    }
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_ACT_106_MASK) == P2P_TARGET_ACT_106_MASK)
                                    {
                                        checkBox_Act_Target_106.Checked = true;
                                    }
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_ACT_212_MASK) == P2P_TARGET_ACT_212_MASK)
                                    {
                                        checkBox_Act_Target_212.Checked = true;
                                    }
                                    if ((pui8Data[P2P_TARGET_INDEX] & P2P_TARGET_ACT_424_MASK) == P2P_TARGET_ACT_424_MASK)
                                    {
                                        checkBox_Act_Target_424.Checked = true;
                                    }
                                }
                            }

                            //
                            // Initiator P2P Configuration
                            //
                            if ((pui8Data[NFC_FW_ENABLE_INDEX] & P2P_INITIATOR_FW_ENABLED) == P2P_INITIATOR_FW_ENABLED)
                            {
                                groupBox_P2P_Config.Enabled = true;
                                p2p_initiator_fw_enabled = true;

                                if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_MODE_MASK) != P2P_INITIATOR_MODE_MASK)
                                {
                                    button_P2P_toggle.Text = "Start P2P";
                                    panel_P2P_Initiator_Support.Enabled = true;
                                    p2p_mode_enabled = false;
                                }

                                if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_MODE_MASK) == P2P_INITIATOR_MODE_MASK)
                                {
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_PASS_106_MASK) == P2P_INITIATOR_PASS_106_MASK)
                                    {
                                        checkBox_Pas_Initiator_106.Checked = true;
                                    }
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_PASS_212_MASK) == P2P_INITIATOR_PASS_212_MASK)
                                    {
                                        checkBox_Pas_Initiator_212.Checked = true;
                                    }
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_PASS_424_MASK) == P2P_INITIATOR_PASS_424_MASK)
                                    {
                                        checkBox_Pas_Initiator_424.Checked = true;
                                    }
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_ACT_106_MASK) == P2P_INITIATOR_ACT_106_MASK)
                                    {
                                        checkBox_Act_Initiator_106.Checked = true;
                                    }
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_ACT_212_MASK) == P2P_INITIATOR_ACT_212_MASK)
                                    {
                                        checkBox_Act_Initiator_212.Checked = true;
                                    }
                                    if ((pui8Data[P2P_INITIATOR_INDEX] & P2P_INITIATOR_ACT_424_MASK) == P2P_TARGET_ACT_424_MASK)
                                    {
                                        checkBox_Act_Initiator_424.Checked = true;
                                    }
                                }
                            }

                            //
                            // Card Emulation Configuration
                            //
                            if ((pui8Data[NFC_FW_ENABLE_INDEX] & CARD_EMULATION_FW_ENABLED) == CARD_EMULATION_FW_ENABLED)
                            {

                                if ((pui8Data[CARD_EMULATION_INDEX] & CE_TYPE4A_MASK) != CE_TYPE4A_MASK
                                    && (pui8Data[CARD_EMULATION_INDEX] & CE_TYPE4B_MASK) != CE_TYPE4B_MASK)
                                {
                                    button_CE_toggle.Text = "Start CE";
                                    panel_CE_Mode_Support.Enabled = true;
                                    ce_mode_enabled = false;
                                }

                                groupBox_CE_Config.Enabled = true;
                                card_emulation_fw_enabled = true;
                                if ((pui8Data[CARD_EMULATION_INDEX] & CE_TYPE4A_MASK) == CE_TYPE4A_MASK)
                                {
                                    checkBox_CE_T4AT.Checked = true;
                                }
                                if ((pui8Data[CARD_EMULATION_INDEX] & CE_TYPE4B_MASK) == CE_TYPE4B_MASK)
                                {
                                    checkBox_CE_T4BT.Checked = true;
                                }
                            }

                            if (p2p_target_fw_enabled == true && p2p_initiator_fw_enabled == true)
                            {
                                if (((pui8Data[P2P_INITIATOR_INDEX] & 0x7E) > 0 && (pui8Data[P2P_INITIATOR_INDEX] & 0x01) > 0)
                                    || ((pui8Data[P2P_TARGET_INDEX] & 0x7E) > 0 && (pui8Data[P2P_TARGET_INDEX] & 0x01) > 0))
                                {
                                    // Assume the code is running P2P
                                    button_P2P_toggle.Text = "Stop P2P";
                                    p2p_mode_enabled = true;
                                    panel_P2P_Target_Support.Enabled = false;
                                    panel_P2P_Initiator_Support.Enabled = false;
                                }
                            }

                            //
                            // Reader/Writer Configuration
                            //
                            if ((pui8Data[NFC_FW_ENABLE_INDEX] & READER_WRITER_FW_ENABLED) == READER_WRITER_FW_ENABLED)
                            {
                                groupBox_RW_Config.Enabled = true;

                                if ((pui8Data[READER_WRITER_INDEX] & 0x0F) > 0)
                                {
                                    button_RW_toggle.Text = "Stop RW";

                                    rw_mode_enabled = true;

                                    panel_RW_Mode_Support.Enabled = false;
                                }
                                else
                                {
                                    button_RW_toggle.Text = "Start RW";

                                    rw_mode_enabled = false;

                                    panel_RW_Mode_Support.Enabled = true;
                                }

                                if ((pui8Data[READER_WRITER_INDEX] & 0x01) > 0)
                                {
                                    checkBox_RW_NFC_A.Checked = true;

                                    // Check 848kbps
                                    if ((pui8Data[READER_WRITER_INDEX + 1] & 0x08) > 0)
                                    {
                                        radioButton_NFC_A_848.Checked = true;
                                    }
                                    // Check 424kbps
                                    else if ((pui8Data[READER_WRITER_INDEX + 1] & 0x04) > 0)
                                    {
                                        radioButton_NFC_A_424.Checked = true;
                                    }
                                    // Check 212kbps
                                    else if ((pui8Data[READER_WRITER_INDEX + 1] & 0x04) > 0)
                                    {
                                        radioButton_NFC_A_212.Checked = true;
                                    }
                                    // Set 106kbps
                                    else
                                    {
                                        radioButton_NFC_A_106.Checked = true;
                                    }
                                }
                                else
                                {
                                    radioButton_NFC_A_106.Checked = true;
                                    checkBox_RW_NFC_A.Checked = false;
                                }
                                if ((pui8Data[READER_WRITER_INDEX] & 0x02) > 0)
                                {
                                    checkBox_RW_NFC_B.Checked = true;

                                    // Check 848kbps
                                    if ((pui8Data[READER_WRITER_INDEX + 1] & 0x80) > 0)
                                    {
                                        radioButton_NFC_B_848.Checked = true;
                                    }
                                    // Check 424kbps
                                    else if ((pui8Data[READER_WRITER_INDEX + 1] & 0x40) > 0)
                                    {
                                        radioButton_NFC_B_424.Checked = true;
                                    }
                                    // Check 212kbps
                                    else if ((pui8Data[READER_WRITER_INDEX + 1] & 0x20) > 0)
                                    {
                                        radioButton_NFC_B_212.Checked = true;
                                    }
                                    // Set 106kbps
                                    else
                                    {
                                        radioButton_NFC_B_106.Checked = true;
                                    }
                                }
                                else
                                {
                                    radioButton_NFC_B_106.Checked = true;
                                    checkBox_RW_NFC_B.Checked = false;
                                }
                                if ((pui8Data[READER_WRITER_INDEX] & 0x04) > 0)
                                {
                                    checkBox_RW_NFC_F.Checked = true;

                                    // Check 424kbps
                                    if ((pui8Data[READER_WRITER_INDEX + 2] & 0x02) > 0)
                                    {
                                        checkBox_NFC_F_424.Checked = true;
                                    }
                                    // Check 212kbps
                                    if ((pui8Data[READER_WRITER_INDEX + 2] & 0x01) > 0)
                                    {
                                        checkBox_NFC_F_212.Checked = true;
                                    }
                                }
                                else
                                {
                                    checkBox_NFC_F_212.Checked = true;
                                    checkBox_RW_NFC_F.Checked = false;
                                }
                                if ((pui8Data[READER_WRITER_INDEX] & 0x08) > 0)
                                {
                                    checkBox_RW_ISO15693.Checked = true;

                                    radioButton_ISO15693_26_48.Checked = true;
                                }
                                else
                                {
                                    checkBox_RW_ISO15693.Checked = false;
                                }
                            }

                            UInt16 ListenTime;
                            Byte TempValue;

                            ListenTime = (UInt16)((UInt16)pui8Data[LISTEN_TIME_1_INDEX] + ((UInt16)pui8Data[LISTEN_TIME_2_INDEX] << 8));

                            // Update Listen Time Value
                            comboBox_Loop_Delay.Text = ListenTime.ToString();

                            // NFC Compliance

                            // TRF79xx Settings
                            if ((pui8Data[NFC_CERT_INDEX] & 0x01) > 0)
                            {
                                panel_P2P_adv_config.Enabled = false;
                                panel_RW_adv_config.Enabled = false;
                                nfc_test_mode_enabled = true;
                                buttonNFC_Compliance_toggle.Text = "Stop NFC Testing";
                            }
                            else
                            {
                                panel_P2P_adv_config.Enabled = true;
                                panel_RW_adv_config.Enabled = true;
                                nfc_test_mode_enabled = false;
                                buttonNFC_Compliance_toggle.Text = "Start NFC Testing";
                            }

                            //
                            // LLCP Enabled Field
                            //
                            if ((pui8Data[P2P_CERT_INDEX] & 0x01) > 0)
                            {
                                checkBox_LLCP_Enable.Checked = true;
                            }
                            else
                            {
                                checkBox_LLCP_Enable.Checked = false;
                            }

                            //
                            // NFC DEP Loopback Field
                            //
                            if ((pui8Data[P2P_CERT_INDEX] & 0x02) > 0)
                            {
                                checkBox_NFC_DEP_Loopback.Checked = true;
                            }
                            else
                            {
                                checkBox_NFC_DEP_Loopback.Checked = false;
                            }

                            // Max TimeOuts
                            TempValue = (Byte) ((pui8Data[P2P_CERT_INDEX] & 0x1C) >> 2);
                            comboBox_P2P_ATN_Retry.Text = TempValue.ToString();

                            // Max Protocol Errors
                            TempValue = (Byte)((pui8Data[P2P_CERT_INDEX] & 0xE0) >> 5);
                            comboBox_P2P_NACK_Retry.Text = TempValue.ToString();

                            // NFC DEP DID
                            comboBox_P2P_DID.Text = pui8Data[P2P_NFC_DEP_DID_INDEX].ToString();

                            // RW NACK
                            TempValue = (Byte)((pui8Data[RW_OPTS_1_INDEX] & 0x07));
                            comboBox_RW_NACK_Retry.Text = TempValue.ToString();

                            // RW DSL
                            TempValue = (Byte)((pui8Data[RW_OPTS_1_INDEX] & 0x38) >> 3);
                            comboBox_RW_DSL_Retry.Text = TempValue.ToString();

                            // RW ACK
                            TempValue = (Byte)(((pui8Data[RW_OPTS_1_INDEX] & 0xC0) >> 6) + ((pui8Data[RW_OPTS_2_INDEX] & 0x01) << 2));
                            comboBox_RW_ACK_Retry.Text = TempValue.ToString();

                            // RW WTX
                            TempValue = (Byte)((pui8Data[RW_OPTS_2_INDEX] & 0x0E) >> 1);
                            comboBox_RW_WTX_Retry.Text = TempValue.ToString();

                            // ISO DEP DID
                            comboBox_RW_DID.Text = pui8Data[RW_ISO_DEP_DID_INDEX].ToString();

                            // TRF Auto SDD
                            if ((pui8Data[TRF_AUTO_SDD_INDEX] & 0x01) > 0)
                            {
                                checkBox_TRF_AutoSDD.Checked = true;
                            }
                            else
                            {
                                checkBox_TRF_AutoSDD.Checked = false;
                            }

                            // TRF External Amplifier
                            if ((pui8Data[TRF_EXT_AMP_INDEX] & 0x01) > 0)
                            {
                                checkBox_TRF_Ext_Amp.Checked = true;
                            }
                            else
                            {
                                checkBox_TRF_Ext_Amp.Checked = false;
                            }

                            // TRF 5V Supply Amplifier
                            if ((pui8Data[TRF_5V_SUPPLY_INDEX] & 0x01) > 0)
                            {
                                radioButton_TRF_5V_supply.Checked = true;
                            }
                            else
                            {
                                radioButton_TRF_3V_supply.Checked = true;
                            }

                            // TRF Version Amplifier
                            if ((pui8Data[TRF_VERSION_INDEX] & 0x01) > 0)
                            {
                                comboBox_TRF_Version.Text = "70A";
                            }

                            groupBox_TRF79xx_Settings.Enabled = true;
                        }

                        break;
                    }
                case eNFCControllerCommands.NFC_MODE_LOST:
                    // P2P clear textboxes
                    richTextBox_P2P_Receive.Text = "";
                    textBox_P2P_Mode.Text = "";
                    textBox_P2P_Technology.Text = "";
                    textBox_P2P_Bitrate.Text = "";
                    textBox_P2P_Rssi.Text = "";
                    textBox_P2P_Received.Text = "";

                    // CE clear textboxes
                    richTextBox_CE_Status.Text = "";
                    textBox_CE_Tag_Type.Text = "";
                    textBox_CE_Technology.Text = "";
                    textBox_CE_Bitrate.Text = "";
                    textBox_CE_Rssi.Text = "";
                    textBox_CE_UID.Text = "";

                    // RW clear textboxes
                    richTextBox_RW_Tag_Content.Text = "";
                    textBox_RW_Tag_Type.Text = "";
                    textBox_RW_Technology.Text = "";
                    textBox_RW_Bitrate.Text = "";
                    textBox_RW_Rssi.Text = "";
                    textBox_RW_UID.Text = "";

                    if (MainTabController.SelectedTab != tabCOMLog)
                    {
                        MainTabController.SelectedTab = tabNfcConfiguration;
                    }

                    richTextBoxColorAppend(serialPortDataTextBox, "NFC Communication Disconnected\n", Color.Purple);
                    break;
                //
                // P2P Commands
                //
                case eNFCControllerCommands.P2P_TECH_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_P2P_Technology.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabPeerToPeer;
                        break;
                    }
                case eNFCControllerCommands.P2P_BITR_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_P2P_Bitrate.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabPeerToPeer;
                        break;
                    }
                case eNFCControllerCommands.P2P_RSSI_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_P2P_Rssi.Text = "0x" + asciiString;
                        break;
                    }
                case eNFCControllerCommands.P2P_MODE_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_P2P_Mode.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabPeerToPeer;
                        richTextBoxColorAppend(serialPortDataTextBox, "P2P Communication Established\n", Color.Purple);
                        break;
                    }
                case eNFCControllerCommands.P2P_RX_SIZE_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_P2P_Received.Text = "0x" + asciiString;
                        break;
                    }
                case eNFCControllerCommands.P2P_NDEF_TOTAL_SIZE:
                    {
                        System.IO.BinaryWriter file = new System.IO.BinaryWriter(new FileStream(TI_NFC_Tool_App_Path + "/NDEF.hex", FileMode.Create, FileAccess.Write));

                        richTextBoxColorAppend(serialPortDataTextBox, "Packet Start \n", Color.Blue);
                        // Beginning of packet
                        CurrentTime = DateTime.Now;
                        NDefLength = 0; // Total Length ==> incomingByte[3] + (incomingByte[4] << 8) + (incomingByte[5] << 16) + (incomingByte[6] << 24);

                        serialPortDataTextBox.AppendText("Payload Size \n");

                        file.Close();

                        break;
                    }
                case eNFCControllerCommands.P2P_PAYLOAD_DATA:
                    {
                        DateTime TempTime = DateTime.Now;

                        String NdefString = "";
                        // Check if for short record
                        if ((pui8Data[3] & 0x10) > 0)
                        {
                            byte ui8RecordLength = pui8Data[5];

                            switch (pui8Data[6])
                            {
                                // Text
                                case 0x54:
                                    {
                                        char[] asciiPayload = new char[ascii.GetCharCount(pui8Data, (int)10, (int)(ui8RecordLength - 3))];
                                        ascii.GetChars(pui8Data, (int)10, (int)(ui8RecordLength - 3), asciiPayload, 0);
                                        richTextBoxColorAppend(richTextBox_P2P_Receive, "Text RTD:\n", Color.Red);
                                        NdefString = new String(asciiPayload);
                                        break;
                                    }
                                // URI
                                case 0x55:
                                    {
                                        char[] asciiPayload = new char[ascii.GetCharCount(pui8Data, (int)8, (int)(ui8RecordLength - 1))];
                                        ascii.GetChars(pui8Data, (int)8, (int)(ui8RecordLength - 1), asciiPayload, 0);
                                        richTextBoxColorAppend(richTextBox_P2P_Receive, "URI RTD:\n", Color.Red);
                                        if (pui8Data[7] == 0x01)
                                        {
                                            NdefString = "http://www." + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x02)
                                        {
                                            NdefString = "https://www." + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x03)
                                        {
                                            NdefString = "http://" + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x04)
                                        {
                                            NdefString = "https://" + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x05)
                                        {
                                            NdefString = "tel:" + new String(asciiPayload);
                                        }
                                        else
                                        {
                                            NdefString = new String(asciiPayload);
                                        }

                                        break;
                                    }
                                default:
                                    NdefString = "Unknown RTD";
                                    break;
                            }
                        }
                        else
                        {
                            NdefString = "Unknown RTD";
                        }

                        richTextBoxColorAppend(richTextBox_P2P_Receive, NdefString + "\n", Color.Black);


                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];

                        Array.Copy(pui8Data, 3, asciiChars, 0, ui8PayLoadLength);

                        System.IO.BinaryWriter file = new System.IO.BinaryWriter(new FileStream(TI_NFC_Tool_App_Path + "/NDEF.hex", FileMode.Append, FileAccess.Write));

                        foreach (byte bValue in asciiChars)
                        {
                            file.Write(bValue);
                        }

                        file.Close();

                        serialPortDataTextBox.AppendText("Payload Size: ");

                        serialPortDataTextBox.AppendText(String.Format("0x{0:X2}\n", incomingByte[2]));

                        NDefLength += incomingByte[2];
                        serialPortDataTextBox.AppendText(String.Format("Total Bytes Received: 0x{0:X4}\n", NDefLength));
                        if (NDefLength > 1000)
                        {
                            richTextBoxColorAppend(serialPortDataTextBox, "Avg. Data Rate (kBps) : " + ((NDefLength) / TempTime.Subtract(CurrentTime).TotalMilliseconds) + "\n", Color.Green);
                        }

                        break;
                    }
                //
                // CE Commands
                //
                case eNFCControllerCommands.CE_TECH_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_CE_Technology.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabCardEmulation;
                        break;
                    }
                case eNFCControllerCommands.CE_BITR_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_CE_Bitrate.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabCardEmulation;
                        break;
                    }
                case eNFCControllerCommands.CE_RSSI_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_CE_Rssi.Text = "0x" + asciiString;
                        break;
                    }
                case eNFCControllerCommands.CE_TYPE_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_CE_Tag_Type.Text = asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabCardEmulation;
                        richTextBoxColorAppend(serialPortDataTextBox, "CE Connection Established\n", Color.Purple);
                        break;
                    }
                case eNFCControllerCommands.CE_UID_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_CE_UID.Text = "0x" + asciiString;
                        // Switch Tab
                        MainTabController.SelectedTab = tabCardEmulation;
                        break;
                    }
                case eNFCControllerCommands.CE_FILE_STATUS:
                    {

                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);

                        richTextBoxColorAppend(richTextBox_CE_Status, asciiString, Color.Black);

                        break;
                    }
                //
                // RW Commands
                //
                case eNFCControllerCommands.RW_TECH_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_RW_Technology.Text = asciiString;
                        // Switch Tab
                        if (MainTabController.SelectedTab != tabCOMLog)
                        {
                            MainTabController.SelectedTab = tabReaderWriter;
                        }
                        break;
                    }
                case eNFCControllerCommands.RW_BITR_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_RW_Bitrate.Text = asciiString;
                        // Switch Tab
                        if (MainTabController.SelectedTab != tabCOMLog)
                        {
                            MainTabController.SelectedTab = tabReaderWriter;
                        }
                        break;
                    }
                case eNFCControllerCommands.RW_RSSI_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_RW_Rssi.Text = "0x" + asciiString;
                        break;
                    }
                case eNFCControllerCommands.RW_TYPE_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_RW_Tag_Type.Text = asciiString;

                        richTextBoxColorAppend(serialPortDataTextBox, asciiString + " Tag Detected\n", Color.DarkGreen);

                        // Switch Tab
                        if (MainTabController.SelectedTab != tabCOMLog)
                        {
                            MainTabController.SelectedTab = tabReaderWriter;
                        }

                        System.IO.BinaryWriter file = new System.IO.BinaryWriter(new FileStream(TI_NFC_Tool_App_Path + "/NDEF.hex", FileMode.Create, FileAccess.Write));

                        file.Close();

                        break;
                    }
                case eNFCControllerCommands.RW_UID_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        textBox_RW_UID.Text = "0x" + asciiString;

                        richTextBoxColorAppend(serialPortDataTextBox, "Tag UID: 0x" + asciiString + "\n", Color.DarkBlue);

                        // Switch Tab
                        if (MainTabController.SelectedTab != tabCOMLog)
                        {
                            MainTabController.SelectedTab = tabReaderWriter;
                        }
                        break;
                    }
                case eNFCControllerCommands.RW_PAYLOAD_DATA:
                    {
                        DateTime TempTime = DateTime.Now;

                        String NdefString = "";
                        // Check if for short record
                        if ((pui8Data[3] & 0x10) > 0)
                        {
                            byte ui8RecordLength = pui8Data[5];

                            switch (pui8Data[6])
                            {
                                // Text
                                case 0x54:
                                    {
                                        char[] asciiPayload = new char[ascii.GetCharCount(pui8Data, (int)10, (int)(ui8RecordLength - 3))];
                                        ascii.GetChars(pui8Data, (int)10, (int)(ui8RecordLength - 3), asciiPayload, 0);
                                        richTextBoxColorAppend(richTextBox_RW_Tag_Content, "Text RTD:\n", Color.Red);
                                        NdefString = new String(asciiPayload);
                                        break;
                                    }
                                // URI
                                case 0x55:
                                    {
                                        char[] asciiPayload = new char[ascii.GetCharCount(pui8Data, (int)8, (int)(ui8RecordLength - 1))];
                                        ascii.GetChars(pui8Data, (int)8, (int)(ui8RecordLength - 1), asciiPayload, 0);
                                        richTextBoxColorAppend(richTextBox_RW_Tag_Content, "URI RTD:\n", Color.Red);
                                        if (pui8Data[7] == 0x01)
                                        {
                                            NdefString = "http://www." + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x02)
                                        {
                                            NdefString = "https://www." + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x03)
                                        {
                                            NdefString = "http://" + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x04)
                                        {
                                            NdefString = "https://" + new String(asciiPayload);
                                        }
                                        else if (pui8Data[7] == 0x05)
                                        {
                                            NdefString = "tel:" + new String(asciiPayload);
                                        }
                                        else
                                        {
                                            NdefString = new String(asciiPayload);
                                        }

                                        break;
                                    }
                                default:
                                    NdefString = "Unknown RTD";
                                    break;
                            }
                        }
                        else
                        {
                            NdefString = "Unknown RTD";
                        }

                        richTextBoxColorAppend(richTextBox_RW_Tag_Content, NdefString + "\n", Color.Black);

                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];

                        Array.Copy(pui8Data, 3, asciiChars, 0, ui8PayLoadLength);

                        System.IO.BinaryWriter file = new System.IO.BinaryWriter(new FileStream(TI_NFC_Tool_App_Path + "/NDEF.hex", FileMode.Append, FileAccess.Write));

                        //System.IO.BinaryWriter file = new System.IO.BinaryWriter(new FileStream(TI_NFC_Tool_App_Path + "/NDEF" + FileCounterVar + ".hex", FileMode.Append, FileAccess.Write));

                        //FileCounterVar++;

                        foreach (byte bValue in asciiChars)
                        {
                            file.Write(bValue);
                        }

                        file.Close();

                        serialPortDataTextBox.AppendText("NDEF Content Read\n");

                        serialPortDataTextBox.AppendText("Payload Size: ");

                        serialPortDataTextBox.AppendText(String.Format("0x{0:X2}\n", incomingByte[2]));

                        NDefLength += incomingByte[2];
                        serialPortDataTextBox.AppendText(String.Format("Total Bytes Received: 0x{0:X4}\n", NDefLength));
//                        richTextBoxColorAppend(serialPortDataTextBox, "Avg. Data Rate (kBps) : " + ((NDefLength) / TempTime.Subtract(CurrentTime).TotalMilliseconds) + "\n", Color.Green);


                        break;
                    }
                case eNFCControllerCommands.RW_STATUS_DATA:
                    {
                        char[] asciiChars = new char[ascii.GetCharCount(pui8Data, (int)3, (int)ui8PayLoadLength)];
                        ascii.GetChars(pui8Data, (int)3, (int)ui8PayLoadLength, asciiChars, 0);

                        string asciiString = new string(asciiChars);
                        richTextBoxColorAppend(richTextBox_RW_Tag_Content, asciiString, Color.Black);   
                        break;
                    }
                default:
                    richTextBoxColorAppend(serialPortDataTextBox, "Unknown Packet Received\n", Color.Brown);
                    richTextBoxColorAppend(serialPortDataTextBox, String.Format("Unknown Packet Received (Length 0x{0:X2})\n", ui8PayLoadLength), Color.Brown);
                    break;
            }
        }

        //
        // Incoming Command (from the NFC Controller) Parser - checks for SOF, Length, and data of incoming packets
        //
        private void parseSerialCommand()
        {
            lock (serialDataQueue.SyncRoot)
            {
                var bytesToRead = serialDataQueue.Count;
                while (bytesToRead != 0)
                {
                    bytesToRead = bytesToRead - 1;

                    incomingByte[bytesReceived] = (byte)serialDataQueue.Dequeue();

                    // Check SOF
                    if (incomingByte[0] == 0xFE)
                    {
                        bytesReceived++;
                    }

                    // Check if Length Byte received
                    if (bytesReceived == 0x03)
                    {
                        serialPacketLength = incomingByte[2];
                    }

                    if (bytesReceived > 2)
                    {
                        // Check if the complete packet has been received
                        if (bytesReceived == serialPacketLength + 3)
                        {
                            serialPacketLength = bytesReceived;
                            processCommand((eNFCControllerCommands)incomingByte[1], 3, incomingByte, (byte)(serialPacketLength - 3));
                            // Reset Bytes Received
                            bytesReceived = 0;
                        }
                    }

                    // Check for overflow case. 
                    if (bytesReceived == 258)
                    {
                        bytesReceived = 0;
                    }
                }
            }
        }

        //
        // Helper function to append color strings to textBoxes.
        //
        private void richTextBoxColorAppend(RichTextBox textBox, String text, Color stringColor)
        {
            textBox.SelectionLength = 0;
            textBox.SelectionStart = textBox.TextLength;

            textBox.SelectionColor = stringColor;
            textBox.AppendText(text);
            textBox.SelectionColor = textBox.ForeColor;
        }

        //------------------------------- Helper Functions END ----------------------------------//

        //--------------------------- Serial Comport Settings START ----------------------------//

        //
        // Checks for the available Comports available and displays them on the comportBox list.
        //
        private void comboBox1_DropDown(object sender, EventArgs e)
        {
            string[] portnames = SerialPort.GetPortNames();
            comportBox.Items.Clear();

            if (portnames.Count() == 0x00)
                comportBox.Text = "";

            foreach (string port in portnames)
            {
                comportBox.Items.Add(port);
            }
        }

        // Helper connect function
        void manualConnectComport()
        {
            if (comportBox.Text != "")
            {
                if (serialPort1.IsOpen == false)
                {
                    try
                    {
                        serialPort1.BaudRate = Int32.Parse(baudRateBox.Text, System.Globalization.NumberStyles.Any);
                        serialPort1.PortName = comportBox.Text;
                        manualConnectButton.Text = "Disconnect";
                        autoConnectButton.Enabled = false;
                        timeOutCounter = 200;
                        fw_version_flag = false;
                        serialPort1.Open();
                        comportBox.Enabled = false;
                        baudRateBox.Enabled = false;
                        hardwareSelectBox.Enabled = false;
                        groupBox_TRF79xx_Settings.Enabled = false;
                        groupBox_NFC_Compliance_Config.Enabled = false;
                        p2p_mode_enabled = false;
                        ce_mode_enabled = false;
                        rw_mode_enabled = false;
                        nfc_test_mode_enabled = false;
                        gui_connect_flag = true;
                    }
                    catch
                    {
                        manualConnectButton.Text = "Manual Connect";
                        toolStripStatusConnectionLabel.Text = "Disconnected";
                        timeOutCounter = -1;
                        toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.Red;
                        autoConnectButton.Enabled = true;
                        comportBox.Enabled = true;
                        baudRateBox.Enabled = true;
                        hardwareSelectBox.Enabled = true;
                        groupBox_P2P_Config.Enabled = false;
                        groupBox_CE_Config.Enabled = false;
                        groupBox_RW_Config.Enabled = false;
                        groupBox_TRF79xx_Settings.Enabled = false;
                        groupBox_NFC_Compliance_Config.Enabled = false;
                        fw_version_flag = false;
                        gui_connect_flag = false;
                        //throw;

                        if (auto_connect_flag == false)
                        {
                            MessageBox.Show("Unable to connect to selected COM Port.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
                        }
                    }
                    if (serialPort1.IsOpen == true)
                    {
                        SendCommand(null, eNFCHostCommands.COMM_START, 0);
                    }
                }
                else
                {
                    SendCommand(null, eNFCHostCommands.COMM_END, 0);
                    manualConnectButton.Text = "Manual Connect";
                    toolStripStatusConnectionLabel.Text = "Disconnected";
                    timeOutCounter = -1;
                    toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.Red;
                    toolStripStatusLabelFWVer.Text = "FW Version ";
                    autoConnectButton.Enabled = true;
                    comportBox.Enabled = true;
                    baudRateBox.Enabled = true;
                    hardwareSelectBox.Enabled = true;
                    groupBox_P2P_Config.Enabled = false;
                    groupBox_CE_Config.Enabled = false;
                    groupBox_RW_Config.Enabled = false;
                    groupBox_TRF79xx_Settings.Enabled = false;
                    groupBox_NFC_Compliance_Config.Enabled = false;
                    fw_version_flag = false;
                    gui_connect_flag = false;
                    try
                    {
                        serialPort1.Close();
                    }
                    catch
                    {
                        //throw;
                    }
                }
            }
            else
            {
                MessageBox.Show("Please select a COM Port.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
            }
        }

        // Helper connect function
        void autoConnectComport()
        {
            if (comportBox.Text != "")
            {
                if (serialPort1.IsOpen == false)
                {
                    try
                    {
                        serialPort1.BaudRate = Int32.Parse(baudRateBox.Text, System.Globalization.NumberStyles.Any);
                        serialPort1.PortName = comportBox.Text;
                        autoConnectButton.Text = "Disconnect";
                        manualConnectButton.Enabled = false;
                        fw_version_flag = false;
                        comportBox.Enabled = true;
                        baudRateBox.Enabled = true;
                        hardwareSelectBox.Enabled = true;
                        timeOutCounter = 200;
                        groupBox_TRF79xx_Settings.Enabled = false;
                        groupBox_NFC_Compliance_Config.Enabled = false;
                        p2p_mode_enabled = false;
                        ce_mode_enabled = false;
                        rw_mode_enabled = false;
                        nfc_test_mode_enabled = false;
                        gui_connect_flag = true;
                        serialPort1.Open();
                    }
                    catch
                    {
                        autoConnectButton.Text = "Auto Connect";
                        toolStripStatusConnectionLabel.Text = "Disconnected";
                        timeOutCounter = -1;
                        toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.Red;
                        manualConnectButton.Enabled = true;
                        comportBox.Enabled = true;
                        baudRateBox.Enabled = true;
                        hardwareSelectBox.Enabled = true; 
                        groupBox_P2P_Config.Enabled = false;
                        groupBox_CE_Config.Enabled = false;
                        groupBox_RW_Config.Enabled = false;
                        groupBox_TRF79xx_Settings.Enabled = false;
                        groupBox_NFC_Compliance_Config.Enabled = false;
                        fw_version_flag = false;
                        gui_connect_flag = false;
                        //throw;
                    }
                    if (serialPort1.IsOpen == true)
                    {
                        SendCommand(null, eNFCHostCommands.COMM_START, 0);
                    }
                }
                else
                {
                    SendCommand(null, eNFCHostCommands.COMM_END, 0);
                    autoConnectButton.Text = "Auto Connect";
                    toolStripStatusConnectionLabel.Text = "Disconnected";
                    timeOutCounter = -1;
                    toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.Red;
                    toolStripStatusLabelFWVer.Text = "FW Version ";
                    manualConnectButton.Enabled = true;
                    comportBox.Enabled = true;
                    baudRateBox.Enabled = true;
                    hardwareSelectBox.Enabled = true;
                    groupBox_P2P_Config.Enabled = false;
                    groupBox_CE_Config.Enabled = false;
                    groupBox_RW_Config.Enabled = false;
                    groupBox_TRF79xx_Settings.Enabled = false;
                    groupBox_NFC_Compliance_Config.Enabled = false;
                    fw_version_flag = false;
                    gui_connect_flag = false;
                    try
                    {
                        serialPort1.Close();
                    }
                    catch
                    {
                        //throw;
                    }
                }
            }
            else
            {
                MessageBox.Show("No COM Port Identified.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
            }
        }

        //
        // Connect / Disconnect button for the Serial Port communication
        //
        private void manualConnectButton_Click(object sender, EventArgs e)
        {
            auto_connect_flag = false;
            manualConnectComport();
        }

        private void autoConnectButton_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen == false)
            {
                autoConnectProcess();
            }
            else
            {
                autoConnectComport();
            }
        }

        // Helper connect function
        void autoConnectProcess()
        {
            List<string> portNameList;
            String hwFriendlyName = "DEFAULT";
            List<string> hwCOMPort;
            string hwCOMPortConnect;
            int hwCOMPortCount;
            int LoopCount = 0;

            MainTabController.SelectedTab = tabNfcConfiguration;

            switch (hardwareSelectBox.Text)
            {
                case "MSP-EXP430F5529LP":
                    hwFriendlyName = "Virtual COM Port";
                    break;
                case "MSP-EXP430F5529":
                    hwFriendlyName = "Virtual COM Port";
                    break;
                case "MSP-EXP432P401R":
                    hwFriendlyName = "XDS110 Class Application/User UART";
                    break;
                default:
                    MessageBox.Show("Please select a connected hardware platform.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
                    break;
            }

            // Checks if a Hardware Friendly Name has been defined
            if (!String.Equals(hwFriendlyName, "DEFAULT"))
            {
                // Get current friendly list name of all comports
                portNameList = findComports();

                // Check if the comport list includes the friendly comport name
                hwCOMPort = portNameList.FindAll(i => i.Contains(hwFriendlyName));

                hwCOMPortCount = hwCOMPort.Count;

                // If a comport friendly name has been found, parse the COM number
                if (hwCOMPortCount > 0)
                {
                    int startIndex;

                    auto_connect_flag = true;
                    gui_connect_flag = false;
                    display_error_flag = true;
                    LoopCount = 0;

                    while (LoopCount < hwCOMPortCount)
                    {
                        startIndex = hwCOMPort[LoopCount].IndexOf("(COM") + 1;

                        hwCOMPortConnect = hwCOMPort[LoopCount].Substring(startIndex, hwCOMPort[LoopCount].Length - startIndex - 1);

                        // Set the current comport text box
                        comportBox.Items.Add(hwCOMPortConnect);
                        comportBox.Text = hwCOMPortConnect;
                        comportBox.Enabled = false;

                        autoConnectComport();

                        // If a connection has been made, check for correct firmware version
                        if (gui_connect_flag == true)
                        {
                            timeOutCounter = 200;
                            while ((fw_version_flag == false) && (gui_connect_flag == true))
                            {
                                parseSerialCommand();
                                Delay(1).Wait();
                                timeOutCounter--;
                                if (timeOutCounter == 0)
                                {
                                    // Disconnect
                                    autoConnectComport();
                                }
                            }
                            timeOutCounter = -1;
                            if (gui_connect_flag == true)
                            {
                                auto_connect_flag = false;
                                display_error_flag = false;
                                MainTabController.SelectedTab = tabNfcConfiguration;
                                break;
                            }
                        }
                        LoopCount++;
                    }

                    if (gui_connect_flag != true)
                    {
                        autoConnectVID();
                    }

                    if (display_error_flag == true)
                    {
                        auto_connect_flag = false;
                        MessageBox.Show("Auto-Connect Failed." + Environment.NewLine + "No device with NFCLink (Standalone) firmware loaded detected.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1);
                    }
                }
                else
                {
                    auto_connect_flag = true;
                    gui_connect_flag = false;
                    display_error_flag = true;

                    autoConnectVID();

                    if (gui_connect_flag != true)
                    {
                        MessageBox.Show("Selected hardware platform not found.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1);
                    }
                }
            }
        }

        private void autoConnectVID()
        {
            List<string> names;
            string hwCOMPortConnect;

            // VID: 0x0451 (Texas Instruments VID)
            names = autoConnectVID("0451");
            if (names.Count > 0)
            {
                foreach (String ComPort in SerialPort.GetPortNames())
                {
                    if (names.Contains(ComPort))
                    {
                        richTextBoxColorAppend(serialPortDataTextBox, "COM Port is: " + ComPort + " VID=0451" + "\n", Color.DarkBlue);

                        hwCOMPortConnect = ComPort;

                        // Set the current comport text box
                        comportBox.Items.Add(hwCOMPortConnect);
                        comportBox.Text = hwCOMPortConnect;
                        comportBox.Enabled = false;

                        autoConnectComport();

                        // If a connection has been made, check for correct firmware version
                        if (gui_connect_flag == true)
                        {
                            timeOutCounter = 200;
                            while ((fw_version_flag == false) && (gui_connect_flag == true))
                            {
                                parseSerialCommand();
                                Delay(1).Wait();
                                timeOutCounter--;
                                if (timeOutCounter == 0)
                                {
                                    // Disconnect
                                    autoConnectComport();
                                }
                            }
                            timeOutCounter = -1;
                            if (gui_connect_flag == true)
                            {
                                auto_connect_flag = false;
                                display_error_flag = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (gui_connect_flag != true)   // Try MSP430 VID since no connection has been made
            {
                // VID: 0x2047 (MSP430 VID)
                names = autoConnectVID("2047");
                if (names.Count > 0)
                {
                    foreach (String ComPort in SerialPort.GetPortNames())
                    {
                        if (names.Contains(ComPort))
                        {
                            richTextBoxColorAppend(serialPortDataTextBox, "COM Port is: " + ComPort + " VID=2047" + "\n", Color.DarkBlue);

                            hwCOMPortConnect = ComPort;

                            // Set the current comport text box
                            comportBox.Items.Add(hwCOMPortConnect);
                            comportBox.Text = hwCOMPortConnect;
                            comportBox.Enabled = false;

                            autoConnectComport();

                            // If a connection has been made, check for correct firmware version
                            if (gui_connect_flag == true)
                            {
                                timeOutCounter = 200;
                                while ((fw_version_flag == false) && (gui_connect_flag == true))
                                {
                                    parseSerialCommand();
                                    Delay(1).Wait();
                                    timeOutCounter--;
                                    if (timeOutCounter == 0)
                                    {
                                        // Disconnect
                                        autoConnectComport();
                                    }
                                }
                                timeOutCounter = -1;
                                if (gui_connect_flag == true)
                                {
                                    auto_connect_flag = false;
                                    display_error_flag = false;
                                    MainTabController.SelectedTab = tabNfcConfiguration;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        private List<string> autoConnectVID(String VID)
        {
            String pattern = String.Format("^VID_{0}", VID);
            Regex _rx = new Regex(pattern, RegexOptions.IgnoreCase);
            List<string> comports = new List<string>();
            RegistryKey rk1 = Registry.LocalMachine;
            RegistryKey rk2 = rk1.OpenSubKey("SYSTEM\\CurrentControlSet\\Enum");
            foreach (String s3 in rk2.GetSubKeyNames())
            {
                RegistryKey rk3 = rk2.OpenSubKey(s3);
                foreach (String s in rk3.GetSubKeyNames())
                {
                    if (_rx.Match(s).Success)
                    {
                        RegistryKey rk4 = rk3.OpenSubKey(s);
                        foreach (String s2 in rk4.GetSubKeyNames())
                        {
                            RegistryKey rk5 = rk4.OpenSubKey(s2);
                            RegistryKey rk6 = rk5.OpenSubKey("Device Parameters");
                            comports.Add((string)rk6.GetValue("PortName"));
                        }
                    }
                }
            }
            return comports;
        }

        //
        // Serial Port Receive Threat - Receives incoming data and stores it inside serialDataQueue
        //
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            int bytesRxed;

            bytesRxed = serialPort1.BytesToRead;

            Byte[] byteBuffer = new Byte[bytesRxed];

            if (bytesRxed > 0)
            {
                try
                {
                    bytesRxed = serialPort1.Read(byteBuffer, 0, byteBuffer.Length);

                    if (bytesRxed < byteBuffer.Length)
                    {
                        Array.Resize(ref byteBuffer, bytesRxed);
                    }

                    lock (serialDataQueue.SyncRoot)
                    {
                        for (int index = 0; index < bytesRxed; index++)
                        {
                            serialDataQueue.Enqueue(byteBuffer[index]);
                        }
                    }
                }
                catch
                {
                }
            }
        }

        private void openNdefButton_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(TI_NFC_Tool_App_Path);
        }

        //--------------------------- Serial Comport Settings END ------------------------------//

        //----------------------------- NFC Configuration Tab START ----------------------------//

        //
        // Start / Stop P2P Button - when it starts it checks the value of the check boxes to select the P2P modes.
        //
        private void button_P2P_toggle_Click(object sender, EventArgs e)
        {
            Byte[] ui8CurrentMode = new Byte[3];

            if (p2p_mode_enabled == false)
            {
                // Ensure Bits are default to 0
                ui8CurrentMode[0] = 0x00;
                ui8CurrentMode[1] = 0x00;
                ui8CurrentMode[2] = 0x00;

                // ------------ Target Support ------------//
                if (p2p_target_fw_enabled == true)
                {
                    if (checkBox_Pas_Target_106.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x01;
                    }
                    if (checkBox_Pas_Target_212.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x02;
                    }
                    if (checkBox_Pas_Target_424.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x04;
                    }
                    if (checkBox_Act_Target_106.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x08;
                    }
                    if (checkBox_Act_Target_212.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x10;
                    }
                    if (checkBox_Act_Target_424.Checked == true)
                    {
                        ui8CurrentMode[0] |= 0x20;
                    }
                }
                else
                {
                    panel_P2P_Target_Support.Enabled = false;
                }

                // ----------- Initiator Support -----------//
                if (p2p_initiator_fw_enabled == true)
                {
                    if (checkBox_Pas_Initiator_106.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x01;
                    }
                    if (checkBox_Pas_Initiator_212.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x02;
                    }
                    if (checkBox_Pas_Initiator_424.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x04;
                    }
                    if (checkBox_Act_Initiator_106.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x08;
                    }
                    if (checkBox_Act_Initiator_212.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x10;
                    }
                    if (checkBox_Act_Initiator_424.Checked == true)
                    {
                        ui8CurrentMode[1] |= 0x20;
                    }
                }
                else
                {
                    panel_P2P_Initiator_Support.Enabled = false;
                }

                // Check if Any Target Mode Enabled
                if (ui8CurrentMode[0] != 0x00)
                {
                    ui8CurrentMode[2] |= 0x02;
                }
                // Check if Any Initiator Mode Enabled
                if (ui8CurrentMode[1] != 0x00)
                {
                    ui8CurrentMode[2] |= 0x01;
                }

                // Send Start Peer to Peer Command to the NFC Controller
                if (ui8CurrentMode[2] != 0x00)
                {
                    SendCommand(ui8CurrentMode, eNFCHostCommands.P2P_START_CMD, 3);
                    p2p_mode_enabled = true;
                    button_P2P_toggle.Text = "Stop P2P";
                    panel_P2P_Target_Support.Enabled = false;
                    panel_P2P_Initiator_Support.Enabled = false;
                }
                else
                {
                    MessageBox.Show("Please select one of the Target/Initiator Modes.","Help",MessageBoxButtons.OK,MessageBoxIcon.Information,MessageBoxDefaultButton.Button1);
                }
            }
            else if (p2p_mode_enabled == true)
            {
                p2p_mode_enabled = false;
                button_P2P_toggle.Text = "Start P2P";
                if (p2p_target_fw_enabled)
                {
                    panel_P2P_Target_Support.Enabled = true;
                }
                if (p2p_initiator_fw_enabled)
                {
                    panel_P2P_Initiator_Support.Enabled = true;
                }
                SendCommand(ui8CurrentMode, eNFCHostCommands.P2P_STOP_CMD, 0);
            }
        }

        //
        // Start / Stop CE Button - when it starts it checks the value of the check boxes to select the CE modes.
        //
        private void button_CE_toggle_Click(object sender, EventArgs e)
        {
            Byte[] ui8CurrentMode = new Byte[1];

            if (ce_mode_enabled == false)
            {
                // Ensure Bits are default to 0
                ui8CurrentMode[0] = 0x00;

                // ------------ Card Emulation Modes ------------//
                if (checkBox_CE_T4AT.Checked == true)
                {
                    ui8CurrentMode[0] |= 0x01;
                }
                if (checkBox_CE_T4BT.Checked == true)
                {
                    ui8CurrentMode[0] |= 0x02;
                }

                // Send Start Card Emulation Command to the NFC Controller
                if (ui8CurrentMode[0] != 0x00)
                {
                    SendCommand(ui8CurrentMode, eNFCHostCommands.CE_START_CMD, 1);
                    ce_mode_enabled = true;
                    button_CE_toggle.Text = "Stop CE";
                    panel_CE_Mode_Support.Enabled = false;
                }
                else
                {
                    MessageBox.Show("Please select one of the Card Emulation modes.", "Help", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
                }
            }
            else if (ce_mode_enabled == true)
            {
                ce_mode_enabled = false;
                button_CE_toggle.Text = "Start CE";
                panel_CE_Mode_Support.Enabled = true;
                SendCommand(ui8CurrentMode, eNFCHostCommands.CE_STOP_CMD, 0);
            }
        }

        //
        // Start / Stop RW Button - when it starts it checks the value of the check boxes to select the RW modes.
        //
        private void button_RW_toggle_Click(object sender, EventArgs e)
        {
            // Read Reader/Writer Configuration
            Byte[] ui8CurrentMode = new Byte[3];

            if (rw_mode_enabled == false)
            {
                // Ensure Bits are default to 0
                ui8CurrentMode[0] = 0x00;
                ui8CurrentMode[1] = 0x00;
                ui8CurrentMode[2] = 0x00;

                if (checkBox_RW_NFC_A.Checked)
                {
                    ui8CurrentMode[2] |= 0x01;

                    if (radioButton_NFC_A_848.Checked)
                    {
                        ui8CurrentMode[0] |= 0x0F;
                    }
                    else if (radioButton_NFC_A_424.Checked)
                    {
                        ui8CurrentMode[0] |= 0x07;
                    }
                    else if (radioButton_NFC_A_212.Checked)
                    {
                        ui8CurrentMode[0] |= 0x03;
                    }
                    else if (radioButton_NFC_A_106.Checked)
                    {
                        ui8CurrentMode[0] |= 0x01;
                    }
                }

                if (checkBox_RW_NFC_B.Checked)
                {
                    ui8CurrentMode[2] |= 0x02;

                    if (radioButton_NFC_B_848.Checked)
                    {
                        ui8CurrentMode[0] |= 0xF0;
                    }
                    else if (radioButton_NFC_B_424.Checked)
                    {
                        ui8CurrentMode[0] |= 0x70;
                    }
                    else if (radioButton_NFC_B_212.Checked)
                    {
                        ui8CurrentMode[0] |= 0x30;
                    }
                    else if (radioButton_NFC_B_106.Checked)
                    {
                        ui8CurrentMode[0] |= 0x10;
                    }
                }

                if (checkBox_RW_NFC_F.Checked)
                {
                    ui8CurrentMode[2] |= 0x04;

                    if (checkBox_NFC_F_424.Checked)
                    {
                        ui8CurrentMode[1] |= 0x02;
                    }
                    if (checkBox_NFC_F_212.Checked)
                    {
                        ui8CurrentMode[1] |= 0x01;
                    }
                    if ((ui8CurrentMode[1] & 0x03) == 0x00)
                    {
                        MessageBox.Show("Please select bitrate 212kbps or 424kbps for NFC-F.", "Help", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
                        return;
                    }
                }

                if (checkBox_RW_ISO15693.Checked)
                {
                    ui8CurrentMode[2] |= 0x08;

                    if (radioButton_ISO15693_26_48.Checked)
                    {
                        ui8CurrentMode[1] |= 0x04;
                    }
                }

                // Check if any check box was checked
                if (checkBox_RW_NFC_A.Checked || checkBox_RW_NFC_B.Checked || checkBox_RW_NFC_F.Checked || checkBox_RW_ISO15693.Checked)
                {
                    button_RW_toggle.Text = "Stop RW";
                    panel_RW_Mode_Support.Enabled = false;
                    rw_mode_enabled = true;
                    SendCommand(ui8CurrentMode, eNFCHostCommands.RW_START_CMD, 3);
                }
                else
                {
                    MessageBox.Show("Please select one of the Reader/Writer modes.", "Help", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
                }
            }
            else
            {
                rw_mode_enabled = false;
                button_RW_toggle.Text = "Start RW";
                panel_RW_Mode_Support.Enabled = true;
                SendCommand(ui8CurrentMode, eNFCHostCommands.RW_STOP_CMD, 0);
            }
        }

        private void buttonNFC_Compliance_toggle_Click(object sender, EventArgs e)
        {
            // Read Reader/Writer Configuration
            Byte[] ui8CurrentMode = new Byte[6];
            Byte TempValue;

            if (nfc_test_mode_enabled == false)
            {
                nfc_test_mode_enabled = true;
                panel_P2P_adv_config.Enabled = false;
                panel_RW_adv_config.Enabled = false;
                // Certification Enabled
                ui8CurrentMode[0] = 0x01;
                buttonNFC_Compliance_toggle.Text = "Stop NFC Testing";
            }
            else
            {
                nfc_test_mode_enabled = false;
                panel_P2P_adv_config.Enabled = true;
                panel_RW_adv_config.Enabled = true;
                // Certification Disabled
                ui8CurrentMode[0] = 0x00;
                buttonNFC_Compliance_toggle.Text = "Start NFC Testing";
            }

            // P2P Options
            
            // ATN
            TempValue = Byte.Parse(comboBox_P2P_ATN_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[1] = (Byte)((TempValue << 2) & 0x1C);

            // NACKs
            TempValue = Byte.Parse(comboBox_P2P_NACK_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[1] |= (Byte)((TempValue << 5) & 0xE0);

            //
            // LLCP Enabled
            //
            if(checkBox_LLCP_Enable.Checked)
            {
                ui8CurrentMode[1] |= 0x01;
            }

            //
            // NFC DEP Loopback Field
            //
            if(checkBox_NFC_DEP_Loopback.Checked)
            {
                ui8CurrentMode[1] |= 0x02;
            }

            // NFC DEP DID
            ui8CurrentMode[2] = Byte.Parse(comboBox_P2P_DID.Text, System.Globalization.NumberStyles.Any);

            // RW Options

            // NACK
            TempValue = Byte.Parse(comboBox_RW_NACK_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[3] = (Byte)(TempValue & 0x07);

            // DSL
            TempValue = Byte.Parse(comboBox_RW_DSL_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[3] |= (Byte) ((TempValue << 3) & 0x38);

            // RW ACK
            TempValue = Byte.Parse(comboBox_RW_ACK_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[3] |= (Byte)((TempValue << 6) & 0xC0);

            // RW ACK
            ui8CurrentMode[4] = (Byte)((TempValue >> 2) & 0x01);

            // RW WTX 
            TempValue = Byte.Parse(comboBox_RW_WTX_Retry.Text, System.Globalization.NumberStyles.Any);
            ui8CurrentMode[4] |= (Byte)((TempValue << 1) & 0x0E);

            // ISO DEP DID
            ui8CurrentMode[5] = Byte.Parse(comboBox_RW_DID.Text, System.Globalization.NumberStyles.Any);

            SendCommand(ui8CurrentMode, eNFCHostCommands.NFC_TEST_CONFIG, 6);
        }

        private void button_TRF_Update_Click(object sender, EventArgs e)
        {
            // Read Reader/Writer Configuration
            Byte[] ui8CurrentMode = new Byte[6];
            Int16 ListenTime;

            // Version Number
            if (comboBox_TRF_Version.Text.Equals("70A"))
            {
                ui8CurrentMode[0] = 0x01;
            }

            // Listen Time
            if (System.Text.RegularExpressions.Regex.IsMatch(comboBox_Loop_Delay.Text, @"\A\b[0-9]+\b\Z"))
            {
                if (Int16.TryParse(comboBox_Loop_Delay.Text, out ListenTime))
                {
                    if (ListenTime < 2001)
                    {
                        //                Int16 ListenTime = Int16.Parse(comboBox_Loop_Delay.Text, System.Globalization.NumberStyles.Any);
                        ui8CurrentMode[1] = (Byte)(ListenTime & 0xFF);
                        ui8CurrentMode[2] = (Byte)(ListenTime >> 8);
                    }
                    else
                    {
                        MessageBox.Show("Please enter valid listen time (0-2000)", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
                    }
                }
                else
                {
                    MessageBox.Show("Please enter valid listen time (0-2000)", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
                }
            }
            else
            {
                MessageBox.Show("Please enter valid listen time (0-2000)", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
            }

            // 5V Power Supply
            if (radioButton_TRF_5V_supply.Checked)
            {
                ui8CurrentMode[3] = 0x01;
            }
            else
            {
                ui8CurrentMode[3] = 0x00;
            }

            // TRF Ext Amplifier
            if (checkBox_TRF_Ext_Amp.Checked)
            {
                ui8CurrentMode[4] = 0x01;
            }
            else
            {
                ui8CurrentMode[4] = 0x00;
            }

            // Auto SDD
            if (checkBox_TRF_AutoSDD.Checked)
            {
                ui8CurrentMode[5] = 0x01;
            }
            else
            {
                ui8CurrentMode[5] = 0x00;
            }

            SendCommand(ui8CurrentMode, eNFCHostCommands.TRF_SETTINGS, 6);
        }

        //------------------------------ NFC Configuration Tab END -----------------------------//

        //------------------------------------- LLCP Tab START ---------------------------------//

        //
        // Helper function to check for Hex characters inside a string - Used by LLCP Tab
        //
        public bool OnlyHexInString(string test)
        {
            // For C-style hex notation (0xFF) you can use @"\A\b(0[xX])?[0-9a-fA-F]+\b\Z"
            return System.Text.RegularExpressions.Regex.IsMatch(test, @"\A\b[0-9a-fA-F]+\b\Z");
        }

        //---------------------------------- LLCP Tab END -------------------------------------//

        //----------------------------------  COM Log Tab START -------------------------------//

        //
        // Clears the Log textbox
        //
        private void button_Clear_Log_Click(object sender, EventArgs e)
        {
            serialPortDataTextBox.Text = "";
        }

        //
        // Helper function to check for Return key, to perform the Save action.
        //
        private void fileNameTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
            {
                button_Save_Log.PerformClick();
            }
        }

        //
        // Helper function to keep the log textbox updated
        //
        private void serialPortDataTextBox_TextChanged(object sender, EventArgs e)
        {
            serialPortDataTextBox.ScrollToCaret();
        }

        //
        // Save Log File to location provided by the user
        //
        private void saveFileDialogCOMLog_FileOk(object sender, CancelEventArgs e)
        {
            System.IO.StreamWriter file = new System.IO.StreamWriter(saveFileDialogCOMLog.FileName, true);

            char[] delimiterChars = { '\n' };
            string[] dataB = serialPortDataTextBox.Text.Split(delimiterChars);
            foreach (String line in dataB)
            {
                file.WriteLine(line);
            }

            file.Close();

            Process.Start(saveFileDialogCOMLog.FileName);
        }

        //
        // Click Save Button on the COM Log Tab
        //
        private void buttonSaveLog_Click(object sender, EventArgs e)
        {
            saveFileDialogCOMLog.ShowDialog();
        }

        //
        // Click Open Button on the COM Log Tab
        //
        private void buttonOpenLog_Click(object sender, EventArgs e)
        {
            openFileDialogCOMLog.ShowDialog();
        }

        //
        // Click on the COM Port objects listed in the COM Log Tab
        //
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem != null)
            {
                MessageBox.Show(listBox1.SelectedItem.ToString());
            }
        }

        //
        // Open Log File to location provided by the user
        //
        private void openFileDialogCOMLog_FileOk(object sender, CancelEventArgs e)
        {
            serialPortDataTextBox.Text = System.IO.File.ReadAllText(openFileDialogCOMLog.FileName);

            // Read the file as one string.
            System.IO.StreamReader myFile =
               new System.IO.StreamReader(openFileDialogCOMLog.FileName);
            serialPortDataTextBox.Text = myFile.ReadToEnd();
        }

        //----------------------------------  COM Log Tab END - ---------------------------------//

        //---------------------------------- Advance COM Tab START ------------------------------//

        //
        // Refresh Comport button - Advance COM Tab
        //
        private void button_Refresh_COM_Click(object sender, EventArgs e)
        {
            updateComports();
        }

        // Helper Function 
        List<string> findComports()
        {
             List<string> portNameList = new List<string>();

             using (var searcher = new ManagementObjectSearcher
                 ("SELECT * FROM Win32_PnPEntity"))
             {
                 string[] portnames = SerialPort.GetPortNames();
                 var ports = searcher.Get().Cast<ManagementBaseObject>().ToList();

                 foreach (ManagementBaseObject queryObj in ports)
                 {
                     foreach (string comObj in portnames)
                     {

                         if (queryObj["Name"] != null && queryObj["Name"].ToString().Contains(comObj))
                         {
                             portNameList.Add(queryObj["Name"].ToString());
                             break;
                         }
                     }
                 }

                 // Check if list is empty
                 if (!portNameList.Any())
                 {
                     portNameList.Add("No available COM device.");
                 }
             }

            return portNameList;
        }

        //
        // Update Comport List in the Advance COM Tab
        //
        private void updateComports()
        {
            listBox1.DataSource = findComports();
        }

        //--------------------------------- Advance COM Tab END --------------------------------//

        private void openNdefButton_MouseHover(object sender, EventArgs e)
        {
            System.Windows.Forms.ToolTip ToolTip1 = new System.Windows.Forms.ToolTip();
            ToolTip1.IsBalloon = true;
            ToolTip1.InitialDelay = 400;
            ToolTip1.SetToolTip(this.openNdefButton, "Open location in file system for NDEF.hex file.\nFile shall contain any Received Data from Peer-to-Peer and Reader/Writer modes.");
        }

    }
}




