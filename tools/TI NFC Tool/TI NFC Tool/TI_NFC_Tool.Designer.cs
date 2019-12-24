namespace TI_NFC_Tool
{
    partial class TI_NFC_Tool
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TI_NFC_Tool));
            this.button_Refresh_COM = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.MainTabController = new System.Windows.Forms.TabControl();
            this.tabNfcConfiguration = new System.Windows.Forms.TabPage();
            this.groupBox_TRF79xx_Settings = new System.Windows.Forms.GroupBox();
            this.comboBox_Loop_Delay = new System.Windows.Forms.ComboBox();
            this.comboBox_TRF_Version = new System.Windows.Forms.ComboBox();
            this.label_Trf_Listen_Time = new System.Windows.Forms.Label();
            this.checkBox_TRF_Ext_Amp = new System.Windows.Forms.CheckBox();
            this.Label_Trf_Vcc_Setting = new System.Windows.Forms.Label();
            this.button_TRF_Update = new System.Windows.Forms.Button();
            this.radioButton_TRF_5V_supply = new System.Windows.Forms.RadioButton();
            this.radioButton_TRF_3V_supply = new System.Windows.Forms.RadioButton();
            this.checkBox_TRF_AutoSDD = new System.Windows.Forms.CheckBox();
            this.label_Trf_Version_No = new System.Windows.Forms.Label();
            this.groupBox_RW_Config = new System.Windows.Forms.GroupBox();
            this.label_RW_Config_Help_Msg = new System.Windows.Forms.Label();
            this.button_RW_toggle = new System.Windows.Forms.Button();
            this.panel_RW_Mode_Support = new System.Windows.Forms.Panel();
            this.checkBox_NFC_F_424 = new System.Windows.Forms.CheckBox();
            this.checkBox_NFC_F_212 = new System.Windows.Forms.CheckBox();
            this.panel_RW_NFC_V = new System.Windows.Forms.Panel();
            this.radioButton_ISO15693_26_48 = new System.Windows.Forms.RadioButton();
            this.panel_RW_NFC_B = new System.Windows.Forms.Panel();
            this.radioButton_NFC_B_106 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_B_212 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_B_424 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_B_848 = new System.Windows.Forms.RadioButton();
            this.panel_RW_NFC_A = new System.Windows.Forms.Panel();
            this.radioButton_NFC_A_106 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_A_212 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_A_424 = new System.Windows.Forms.RadioButton();
            this.radioButton_NFC_A_848 = new System.Windows.Forms.RadioButton();
            this.checkBox_RW_ISO15693 = new System.Windows.Forms.CheckBox();
            this.checkBox_RW_NFC_F = new System.Windows.Forms.CheckBox();
            this.Label_Reader_Writer_Modes = new System.Windows.Forms.Label();
            this.checkBox_RW_NFC_B = new System.Windows.Forms.CheckBox();
            this.checkBox_RW_NFC_A = new System.Windows.Forms.CheckBox();
            this.groupBox_CE_Config = new System.Windows.Forms.GroupBox();
            this.button_CE_toggle = new System.Windows.Forms.Button();
            this.panel_CE_Mode_Support = new System.Windows.Forms.Panel();
            this.Label_Card_Emulation_Modes = new System.Windows.Forms.Label();
            this.checkBox_CE_T4BT = new System.Windows.Forms.CheckBox();
            this.checkBox_CE_T4AT = new System.Windows.Forms.CheckBox();
            this.groupBox_NFC_Compliance_Config = new System.Windows.Forms.GroupBox();
            this.buttonNFC_Compliance_toggle = new System.Windows.Forms.Button();
            this.panel_RW_adv_config = new System.Windows.Forms.Panel();
            this.Label_RW_Adv_Config = new System.Windows.Forms.Label();
            this.comboBox_RW_DSL_Retry = new System.Windows.Forms.ComboBox();
            this.label_RW_DSL_Retry = new System.Windows.Forms.Label();
            this.comboBox_RW_ACK_Retry = new System.Windows.Forms.ComboBox();
            this.label_RW_ACK_Retry = new System.Windows.Forms.Label();
            this.comboBox_RW_DID = new System.Windows.Forms.ComboBox();
            this.label_RW_DID = new System.Windows.Forms.Label();
            this.comboBox_RW_WTX_Retry = new System.Windows.Forms.ComboBox();
            this.comboBox_RW_NACK_Retry = new System.Windows.Forms.ComboBox();
            this.label_RW_WTX_Retry = new System.Windows.Forms.Label();
            this.label_RW_NACK_Retry = new System.Windows.Forms.Label();
            this.panel_P2P_adv_config = new System.Windows.Forms.Panel();
            this.comboBox_P2P_DID = new System.Windows.Forms.ComboBox();
            this.label_P2P_DID = new System.Windows.Forms.Label();
            this.comboBox_P2P_NACK_Retry = new System.Windows.Forms.ComboBox();
            this.comboBox_P2P_ATN_Retry = new System.Windows.Forms.ComboBox();
            this.label_P2P_NACK_Retry = new System.Windows.Forms.Label();
            this.label_P2P_ATN_Retry = new System.Windows.Forms.Label();
            this.checkBox_NFC_DEP_Loopback = new System.Windows.Forms.CheckBox();
            this.checkBox_LLCP_Enable = new System.Windows.Forms.CheckBox();
            this.Label_P2P_Adv_Config = new System.Windows.Forms.Label();
            this.groupBox_P2P_Config = new System.Windows.Forms.GroupBox();
            this.button_P2P_toggle = new System.Windows.Forms.Button();
            this.panel_P2P_Target_Support = new System.Windows.Forms.Panel();
            this.checkBox_Act_Target_424 = new System.Windows.Forms.CheckBox();
            this.Label_Target = new System.Windows.Forms.Label();
            this.checkBox_Act_Target_212 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Target_106 = new System.Windows.Forms.CheckBox();
            this.checkBox_Act_Target_106 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Target_212 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Target_424 = new System.Windows.Forms.CheckBox();
            this.panel_P2P_Initiator_Support = new System.Windows.Forms.Panel();
            this.checkBox_Act_Initiator_424 = new System.Windows.Forms.CheckBox();
            this.Label_Initiator = new System.Windows.Forms.Label();
            this.checkBox_Act_Initiator_212 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Initiator_106 = new System.Windows.Forms.CheckBox();
            this.checkBox_Act_Initiator_106 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Initiator_212 = new System.Windows.Forms.CheckBox();
            this.checkBox_Pas_Initiator_424 = new System.Windows.Forms.CheckBox();
            this.tabPeerToPeer = new System.Windows.Forms.TabPage();
            this.groupBox_P2P_Status = new System.Windows.Forms.GroupBox();
            this.textBox_P2P_Mode = new System.Windows.Forms.TextBox();
            this.textBox_P2P_Technology = new System.Windows.Forms.TextBox();
            this.label_P2P_Receive = new System.Windows.Forms.Label();
            this.label_P2P_Mode = new System.Windows.Forms.Label();
            this.label_P2P_Technology = new System.Windows.Forms.Label();
            this.textBox_P2P_Rssi = new System.Windows.Forms.TextBox();
            this.textBox_P2P_Received = new System.Windows.Forms.TextBox();
            this.label_P2P_Bitrate = new System.Windows.Forms.Label();
            this.textBox_P2P_Bitrate = new System.Windows.Forms.TextBox();
            this.label_P2P_Rssi = new System.Windows.Forms.Label();
            this.groupBox_P2P_Rx = new System.Windows.Forms.GroupBox();
            this.richTextBox_P2P_Receive = new System.Windows.Forms.RichTextBox();
            this.groupBox_P2P_Tx = new System.Windows.Forms.GroupBox();
            this.label_P2P_Max_Size_Msg = new System.Windows.Forms.Label();
            this.button_P2P_Send = new System.Windows.Forms.Button();
            this.tabControl_P2P_Transmit = new System.Windows.Forms.TabControl();
            this.tabP2PRTDText = new System.Windows.Forms.TabPage();
            this.textBox_P2P_RTD_Text_Counter = new System.Windows.Forms.TextBox();
            this.label_P2P_RTD_Text = new System.Windows.Forms.Label();
            this.richTextBox_P2P_RTD_Text = new System.Windows.Forms.RichTextBox();
            this.tabP2PRTDURI = new System.Windows.Forms.TabPage();
            this.richTextBox_P2P_RTD_URI = new System.Windows.Forms.RichTextBox();
            this.label_P2P_RTD_URI = new System.Windows.Forms.Label();
            this.tabCardEmulation = new System.Windows.Forms.TabPage();
            this.groupBox_CE_Emulated_Content = new System.Windows.Forms.GroupBox();
            this.label_CE_Max_Size_Msg = new System.Windows.Forms.Label();
            this.button_CE_Emulate = new System.Windows.Forms.Button();
            this.tabControl_CE_Emulated_Content = new System.Windows.Forms.TabControl();
            this.tabCERTDText = new System.Windows.Forms.TabPage();
            this.textBox_CE_RTD_Text_Counter = new System.Windows.Forms.TextBox();
            this.label_CE_RTD_Text = new System.Windows.Forms.Label();
            this.richTextBox_CE_RTD_Text = new System.Windows.Forms.RichTextBox();
            this.tabCERTDURI = new System.Windows.Forms.TabPage();
            this.richTextBox_CE_RTD_URI = new System.Windows.Forms.RichTextBox();
            this.label_CE_RTD_URI = new System.Windows.Forms.Label();
            this.groupBox_CE_Read_Write_Status = new System.Windows.Forms.GroupBox();
            this.richTextBox_CE_Status = new System.Windows.Forms.RichTextBox();
            this.groupBox_CE_Status = new System.Windows.Forms.GroupBox();
            this.textBox_CE_Tag_Type = new System.Windows.Forms.TextBox();
            this.textBox_CE_Technology = new System.Windows.Forms.TextBox();
            this.label_CE_Rssi = new System.Windows.Forms.Label();
            this.label_CE_Tag_Type = new System.Windows.Forms.Label();
            this.label_CE_Technology = new System.Windows.Forms.Label();
            this.textBox_CE_UID = new System.Windows.Forms.TextBox();
            this.textBox_CE_Rssi = new System.Windows.Forms.TextBox();
            this.label_CE_Bitrate = new System.Windows.Forms.Label();
            this.textBox_CE_Bitrate = new System.Windows.Forms.TextBox();
            this.label_CE_UID = new System.Windows.Forms.Label();
            this.tabReaderWriter = new System.Windows.Forms.TabPage();
            this.groupBox_RW_Tag_Write = new System.Windows.Forms.GroupBox();
            this.button_RW_Write = new System.Windows.Forms.Button();
            this.button_RW_TagFormat = new System.Windows.Forms.Button();
            this.tabControl_RW_Tag_Write = new System.Windows.Forms.TabControl();
            this.tabRWRTDText = new System.Windows.Forms.TabPage();
            this.textBox_RW_RTD_Text_Counter = new System.Windows.Forms.TextBox();
            this.label_RW_RTD_Text = new System.Windows.Forms.Label();
            this.richTextBox_RW_RTD_Text = new System.Windows.Forms.RichTextBox();
            this.tabRWRTDURI = new System.Windows.Forms.TabPage();
            this.richTextBox_RW_RTD_URI = new System.Windows.Forms.RichTextBox();
            this.label_RW_RTD_URI = new System.Windows.Forms.Label();
            this.groupBox_RW_Tag_Content = new System.Windows.Forms.GroupBox();
            this.richTextBox_RW_Tag_Content = new System.Windows.Forms.RichTextBox();
            this.groupBox_RW_Status = new System.Windows.Forms.GroupBox();
            this.textBox_RW_Tag_Type = new System.Windows.Forms.TextBox();
            this.textBox_RW_Technology = new System.Windows.Forms.TextBox();
            this.label_RW_Rssi = new System.Windows.Forms.Label();
            this.label_RW_Tag_Type = new System.Windows.Forms.Label();
            this.label_RW_Technology = new System.Windows.Forms.Label();
            this.textBox_RW_UID = new System.Windows.Forms.TextBox();
            this.textBox_RW_Rssi = new System.Windows.Forms.TextBox();
            this.label_RW_Bitrate = new System.Windows.Forms.Label();
            this.textBox_RW_Bitrate = new System.Windows.Forms.TextBox();
            this.label_RW_UID = new System.Windows.Forms.Label();
            this.tabCOMLog = new System.Windows.Forms.TabPage();
            this.button_Open_Log = new System.Windows.Forms.Button();
            this.button_Save_Log = new System.Windows.Forms.Button();
            this.button_Clear_Log = new System.Windows.Forms.Button();
            this.serialPortDataTextBox = new System.Windows.Forms.RichTextBox();
            this.tabAdvancedCOM = new System.Windows.Forms.TabPage();
            this.tabHWGuide = new System.Windows.Forms.TabPage();
            this.label_Hardware_BP_LEDs = new System.Windows.Forms.Label();
            this.pictureBox_BP_LP_Board = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_LEDs = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_LED3 = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_LED1 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_LED2 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_RF_Field = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_GUI = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_RX = new System.Windows.Forms.Label();
            this.label_Hardware_Exp_TX = new System.Windows.Forms.Label();
            this.pictureBox_Exp_Board = new System.Windows.Forms.PictureBox();
            this.label_RW_Max_Size_Msg = new System.Windows.Forms.Label();
            this.baudRateBox = new System.Windows.Forms.ComboBox();
            this.Label_BaudRate = new System.Windows.Forms.Label();
            this.Label_COM = new System.Windows.Forms.Label();
            this.comportBox = new System.Windows.Forms.ComboBox();
            this.manualConnectButton = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.saveFileDialogCOMLog = new System.Windows.Forms.SaveFileDialog();
            this.openFileDialogCOMLog = new System.Windows.Forms.OpenFileDialog();
            this.timerBackground = new System.Windows.Forms.Timer(this.components);
            this.statusStripConnectionStatus = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusConnectionLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabelFWVer = new System.Windows.Forms.ToolStripStatusLabel();
            this.hardwareSelectBox = new System.Windows.Forms.ComboBox();
            this.Label_Hardware = new System.Windows.Forms.Label();
            this.autoConnectButton = new System.Windows.Forms.Button();
            this.pictureBox_TI_Logo = new System.Windows.Forms.PictureBox();
            this.openNdefButton = new System.Windows.Forms.Button();
            this.MainTabController.SuspendLayout();
            this.tabNfcConfiguration.SuspendLayout();
            this.groupBox_TRF79xx_Settings.SuspendLayout();
            this.groupBox_RW_Config.SuspendLayout();
            this.panel_RW_Mode_Support.SuspendLayout();
            this.panel_RW_NFC_V.SuspendLayout();
            this.panel_RW_NFC_B.SuspendLayout();
            this.panel_RW_NFC_A.SuspendLayout();
            this.groupBox_CE_Config.SuspendLayout();
            this.panel_CE_Mode_Support.SuspendLayout();
            this.groupBox_NFC_Compliance_Config.SuspendLayout();
            this.panel_RW_adv_config.SuspendLayout();
            this.panel_P2P_adv_config.SuspendLayout();
            this.groupBox_P2P_Config.SuspendLayout();
            this.panel_P2P_Target_Support.SuspendLayout();
            this.panel_P2P_Initiator_Support.SuspendLayout();
            this.tabPeerToPeer.SuspendLayout();
            this.groupBox_P2P_Status.SuspendLayout();
            this.groupBox_P2P_Rx.SuspendLayout();
            this.groupBox_P2P_Tx.SuspendLayout();
            this.tabControl_P2P_Transmit.SuspendLayout();
            this.tabP2PRTDText.SuspendLayout();
            this.tabP2PRTDURI.SuspendLayout();
            this.tabCardEmulation.SuspendLayout();
            this.groupBox_CE_Emulated_Content.SuspendLayout();
            this.tabControl_CE_Emulated_Content.SuspendLayout();
            this.tabCERTDText.SuspendLayout();
            this.tabCERTDURI.SuspendLayout();
            this.groupBox_CE_Read_Write_Status.SuspendLayout();
            this.groupBox_CE_Status.SuspendLayout();
            this.tabReaderWriter.SuspendLayout();
            this.groupBox_RW_Tag_Write.SuspendLayout();
            this.tabControl_RW_Tag_Write.SuspendLayout();
            this.tabRWRTDText.SuspendLayout();
            this.tabRWRTDURI.SuspendLayout();
            this.groupBox_RW_Tag_Content.SuspendLayout();
            this.groupBox_RW_Status.SuspendLayout();
            this.tabCOMLog.SuspendLayout();
            this.tabAdvancedCOM.SuspendLayout();
            this.tabHWGuide.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_BP_LP_Board)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Exp_Board)).BeginInit();
            this.statusStripConnectionStatus.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_TI_Logo)).BeginInit();
            this.SuspendLayout();
            // 
            // button_Refresh_COM
            // 
            this.button_Refresh_COM.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.button_Refresh_COM.Location = new System.Drawing.Point(6, 8);
            this.button_Refresh_COM.Name = "button_Refresh_COM";
            this.button_Refresh_COM.Size = new System.Drawing.Size(932, 28);
            this.button_Refresh_COM.TabIndex = 10;
            this.button_Refresh_COM.Text = "Refresh Available COM";
            this.button_Refresh_COM.UseVisualStyleBackColor = true;
            this.button_Refresh_COM.Click += new System.EventHandler(this.button_Refresh_COM_Click);
            // 
            // listBox1
            // 
            this.listBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 25;
            this.listBox1.Location = new System.Drawing.Point(6, 42);
            this.listBox1.Name = "listBox1";
            this.listBox1.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.listBox1.Size = new System.Drawing.Size(932, 454);
            this.listBox1.TabIndex = 11;
            this.listBox1.DoubleClick += new System.EventHandler(this.listBox1_DoubleClick);
            // 
            // splitter1
            // 
            this.splitter1.Location = new System.Drawing.Point(0, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(3, 682);
            this.splitter1.TabIndex = 12;
            this.splitter1.TabStop = false;
            // 
            // MainTabController
            // 
            this.MainTabController.Controls.Add(this.tabNfcConfiguration);
            this.MainTabController.Controls.Add(this.tabPeerToPeer);
            this.MainTabController.Controls.Add(this.tabCardEmulation);
            this.MainTabController.Controls.Add(this.tabReaderWriter);
            this.MainTabController.Controls.Add(this.tabCOMLog);
            this.MainTabController.Controls.Add(this.tabAdvancedCOM);
            this.MainTabController.Controls.Add(this.tabHWGuide);
            this.MainTabController.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MainTabController.Location = new System.Drawing.Point(9, 65);
            this.MainTabController.Name = "MainTabController";
            this.MainTabController.Padding = new System.Drawing.Point(12, 6);
            this.MainTabController.SelectedIndex = 0;
            this.MainTabController.Size = new System.Drawing.Size(963, 587);
            this.MainTabController.TabIndex = 13;
            // 
            // tabNfcConfiguration
            // 
            this.tabNfcConfiguration.BackColor = System.Drawing.Color.LightGray;
            this.tabNfcConfiguration.Controls.Add(this.groupBox_TRF79xx_Settings);
            this.tabNfcConfiguration.Controls.Add(this.groupBox_RW_Config);
            this.tabNfcConfiguration.Controls.Add(this.groupBox_CE_Config);
            this.tabNfcConfiguration.Controls.Add(this.groupBox_NFC_Compliance_Config);
            this.tabNfcConfiguration.Controls.Add(this.groupBox_P2P_Config);
            this.tabNfcConfiguration.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabNfcConfiguration.Location = new System.Drawing.Point(4, 31);
            this.tabNfcConfiguration.Name = "tabNfcConfiguration";
            this.tabNfcConfiguration.Size = new System.Drawing.Size(955, 552);
            this.tabNfcConfiguration.TabIndex = 4;
            this.tabNfcConfiguration.Text = "NFC Configuration";
            // 
            // groupBox_TRF79xx_Settings
            // 
            this.groupBox_TRF79xx_Settings.BackColor = System.Drawing.Color.LightGray;
            this.groupBox_TRF79xx_Settings.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox_TRF79xx_Settings.Controls.Add(this.comboBox_Loop_Delay);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.comboBox_TRF_Version);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.label_Trf_Listen_Time);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.checkBox_TRF_Ext_Amp);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.Label_Trf_Vcc_Setting);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.button_TRF_Update);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.radioButton_TRF_5V_supply);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.radioButton_TRF_3V_supply);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.checkBox_TRF_AutoSDD);
            this.groupBox_TRF79xx_Settings.Controls.Add(this.label_Trf_Version_No);
            this.groupBox_TRF79xx_Settings.Enabled = false;
            this.groupBox_TRF79xx_Settings.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_TRF79xx_Settings.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_TRF79xx_Settings.Location = new System.Drawing.Point(686, 279);
            this.groupBox_TRF79xx_Settings.Name = "groupBox_TRF79xx_Settings";
            this.groupBox_TRF79xx_Settings.Size = new System.Drawing.Size(255, 258);
            this.groupBox_TRF79xx_Settings.TabIndex = 24;
            this.groupBox_TRF79xx_Settings.TabStop = false;
            this.groupBox_TRF79xx_Settings.Text = "TRF79xx Settings";
            // 
            // comboBox_Loop_Delay
            // 
            this.comboBox_Loop_Delay.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_Loop_Delay.FormattingEnabled = true;
            this.comboBox_Loop_Delay.Items.AddRange(new object[] {
            "0",
            "250",
            "500",
            "750",
            "1000"});
            this.comboBox_Loop_Delay.Location = new System.Drawing.Point(169, 65);
            this.comboBox_Loop_Delay.Name = "comboBox_Loop_Delay";
            this.comboBox_Loop_Delay.Size = new System.Drawing.Size(68, 24);
            this.comboBox_Loop_Delay.TabIndex = 39;
            this.comboBox_Loop_Delay.Text = "0";
            // 
            // comboBox_TRF_Version
            // 
            this.comboBox_TRF_Version.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_TRF_Version.Enabled = false;
            this.comboBox_TRF_Version.FormattingEnabled = true;
            this.comboBox_TRF_Version.Items.AddRange(new object[] {
            "70A"});
            this.comboBox_TRF_Version.Location = new System.Drawing.Point(170, 34);
            this.comboBox_TRF_Version.Name = "comboBox_TRF_Version";
            this.comboBox_TRF_Version.Size = new System.Drawing.Size(68, 26);
            this.comboBox_TRF_Version.TabIndex = 30;
            // 
            // label_Trf_Listen_Time
            // 
            this.label_Trf_Listen_Time.AutoSize = true;
            this.label_Trf_Listen_Time.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Trf_Listen_Time.ForeColor = System.Drawing.Color.Maroon;
            this.label_Trf_Listen_Time.Location = new System.Drawing.Point(22, 68);
            this.label_Trf_Listen_Time.Name = "label_Trf_Listen_Time";
            this.label_Trf_Listen_Time.Size = new System.Drawing.Size(141, 16);
            this.label_Trf_Listen_Time.TabIndex = 38;
            this.label_Trf_Listen_Time.Text = "Listen Time (mSec)";
            // 
            // checkBox_TRF_Ext_Amp
            // 
            this.checkBox_TRF_Ext_Amp.AutoSize = true;
            this.checkBox_TRF_Ext_Amp.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_TRF_Ext_Amp.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_TRF_Ext_Amp.Location = new System.Drawing.Point(60, 163);
            this.checkBox_TRF_Ext_Amp.Name = "checkBox_TRF_Ext_Amp";
            this.checkBox_TRF_Ext_Amp.Size = new System.Drawing.Size(103, 20);
            this.checkBox_TRF_Ext_Amp.TabIndex = 35;
            this.checkBox_TRF_Ext_Amp.Text = "Ext Amplifier";
            this.checkBox_TRF_Ext_Amp.UseVisualStyleBackColor = true;
            // 
            // Label_Trf_Vcc_Setting
            // 
            this.Label_Trf_Vcc_Setting.AutoSize = true;
            this.Label_Trf_Vcc_Setting.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Trf_Vcc_Setting.ForeColor = System.Drawing.Color.Maroon;
            this.Label_Trf_Vcc_Setting.Location = new System.Drawing.Point(57, 103);
            this.Label_Trf_Vcc_Setting.Name = "Label_Trf_Vcc_Setting";
            this.Label_Trf_Vcc_Setting.Size = new System.Drawing.Size(38, 16);
            this.Label_Trf_Vcc_Setting.TabIndex = 34;
            this.Label_Trf_Vcc_Setting.Text = "VCC";
            // 
            // button_TRF_Update
            // 
            this.button_TRF_Update.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_TRF_Update.Location = new System.Drawing.Point(71, 213);
            this.button_TRF_Update.Name = "button_TRF_Update";
            this.button_TRF_Update.Size = new System.Drawing.Size(107, 34);
            this.button_TRF_Update.TabIndex = 18;
            this.button_TRF_Update.Text = "Update";
            this.button_TRF_Update.UseVisualStyleBackColor = true;
            this.button_TRF_Update.Click += new System.EventHandler(this.button_TRF_Update_Click);
            // 
            // radioButton_TRF_5V_supply
            // 
            this.radioButton_TRF_5V_supply.AutoSize = true;
            this.radioButton_TRF_5V_supply.Checked = true;
            this.radioButton_TRF_5V_supply.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_TRF_5V_supply.Location = new System.Drawing.Point(155, 101);
            this.radioButton_TRF_5V_supply.Name = "radioButton_TRF_5V_supply";
            this.radioButton_TRF_5V_supply.Size = new System.Drawing.Size(45, 21);
            this.radioButton_TRF_5V_supply.TabIndex = 33;
            this.radioButton_TRF_5V_supply.TabStop = true;
            this.radioButton_TRF_5V_supply.Text = "5V";
            this.radioButton_TRF_5V_supply.UseVisualStyleBackColor = true;
            // 
            // radioButton_TRF_3V_supply
            // 
            this.radioButton_TRF_3V_supply.AutoSize = true;
            this.radioButton_TRF_3V_supply.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_TRF_3V_supply.Location = new System.Drawing.Point(105, 101);
            this.radioButton_TRF_3V_supply.Name = "radioButton_TRF_3V_supply";
            this.radioButton_TRF_3V_supply.Size = new System.Drawing.Size(45, 21);
            this.radioButton_TRF_3V_supply.TabIndex = 32;
            this.radioButton_TRF_3V_supply.Text = "3V";
            this.radioButton_TRF_3V_supply.UseVisualStyleBackColor = true;
            // 
            // checkBox_TRF_AutoSDD
            // 
            this.checkBox_TRF_AutoSDD.AutoSize = true;
            this.checkBox_TRF_AutoSDD.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_TRF_AutoSDD.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_TRF_AutoSDD.Location = new System.Drawing.Point(60, 137);
            this.checkBox_TRF_AutoSDD.Name = "checkBox_TRF_AutoSDD";
            this.checkBox_TRF_AutoSDD.Size = new System.Drawing.Size(90, 20);
            this.checkBox_TRF_AutoSDD.TabIndex = 31;
            this.checkBox_TRF_AutoSDD.Text = "Auto-SDD";
            this.checkBox_TRF_AutoSDD.UseVisualStyleBackColor = true;
            // 
            // label_Trf_Version_No
            // 
            this.label_Trf_Version_No.AutoSize = true;
            this.label_Trf_Version_No.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Trf_Version_No.ForeColor = System.Drawing.Color.Maroon;
            this.label_Trf_Version_No.Location = new System.Drawing.Point(23, 38);
            this.label_Trf_Version_No.Name = "label_Trf_Version_No";
            this.label_Trf_Version_No.Size = new System.Drawing.Size(96, 16);
            this.label_Trf_Version_No.TabIndex = 25;
            this.label_Trf_Version_No.Text = "TRF79xx Ver";
            // 
            // groupBox_RW_Config
            // 
            this.groupBox_RW_Config.BackColor = System.Drawing.Color.LightGray;
            this.groupBox_RW_Config.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox_RW_Config.Controls.Add(this.label_RW_Config_Help_Msg);
            this.groupBox_RW_Config.Controls.Add(this.button_RW_toggle);
            this.groupBox_RW_Config.Controls.Add(this.panel_RW_Mode_Support);
            this.groupBox_RW_Config.Enabled = false;
            this.groupBox_RW_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_RW_Config.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_RW_Config.Location = new System.Drawing.Point(605, 12);
            this.groupBox_RW_Config.Name = "groupBox_RW_Config";
            this.groupBox_RW_Config.Size = new System.Drawing.Size(336, 261);
            this.groupBox_RW_Config.TabIndex = 24;
            this.groupBox_RW_Config.TabStop = false;
            this.groupBox_RW_Config.Text = "TRF7970 RW Configuration";
            // 
            // label_RW_Config_Help_Msg
            // 
            this.label_RW_Config_Help_Msg.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_Config_Help_Msg.ForeColor = System.Drawing.Color.DarkGreen;
            this.label_RW_Config_Help_Msg.Location = new System.Drawing.Point(37, 174);
            this.label_RW_Config_Help_Msg.Name = "label_RW_Config_Help_Msg";
            this.label_RW_Config_Help_Msg.Size = new System.Drawing.Size(261, 42);
            this.label_RW_Config_Help_Msg.TabIndex = 40;
            this.label_RW_Config_Help_Msg.Text = "Present only one NFC/RFID Tag to the RF Field at a time.";
            this.label_RW_Config_Help_Msg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button_RW_toggle
            // 
            this.button_RW_toggle.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_RW_toggle.Location = new System.Drawing.Point(114, 216);
            this.button_RW_toggle.Name = "button_RW_toggle";
            this.button_RW_toggle.Size = new System.Drawing.Size(107, 34);
            this.button_RW_toggle.TabIndex = 18;
            this.button_RW_toggle.Text = "Start RW";
            this.button_RW_toggle.UseVisualStyleBackColor = true;
            this.button_RW_toggle.Click += new System.EventHandler(this.button_RW_toggle_Click);
            // 
            // panel_RW_Mode_Support
            // 
            this.panel_RW_Mode_Support.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_NFC_F_424);
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_NFC_F_212);
            this.panel_RW_Mode_Support.Controls.Add(this.panel_RW_NFC_V);
            this.panel_RW_Mode_Support.Controls.Add(this.panel_RW_NFC_B);
            this.panel_RW_Mode_Support.Controls.Add(this.panel_RW_NFC_A);
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_RW_ISO15693);
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_RW_NFC_F);
            this.panel_RW_Mode_Support.Controls.Add(this.Label_Reader_Writer_Modes);
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_RW_NFC_B);
            this.panel_RW_Mode_Support.Controls.Add(this.checkBox_RW_NFC_A);
            this.panel_RW_Mode_Support.Location = new System.Drawing.Point(6, 22);
            this.panel_RW_Mode_Support.Name = "panel_RW_Mode_Support";
            this.panel_RW_Mode_Support.Size = new System.Drawing.Size(324, 151);
            this.panel_RW_Mode_Support.TabIndex = 10;
            // 
            // checkBox_NFC_F_424
            // 
            this.checkBox_NFC_F_424.AutoSize = true;
            this.checkBox_NFC_F_424.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_NFC_F_424.ForeColor = System.Drawing.Color.Maroon;
            this.checkBox_NFC_F_424.Location = new System.Drawing.Point(152, 84);
            this.checkBox_NFC_F_424.Name = "checkBox_NFC_F_424";
            this.checkBox_NFC_F_424.Size = new System.Drawing.Size(47, 17);
            this.checkBox_NFC_F_424.TabIndex = 27;
            this.checkBox_NFC_F_424.Text = "424";
            this.checkBox_NFC_F_424.UseVisualStyleBackColor = true;
            // 
            // checkBox_NFC_F_212
            // 
            this.checkBox_NFC_F_212.AutoSize = true;
            this.checkBox_NFC_F_212.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_NFC_F_212.ForeColor = System.Drawing.Color.Maroon;
            this.checkBox_NFC_F_212.Location = new System.Drawing.Point(93, 84);
            this.checkBox_NFC_F_212.Name = "checkBox_NFC_F_212";
            this.checkBox_NFC_F_212.Size = new System.Drawing.Size(47, 17);
            this.checkBox_NFC_F_212.TabIndex = 26;
            this.checkBox_NFC_F_212.Text = "212";
            this.checkBox_NFC_F_212.UseVisualStyleBackColor = true;
            // 
            // panel_RW_NFC_V
            // 
            this.panel_RW_NFC_V.Controls.Add(this.radioButton_ISO15693_26_48);
            this.panel_RW_NFC_V.Location = new System.Drawing.Point(90, 106);
            this.panel_RW_NFC_V.Name = "panel_RW_NFC_V";
            this.panel_RW_NFC_V.Size = new System.Drawing.Size(229, 24);
            this.panel_RW_NFC_V.TabIndex = 25;
            // 
            // radioButton_ISO15693_26_48
            // 
            this.radioButton_ISO15693_26_48.AutoSize = true;
            this.radioButton_ISO15693_26_48.Checked = true;
            this.radioButton_ISO15693_26_48.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_ISO15693_26_48.Location = new System.Drawing.Point(3, 3);
            this.radioButton_ISO15693_26_48.Name = "radioButton_ISO15693_26_48";
            this.radioButton_ISO15693_26_48.Size = new System.Drawing.Size(57, 17);
            this.radioButton_ISO15693_26_48.TabIndex = 17;
            this.radioButton_ISO15693_26_48.TabStop = true;
            this.radioButton_ISO15693_26_48.Text = "26.48";
            this.radioButton_ISO15693_26_48.UseVisualStyleBackColor = true;
            // 
            // panel_RW_NFC_B
            // 
            this.panel_RW_NFC_B.Controls.Add(this.radioButton_NFC_B_106);
            this.panel_RW_NFC_B.Controls.Add(this.radioButton_NFC_B_212);
            this.panel_RW_NFC_B.Controls.Add(this.radioButton_NFC_B_424);
            this.panel_RW_NFC_B.Controls.Add(this.radioButton_NFC_B_848);
            this.panel_RW_NFC_B.Location = new System.Drawing.Point(90, 54);
            this.panel_RW_NFC_B.Name = "panel_RW_NFC_B";
            this.panel_RW_NFC_B.Size = new System.Drawing.Size(229, 24);
            this.panel_RW_NFC_B.TabIndex = 23;
            // 
            // radioButton_NFC_B_106
            // 
            this.radioButton_NFC_B_106.AutoSize = true;
            this.radioButton_NFC_B_106.Checked = true;
            this.radioButton_NFC_B_106.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_B_106.Location = new System.Drawing.Point(3, 4);
            this.radioButton_NFC_B_106.Name = "radioButton_NFC_B_106";
            this.radioButton_NFC_B_106.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_B_106.TabIndex = 14;
            this.radioButton_NFC_B_106.TabStop = true;
            this.radioButton_NFC_B_106.Text = "106";
            this.radioButton_NFC_B_106.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_B_212
            // 
            this.radioButton_NFC_B_212.AutoSize = true;
            this.radioButton_NFC_B_212.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_B_212.Location = new System.Drawing.Point(62, 4);
            this.radioButton_NFC_B_212.Name = "radioButton_NFC_B_212";
            this.radioButton_NFC_B_212.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_B_212.TabIndex = 16;
            this.radioButton_NFC_B_212.Text = "212";
            this.radioButton_NFC_B_212.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_B_424
            // 
            this.radioButton_NFC_B_424.AutoSize = true;
            this.radioButton_NFC_B_424.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_B_424.Location = new System.Drawing.Point(121, 4);
            this.radioButton_NFC_B_424.Name = "radioButton_NFC_B_424";
            this.radioButton_NFC_B_424.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_B_424.TabIndex = 17;
            this.radioButton_NFC_B_424.Text = "424";
            this.radioButton_NFC_B_424.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_B_848
            // 
            this.radioButton_NFC_B_848.AutoSize = true;
            this.radioButton_NFC_B_848.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_B_848.Location = new System.Drawing.Point(176, 4);
            this.radioButton_NFC_B_848.Name = "radioButton_NFC_B_848";
            this.radioButton_NFC_B_848.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_B_848.TabIndex = 18;
            this.radioButton_NFC_B_848.Text = "848";
            this.radioButton_NFC_B_848.UseVisualStyleBackColor = true;
            // 
            // panel_RW_NFC_A
            // 
            this.panel_RW_NFC_A.Controls.Add(this.radioButton_NFC_A_106);
            this.panel_RW_NFC_A.Controls.Add(this.radioButton_NFC_A_212);
            this.panel_RW_NFC_A.Controls.Add(this.radioButton_NFC_A_424);
            this.panel_RW_NFC_A.Controls.Add(this.radioButton_NFC_A_848);
            this.panel_RW_NFC_A.Location = new System.Drawing.Point(90, 29);
            this.panel_RW_NFC_A.Name = "panel_RW_NFC_A";
            this.panel_RW_NFC_A.Size = new System.Drawing.Size(229, 24);
            this.panel_RW_NFC_A.TabIndex = 22;
            // 
            // radioButton_NFC_A_106
            // 
            this.radioButton_NFC_A_106.AutoSize = true;
            this.radioButton_NFC_A_106.Checked = true;
            this.radioButton_NFC_A_106.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_A_106.Location = new System.Drawing.Point(3, 4);
            this.radioButton_NFC_A_106.Name = "radioButton_NFC_A_106";
            this.radioButton_NFC_A_106.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_A_106.TabIndex = 14;
            this.radioButton_NFC_A_106.TabStop = true;
            this.radioButton_NFC_A_106.Text = "106";
            this.radioButton_NFC_A_106.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_A_212
            // 
            this.radioButton_NFC_A_212.AutoSize = true;
            this.radioButton_NFC_A_212.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_A_212.Location = new System.Drawing.Point(62, 4);
            this.radioButton_NFC_A_212.Name = "radioButton_NFC_A_212";
            this.radioButton_NFC_A_212.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_A_212.TabIndex = 16;
            this.radioButton_NFC_A_212.Text = "212";
            this.radioButton_NFC_A_212.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_A_424
            // 
            this.radioButton_NFC_A_424.AutoSize = true;
            this.radioButton_NFC_A_424.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_A_424.Location = new System.Drawing.Point(121, 4);
            this.radioButton_NFC_A_424.Name = "radioButton_NFC_A_424";
            this.radioButton_NFC_A_424.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_A_424.TabIndex = 17;
            this.radioButton_NFC_A_424.Text = "424";
            this.radioButton_NFC_A_424.UseVisualStyleBackColor = true;
            // 
            // radioButton_NFC_A_848
            // 
            this.radioButton_NFC_A_848.AutoSize = true;
            this.radioButton_NFC_A_848.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButton_NFC_A_848.Location = new System.Drawing.Point(176, 4);
            this.radioButton_NFC_A_848.Name = "radioButton_NFC_A_848";
            this.radioButton_NFC_A_848.Size = new System.Drawing.Size(46, 17);
            this.radioButton_NFC_A_848.TabIndex = 18;
            this.radioButton_NFC_A_848.Text = "848";
            this.radioButton_NFC_A_848.UseVisualStyleBackColor = true;
            // 
            // checkBox_RW_ISO15693
            // 
            this.checkBox_RW_ISO15693.AutoSize = true;
            this.checkBox_RW_ISO15693.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_RW_ISO15693.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_RW_ISO15693.Location = new System.Drawing.Point(3, 108);
            this.checkBox_RW_ISO15693.Name = "checkBox_RW_ISO15693";
            this.checkBox_RW_ISO15693.Size = new System.Drawing.Size(90, 20);
            this.checkBox_RW_ISO15693.TabIndex = 13;
            this.checkBox_RW_ISO15693.Text = "ISO15693";
            this.checkBox_RW_ISO15693.UseVisualStyleBackColor = true;
            // 
            // checkBox_RW_NFC_F
            // 
            this.checkBox_RW_NFC_F.AutoSize = true;
            this.checkBox_RW_NFC_F.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_RW_NFC_F.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_RW_NFC_F.Location = new System.Drawing.Point(3, 82);
            this.checkBox_RW_NFC_F.Name = "checkBox_RW_NFC_F";
            this.checkBox_RW_NFC_F.Size = new System.Drawing.Size(69, 20);
            this.checkBox_RW_NFC_F.TabIndex = 12;
            this.checkBox_RW_NFC_F.Text = "NFC-F";
            this.checkBox_RW_NFC_F.UseVisualStyleBackColor = true;
            // 
            // Label_Reader_Writer_Modes
            // 
            this.Label_Reader_Writer_Modes.AutoSize = true;
            this.Label_Reader_Writer_Modes.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Reader_Writer_Modes.ForeColor = System.Drawing.Color.Black;
            this.Label_Reader_Writer_Modes.Location = new System.Drawing.Point(71, 5);
            this.Label_Reader_Writer_Modes.Name = "Label_Reader_Writer_Modes";
            this.Label_Reader_Writer_Modes.Size = new System.Drawing.Size(181, 18);
            this.Label_Reader_Writer_Modes.TabIndex = 1;
            this.Label_Reader_Writer_Modes.Text = "Reader/Writer Mode(s)";
            // 
            // checkBox_RW_NFC_B
            // 
            this.checkBox_RW_NFC_B.AutoSize = true;
            this.checkBox_RW_NFC_B.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_RW_NFC_B.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_RW_NFC_B.Location = new System.Drawing.Point(3, 58);
            this.checkBox_RW_NFC_B.Name = "checkBox_RW_NFC_B";
            this.checkBox_RW_NFC_B.Size = new System.Drawing.Size(69, 20);
            this.checkBox_RW_NFC_B.TabIndex = 11;
            this.checkBox_RW_NFC_B.Text = "NFC-B";
            this.checkBox_RW_NFC_B.UseVisualStyleBackColor = true;
            // 
            // checkBox_RW_NFC_A
            // 
            this.checkBox_RW_NFC_A.AutoSize = true;
            this.checkBox_RW_NFC_A.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_RW_NFC_A.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_RW_NFC_A.Location = new System.Drawing.Point(3, 33);
            this.checkBox_RW_NFC_A.Name = "checkBox_RW_NFC_A";
            this.checkBox_RW_NFC_A.Size = new System.Drawing.Size(69, 20);
            this.checkBox_RW_NFC_A.TabIndex = 4;
            this.checkBox_RW_NFC_A.Text = "NFC-A";
            this.checkBox_RW_NFC_A.UseVisualStyleBackColor = true;
            // 
            // groupBox_CE_Config
            // 
            this.groupBox_CE_Config.BackColor = System.Drawing.Color.LightGray;
            this.groupBox_CE_Config.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox_CE_Config.Controls.Add(this.button_CE_toggle);
            this.groupBox_CE_Config.Controls.Add(this.panel_CE_Mode_Support);
            this.groupBox_CE_Config.Enabled = false;
            this.groupBox_CE_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_CE_Config.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_CE_Config.Location = new System.Drawing.Point(372, 12);
            this.groupBox_CE_Config.Name = "groupBox_CE_Config";
            this.groupBox_CE_Config.Size = new System.Drawing.Size(227, 261);
            this.groupBox_CE_Config.TabIndex = 23;
            this.groupBox_CE_Config.TabStop = false;
            this.groupBox_CE_Config.Text = "TRF7970 CE Configuration";
            // 
            // button_CE_toggle
            // 
            this.button_CE_toggle.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_CE_toggle.Location = new System.Drawing.Point(60, 216);
            this.button_CE_toggle.Name = "button_CE_toggle";
            this.button_CE_toggle.Size = new System.Drawing.Size(107, 34);
            this.button_CE_toggle.TabIndex = 18;
            this.button_CE_toggle.Text = "Start CE";
            this.button_CE_toggle.UseVisualStyleBackColor = true;
            this.button_CE_toggle.Click += new System.EventHandler(this.button_CE_toggle_Click);
            // 
            // panel_CE_Mode_Support
            // 
            this.panel_CE_Mode_Support.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_CE_Mode_Support.Controls.Add(this.Label_Card_Emulation_Modes);
            this.panel_CE_Mode_Support.Controls.Add(this.checkBox_CE_T4BT);
            this.panel_CE_Mode_Support.Controls.Add(this.checkBox_CE_T4AT);
            this.panel_CE_Mode_Support.Location = new System.Drawing.Point(34, 22);
            this.panel_CE_Mode_Support.Name = "panel_CE_Mode_Support";
            this.panel_CE_Mode_Support.Size = new System.Drawing.Size(156, 101);
            this.panel_CE_Mode_Support.TabIndex = 10;
            // 
            // Label_Card_Emulation_Modes
            // 
            this.Label_Card_Emulation_Modes.AutoSize = true;
            this.Label_Card_Emulation_Modes.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Card_Emulation_Modes.ForeColor = System.Drawing.Color.Black;
            this.Label_Card_Emulation_Modes.Location = new System.Drawing.Point(6, 5);
            this.Label_Card_Emulation_Modes.Name = "Label_Card_Emulation_Modes";
            this.Label_Card_Emulation_Modes.Size = new System.Drawing.Size(146, 18);
            this.Label_Card_Emulation_Modes.TabIndex = 1;
            this.Label_Card_Emulation_Modes.Text = "Emulated Mode(s)";
            // 
            // checkBox_CE_T4BT
            // 
            this.checkBox_CE_T4BT.AutoSize = true;
            this.checkBox_CE_T4BT.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_CE_T4BT.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_CE_T4BT.Location = new System.Drawing.Point(9, 58);
            this.checkBox_CE_T4BT.Name = "checkBox_CE_T4BT";
            this.checkBox_CE_T4BT.Size = new System.Drawing.Size(137, 20);
            this.checkBox_CE_T4BT.TabIndex = 11;
            this.checkBox_CE_T4BT.Text = "Card Emulation B";
            this.checkBox_CE_T4BT.UseVisualStyleBackColor = true;
            // 
            // checkBox_CE_T4AT
            // 
            this.checkBox_CE_T4AT.AutoSize = true;
            this.checkBox_CE_T4AT.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_CE_T4AT.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_CE_T4AT.Location = new System.Drawing.Point(9, 33);
            this.checkBox_CE_T4AT.Name = "checkBox_CE_T4AT";
            this.checkBox_CE_T4AT.Size = new System.Drawing.Size(136, 20);
            this.checkBox_CE_T4AT.TabIndex = 4;
            this.checkBox_CE_T4AT.Text = "Card Emulation A";
            this.checkBox_CE_T4AT.UseVisualStyleBackColor = true;
            // 
            // groupBox_NFC_Compliance_Config
            // 
            this.groupBox_NFC_Compliance_Config.BackColor = System.Drawing.Color.LightGray;
            this.groupBox_NFC_Compliance_Config.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox_NFC_Compliance_Config.Controls.Add(this.buttonNFC_Compliance_toggle);
            this.groupBox_NFC_Compliance_Config.Controls.Add(this.panel_RW_adv_config);
            this.groupBox_NFC_Compliance_Config.Controls.Add(this.panel_P2P_adv_config);
            this.groupBox_NFC_Compliance_Config.Enabled = false;
            this.groupBox_NFC_Compliance_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_NFC_Compliance_Config.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_NFC_Compliance_Config.Location = new System.Drawing.Point(15, 279);
            this.groupBox_NFC_Compliance_Config.Name = "groupBox_NFC_Compliance_Config";
            this.groupBox_NFC_Compliance_Config.Size = new System.Drawing.Size(653, 258);
            this.groupBox_NFC_Compliance_Config.TabIndex = 22;
            this.groupBox_NFC_Compliance_Config.TabStop = false;
            this.groupBox_NFC_Compliance_Config.Text = "NFC Compliance Configuration";
            // 
            // buttonNFC_Compliance_toggle
            // 
            this.buttonNFC_Compliance_toggle.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonNFC_Compliance_toggle.Location = new System.Drawing.Point(239, 214);
            this.buttonNFC_Compliance_toggle.Name = "buttonNFC_Compliance_toggle";
            this.buttonNFC_Compliance_toggle.Size = new System.Drawing.Size(173, 34);
            this.buttonNFC_Compliance_toggle.TabIndex = 22;
            this.buttonNFC_Compliance_toggle.Text = "Start NFC Testing";
            this.buttonNFC_Compliance_toggle.UseVisualStyleBackColor = true;
            this.buttonNFC_Compliance_toggle.Click += new System.EventHandler(this.buttonNFC_Compliance_toggle_Click);
            // 
            // panel_RW_adv_config
            // 
            this.panel_RW_adv_config.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_RW_adv_config.Controls.Add(this.Label_RW_Adv_Config);
            this.panel_RW_adv_config.Controls.Add(this.comboBox_RW_DSL_Retry);
            this.panel_RW_adv_config.Controls.Add(this.label_RW_DSL_Retry);
            this.panel_RW_adv_config.Controls.Add(this.comboBox_RW_ACK_Retry);
            this.panel_RW_adv_config.Controls.Add(this.label_RW_ACK_Retry);
            this.panel_RW_adv_config.Controls.Add(this.comboBox_RW_DID);
            this.panel_RW_adv_config.Controls.Add(this.label_RW_DID);
            this.panel_RW_adv_config.Controls.Add(this.comboBox_RW_WTX_Retry);
            this.panel_RW_adv_config.Controls.Add(this.comboBox_RW_NACK_Retry);
            this.panel_RW_adv_config.Controls.Add(this.label_RW_WTX_Retry);
            this.panel_RW_adv_config.Controls.Add(this.label_RW_NACK_Retry);
            this.panel_RW_adv_config.Location = new System.Drawing.Point(334, 23);
            this.panel_RW_adv_config.Name = "panel_RW_adv_config";
            this.panel_RW_adv_config.Size = new System.Drawing.Size(299, 184);
            this.panel_RW_adv_config.TabIndex = 21;
            // 
            // Label_RW_Adv_Config
            // 
            this.Label_RW_Adv_Config.AutoSize = true;
            this.Label_RW_Adv_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_RW_Adv_Config.ForeColor = System.Drawing.Color.Black;
            this.Label_RW_Adv_Config.Location = new System.Drawing.Point(93, 7);
            this.Label_RW_Adv_Config.Name = "Label_RW_Adv_Config";
            this.Label_RW_Adv_Config.Size = new System.Drawing.Size(113, 18);
            this.Label_RW_Adv_Config.TabIndex = 40;
            this.Label_RW_Adv_Config.Text = "Reader/Writer";
            // 
            // comboBox_RW_DSL_Retry
            // 
            this.comboBox_RW_DSL_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_RW_DSL_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_RW_DSL_Retry.FormattingEnabled = true;
            this.comboBox_RW_DSL_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_RW_DSL_Retry.Location = new System.Drawing.Point(189, 31);
            this.comboBox_RW_DSL_Retry.Name = "comboBox_RW_DSL_Retry";
            this.comboBox_RW_DSL_Retry.Size = new System.Drawing.Size(68, 24);
            this.comboBox_RW_DSL_Retry.TabIndex = 39;
            // 
            // label_RW_DSL_Retry
            // 
            this.label_RW_DSL_Retry.AutoSize = true;
            this.label_RW_DSL_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_DSL_Retry.Location = new System.Drawing.Point(30, 34);
            this.label_RW_DSL_Retry.Name = "label_RW_DSL_Retry";
            this.label_RW_DSL_Retry.Size = new System.Drawing.Size(96, 16);
            this.label_RW_DSL_Retry.TabIndex = 38;
            this.label_RW_DSL_Retry.Text = "DSL Retry(s)";
            // 
            // comboBox_RW_ACK_Retry
            // 
            this.comboBox_RW_ACK_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_RW_ACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_RW_ACK_Retry.FormattingEnabled = true;
            this.comboBox_RW_ACK_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_RW_ACK_Retry.Location = new System.Drawing.Point(189, 61);
            this.comboBox_RW_ACK_Retry.Name = "comboBox_RW_ACK_Retry";
            this.comboBox_RW_ACK_Retry.Size = new System.Drawing.Size(68, 24);
            this.comboBox_RW_ACK_Retry.TabIndex = 37;
            // 
            // label_RW_ACK_Retry
            // 
            this.label_RW_ACK_Retry.AutoSize = true;
            this.label_RW_ACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_ACK_Retry.Location = new System.Drawing.Point(30, 64);
            this.label_RW_ACK_Retry.Name = "label_RW_ACK_Retry";
            this.label_RW_ACK_Retry.Size = new System.Drawing.Size(96, 16);
            this.label_RW_ACK_Retry.TabIndex = 36;
            this.label_RW_ACK_Retry.Text = "ACK Retry(s)";
            // 
            // comboBox_RW_DID
            // 
            this.comboBox_RW_DID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_RW_DID.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_RW_DID.FormattingEnabled = true;
            this.comboBox_RW_DID.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14"});
            this.comboBox_RW_DID.Location = new System.Drawing.Point(189, 151);
            this.comboBox_RW_DID.Name = "comboBox_RW_DID";
            this.comboBox_RW_DID.Size = new System.Drawing.Size(68, 24);
            this.comboBox_RW_DID.TabIndex = 35;
            // 
            // label_RW_DID
            // 
            this.label_RW_DID.AutoSize = true;
            this.label_RW_DID.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_DID.Location = new System.Drawing.Point(30, 154);
            this.label_RW_DID.Name = "label_RW_DID";
            this.label_RW_DID.Size = new System.Drawing.Size(92, 16);
            this.label_RW_DID.TabIndex = 34;
            this.label_RW_DID.Text = "DID Number";
            // 
            // comboBox_RW_WTX_Retry
            // 
            this.comboBox_RW_WTX_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_RW_WTX_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_RW_WTX_Retry.FormattingEnabled = true;
            this.comboBox_RW_WTX_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_RW_WTX_Retry.Location = new System.Drawing.Point(189, 121);
            this.comboBox_RW_WTX_Retry.Name = "comboBox_RW_WTX_Retry";
            this.comboBox_RW_WTX_Retry.Size = new System.Drawing.Size(68, 24);
            this.comboBox_RW_WTX_Retry.TabIndex = 33;
            // 
            // comboBox_RW_NACK_Retry
            // 
            this.comboBox_RW_NACK_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_RW_NACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_RW_NACK_Retry.FormattingEnabled = true;
            this.comboBox_RW_NACK_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_RW_NACK_Retry.Location = new System.Drawing.Point(189, 91);
            this.comboBox_RW_NACK_Retry.Name = "comboBox_RW_NACK_Retry";
            this.comboBox_RW_NACK_Retry.Size = new System.Drawing.Size(68, 24);
            this.comboBox_RW_NACK_Retry.TabIndex = 32;
            // 
            // label_RW_WTX_Retry
            // 
            this.label_RW_WTX_Retry.AutoSize = true;
            this.label_RW_WTX_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_WTX_Retry.Location = new System.Drawing.Point(30, 124);
            this.label_RW_WTX_Retry.Name = "label_RW_WTX_Retry";
            this.label_RW_WTX_Retry.Size = new System.Drawing.Size(100, 16);
            this.label_RW_WTX_Retry.TabIndex = 31;
            this.label_RW_WTX_Retry.Text = "WTX Retry(s)";
            // 
            // label_RW_NACK_Retry
            // 
            this.label_RW_NACK_Retry.AutoSize = true;
            this.label_RW_NACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_NACK_Retry.Location = new System.Drawing.Point(30, 94);
            this.label_RW_NACK_Retry.Name = "label_RW_NACK_Retry";
            this.label_RW_NACK_Retry.Size = new System.Drawing.Size(107, 16);
            this.label_RW_NACK_Retry.TabIndex = 30;
            this.label_RW_NACK_Retry.Text = "NACK Retry(s)";
            // 
            // panel_P2P_adv_config
            // 
            this.panel_P2P_adv_config.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_P2P_adv_config.Controls.Add(this.comboBox_P2P_DID);
            this.panel_P2P_adv_config.Controls.Add(this.label_P2P_DID);
            this.panel_P2P_adv_config.Controls.Add(this.comboBox_P2P_NACK_Retry);
            this.panel_P2P_adv_config.Controls.Add(this.comboBox_P2P_ATN_Retry);
            this.panel_P2P_adv_config.Controls.Add(this.label_P2P_NACK_Retry);
            this.panel_P2P_adv_config.Controls.Add(this.label_P2P_ATN_Retry);
            this.panel_P2P_adv_config.Controls.Add(this.checkBox_NFC_DEP_Loopback);
            this.panel_P2P_adv_config.Controls.Add(this.checkBox_LLCP_Enable);
            this.panel_P2P_adv_config.Controls.Add(this.Label_P2P_Adv_Config);
            this.panel_P2P_adv_config.Location = new System.Drawing.Point(20, 23);
            this.panel_P2P_adv_config.Name = "panel_P2P_adv_config";
            this.panel_P2P_adv_config.Size = new System.Drawing.Size(299, 184);
            this.panel_P2P_adv_config.TabIndex = 19;
            // 
            // comboBox_P2P_DID
            // 
            this.comboBox_P2P_DID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_P2P_DID.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_P2P_DID.FormattingEnabled = true;
            this.comboBox_P2P_DID.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14"});
            this.comboBox_P2P_DID.Location = new System.Drawing.Point(181, 121);
            this.comboBox_P2P_DID.Name = "comboBox_P2P_DID";
            this.comboBox_P2P_DID.Size = new System.Drawing.Size(68, 24);
            this.comboBox_P2P_DID.TabIndex = 29;
            // 
            // label_P2P_DID
            // 
            this.label_P2P_DID.AutoSize = true;
            this.label_P2P_DID.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_P2P_DID.Location = new System.Drawing.Point(22, 124);
            this.label_P2P_DID.Name = "label_P2P_DID";
            this.label_P2P_DID.Size = new System.Drawing.Size(92, 16);
            this.label_P2P_DID.TabIndex = 28;
            this.label_P2P_DID.Text = "DID Number";
            // 
            // comboBox_P2P_NACK_Retry
            // 
            this.comboBox_P2P_NACK_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_P2P_NACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_P2P_NACK_Retry.FormattingEnabled = true;
            this.comboBox_P2P_NACK_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_P2P_NACK_Retry.Location = new System.Drawing.Point(181, 91);
            this.comboBox_P2P_NACK_Retry.Name = "comboBox_P2P_NACK_Retry";
            this.comboBox_P2P_NACK_Retry.Size = new System.Drawing.Size(69, 24);
            this.comboBox_P2P_NACK_Retry.TabIndex = 27;
            // 
            // comboBox_P2P_ATN_Retry
            // 
            this.comboBox_P2P_ATN_Retry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_P2P_ATN_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_P2P_ATN_Retry.FormattingEnabled = true;
            this.comboBox_P2P_ATN_Retry.Items.AddRange(new object[] {
            "2",
            "3",
            "4",
            "5"});
            this.comboBox_P2P_ATN_Retry.Location = new System.Drawing.Point(181, 61);
            this.comboBox_P2P_ATN_Retry.Name = "comboBox_P2P_ATN_Retry";
            this.comboBox_P2P_ATN_Retry.Size = new System.Drawing.Size(68, 24);
            this.comboBox_P2P_ATN_Retry.TabIndex = 26;
            // 
            // label_P2P_NACK_Retry
            // 
            this.label_P2P_NACK_Retry.AutoSize = true;
            this.label_P2P_NACK_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_P2P_NACK_Retry.Location = new System.Drawing.Point(22, 94);
            this.label_P2P_NACK_Retry.Name = "label_P2P_NACK_Retry";
            this.label_P2P_NACK_Retry.Size = new System.Drawing.Size(107, 16);
            this.label_P2P_NACK_Retry.TabIndex = 25;
            this.label_P2P_NACK_Retry.Text = "NACK Retry(s)";
            // 
            // label_P2P_ATN_Retry
            // 
            this.label_P2P_ATN_Retry.AutoSize = true;
            this.label_P2P_ATN_Retry.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_P2P_ATN_Retry.Location = new System.Drawing.Point(22, 64);
            this.label_P2P_ATN_Retry.Name = "label_P2P_ATN_Retry";
            this.label_P2P_ATN_Retry.Size = new System.Drawing.Size(98, 16);
            this.label_P2P_ATN_Retry.TabIndex = 24;
            this.label_P2P_ATN_Retry.Text = "ATN Retry(s)";
            // 
            // checkBox_NFC_DEP_Loopback
            // 
            this.checkBox_NFC_DEP_Loopback.AutoSize = true;
            this.checkBox_NFC_DEP_Loopback.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_NFC_DEP_Loopback.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_NFC_DEP_Loopback.Location = new System.Drawing.Point(101, 35);
            this.checkBox_NFC_DEP_Loopback.Name = "checkBox_NFC_DEP_Loopback";
            this.checkBox_NFC_DEP_Loopback.Size = new System.Drawing.Size(154, 20);
            this.checkBox_NFC_DEP_Loopback.TabIndex = 23;
            this.checkBox_NFC_DEP_Loopback.Text = "NFC-DEP Loopback";
            this.checkBox_NFC_DEP_Loopback.UseVisualStyleBackColor = true;
            // 
            // checkBox_LLCP_Enable
            // 
            this.checkBox_LLCP_Enable.AutoSize = true;
            this.checkBox_LLCP_Enable.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_LLCP_Enable.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_LLCP_Enable.Location = new System.Drawing.Point(24, 35);
            this.checkBox_LLCP_Enable.Name = "checkBox_LLCP_Enable";
            this.checkBox_LLCP_Enable.Size = new System.Drawing.Size(62, 20);
            this.checkBox_LLCP_Enable.TabIndex = 22;
            this.checkBox_LLCP_Enable.Text = "LLCP";
            this.checkBox_LLCP_Enable.UseVisualStyleBackColor = true;
            // 
            // Label_P2P_Adv_Config
            // 
            this.Label_P2P_Adv_Config.AutoSize = true;
            this.Label_P2P_Adv_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_P2P_Adv_Config.ForeColor = System.Drawing.Color.Black;
            this.Label_P2P_Adv_Config.Location = new System.Drawing.Point(98, 7);
            this.Label_P2P_Adv_Config.Name = "Label_P2P_Adv_Config";
            this.Label_P2P_Adv_Config.Size = new System.Drawing.Size(103, 18);
            this.Label_P2P_Adv_Config.TabIndex = 3;
            this.Label_P2P_Adv_Config.Text = "Peer to Peer";
            // 
            // groupBox_P2P_Config
            // 
            this.groupBox_P2P_Config.BackColor = System.Drawing.Color.LightGray;
            this.groupBox_P2P_Config.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox_P2P_Config.Controls.Add(this.button_P2P_toggle);
            this.groupBox_P2P_Config.Controls.Add(this.panel_P2P_Target_Support);
            this.groupBox_P2P_Config.Controls.Add(this.panel_P2P_Initiator_Support);
            this.groupBox_P2P_Config.Enabled = false;
            this.groupBox_P2P_Config.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_P2P_Config.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_P2P_Config.Location = new System.Drawing.Point(15, 12);
            this.groupBox_P2P_Config.Name = "groupBox_P2P_Config";
            this.groupBox_P2P_Config.Size = new System.Drawing.Size(351, 261);
            this.groupBox_P2P_Config.TabIndex = 19;
            this.groupBox_P2P_Config.TabStop = false;
            this.groupBox_P2P_Config.Text = "TRF7970 P2P Configuration";
            // 
            // button_P2P_toggle
            // 
            this.button_P2P_toggle.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_P2P_toggle.Location = new System.Drawing.Point(122, 216);
            this.button_P2P_toggle.Name = "button_P2P_toggle";
            this.button_P2P_toggle.Size = new System.Drawing.Size(107, 34);
            this.button_P2P_toggle.TabIndex = 18;
            this.button_P2P_toggle.Text = "Start P2P";
            this.button_P2P_toggle.UseVisualStyleBackColor = true;
            this.button_P2P_toggle.Click += new System.EventHandler(this.button_P2P_toggle_Click);
            // 
            // panel_P2P_Target_Support
            // 
            this.panel_P2P_Target_Support.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Act_Target_424);
            this.panel_P2P_Target_Support.Controls.Add(this.Label_Target);
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Act_Target_212);
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Pas_Target_106);
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Act_Target_106);
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Pas_Target_212);
            this.panel_P2P_Target_Support.Controls.Add(this.checkBox_Pas_Target_424);
            this.panel_P2P_Target_Support.Location = new System.Drawing.Point(13, 22);
            this.panel_P2P_Target_Support.Name = "panel_P2P_Target_Support";
            this.panel_P2P_Target_Support.Size = new System.Drawing.Size(160, 188);
            this.panel_P2P_Target_Support.TabIndex = 10;
            // 
            // checkBox_Act_Target_424
            // 
            this.checkBox_Act_Target_424.AutoSize = true;
            this.checkBox_Act_Target_424.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Target_424.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Target_424.Location = new System.Drawing.Point(20, 155);
            this.checkBox_Act_Target_424.Name = "checkBox_Act_Target_424";
            this.checkBox_Act_Target_424.Size = new System.Drawing.Size(123, 20);
            this.checkBox_Act_Target_424.TabIndex = 9;
            this.checkBox_Act_Target_424.Text = "Active 424kbps";
            this.checkBox_Act_Target_424.UseVisualStyleBackColor = true;
            // 
            // Label_Target
            // 
            this.Label_Target.AutoSize = true;
            this.Label_Target.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Target.ForeColor = System.Drawing.Color.Black;
            this.Label_Target.Location = new System.Drawing.Point(17, 5);
            this.Label_Target.Name = "Label_Target";
            this.Label_Target.Size = new System.Drawing.Size(124, 18);
            this.Label_Target.TabIndex = 1;
            this.Label_Target.Text = "Target Mode(s)";
            // 
            // checkBox_Act_Target_212
            // 
            this.checkBox_Act_Target_212.AutoSize = true;
            this.checkBox_Act_Target_212.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Target_212.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Target_212.Location = new System.Drawing.Point(20, 131);
            this.checkBox_Act_Target_212.Name = "checkBox_Act_Target_212";
            this.checkBox_Act_Target_212.Size = new System.Drawing.Size(123, 20);
            this.checkBox_Act_Target_212.TabIndex = 8;
            this.checkBox_Act_Target_212.Text = "Active 212kbps";
            this.checkBox_Act_Target_212.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Target_106
            // 
            this.checkBox_Pas_Target_106.AutoSize = true;
            this.checkBox_Pas_Target_106.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Target_106.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Target_106.Location = new System.Drawing.Point(20, 35);
            this.checkBox_Pas_Target_106.Name = "checkBox_Pas_Target_106";
            this.checkBox_Pas_Target_106.Size = new System.Drawing.Size(134, 20);
            this.checkBox_Pas_Target_106.TabIndex = 4;
            this.checkBox_Pas_Target_106.Text = "Passive 106kbps";
            this.checkBox_Pas_Target_106.UseVisualStyleBackColor = true;
            // 
            // checkBox_Act_Target_106
            // 
            this.checkBox_Act_Target_106.AutoSize = true;
            this.checkBox_Act_Target_106.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Target_106.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Target_106.Location = new System.Drawing.Point(20, 107);
            this.checkBox_Act_Target_106.Name = "checkBox_Act_Target_106";
            this.checkBox_Act_Target_106.Size = new System.Drawing.Size(123, 20);
            this.checkBox_Act_Target_106.TabIndex = 7;
            this.checkBox_Act_Target_106.Text = "Active 106kbps";
            this.checkBox_Act_Target_106.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Target_212
            // 
            this.checkBox_Pas_Target_212.AutoSize = true;
            this.checkBox_Pas_Target_212.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Target_212.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Target_212.Location = new System.Drawing.Point(20, 59);
            this.checkBox_Pas_Target_212.Name = "checkBox_Pas_Target_212";
            this.checkBox_Pas_Target_212.Size = new System.Drawing.Size(134, 20);
            this.checkBox_Pas_Target_212.TabIndex = 5;
            this.checkBox_Pas_Target_212.Text = "Passive 212kbps";
            this.checkBox_Pas_Target_212.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Target_424
            // 
            this.checkBox_Pas_Target_424.AutoSize = true;
            this.checkBox_Pas_Target_424.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Target_424.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Target_424.Location = new System.Drawing.Point(20, 83);
            this.checkBox_Pas_Target_424.Name = "checkBox_Pas_Target_424";
            this.checkBox_Pas_Target_424.Size = new System.Drawing.Size(134, 20);
            this.checkBox_Pas_Target_424.TabIndex = 6;
            this.checkBox_Pas_Target_424.Text = "Passive 424kbps";
            this.checkBox_Pas_Target_424.UseVisualStyleBackColor = true;
            // 
            // panel_P2P_Initiator_Support
            // 
            this.panel_P2P_Initiator_Support.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Act_Initiator_424);
            this.panel_P2P_Initiator_Support.Controls.Add(this.Label_Initiator);
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Act_Initiator_212);
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Pas_Initiator_106);
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Act_Initiator_106);
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Pas_Initiator_212);
            this.panel_P2P_Initiator_Support.Controls.Add(this.checkBox_Pas_Initiator_424);
            this.panel_P2P_Initiator_Support.Location = new System.Drawing.Point(179, 22);
            this.panel_P2P_Initiator_Support.Name = "panel_P2P_Initiator_Support";
            this.panel_P2P_Initiator_Support.Size = new System.Drawing.Size(160, 188);
            this.panel_P2P_Initiator_Support.TabIndex = 17;
            // 
            // checkBox_Act_Initiator_424
            // 
            this.checkBox_Act_Initiator_424.AutoSize = true;
            this.checkBox_Act_Initiator_424.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Initiator_424.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Initiator_424.Location = new System.Drawing.Point(18, 153);
            this.checkBox_Act_Initiator_424.Name = "checkBox_Act_Initiator_424";
            this.checkBox_Act_Initiator_424.Size = new System.Drawing.Size(132, 20);
            this.checkBox_Act_Initiator_424.TabIndex = 16;
            this.checkBox_Act_Initiator_424.Text = "Active 424kbps *";
            this.checkBox_Act_Initiator_424.UseVisualStyleBackColor = true;
            // 
            // Label_Initiator
            // 
            this.Label_Initiator.AutoSize = true;
            this.Label_Initiator.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Initiator.ForeColor = System.Drawing.Color.Black;
            this.Label_Initiator.Location = new System.Drawing.Point(15, 5);
            this.Label_Initiator.Name = "Label_Initiator";
            this.Label_Initiator.Size = new System.Drawing.Size(132, 18);
            this.Label_Initiator.TabIndex = 2;
            this.Label_Initiator.Text = "Initiator Mode(s)";
            // 
            // checkBox_Act_Initiator_212
            // 
            this.checkBox_Act_Initiator_212.AutoSize = true;
            this.checkBox_Act_Initiator_212.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Initiator_212.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Initiator_212.Location = new System.Drawing.Point(18, 130);
            this.checkBox_Act_Initiator_212.Name = "checkBox_Act_Initiator_212";
            this.checkBox_Act_Initiator_212.Size = new System.Drawing.Size(123, 20);
            this.checkBox_Act_Initiator_212.TabIndex = 15;
            this.checkBox_Act_Initiator_212.Text = "Active 212kbps";
            this.checkBox_Act_Initiator_212.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Initiator_106
            // 
            this.checkBox_Pas_Initiator_106.AutoSize = true;
            this.checkBox_Pas_Initiator_106.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Initiator_106.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Initiator_106.Location = new System.Drawing.Point(18, 35);
            this.checkBox_Pas_Initiator_106.Name = "checkBox_Pas_Initiator_106";
            this.checkBox_Pas_Initiator_106.Size = new System.Drawing.Size(134, 20);
            this.checkBox_Pas_Initiator_106.TabIndex = 11;
            this.checkBox_Pas_Initiator_106.Text = "Passive 106kbps";
            this.checkBox_Pas_Initiator_106.UseVisualStyleBackColor = true;
            // 
            // checkBox_Act_Initiator_106
            // 
            this.checkBox_Act_Initiator_106.AutoSize = true;
            this.checkBox_Act_Initiator_106.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Act_Initiator_106.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Act_Initiator_106.Location = new System.Drawing.Point(18, 106);
            this.checkBox_Act_Initiator_106.Name = "checkBox_Act_Initiator_106";
            this.checkBox_Act_Initiator_106.Size = new System.Drawing.Size(123, 20);
            this.checkBox_Act_Initiator_106.TabIndex = 14;
            this.checkBox_Act_Initiator_106.Text = "Active 106kbps";
            this.checkBox_Act_Initiator_106.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Initiator_212
            // 
            this.checkBox_Pas_Initiator_212.AutoSize = true;
            this.checkBox_Pas_Initiator_212.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Initiator_212.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Initiator_212.Location = new System.Drawing.Point(18, 58);
            this.checkBox_Pas_Initiator_212.Name = "checkBox_Pas_Initiator_212";
            this.checkBox_Pas_Initiator_212.Size = new System.Drawing.Size(134, 20);
            this.checkBox_Pas_Initiator_212.TabIndex = 12;
            this.checkBox_Pas_Initiator_212.Text = "Passive 212kbps";
            this.checkBox_Pas_Initiator_212.UseVisualStyleBackColor = true;
            // 
            // checkBox_Pas_Initiator_424
            // 
            this.checkBox_Pas_Initiator_424.AutoSize = true;
            this.checkBox_Pas_Initiator_424.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.checkBox_Pas_Initiator_424.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.checkBox_Pas_Initiator_424.Location = new System.Drawing.Point(18, 82);
            this.checkBox_Pas_Initiator_424.Name = "checkBox_Pas_Initiator_424";
            this.checkBox_Pas_Initiator_424.Size = new System.Drawing.Size(138, 20);
            this.checkBox_Pas_Initiator_424.TabIndex = 13;
            this.checkBox_Pas_Initiator_424.Text = "Passive 424kbps ";
            this.checkBox_Pas_Initiator_424.UseVisualStyleBackColor = true;
            // 
            // tabPeerToPeer
            // 
            this.tabPeerToPeer.BackColor = System.Drawing.Color.LightGray;
            this.tabPeerToPeer.Controls.Add(this.groupBox_P2P_Status);
            this.tabPeerToPeer.Controls.Add(this.groupBox_P2P_Rx);
            this.tabPeerToPeer.Controls.Add(this.groupBox_P2P_Tx);
            this.tabPeerToPeer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabPeerToPeer.Location = new System.Drawing.Point(4, 31);
            this.tabPeerToPeer.Name = "tabPeerToPeer";
            this.tabPeerToPeer.Padding = new System.Windows.Forms.Padding(3);
            this.tabPeerToPeer.Size = new System.Drawing.Size(955, 552);
            this.tabPeerToPeer.TabIndex = 5;
            this.tabPeerToPeer.Text = "Peer-to-Peer";
            // 
            // groupBox_P2P_Status
            // 
            this.groupBox_P2P_Status.Controls.Add(this.textBox_P2P_Mode);
            this.groupBox_P2P_Status.Controls.Add(this.textBox_P2P_Technology);
            this.groupBox_P2P_Status.Controls.Add(this.label_P2P_Receive);
            this.groupBox_P2P_Status.Controls.Add(this.label_P2P_Mode);
            this.groupBox_P2P_Status.Controls.Add(this.label_P2P_Technology);
            this.groupBox_P2P_Status.Controls.Add(this.textBox_P2P_Rssi);
            this.groupBox_P2P_Status.Controls.Add(this.textBox_P2P_Received);
            this.groupBox_P2P_Status.Controls.Add(this.label_P2P_Bitrate);
            this.groupBox_P2P_Status.Controls.Add(this.textBox_P2P_Bitrate);
            this.groupBox_P2P_Status.Controls.Add(this.label_P2P_Rssi);
            this.groupBox_P2P_Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_P2P_Status.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_P2P_Status.Location = new System.Drawing.Point(10, 7);
            this.groupBox_P2P_Status.Name = "groupBox_P2P_Status";
            this.groupBox_P2P_Status.Size = new System.Drawing.Size(257, 203);
            this.groupBox_P2P_Status.TabIndex = 14;
            this.groupBox_P2P_Status.TabStop = false;
            this.groupBox_P2P_Status.Text = "P2P Status";
            // 
            // textBox_P2P_Mode
            // 
            this.textBox_P2P_Mode.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_P2P_Mode.Location = new System.Drawing.Point(133, 20);
            this.textBox_P2P_Mode.Name = "textBox_P2P_Mode";
            this.textBox_P2P_Mode.ReadOnly = true;
            this.textBox_P2P_Mode.Size = new System.Drawing.Size(100, 30);
            this.textBox_P2P_Mode.TabIndex = 5;
            // 
            // textBox_P2P_Technology
            // 
            this.textBox_P2P_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_P2P_Technology.Location = new System.Drawing.Point(133, 92);
            this.textBox_P2P_Technology.Name = "textBox_P2P_Technology";
            this.textBox_P2P_Technology.ReadOnly = true;
            this.textBox_P2P_Technology.Size = new System.Drawing.Size(100, 30);
            this.textBox_P2P_Technology.TabIndex = 0;
            // 
            // label_P2P_Receive
            // 
            this.label_P2P_Receive.AutoSize = true;
            this.label_P2P_Receive.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_Receive.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_Receive.Location = new System.Drawing.Point(6, 131);
            this.label_P2P_Receive.Name = "label_P2P_Receive";
            this.label_P2P_Receive.Size = new System.Drawing.Size(107, 25);
            this.label_P2P_Receive.TabIndex = 6;
            this.label_P2P_Receive.Text = "Received ";
            // 
            // label_P2P_Mode
            // 
            this.label_P2P_Mode.AutoSize = true;
            this.label_P2P_Mode.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_Mode.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_Mode.Location = new System.Drawing.Point(6, 23);
            this.label_P2P_Mode.Name = "label_P2P_Mode";
            this.label_P2P_Mode.Size = new System.Drawing.Size(66, 25);
            this.label_P2P_Mode.TabIndex = 1;
            this.label_P2P_Mode.Text = "Mode";
            // 
            // label_P2P_Technology
            // 
            this.label_P2P_Technology.AutoSize = true;
            this.label_P2P_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_Technology.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_Technology.Location = new System.Drawing.Point(6, 95);
            this.label_P2P_Technology.Name = "label_P2P_Technology";
            this.label_P2P_Technology.Size = new System.Drawing.Size(125, 25);
            this.label_P2P_Technology.TabIndex = 4;
            this.label_P2P_Technology.Text = "Technology";
            // 
            // textBox_P2P_Rssi
            // 
            this.textBox_P2P_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_P2P_Rssi.Location = new System.Drawing.Point(133, 164);
            this.textBox_P2P_Rssi.Name = "textBox_P2P_Rssi";
            this.textBox_P2P_Rssi.ReadOnly = true;
            this.textBox_P2P_Rssi.Size = new System.Drawing.Size(100, 30);
            this.textBox_P2P_Rssi.TabIndex = 9;
            // 
            // textBox_P2P_Received
            // 
            this.textBox_P2P_Received.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_P2P_Received.Location = new System.Drawing.Point(133, 128);
            this.textBox_P2P_Received.Name = "textBox_P2P_Received";
            this.textBox_P2P_Received.ReadOnly = true;
            this.textBox_P2P_Received.Size = new System.Drawing.Size(100, 30);
            this.textBox_P2P_Received.TabIndex = 7;
            // 
            // label_P2P_Bitrate
            // 
            this.label_P2P_Bitrate.AutoSize = true;
            this.label_P2P_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_Bitrate.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_Bitrate.Location = new System.Drawing.Point(6, 59);
            this.label_P2P_Bitrate.Name = "label_P2P_Bitrate";
            this.label_P2P_Bitrate.Size = new System.Drawing.Size(74, 25);
            this.label_P2P_Bitrate.TabIndex = 2;
            this.label_P2P_Bitrate.Text = "Bitrate";
            // 
            // textBox_P2P_Bitrate
            // 
            this.textBox_P2P_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_P2P_Bitrate.Location = new System.Drawing.Point(133, 56);
            this.textBox_P2P_Bitrate.Name = "textBox_P2P_Bitrate";
            this.textBox_P2P_Bitrate.ReadOnly = true;
            this.textBox_P2P_Bitrate.Size = new System.Drawing.Size(100, 30);
            this.textBox_P2P_Bitrate.TabIndex = 3;
            // 
            // label_P2P_Rssi
            // 
            this.label_P2P_Rssi.AutoSize = true;
            this.label_P2P_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_Rssi.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_Rssi.Location = new System.Drawing.Point(6, 167);
            this.label_P2P_Rssi.Name = "label_P2P_Rssi";
            this.label_P2P_Rssi.Size = new System.Drawing.Size(68, 25);
            this.label_P2P_Rssi.TabIndex = 8;
            this.label_P2P_Rssi.Text = "RSSI ";
            // 
            // groupBox_P2P_Rx
            // 
            this.groupBox_P2P_Rx.Controls.Add(this.richTextBox_P2P_Receive);
            this.groupBox_P2P_Rx.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_P2P_Rx.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_P2P_Rx.Location = new System.Drawing.Point(273, 7);
            this.groupBox_P2P_Rx.Name = "groupBox_P2P_Rx";
            this.groupBox_P2P_Rx.Size = new System.Drawing.Size(665, 203);
            this.groupBox_P2P_Rx.TabIndex = 13;
            this.groupBox_P2P_Rx.TabStop = false;
            this.groupBox_P2P_Rx.Text = "Receive";
            // 
            // richTextBox_P2P_Receive
            // 
            this.richTextBox_P2P_Receive.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold);
            this.richTextBox_P2P_Receive.Location = new System.Drawing.Point(6, 29);
            this.richTextBox_P2P_Receive.Name = "richTextBox_P2P_Receive";
            this.richTextBox_P2P_Receive.Size = new System.Drawing.Size(653, 162);
            this.richTextBox_P2P_Receive.TabIndex = 10;
            this.richTextBox_P2P_Receive.Text = "";
            this.richTextBox_P2P_Receive.LinkClicked += new System.Windows.Forms.LinkClickedEventHandler(this.richTextBoxP2PReceive_LinkClicked);
            this.richTextBox_P2P_Receive.TextChanged += new System.EventHandler(this.richTextBoxP2PReceive_TextChanged);
            // 
            // groupBox_P2P_Tx
            // 
            this.groupBox_P2P_Tx.Controls.Add(this.label_P2P_Max_Size_Msg);
            this.groupBox_P2P_Tx.Controls.Add(this.button_P2P_Send);
            this.groupBox_P2P_Tx.Controls.Add(this.tabControl_P2P_Transmit);
            this.groupBox_P2P_Tx.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_P2P_Tx.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_P2P_Tx.Location = new System.Drawing.Point(10, 216);
            this.groupBox_P2P_Tx.Name = "groupBox_P2P_Tx";
            this.groupBox_P2P_Tx.Size = new System.Drawing.Size(928, 307);
            this.groupBox_P2P_Tx.TabIndex = 15;
            this.groupBox_P2P_Tx.TabStop = false;
            this.groupBox_P2P_Tx.Text = "Transmit";
            // 
            // label_P2P_Max_Size_Msg
            // 
            this.label_P2P_Max_Size_Msg.AutoSize = true;
            this.label_P2P_Max_Size_Msg.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_P2P_Max_Size_Msg.ForeColor = System.Drawing.Color.DarkGreen;
            this.label_P2P_Max_Size_Msg.Location = new System.Drawing.Point(130, 30);
            this.label_P2P_Max_Size_Msg.Name = "label_P2P_Max_Size_Msg";
            this.label_P2P_Max_Size_Msg.Size = new System.Drawing.Size(740, 24);
            this.label_P2P_Max_Size_Msg.TabIndex = 15;
            this.label_P2P_Max_Size_Msg.Text = "  Data packets will be sent in NDEF Format. Maximum packet size is 235 bytes.";
            // 
            // button_P2P_Send
            // 
            this.button_P2P_Send.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_P2P_Send.Location = new System.Drawing.Point(717, 116);
            this.button_P2P_Send.Name = "button_P2P_Send";
            this.button_P2P_Send.Size = new System.Drawing.Size(181, 143);
            this.button_P2P_Send.TabIndex = 12;
            this.button_P2P_Send.Text = "SEND";
            this.button_P2P_Send.UseVisualStyleBackColor = true;
            this.button_P2P_Send.Click += new System.EventHandler(this.buttonP2PSend_Click);
            // 
            // tabControl_P2P_Transmit
            // 
            this.tabControl_P2P_Transmit.Controls.Add(this.tabP2PRTDText);
            this.tabControl_P2P_Transmit.Controls.Add(this.tabP2PRTDURI);
            this.tabControl_P2P_Transmit.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.tabControl_P2P_Transmit.Location = new System.Drawing.Point(6, 29);
            this.tabControl_P2P_Transmit.Name = "tabControl_P2P_Transmit";
            this.tabControl_P2P_Transmit.Padding = new System.Drawing.Point(12, 6);
            this.tabControl_P2P_Transmit.SelectedIndex = 0;
            this.tabControl_P2P_Transmit.Size = new System.Drawing.Size(691, 261);
            this.tabControl_P2P_Transmit.TabIndex = 11;
            // 
            // tabP2PRTDText
            // 
            this.tabP2PRTDText.BackColor = System.Drawing.Color.LightGray;
            this.tabP2PRTDText.Controls.Add(this.textBox_P2P_RTD_Text_Counter);
            this.tabP2PRTDText.Controls.Add(this.label_P2P_RTD_Text);
            this.tabP2PRTDText.Controls.Add(this.richTextBox_P2P_RTD_Text);
            this.tabP2PRTDText.Location = new System.Drawing.Point(4, 31);
            this.tabP2PRTDText.Name = "tabP2PRTDText";
            this.tabP2PRTDText.Padding = new System.Windows.Forms.Padding(3);
            this.tabP2PRTDText.Size = new System.Drawing.Size(683, 226);
            this.tabP2PRTDText.TabIndex = 0;
            this.tabP2PRTDText.Text = "Text";
            // 
            // textBox_P2P_RTD_Text_Counter
            // 
            this.textBox_P2P_RTD_Text_Counter.BackColor = System.Drawing.SystemColors.Control;
            this.textBox_P2P_RTD_Text_Counter.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_P2P_RTD_Text_Counter.Location = new System.Drawing.Point(476, 201);
            this.textBox_P2P_RTD_Text_Counter.Name = "textBox_P2P_RTD_Text_Counter";
            this.textBox_P2P_RTD_Text_Counter.ReadOnly = true;
            this.textBox_P2P_RTD_Text_Counter.Size = new System.Drawing.Size(201, 22);
            this.textBox_P2P_RTD_Text_Counter.TabIndex = 11;
            // 
            // label_P2P_RTD_Text
            // 
            this.label_P2P_RTD_Text.AutoSize = true;
            this.label_P2P_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_RTD_Text.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_RTD_Text.Location = new System.Drawing.Point(6, 20);
            this.label_P2P_RTD_Text.Name = "label_P2P_RTD_Text";
            this.label_P2P_RTD_Text.Size = new System.Drawing.Size(55, 25);
            this.label_P2P_RTD_Text.TabIndex = 1;
            this.label_P2P_RTD_Text.Text = "Text";
            // 
            // richTextBox_P2P_RTD_Text
            // 
            this.richTextBox_P2P_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_P2P_RTD_Text.Location = new System.Drawing.Point(69, 20);
            this.richTextBox_P2P_RTD_Text.MaxLength = 235;
            this.richTextBox_P2P_RTD_Text.Name = "richTextBox_P2P_RTD_Text";
            this.richTextBox_P2P_RTD_Text.Size = new System.Drawing.Size(608, 179);
            this.richTextBox_P2P_RTD_Text.TabIndex = 0;
            this.richTextBox_P2P_RTD_Text.Text = "Peer-to-Peer is awesome!";
            this.richTextBox_P2P_RTD_Text.TextChanged += new System.EventHandler(this.richTextBox_P2P_RTD_Text_TextChanged);
            // 
            // tabP2PRTDURI
            // 
            this.tabP2PRTDURI.BackColor = System.Drawing.Color.LightGray;
            this.tabP2PRTDURI.Controls.Add(this.richTextBox_P2P_RTD_URI);
            this.tabP2PRTDURI.Controls.Add(this.label_P2P_RTD_URI);
            this.tabP2PRTDURI.Location = new System.Drawing.Point(4, 31);
            this.tabP2PRTDURI.Name = "tabP2PRTDURI";
            this.tabP2PRTDURI.Padding = new System.Windows.Forms.Padding(3);
            this.tabP2PRTDURI.Size = new System.Drawing.Size(683, 226);
            this.tabP2PRTDURI.TabIndex = 1;
            this.tabP2PRTDURI.Text = "URL";
            // 
            // richTextBox_P2P_RTD_URI
            // 
            this.richTextBox_P2P_RTD_URI.DetectUrls = false;
            this.richTextBox_P2P_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_P2P_RTD_URI.Location = new System.Drawing.Point(69, 22);
            this.richTextBox_P2P_RTD_URI.MaxLength = 249;
            this.richTextBox_P2P_RTD_URI.Name = "richTextBox_P2P_RTD_URI";
            this.richTextBox_P2P_RTD_URI.Size = new System.Drawing.Size(393, 33);
            this.richTextBox_P2P_RTD_URI.TabIndex = 3;
            this.richTextBox_P2P_RTD_URI.Text = "http://www.ti.com/nfc";
            // 
            // label_P2P_RTD_URI
            // 
            this.label_P2P_RTD_URI.AutoSize = true;
            this.label_P2P_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_P2P_RTD_URI.ForeColor = System.Drawing.Color.Black;
            this.label_P2P_RTD_URI.Location = new System.Drawing.Point(6, 20);
            this.label_P2P_RTD_URI.Name = "label_P2P_RTD_URI";
            this.label_P2P_RTD_URI.Size = new System.Drawing.Size(53, 25);
            this.label_P2P_RTD_URI.TabIndex = 2;
            this.label_P2P_RTD_URI.Text = "URL";
            // 
            // tabCardEmulation
            // 
            this.tabCardEmulation.BackColor = System.Drawing.Color.LightGray;
            this.tabCardEmulation.Controls.Add(this.groupBox_CE_Emulated_Content);
            this.tabCardEmulation.Controls.Add(this.groupBox_CE_Read_Write_Status);
            this.tabCardEmulation.Controls.Add(this.groupBox_CE_Status);
            this.tabCardEmulation.Location = new System.Drawing.Point(4, 31);
            this.tabCardEmulation.Name = "tabCardEmulation";
            this.tabCardEmulation.Size = new System.Drawing.Size(955, 552);
            this.tabCardEmulation.TabIndex = 8;
            this.tabCardEmulation.Text = "Card Emulation";
            // 
            // groupBox_CE_Emulated_Content
            // 
            this.groupBox_CE_Emulated_Content.Controls.Add(this.label_CE_Max_Size_Msg);
            this.groupBox_CE_Emulated_Content.Controls.Add(this.button_CE_Emulate);
            this.groupBox_CE_Emulated_Content.Controls.Add(this.tabControl_CE_Emulated_Content);
            this.groupBox_CE_Emulated_Content.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_CE_Emulated_Content.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_CE_Emulated_Content.Location = new System.Drawing.Point(10, 216);
            this.groupBox_CE_Emulated_Content.Name = "groupBox_CE_Emulated_Content";
            this.groupBox_CE_Emulated_Content.Size = new System.Drawing.Size(928, 307);
            this.groupBox_CE_Emulated_Content.TabIndex = 20;
            this.groupBox_CE_Emulated_Content.TabStop = false;
            this.groupBox_CE_Emulated_Content.Text = "Emulated Content";
            // 
            // label_CE_Max_Size_Msg
            // 
            this.label_CE_Max_Size_Msg.AutoSize = true;
            this.label_CE_Max_Size_Msg.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_CE_Max_Size_Msg.ForeColor = System.Drawing.Color.DarkGreen;
            this.label_CE_Max_Size_Msg.Location = new System.Drawing.Point(130, 30);
            this.label_CE_Max_Size_Msg.Name = "label_CE_Max_Size_Msg";
            this.label_CE_Max_Size_Msg.Size = new System.Drawing.Size(470, 24);
            this.label_CE_Max_Size_Msg.TabIndex = 14;
            this.label_CE_Max_Size_Msg.Text = "  Maximum size of emulated tag data is 246 bytes.";
            // 
            // button_CE_Emulate
            // 
            this.button_CE_Emulate.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold);
            this.button_CE_Emulate.Location = new System.Drawing.Point(717, 116);
            this.button_CE_Emulate.Name = "button_CE_Emulate";
            this.button_CE_Emulate.Size = new System.Drawing.Size(181, 143);
            this.button_CE_Emulate.TabIndex = 12;
            this.button_CE_Emulate.Text = "EMULATE";
            this.button_CE_Emulate.UseVisualStyleBackColor = true;
            this.button_CE_Emulate.Click += new System.EventHandler(this.button_CE_Configure_Click);
            // 
            // tabControl_CE_Emulated_Content
            // 
            this.tabControl_CE_Emulated_Content.Controls.Add(this.tabCERTDText);
            this.tabControl_CE_Emulated_Content.Controls.Add(this.tabCERTDURI);
            this.tabControl_CE_Emulated_Content.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.tabControl_CE_Emulated_Content.Location = new System.Drawing.Point(6, 29);
            this.tabControl_CE_Emulated_Content.Name = "tabControl_CE_Emulated_Content";
            this.tabControl_CE_Emulated_Content.Padding = new System.Drawing.Point(12, 6);
            this.tabControl_CE_Emulated_Content.SelectedIndex = 0;
            this.tabControl_CE_Emulated_Content.Size = new System.Drawing.Size(691, 261);
            this.tabControl_CE_Emulated_Content.TabIndex = 11;
            // 
            // tabCERTDText
            // 
            this.tabCERTDText.BackColor = System.Drawing.Color.LightGray;
            this.tabCERTDText.Controls.Add(this.textBox_CE_RTD_Text_Counter);
            this.tabCERTDText.Controls.Add(this.label_CE_RTD_Text);
            this.tabCERTDText.Controls.Add(this.richTextBox_CE_RTD_Text);
            this.tabCERTDText.Location = new System.Drawing.Point(4, 31);
            this.tabCERTDText.Name = "tabCERTDText";
            this.tabCERTDText.Padding = new System.Windows.Forms.Padding(3);
            this.tabCERTDText.Size = new System.Drawing.Size(683, 226);
            this.tabCERTDText.TabIndex = 0;
            this.tabCERTDText.Text = "Text";
            // 
            // textBox_CE_RTD_Text_Counter
            // 
            this.textBox_CE_RTD_Text_Counter.BackColor = System.Drawing.SystemColors.Control;
            this.textBox_CE_RTD_Text_Counter.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_CE_RTD_Text_Counter.Location = new System.Drawing.Point(476, 201);
            this.textBox_CE_RTD_Text_Counter.Name = "textBox_CE_RTD_Text_Counter";
            this.textBox_CE_RTD_Text_Counter.ReadOnly = true;
            this.textBox_CE_RTD_Text_Counter.Size = new System.Drawing.Size(201, 22);
            this.textBox_CE_RTD_Text_Counter.TabIndex = 11;
            // 
            // label_CE_RTD_Text
            // 
            this.label_CE_RTD_Text.AutoSize = true;
            this.label_CE_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_RTD_Text.ForeColor = System.Drawing.Color.Black;
            this.label_CE_RTD_Text.Location = new System.Drawing.Point(6, 20);
            this.label_CE_RTD_Text.Name = "label_CE_RTD_Text";
            this.label_CE_RTD_Text.Size = new System.Drawing.Size(55, 25);
            this.label_CE_RTD_Text.TabIndex = 1;
            this.label_CE_RTD_Text.Text = "Text";
            // 
            // richTextBox_CE_RTD_Text
            // 
            this.richTextBox_CE_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_CE_RTD_Text.Location = new System.Drawing.Point(69, 20);
            this.richTextBox_CE_RTD_Text.MaxLength = 246;
            this.richTextBox_CE_RTD_Text.Name = "richTextBox_CE_RTD_Text";
            this.richTextBox_CE_RTD_Text.Size = new System.Drawing.Size(608, 179);
            this.richTextBox_CE_RTD_Text.TabIndex = 0;
            this.richTextBox_CE_RTD_Text.Text = "Card Emulation is phenomenal!";
            this.richTextBox_CE_RTD_Text.TextChanged += new System.EventHandler(this.richTextBox_CE_RTD_Text_TextChanged);
            // 
            // tabCERTDURI
            // 
            this.tabCERTDURI.BackColor = System.Drawing.Color.LightGray;
            this.tabCERTDURI.Controls.Add(this.richTextBox_CE_RTD_URI);
            this.tabCERTDURI.Controls.Add(this.label_CE_RTD_URI);
            this.tabCERTDURI.Location = new System.Drawing.Point(4, 31);
            this.tabCERTDURI.Name = "tabCERTDURI";
            this.tabCERTDURI.Padding = new System.Windows.Forms.Padding(3);
            this.tabCERTDURI.Size = new System.Drawing.Size(683, 226);
            this.tabCERTDURI.TabIndex = 1;
            this.tabCERTDURI.Text = "URL";
            // 
            // richTextBox_CE_RTD_URI
            // 
            this.richTextBox_CE_RTD_URI.DetectUrls = false;
            this.richTextBox_CE_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_CE_RTD_URI.Location = new System.Drawing.Point(69, 22);
            this.richTextBox_CE_RTD_URI.MaxLength = 249;
            this.richTextBox_CE_RTD_URI.Name = "richTextBox_CE_RTD_URI";
            this.richTextBox_CE_RTD_URI.Size = new System.Drawing.Size(393, 33);
            this.richTextBox_CE_RTD_URI.TabIndex = 3;
            this.richTextBox_CE_RTD_URI.Text = "http://www.ti.com/nfc";
            // 
            // label_CE_RTD_URI
            // 
            this.label_CE_RTD_URI.AutoSize = true;
            this.label_CE_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_RTD_URI.ForeColor = System.Drawing.Color.Black;
            this.label_CE_RTD_URI.Location = new System.Drawing.Point(6, 20);
            this.label_CE_RTD_URI.Name = "label_CE_RTD_URI";
            this.label_CE_RTD_URI.Size = new System.Drawing.Size(53, 25);
            this.label_CE_RTD_URI.TabIndex = 2;
            this.label_CE_RTD_URI.Text = "URL";
            // 
            // groupBox_CE_Read_Write_Status
            // 
            this.groupBox_CE_Read_Write_Status.Controls.Add(this.richTextBox_CE_Status);
            this.groupBox_CE_Read_Write_Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_CE_Read_Write_Status.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_CE_Read_Write_Status.Location = new System.Drawing.Point(421, 6);
            this.groupBox_CE_Read_Write_Status.Name = "groupBox_CE_Read_Write_Status";
            this.groupBox_CE_Read_Write_Status.Size = new System.Drawing.Size(517, 203);
            this.groupBox_CE_Read_Write_Status.TabIndex = 19;
            this.groupBox_CE_Read_Write_Status.TabStop = false;
            this.groupBox_CE_Read_Write_Status.Text = "Received Read / Write Cmds";
            // 
            // richTextBox_CE_Status
            // 
            this.richTextBox_CE_Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold);
            this.richTextBox_CE_Status.Location = new System.Drawing.Point(6, 29);
            this.richTextBox_CE_Status.Name = "richTextBox_CE_Status";
            this.richTextBox_CE_Status.Size = new System.Drawing.Size(500, 162);
            this.richTextBox_CE_Status.TabIndex = 10;
            this.richTextBox_CE_Status.Text = "";
            this.richTextBox_CE_Status.TextChanged += new System.EventHandler(this.richTextBox_CE_Status_TextChanged);
            // 
            // groupBox_CE_Status
            // 
            this.groupBox_CE_Status.Controls.Add(this.textBox_CE_Tag_Type);
            this.groupBox_CE_Status.Controls.Add(this.textBox_CE_Technology);
            this.groupBox_CE_Status.Controls.Add(this.label_CE_Rssi);
            this.groupBox_CE_Status.Controls.Add(this.label_CE_Tag_Type);
            this.groupBox_CE_Status.Controls.Add(this.label_CE_Technology);
            this.groupBox_CE_Status.Controls.Add(this.textBox_CE_UID);
            this.groupBox_CE_Status.Controls.Add(this.textBox_CE_Rssi);
            this.groupBox_CE_Status.Controls.Add(this.label_CE_Bitrate);
            this.groupBox_CE_Status.Controls.Add(this.textBox_CE_Bitrate);
            this.groupBox_CE_Status.Controls.Add(this.label_CE_UID);
            this.groupBox_CE_Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_CE_Status.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_CE_Status.Location = new System.Drawing.Point(10, 7);
            this.groupBox_CE_Status.Name = "groupBox_CE_Status";
            this.groupBox_CE_Status.Size = new System.Drawing.Size(405, 203);
            this.groupBox_CE_Status.TabIndex = 18;
            this.groupBox_CE_Status.TabStop = false;
            this.groupBox_CE_Status.Text = "CE Status";
            // 
            // textBox_CE_Tag_Type
            // 
            this.textBox_CE_Tag_Type.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_CE_Tag_Type.Location = new System.Drawing.Point(134, 20);
            this.textBox_CE_Tag_Type.Name = "textBox_CE_Tag_Type";
            this.textBox_CE_Tag_Type.ReadOnly = true;
            this.textBox_CE_Tag_Type.Size = new System.Drawing.Size(255, 30);
            this.textBox_CE_Tag_Type.TabIndex = 5;
            // 
            // textBox_CE_Technology
            // 
            this.textBox_CE_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_CE_Technology.Location = new System.Drawing.Point(134, 92);
            this.textBox_CE_Technology.Name = "textBox_CE_Technology";
            this.textBox_CE_Technology.ReadOnly = true;
            this.textBox_CE_Technology.Size = new System.Drawing.Size(255, 30);
            this.textBox_CE_Technology.TabIndex = 0;
            // 
            // label_CE_Rssi
            // 
            this.label_CE_Rssi.AutoSize = true;
            this.label_CE_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_Rssi.ForeColor = System.Drawing.Color.Black;
            this.label_CE_Rssi.Location = new System.Drawing.Point(6, 131);
            this.label_CE_Rssi.Name = "label_CE_Rssi";
            this.label_CE_Rssi.Size = new System.Drawing.Size(62, 25);
            this.label_CE_Rssi.TabIndex = 6;
            this.label_CE_Rssi.Text = "RSSI";
            // 
            // label_CE_Tag_Type
            // 
            this.label_CE_Tag_Type.AutoSize = true;
            this.label_CE_Tag_Type.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_Tag_Type.ForeColor = System.Drawing.Color.Black;
            this.label_CE_Tag_Type.Location = new System.Drawing.Point(6, 23);
            this.label_CE_Tag_Type.Name = "label_CE_Tag_Type";
            this.label_CE_Tag_Type.Size = new System.Drawing.Size(105, 25);
            this.label_CE_Tag_Type.TabIndex = 1;
            this.label_CE_Tag_Type.Text = "Tag Type";
            // 
            // label_CE_Technology
            // 
            this.label_CE_Technology.AutoSize = true;
            this.label_CE_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_Technology.ForeColor = System.Drawing.Color.Black;
            this.label_CE_Technology.Location = new System.Drawing.Point(6, 95);
            this.label_CE_Technology.Name = "label_CE_Technology";
            this.label_CE_Technology.Size = new System.Drawing.Size(125, 25);
            this.label_CE_Technology.TabIndex = 4;
            this.label_CE_Technology.Text = "Technology";
            // 
            // textBox_CE_UID
            // 
            this.textBox_CE_UID.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_CE_UID.Location = new System.Drawing.Point(134, 164);
            this.textBox_CE_UID.Name = "textBox_CE_UID";
            this.textBox_CE_UID.ReadOnly = true;
            this.textBox_CE_UID.Size = new System.Drawing.Size(254, 30);
            this.textBox_CE_UID.TabIndex = 9;
            // 
            // textBox_CE_Rssi
            // 
            this.textBox_CE_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_CE_Rssi.Location = new System.Drawing.Point(134, 128);
            this.textBox_CE_Rssi.Name = "textBox_CE_Rssi";
            this.textBox_CE_Rssi.ReadOnly = true;
            this.textBox_CE_Rssi.Size = new System.Drawing.Size(254, 30);
            this.textBox_CE_Rssi.TabIndex = 7;
            // 
            // label_CE_Bitrate
            // 
            this.label_CE_Bitrate.AutoSize = true;
            this.label_CE_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_Bitrate.ForeColor = System.Drawing.Color.Black;
            this.label_CE_Bitrate.Location = new System.Drawing.Point(6, 59);
            this.label_CE_Bitrate.Name = "label_CE_Bitrate";
            this.label_CE_Bitrate.Size = new System.Drawing.Size(74, 25);
            this.label_CE_Bitrate.TabIndex = 2;
            this.label_CE_Bitrate.Text = "Bitrate";
            // 
            // textBox_CE_Bitrate
            // 
            this.textBox_CE_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_CE_Bitrate.Location = new System.Drawing.Point(134, 56);
            this.textBox_CE_Bitrate.Name = "textBox_CE_Bitrate";
            this.textBox_CE_Bitrate.ReadOnly = true;
            this.textBox_CE_Bitrate.Size = new System.Drawing.Size(255, 30);
            this.textBox_CE_Bitrate.TabIndex = 3;
            // 
            // label_CE_UID
            // 
            this.label_CE_UID.AutoSize = true;
            this.label_CE_UID.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_CE_UID.ForeColor = System.Drawing.Color.Black;
            this.label_CE_UID.Location = new System.Drawing.Point(6, 167);
            this.label_CE_UID.Name = "label_CE_UID";
            this.label_CE_UID.Size = new System.Drawing.Size(48, 25);
            this.label_CE_UID.TabIndex = 8;
            this.label_CE_UID.Text = "UID";
            // 
            // tabReaderWriter
            // 
            this.tabReaderWriter.BackColor = System.Drawing.Color.LightGray;
            this.tabReaderWriter.Controls.Add(this.groupBox_RW_Tag_Write);
            this.tabReaderWriter.Controls.Add(this.groupBox_RW_Tag_Content);
            this.tabReaderWriter.Controls.Add(this.groupBox_RW_Status);
            this.tabReaderWriter.Location = new System.Drawing.Point(4, 31);
            this.tabReaderWriter.Name = "tabReaderWriter";
            this.tabReaderWriter.Size = new System.Drawing.Size(955, 552);
            this.tabReaderWriter.TabIndex = 7;
            this.tabReaderWriter.Text = "Reader/Writer";
            // 
            // groupBox_RW_Tag_Write
            // 
            this.groupBox_RW_Tag_Write.Controls.Add(this.button_RW_Write);
            this.groupBox_RW_Tag_Write.Controls.Add(this.button_RW_TagFormat);
            this.groupBox_RW_Tag_Write.Controls.Add(this.tabControl_RW_Tag_Write);
            this.groupBox_RW_Tag_Write.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_RW_Tag_Write.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_RW_Tag_Write.Location = new System.Drawing.Point(10, 249);
            this.groupBox_RW_Tag_Write.Name = "groupBox_RW_Tag_Write";
            this.groupBox_RW_Tag_Write.Size = new System.Drawing.Size(928, 274);
            this.groupBox_RW_Tag_Write.TabIndex = 22;
            this.groupBox_RW_Tag_Write.TabStop = false;
            this.groupBox_RW_Tag_Write.Text = "Tag Write";
            // 
            // button_RW_Write
            // 
            this.button_RW_Write.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold);
            this.button_RW_Write.Location = new System.Drawing.Point(717, 63);
            this.button_RW_Write.Name = "button_RW_Write";
            this.button_RW_Write.Size = new System.Drawing.Size(181, 115);
            this.button_RW_Write.TabIndex = 12;
            this.button_RW_Write.Text = "WRITE";
            this.button_RW_Write.UseVisualStyleBackColor = true;
            this.button_RW_Write.Click += new System.EventHandler(this.button_RW_Write_Click);
            // 
            // button_RW_TagFormat
            // 
            this.button_RW_TagFormat.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Bold);
            this.button_RW_TagFormat.Location = new System.Drawing.Point(717, 184);
            this.button_RW_TagFormat.Name = "button_RW_TagFormat";
            this.button_RW_TagFormat.Size = new System.Drawing.Size(181, 69);
            this.button_RW_TagFormat.TabIndex = 14;
            this.button_RW_TagFormat.Text = " FORMAT   T5T TAG";
            this.button_RW_TagFormat.UseVisualStyleBackColor = true;
            this.button_RW_TagFormat.Click += new System.EventHandler(this.button_RW_TagFormat_Click);
            // 
            // tabControl_RW_Tag_Write
            // 
            this.tabControl_RW_Tag_Write.Controls.Add(this.tabRWRTDText);
            this.tabControl_RW_Tag_Write.Controls.Add(this.tabRWRTDURI);
            this.tabControl_RW_Tag_Write.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.tabControl_RW_Tag_Write.Location = new System.Drawing.Point(6, 32);
            this.tabControl_RW_Tag_Write.Name = "tabControl_RW_Tag_Write";
            this.tabControl_RW_Tag_Write.Padding = new System.Drawing.Point(12, 6);
            this.tabControl_RW_Tag_Write.SelectedIndex = 0;
            this.tabControl_RW_Tag_Write.Size = new System.Drawing.Size(691, 225);
            this.tabControl_RW_Tag_Write.TabIndex = 11;
            // 
            // tabRWRTDText
            // 
            this.tabRWRTDText.BackColor = System.Drawing.Color.LightGray;
            this.tabRWRTDText.Controls.Add(this.textBox_RW_RTD_Text_Counter);
            this.tabRWRTDText.Controls.Add(this.label_RW_RTD_Text);
            this.tabRWRTDText.Controls.Add(this.richTextBox_RW_RTD_Text);
            this.tabRWRTDText.Location = new System.Drawing.Point(4, 31);
            this.tabRWRTDText.Name = "tabRWRTDText";
            this.tabRWRTDText.Padding = new System.Windows.Forms.Padding(3);
            this.tabRWRTDText.Size = new System.Drawing.Size(683, 190);
            this.tabRWRTDText.TabIndex = 0;
            this.tabRWRTDText.Text = "Text";
            // 
            // textBox_RW_RTD_Text_Counter
            // 
            this.textBox_RW_RTD_Text_Counter.BackColor = System.Drawing.SystemColors.Control;
            this.textBox_RW_RTD_Text_Counter.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_RW_RTD_Text_Counter.Location = new System.Drawing.Point(476, 165);
            this.textBox_RW_RTD_Text_Counter.Name = "textBox_RW_RTD_Text_Counter";
            this.textBox_RW_RTD_Text_Counter.ReadOnly = true;
            this.textBox_RW_RTD_Text_Counter.Size = new System.Drawing.Size(201, 22);
            this.textBox_RW_RTD_Text_Counter.TabIndex = 10;
            // 
            // label_RW_RTD_Text
            // 
            this.label_RW_RTD_Text.AutoSize = true;
            this.label_RW_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_RTD_Text.ForeColor = System.Drawing.Color.Black;
            this.label_RW_RTD_Text.Location = new System.Drawing.Point(6, 20);
            this.label_RW_RTD_Text.Name = "label_RW_RTD_Text";
            this.label_RW_RTD_Text.Size = new System.Drawing.Size(55, 25);
            this.label_RW_RTD_Text.TabIndex = 1;
            this.label_RW_RTD_Text.Text = "Text";
            // 
            // richTextBox_RW_RTD_Text
            // 
            this.richTextBox_RW_RTD_Text.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_RW_RTD_Text.Location = new System.Drawing.Point(69, 20);
            this.richTextBox_RW_RTD_Text.MaxLength = 246;
            this.richTextBox_RW_RTD_Text.Name = "richTextBox_RW_RTD_Text";
            this.richTextBox_RW_RTD_Text.Size = new System.Drawing.Size(608, 143);
            this.richTextBox_RW_RTD_Text.TabIndex = 0;
            this.richTextBox_RW_RTD_Text.Text = "Reader/Writer is amazing!";
            this.richTextBox_RW_RTD_Text.TextChanged += new System.EventHandler(this.richTextBox_RW_RTD_Text_TextChanged);
            // 
            // tabRWRTDURI
            // 
            this.tabRWRTDURI.BackColor = System.Drawing.Color.LightGray;
            this.tabRWRTDURI.Controls.Add(this.richTextBox_RW_RTD_URI);
            this.tabRWRTDURI.Controls.Add(this.label_RW_RTD_URI);
            this.tabRWRTDURI.Location = new System.Drawing.Point(4, 31);
            this.tabRWRTDURI.Name = "tabRWRTDURI";
            this.tabRWRTDURI.Padding = new System.Windows.Forms.Padding(3);
            this.tabRWRTDURI.Size = new System.Drawing.Size(683, 190);
            this.tabRWRTDURI.TabIndex = 1;
            this.tabRWRTDURI.Text = "URL";
            // 
            // richTextBox_RW_RTD_URI
            // 
            this.richTextBox_RW_RTD_URI.DetectUrls = false;
            this.richTextBox_RW_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.richTextBox_RW_RTD_URI.Location = new System.Drawing.Point(69, 20);
            this.richTextBox_RW_RTD_URI.MaxLength = 249;
            this.richTextBox_RW_RTD_URI.Name = "richTextBox_RW_RTD_URI";
            this.richTextBox_RW_RTD_URI.Size = new System.Drawing.Size(556, 33);
            this.richTextBox_RW_RTD_URI.TabIndex = 3;
            this.richTextBox_RW_RTD_URI.Text = "http://www.ti.com/nfc";
            // 
            // label_RW_RTD_URI
            // 
            this.label_RW_RTD_URI.AutoSize = true;
            this.label_RW_RTD_URI.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_RTD_URI.ForeColor = System.Drawing.Color.Black;
            this.label_RW_RTD_URI.Location = new System.Drawing.Point(6, 20);
            this.label_RW_RTD_URI.Name = "label_RW_RTD_URI";
            this.label_RW_RTD_URI.Size = new System.Drawing.Size(53, 25);
            this.label_RW_RTD_URI.TabIndex = 2;
            this.label_RW_RTD_URI.Text = "URL";
            // 
            // groupBox_RW_Tag_Content
            // 
            this.groupBox_RW_Tag_Content.Controls.Add(this.richTextBox_RW_Tag_Content);
            this.groupBox_RW_Tag_Content.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_RW_Tag_Content.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_RW_Tag_Content.Location = new System.Drawing.Point(421, 6);
            this.groupBox_RW_Tag_Content.Name = "groupBox_RW_Tag_Content";
            this.groupBox_RW_Tag_Content.Size = new System.Drawing.Size(517, 237);
            this.groupBox_RW_Tag_Content.TabIndex = 21;
            this.groupBox_RW_Tag_Content.TabStop = false;
            this.groupBox_RW_Tag_Content.Text = "Tag Content";
            // 
            // richTextBox_RW_Tag_Content
            // 
            this.richTextBox_RW_Tag_Content.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBox_RW_Tag_Content.Location = new System.Drawing.Point(6, 29);
            this.richTextBox_RW_Tag_Content.Name = "richTextBox_RW_Tag_Content";
            this.richTextBox_RW_Tag_Content.Size = new System.Drawing.Size(500, 196);
            this.richTextBox_RW_Tag_Content.TabIndex = 10;
            this.richTextBox_RW_Tag_Content.Text = "";
            this.richTextBox_RW_Tag_Content.TextChanged += new System.EventHandler(this.richTextBox_RW_Tag_Content_TextChanged);
            // 
            // groupBox_RW_Status
            // 
            this.groupBox_RW_Status.Controls.Add(this.textBox_RW_Tag_Type);
            this.groupBox_RW_Status.Controls.Add(this.textBox_RW_Technology);
            this.groupBox_RW_Status.Controls.Add(this.label_RW_Rssi);
            this.groupBox_RW_Status.Controls.Add(this.label_RW_Tag_Type);
            this.groupBox_RW_Status.Controls.Add(this.label_RW_Technology);
            this.groupBox_RW_Status.Controls.Add(this.textBox_RW_UID);
            this.groupBox_RW_Status.Controls.Add(this.textBox_RW_Rssi);
            this.groupBox_RW_Status.Controls.Add(this.label_RW_Bitrate);
            this.groupBox_RW_Status.Controls.Add(this.textBox_RW_Bitrate);
            this.groupBox_RW_Status.Controls.Add(this.label_RW_UID);
            this.groupBox_RW_Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.groupBox_RW_Status.ForeColor = System.Drawing.Color.Maroon;
            this.groupBox_RW_Status.Location = new System.Drawing.Point(10, 7);
            this.groupBox_RW_Status.Name = "groupBox_RW_Status";
            this.groupBox_RW_Status.Size = new System.Drawing.Size(405, 236);
            this.groupBox_RW_Status.TabIndex = 20;
            this.groupBox_RW_Status.TabStop = false;
            this.groupBox_RW_Status.Text = "RW Status";
            // 
            // textBox_RW_Tag_Type
            // 
            this.textBox_RW_Tag_Type.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_RW_Tag_Type.Location = new System.Drawing.Point(134, 20);
            this.textBox_RW_Tag_Type.Name = "textBox_RW_Tag_Type";
            this.textBox_RW_Tag_Type.ReadOnly = true;
            this.textBox_RW_Tag_Type.Size = new System.Drawing.Size(255, 30);
            this.textBox_RW_Tag_Type.TabIndex = 5;
            // 
            // textBox_RW_Technology
            // 
            this.textBox_RW_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_RW_Technology.Location = new System.Drawing.Point(134, 92);
            this.textBox_RW_Technology.Name = "textBox_RW_Technology";
            this.textBox_RW_Technology.ReadOnly = true;
            this.textBox_RW_Technology.Size = new System.Drawing.Size(255, 30);
            this.textBox_RW_Technology.TabIndex = 0;
            // 
            // label_RW_Rssi
            // 
            this.label_RW_Rssi.AutoSize = true;
            this.label_RW_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_Rssi.ForeColor = System.Drawing.Color.Black;
            this.label_RW_Rssi.Location = new System.Drawing.Point(6, 131);
            this.label_RW_Rssi.Name = "label_RW_Rssi";
            this.label_RW_Rssi.Size = new System.Drawing.Size(62, 25);
            this.label_RW_Rssi.TabIndex = 6;
            this.label_RW_Rssi.Text = "RSSI";
            // 
            // label_RW_Tag_Type
            // 
            this.label_RW_Tag_Type.AutoSize = true;
            this.label_RW_Tag_Type.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_Tag_Type.ForeColor = System.Drawing.Color.Black;
            this.label_RW_Tag_Type.Location = new System.Drawing.Point(6, 23);
            this.label_RW_Tag_Type.Name = "label_RW_Tag_Type";
            this.label_RW_Tag_Type.Size = new System.Drawing.Size(105, 25);
            this.label_RW_Tag_Type.TabIndex = 1;
            this.label_RW_Tag_Type.Text = "Tag Type";
            // 
            // label_RW_Technology
            // 
            this.label_RW_Technology.AutoSize = true;
            this.label_RW_Technology.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_Technology.ForeColor = System.Drawing.Color.Black;
            this.label_RW_Technology.Location = new System.Drawing.Point(6, 95);
            this.label_RW_Technology.Name = "label_RW_Technology";
            this.label_RW_Technology.Size = new System.Drawing.Size(125, 25);
            this.label_RW_Technology.TabIndex = 4;
            this.label_RW_Technology.Text = "Technology";
            // 
            // textBox_RW_UID
            // 
            this.textBox_RW_UID.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_RW_UID.Location = new System.Drawing.Point(134, 164);
            this.textBox_RW_UID.Name = "textBox_RW_UID";
            this.textBox_RW_UID.ReadOnly = true;
            this.textBox_RW_UID.Size = new System.Drawing.Size(254, 30);
            this.textBox_RW_UID.TabIndex = 9;
            // 
            // textBox_RW_Rssi
            // 
            this.textBox_RW_Rssi.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_RW_Rssi.Location = new System.Drawing.Point(134, 128);
            this.textBox_RW_Rssi.Name = "textBox_RW_Rssi";
            this.textBox_RW_Rssi.ReadOnly = true;
            this.textBox_RW_Rssi.Size = new System.Drawing.Size(254, 30);
            this.textBox_RW_Rssi.TabIndex = 7;
            // 
            // label_RW_Bitrate
            // 
            this.label_RW_Bitrate.AutoSize = true;
            this.label_RW_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_Bitrate.ForeColor = System.Drawing.Color.Black;
            this.label_RW_Bitrate.Location = new System.Drawing.Point(6, 59);
            this.label_RW_Bitrate.Name = "label_RW_Bitrate";
            this.label_RW_Bitrate.Size = new System.Drawing.Size(74, 25);
            this.label_RW_Bitrate.TabIndex = 2;
            this.label_RW_Bitrate.Text = "Bitrate";
            // 
            // textBox_RW_Bitrate
            // 
            this.textBox_RW_Bitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F);
            this.textBox_RW_Bitrate.Location = new System.Drawing.Point(134, 56);
            this.textBox_RW_Bitrate.Name = "textBox_RW_Bitrate";
            this.textBox_RW_Bitrate.ReadOnly = true;
            this.textBox_RW_Bitrate.Size = new System.Drawing.Size(255, 30);
            this.textBox_RW_Bitrate.TabIndex = 3;
            // 
            // label_RW_UID
            // 
            this.label_RW_UID.AutoSize = true;
            this.label_RW_UID.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.label_RW_UID.ForeColor = System.Drawing.Color.Black;
            this.label_RW_UID.Location = new System.Drawing.Point(6, 167);
            this.label_RW_UID.Name = "label_RW_UID";
            this.label_RW_UID.Size = new System.Drawing.Size(48, 25);
            this.label_RW_UID.TabIndex = 8;
            this.label_RW_UID.Text = "UID";
            // 
            // tabCOMLog
            // 
            this.tabCOMLog.BackColor = System.Drawing.Color.LightGray;
            this.tabCOMLog.Controls.Add(this.button_Open_Log);
            this.tabCOMLog.Controls.Add(this.button_Save_Log);
            this.tabCOMLog.Controls.Add(this.button_Clear_Log);
            this.tabCOMLog.Controls.Add(this.serialPortDataTextBox);
            this.tabCOMLog.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabCOMLog.Location = new System.Drawing.Point(4, 31);
            this.tabCOMLog.Name = "tabCOMLog";
            this.tabCOMLog.Size = new System.Drawing.Size(955, 552);
            this.tabCOMLog.TabIndex = 3;
            this.tabCOMLog.Text = "COM Log";
            // 
            // button_Open_Log
            // 
            this.button_Open_Log.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.button_Open_Log.Location = new System.Drawing.Point(138, 4);
            this.button_Open_Log.Name = "button_Open_Log";
            this.button_Open_Log.Size = new System.Drawing.Size(120, 41);
            this.button_Open_Log.TabIndex = 10;
            this.button_Open_Log.Text = "Open Log";
            this.button_Open_Log.UseVisualStyleBackColor = true;
            this.button_Open_Log.Click += new System.EventHandler(this.buttonOpenLog_Click);
            // 
            // button_Save_Log
            // 
            this.button_Save_Log.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.button_Save_Log.Location = new System.Drawing.Point(11, 4);
            this.button_Save_Log.Name = "button_Save_Log";
            this.button_Save_Log.Size = new System.Drawing.Size(120, 41);
            this.button_Save_Log.TabIndex = 9;
            this.button_Save_Log.Text = "Save Log";
            this.button_Save_Log.UseVisualStyleBackColor = true;
            this.button_Save_Log.Click += new System.EventHandler(this.buttonSaveLog_Click);
            // 
            // button_Clear_Log
            // 
            this.button_Clear_Log.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold);
            this.button_Clear_Log.Location = new System.Drawing.Point(804, 4);
            this.button_Clear_Log.Name = "button_Clear_Log";
            this.button_Clear_Log.Size = new System.Drawing.Size(134, 42);
            this.button_Clear_Log.TabIndex = 1;
            this.button_Clear_Log.Text = "Clear Log";
            this.button_Clear_Log.UseVisualStyleBackColor = true;
            this.button_Clear_Log.Click += new System.EventHandler(this.button_Clear_Log_Click);
            // 
            // serialPortDataTextBox
            // 
            this.serialPortDataTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F);
            this.serialPortDataTextBox.Location = new System.Drawing.Point(11, 50);
            this.serialPortDataTextBox.Name = "serialPortDataTextBox";
            this.serialPortDataTextBox.ReadOnly = true;
            this.serialPortDataTextBox.Size = new System.Drawing.Size(927, 463);
            this.serialPortDataTextBox.TabIndex = 0;
            this.serialPortDataTextBox.Text = "";
            this.serialPortDataTextBox.TextChanged += new System.EventHandler(this.serialPortDataTextBox_TextChanged);
            // 
            // tabAdvancedCOM
            // 
            this.tabAdvancedCOM.BackColor = System.Drawing.Color.LightGray;
            this.tabAdvancedCOM.Controls.Add(this.listBox1);
            this.tabAdvancedCOM.Controls.Add(this.button_Refresh_COM);
            this.tabAdvancedCOM.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabAdvancedCOM.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.tabAdvancedCOM.Location = new System.Drawing.Point(4, 31);
            this.tabAdvancedCOM.Name = "tabAdvancedCOM";
            this.tabAdvancedCOM.Padding = new System.Windows.Forms.Padding(3);
            this.tabAdvancedCOM.Size = new System.Drawing.Size(955, 552);
            this.tabAdvancedCOM.TabIndex = 1;
            this.tabAdvancedCOM.Text = "Advanced COM";
            // 
            // tabHWGuide
            // 
            this.tabHWGuide.BackColor = System.Drawing.Color.LightGray;
            this.tabHWGuide.Controls.Add(this.label_Hardware_BP_LEDs);
            this.tabHWGuide.Controls.Add(this.pictureBox_BP_LP_Board);
            this.tabHWGuide.Controls.Add(this.label1);
            this.tabHWGuide.Controls.Add(this.label2);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_LEDs);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_LED3);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_LED1);
            this.tabHWGuide.Controls.Add(this.label19);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_LED2);
            this.tabHWGuide.Controls.Add(this.label8);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_RF_Field);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_GUI);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_RX);
            this.tabHWGuide.Controls.Add(this.label_Hardware_Exp_TX);
            this.tabHWGuide.Controls.Add(this.pictureBox_Exp_Board);
            this.tabHWGuide.Location = new System.Drawing.Point(4, 31);
            this.tabHWGuide.Name = "tabHWGuide";
            this.tabHWGuide.Size = new System.Drawing.Size(955, 552);
            this.tabHWGuide.TabIndex = 6;
            this.tabHWGuide.Text = "Hardware Guide";
            // 
            // label_Hardware_BP_LEDs
            // 
            this.label_Hardware_BP_LEDs.AutoSize = true;
            this.label_Hardware_BP_LEDs.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_BP_LEDs.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_BP_LEDs.Location = new System.Drawing.Point(476, 324);
            this.label_Hardware_BP_LEDs.Name = "label_Hardware_BP_LEDs";
            this.label_Hardware_BP_LEDs.Size = new System.Drawing.Size(476, 192);
            this.label_Hardware_BP_LEDs.TabIndex = 24;
            this.label_Hardware_BP_LEDs.Text = resources.GetString("label_Hardware_BP_LEDs.Text");
            // 
            // pictureBox_BP_LP_Board
            // 
            this.pictureBox_BP_LP_Board.BackColor = System.Drawing.Color.Transparent;
            this.pictureBox_BP_LP_Board.Image = global::TI_NFC_Tool.Properties.Resources.BPButtons_zoomed;
            this.pictureBox_BP_LP_Board.Location = new System.Drawing.Point(15, 318);
            this.pictureBox_BP_LP_Board.Name = "pictureBox_BP_LP_Board";
            this.pictureBox_BP_LP_Board.Size = new System.Drawing.Size(455, 222);
            this.pictureBox_BP_LP_Board.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox_BP_LP_Board.TabIndex = 23;
            this.pictureBox_BP_LP_Board.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(387, 282);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(181, 25);
            this.label1.TabIndex = 22;
            this.label1.Text = "Hardware Guide";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.ForeColor = System.Drawing.Color.Black;
            this.label2.Location = new System.Drawing.Point(311, 255);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(333, 25);
            this.label2.TabIndex = 21;
            this.label2.Text = "DLP7970-ABP + TI LaunchPad";
            // 
            // label_Hardware_Exp_LEDs
            // 
            this.label_Hardware_Exp_LEDs.AutoSize = true;
            this.label_Hardware_Exp_LEDs.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_LEDs.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_Exp_LEDs.Location = new System.Drawing.Point(554, 109);
            this.label_Hardware_Exp_LEDs.Name = "label_Hardware_Exp_LEDs";
            this.label_Hardware_Exp_LEDs.Size = new System.Drawing.Size(374, 72);
            this.label_Hardware_Exp_LEDs.TabIndex = 20;
            this.label_Hardware_Exp_LEDs.Text = "1: Red LED for Reader/Writer Mode\r\n2: Orange LED for Peer-to-Peer Mode\r\n3: Green " +
                "LED for Card Emulation Mode";
            // 
            // label_Hardware_Exp_LED3
            // 
            this.label_Hardware_Exp_LED3.AutoSize = true;
            this.label_Hardware_Exp_LED3.BackColor = System.Drawing.Color.White;
            this.label_Hardware_Exp_LED3.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_LED3.ForeColor = System.Drawing.Color.Red;
            this.label_Hardware_Exp_LED3.Location = new System.Drawing.Point(96, 184);
            this.label_Hardware_Exp_LED3.Name = "label_Hardware_Exp_LED3";
            this.label_Hardware_Exp_LED3.Size = new System.Drawing.Size(21, 22);
            this.label_Hardware_Exp_LED3.TabIndex = 19;
            this.label_Hardware_Exp_LED3.Text = "3";
            // 
            // label_Hardware_Exp_LED1
            // 
            this.label_Hardware_Exp_LED1.AutoSize = true;
            this.label_Hardware_Exp_LED1.BackColor = System.Drawing.Color.White;
            this.label_Hardware_Exp_LED1.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_LED1.ForeColor = System.Drawing.Color.Red;
            this.label_Hardware_Exp_LED1.Location = new System.Drawing.Point(96, 134);
            this.label_Hardware_Exp_LED1.Name = "label_Hardware_Exp_LED1";
            this.label_Hardware_Exp_LED1.Size = new System.Drawing.Size(21, 22);
            this.label_Hardware_Exp_LED1.TabIndex = 18;
            this.label_Hardware_Exp_LED1.Text = "1";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.BackColor = System.Drawing.Color.Transparent;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.ForeColor = System.Drawing.Color.Black;
            this.label19.Location = new System.Drawing.Point(387, 39);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(181, 25);
            this.label19.TabIndex = 17;
            this.label19.Text = "Hardware Guide";
            // 
            // label_Hardware_Exp_LED2
            // 
            this.label_Hardware_Exp_LED2.AutoSize = true;
            this.label_Hardware_Exp_LED2.BackColor = System.Drawing.Color.White;
            this.label_Hardware_Exp_LED2.Font = new System.Drawing.Font("Arial", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_LED2.ForeColor = System.Drawing.Color.Red;
            this.label_Hardware_Exp_LED2.Location = new System.Drawing.Point(96, 159);
            this.label_Hardware_Exp_LED2.Name = "label_Hardware_Exp_LED2";
            this.label_Hardware_Exp_LED2.Size = new System.Drawing.Size(21, 22);
            this.label_Hardware_Exp_LED2.TabIndex = 11;
            this.label_Hardware_Exp_LED2.Text = "2";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.BackColor = System.Drawing.Color.Transparent;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.ForeColor = System.Drawing.Color.Black;
            this.label8.Location = new System.Drawing.Point(336, 12);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(282, 25);
            this.label8.TabIndex = 8;
            this.label8.Text = "MSP430F5529 Exp Board";
            // 
            // label_Hardware_Exp_RF_Field
            // 
            this.label_Hardware_Exp_RF_Field.AutoSize = true;
            this.label_Hardware_Exp_RF_Field.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_RF_Field.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_Exp_RF_Field.Location = new System.Drawing.Point(115, 218);
            this.label_Hardware_Exp_RF_Field.Name = "label_Hardware_Exp_RF_Field";
            this.label_Hardware_Exp_RF_Field.Size = new System.Drawing.Size(90, 24);
            this.label_Hardware_Exp_RF_Field.TabIndex = 4;
            this.label_Hardware_Exp_RF_Field.Text = "RF Field";
            // 
            // label_Hardware_Exp_GUI
            // 
            this.label_Hardware_Exp_GUI.AutoSize = true;
            this.label_Hardware_Exp_GUI.BackColor = System.Drawing.Color.Transparent;
            this.label_Hardware_Exp_GUI.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_GUI.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_Exp_GUI.Location = new System.Drawing.Point(290, 218);
            this.label_Hardware_Exp_GUI.Name = "label_Hardware_Exp_GUI";
            this.label_Hardware_Exp_GUI.Size = new System.Drawing.Size(44, 24);
            this.label_Hardware_Exp_GUI.TabIndex = 3;
            this.label_Hardware_Exp_GUI.Text = "GUI";
            // 
            // label_Hardware_Exp_RX
            // 
            this.label_Hardware_Exp_RX.AutoSize = true;
            this.label_Hardware_Exp_RX.BackColor = System.Drawing.Color.Transparent;
            this.label_Hardware_Exp_RX.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_RX.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_Exp_RX.Location = new System.Drawing.Point(366, 218);
            this.label_Hardware_Exp_RX.Name = "label_Hardware_Exp_RX";
            this.label_Hardware_Exp_RX.Size = new System.Drawing.Size(39, 24);
            this.label_Hardware_Exp_RX.TabIndex = 2;
            this.label_Hardware_Exp_RX.Text = "RX";
            // 
            // label_Hardware_Exp_TX
            // 
            this.label_Hardware_Exp_TX.AutoSize = true;
            this.label_Hardware_Exp_TX.BackColor = System.Drawing.Color.Transparent;
            this.label_Hardware_Exp_TX.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_Hardware_Exp_TX.ForeColor = System.Drawing.Color.Firebrick;
            this.label_Hardware_Exp_TX.Location = new System.Drawing.Point(443, 218);
            this.label_Hardware_Exp_TX.Name = "label_Hardware_Exp_TX";
            this.label_Hardware_Exp_TX.Size = new System.Drawing.Size(38, 24);
            this.label_Hardware_Exp_TX.TabIndex = 1;
            this.label_Hardware_Exp_TX.Text = "TX";
            // 
            // pictureBox_Exp_Board
            // 
            this.pictureBox_Exp_Board.BackColor = System.Drawing.Color.Transparent;
            this.pictureBox_Exp_Board.Image = global::TI_NFC_Tool.Properties.Resources.MSP430F5529_PADs;
            this.pictureBox_Exp_Board.Location = new System.Drawing.Point(15, 74);
            this.pictureBox_Exp_Board.Name = "pictureBox_Exp_Board";
            this.pictureBox_Exp_Board.Size = new System.Drawing.Size(509, 142);
            this.pictureBox_Exp_Board.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox_Exp_Board.TabIndex = 0;
            this.pictureBox_Exp_Board.TabStop = false;
            // 
            // label_RW_Max_Size_Msg
            // 
            this.label_RW_Max_Size_Msg.AutoSize = true;
            this.label_RW_Max_Size_Msg.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_RW_Max_Size_Msg.ForeColor = System.Drawing.Color.DarkGreen;
            this.label_RW_Max_Size_Msg.Location = new System.Drawing.Point(130, 33);
            this.label_RW_Max_Size_Msg.Name = "label_RW_Max_Size_Msg";
            this.label_RW_Max_Size_Msg.Size = new System.Drawing.Size(656, 24);
            this.label_RW_Max_Size_Msg.TabIndex = 13;
            this.label_RW_Max_Size_Msg.Text = "  Tag writes will be in NDEF Format. Maximum write size is 246 bytes.";
            // 
            // baudRateBox
            // 
            this.baudRateBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.baudRateBox.FormattingEnabled = true;
            this.baudRateBox.Items.AddRange(new object[] {
            "38400",
            "56000",
            "57600",
            "115200",
            "230400",
            "460800",
            "921600"});
            this.baudRateBox.Location = new System.Drawing.Point(543, 37);
            this.baudRateBox.Name = "baudRateBox";
            this.baudRateBox.Size = new System.Drawing.Size(121, 21);
            this.baudRateBox.TabIndex = 6;
            // 
            // Label_BaudRate
            // 
            this.Label_BaudRate.AutoSize = true;
            this.Label_BaudRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_BaudRate.ForeColor = System.Drawing.Color.Black;
            this.Label_BaudRate.Location = new System.Drawing.Point(462, 40);
            this.Label_BaudRate.Name = "Label_BaudRate";
            this.Label_BaudRate.Size = new System.Drawing.Size(77, 16);
            this.Label_BaudRate.TabIndex = 5;
            this.Label_BaudRate.Text = "BaudRate";
            // 
            // Label_COM
            // 
            this.Label_COM.AutoSize = true;
            this.Label_COM.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_COM.ForeColor = System.Drawing.Color.Black;
            this.Label_COM.Location = new System.Drawing.Point(465, 13);
            this.Label_COM.Name = "Label_COM";
            this.Label_COM.Size = new System.Drawing.Size(73, 16);
            this.Label_COM.TabIndex = 4;
            this.Label_COM.Text = "COM Port";
            // 
            // comportBox
            // 
            this.comportBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comportBox.FormattingEnabled = true;
            this.comportBox.Location = new System.Drawing.Point(543, 10);
            this.comportBox.Name = "comportBox";
            this.comportBox.Size = new System.Drawing.Size(121, 21);
            this.comportBox.TabIndex = 3;
            this.comportBox.DropDown += new System.EventHandler(this.comboBox1_DropDown);
            // 
            // manualConnectButton
            // 
            this.manualConnectButton.BackColor = System.Drawing.Color.Transparent;
            this.manualConnectButton.FlatAppearance.BorderColor = System.Drawing.Color.White;
            this.manualConnectButton.FlatAppearance.BorderSize = 2;
            this.manualConnectButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.manualConnectButton.Location = new System.Drawing.Point(676, 9);
            this.manualConnectButton.Name = "manualConnectButton";
            this.manualConnectButton.Size = new System.Drawing.Size(111, 50);
            this.manualConnectButton.TabIndex = 2;
            this.manualConnectButton.Text = "Manual Connect";
            this.manualConnectButton.UseVisualStyleBackColor = false;
            this.manualConnectButton.Click += new System.EventHandler(this.manualConnectButton_Click);
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.PortName = "COM5";
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // saveFileDialogCOMLog
            // 
            this.saveFileDialogCOMLog.Filter = "Log Files|*.log|All Files|*.*";
            this.saveFileDialogCOMLog.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileDialogCOMLog_FileOk);
            // 
            // openFileDialogCOMLog
            // 
            this.openFileDialogCOMLog.Filter = "Log Files|*.log";
            this.openFileDialogCOMLog.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialogCOMLog_FileOk);
            // 
            // timerBackground
            // 
            this.timerBackground.Enabled = true;
            this.timerBackground.Interval = 1;
            this.timerBackground.Tick += new System.EventHandler(this.timerBackground_Tick);
            // 
            // statusStripConnectionStatus
            // 
            this.statusStripConnectionStatus.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusConnectionLabel,
            this.toolStripStatusLabelFWVer});
            this.statusStripConnectionStatus.Location = new System.Drawing.Point(3, 660);
            this.statusStripConnectionStatus.Name = "statusStripConnectionStatus";
            this.statusStripConnectionStatus.Size = new System.Drawing.Size(981, 22);
            this.statusStripConnectionStatus.TabIndex = 16;
            this.statusStripConnectionStatus.Text = "statusStrip1";
            // 
            // toolStripStatusConnectionLabel
            // 
            this.toolStripStatusConnectionLabel.BackColor = System.Drawing.Color.Red;
            this.toolStripStatusConnectionLabel.Name = "toolStripStatusConnectionLabel";
            this.toolStripStatusConnectionLabel.Size = new System.Drawing.Size(79, 17);
            this.toolStripStatusConnectionLabel.Text = "Disconnected";
            // 
            // toolStripStatusLabelFWVer
            // 
            this.toolStripStatusLabelFWVer.Name = "toolStripStatusLabelFWVer";
            this.toolStripStatusLabelFWVer.Size = new System.Drawing.Size(66, 17);
            this.toolStripStatusLabelFWVer.Text = "FW Version";
            // 
            // hardwareSelectBox
            // 
            this.hardwareSelectBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.hardwareSelectBox.FormattingEnabled = true;
            this.hardwareSelectBox.Items.AddRange(new object[] {
            "MSP-EXP430F5529LP",
            "MSP-EXP430F5529",
            "MSP-EXP432P401R"});
            this.hardwareSelectBox.Location = new System.Drawing.Point(23, 37);
            this.hardwareSelectBox.Name = "hardwareSelectBox";
            this.hardwareSelectBox.Size = new System.Drawing.Size(149, 21);
            this.hardwareSelectBox.TabIndex = 17;
            // 
            // Label_Hardware
            // 
            this.Label_Hardware.AutoSize = true;
            this.Label_Hardware.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label_Hardware.ForeColor = System.Drawing.Color.Black;
            this.Label_Hardware.Location = new System.Drawing.Point(26, 13);
            this.Label_Hardware.Name = "Label_Hardware";
            this.Label_Hardware.Size = new System.Drawing.Size(144, 16);
            this.Label_Hardware.TabIndex = 18;
            this.Label_Hardware.Text = "Hardware Selection";
            // 
            // autoConnectButton
            // 
            this.autoConnectButton.BackColor = System.Drawing.Color.Transparent;
            this.autoConnectButton.FlatAppearance.BorderColor = System.Drawing.Color.White;
            this.autoConnectButton.FlatAppearance.BorderSize = 2;
            this.autoConnectButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.autoConnectButton.Location = new System.Drawing.Point(179, 9);
            this.autoConnectButton.Name = "autoConnectButton";
            this.autoConnectButton.Size = new System.Drawing.Size(111, 50);
            this.autoConnectButton.TabIndex = 19;
            this.autoConnectButton.Text = "Auto Connect";
            this.autoConnectButton.UseVisualStyleBackColor = false;
            this.autoConnectButton.Click += new System.EventHandler(this.autoConnectButton_Click);
            // 
            // pictureBox_TI_Logo
            // 
            this.pictureBox_TI_Logo.BackColor = System.Drawing.Color.Transparent;
            this.pictureBox_TI_Logo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox_TI_Logo.Image = global::TI_NFC_Tool.Properties.Resources.TI_Logo;
            this.pictureBox_TI_Logo.Location = new System.Drawing.Point(822, 1);
            this.pictureBox_TI_Logo.Name = "pictureBox_TI_Logo";
            this.pictureBox_TI_Logo.Size = new System.Drawing.Size(171, 96);
            this.pictureBox_TI_Logo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox_TI_Logo.TabIndex = 15;
            this.pictureBox_TI_Logo.TabStop = false;
            // 
            // openNdefButton
            // 
            this.openNdefButton.BackColor = System.Drawing.Color.Transparent;
            this.openNdefButton.FlatAppearance.BorderColor = System.Drawing.Color.White;
            this.openNdefButton.FlatAppearance.BorderSize = 2;
            this.openNdefButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Bold);
            this.openNdefButton.Location = new System.Drawing.Point(320, 9);
            this.openNdefButton.Name = "openNdefButton";
            this.openNdefButton.Size = new System.Drawing.Size(114, 50);
            this.openNdefButton.TabIndex = 20;
            this.openNdefButton.Text = "NDEF.hex File Location";
            this.openNdefButton.UseVisualStyleBackColor = false;
            this.openNdefButton.Click += new System.EventHandler(this.openNdefButton_Click);
            this.openNdefButton.MouseHover += new System.EventHandler(this.openNdefButton_MouseHover);
            // 
            // TI_NFC_Tool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightCyan;
            this.ClientSize = new System.Drawing.Size(984, 682);
            this.Controls.Add(this.openNdefButton);
            this.Controls.Add(this.autoConnectButton);
            this.Controls.Add(this.Label_Hardware);
            this.Controls.Add(this.hardwareSelectBox);
            this.Controls.Add(this.statusStripConnectionStatus);
            this.Controls.Add(this.MainTabController);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.manualConnectButton);
            this.Controls.Add(this.Label_COM);
            this.Controls.Add(this.baudRateBox);
            this.Controls.Add(this.comportBox);
            this.Controls.Add(this.Label_BaudRate);
            this.Controls.Add(this.pictureBox_TI_Logo);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(1000, 750);
            this.MinimumSize = new System.Drawing.Size(620, 550);
            this.Name = "TI_NFC_Tool";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "TI NFC Tool Ver 1.8";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TI_NFC_Tool_FormClosing);
            this.MainTabController.ResumeLayout(false);
            this.tabNfcConfiguration.ResumeLayout(false);
            this.groupBox_TRF79xx_Settings.ResumeLayout(false);
            this.groupBox_TRF79xx_Settings.PerformLayout();
            this.groupBox_RW_Config.ResumeLayout(false);
            this.panel_RW_Mode_Support.ResumeLayout(false);
            this.panel_RW_Mode_Support.PerformLayout();
            this.panel_RW_NFC_V.ResumeLayout(false);
            this.panel_RW_NFC_V.PerformLayout();
            this.panel_RW_NFC_B.ResumeLayout(false);
            this.panel_RW_NFC_B.PerformLayout();
            this.panel_RW_NFC_A.ResumeLayout(false);
            this.panel_RW_NFC_A.PerformLayout();
            this.groupBox_CE_Config.ResumeLayout(false);
            this.panel_CE_Mode_Support.ResumeLayout(false);
            this.panel_CE_Mode_Support.PerformLayout();
            this.groupBox_NFC_Compliance_Config.ResumeLayout(false);
            this.panel_RW_adv_config.ResumeLayout(false);
            this.panel_RW_adv_config.PerformLayout();
            this.panel_P2P_adv_config.ResumeLayout(false);
            this.panel_P2P_adv_config.PerformLayout();
            this.groupBox_P2P_Config.ResumeLayout(false);
            this.panel_P2P_Target_Support.ResumeLayout(false);
            this.panel_P2P_Target_Support.PerformLayout();
            this.panel_P2P_Initiator_Support.ResumeLayout(false);
            this.panel_P2P_Initiator_Support.PerformLayout();
            this.tabPeerToPeer.ResumeLayout(false);
            this.groupBox_P2P_Status.ResumeLayout(false);
            this.groupBox_P2P_Status.PerformLayout();
            this.groupBox_P2P_Rx.ResumeLayout(false);
            this.groupBox_P2P_Tx.ResumeLayout(false);
            this.groupBox_P2P_Tx.PerformLayout();
            this.tabControl_P2P_Transmit.ResumeLayout(false);
            this.tabP2PRTDText.ResumeLayout(false);
            this.tabP2PRTDText.PerformLayout();
            this.tabP2PRTDURI.ResumeLayout(false);
            this.tabP2PRTDURI.PerformLayout();
            this.tabCardEmulation.ResumeLayout(false);
            this.groupBox_CE_Emulated_Content.ResumeLayout(false);
            this.groupBox_CE_Emulated_Content.PerformLayout();
            this.tabControl_CE_Emulated_Content.ResumeLayout(false);
            this.tabCERTDText.ResumeLayout(false);
            this.tabCERTDText.PerformLayout();
            this.tabCERTDURI.ResumeLayout(false);
            this.tabCERTDURI.PerformLayout();
            this.groupBox_CE_Read_Write_Status.ResumeLayout(false);
            this.groupBox_CE_Status.ResumeLayout(false);
            this.groupBox_CE_Status.PerformLayout();
            this.tabReaderWriter.ResumeLayout(false);
            this.groupBox_RW_Tag_Write.ResumeLayout(false);
            this.tabControl_RW_Tag_Write.ResumeLayout(false);
            this.tabRWRTDText.ResumeLayout(false);
            this.tabRWRTDText.PerformLayout();
            this.tabRWRTDURI.ResumeLayout(false);
            this.tabRWRTDURI.PerformLayout();
            this.groupBox_RW_Tag_Content.ResumeLayout(false);
            this.groupBox_RW_Status.ResumeLayout(false);
            this.groupBox_RW_Status.PerformLayout();
            this.tabCOMLog.ResumeLayout(false);
            this.tabAdvancedCOM.ResumeLayout(false);
            this.tabHWGuide.ResumeLayout(false);
            this.tabHWGuide.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_BP_LP_Board)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Exp_Board)).EndInit();
            this.statusStripConnectionStatus.ResumeLayout(false);
            this.statusStripConnectionStatus.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_TI_Logo)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_Refresh_COM;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.TabPage tabAdvancedCOM;
        private System.Windows.Forms.TabPage tabCOMLog;
        private System.Windows.Forms.Label Label_COM;
        private System.Windows.Forms.ComboBox comportBox;
        private System.Windows.Forms.Button manualConnectButton;
        private System.Windows.Forms.Button button_Clear_Log;
        private System.Windows.Forms.RichTextBox serialPortDataTextBox;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.ComboBox baudRateBox;
        private System.Windows.Forms.Label Label_BaudRate;
        private System.Windows.Forms.Button button_Save_Log;
        private System.Windows.Forms.TabPage tabNfcConfiguration;
        private System.Windows.Forms.Label Label_Initiator;
        private System.Windows.Forms.Label Label_Target;
        private System.Windows.Forms.Panel panel_P2P_Target_Support;
        private System.Windows.Forms.CheckBox checkBox_Act_Target_424;
        private System.Windows.Forms.CheckBox checkBox_Act_Target_212;
        private System.Windows.Forms.CheckBox checkBox_Pas_Target_106;
        private System.Windows.Forms.CheckBox checkBox_Act_Target_106;
        private System.Windows.Forms.CheckBox checkBox_Pas_Target_212;
        private System.Windows.Forms.CheckBox checkBox_Pas_Target_424;
        private System.Windows.Forms.CheckBox checkBox_Act_Initiator_424;
        private System.Windows.Forms.CheckBox checkBox_Act_Initiator_212;
        private System.Windows.Forms.CheckBox checkBox_Act_Initiator_106;
        private System.Windows.Forms.CheckBox checkBox_Pas_Initiator_424;
        private System.Windows.Forms.CheckBox checkBox_Pas_Initiator_212;
        private System.Windows.Forms.CheckBox checkBox_Pas_Initiator_106;
        private System.Windows.Forms.Panel panel_P2P_Initiator_Support;
        private System.Windows.Forms.GroupBox groupBox_P2P_Config;
        private System.Windows.Forms.Button button_P2P_toggle;
        private System.Windows.Forms.TabPage tabPeerToPeer;
        private System.Windows.Forms.TextBox textBox_P2P_Technology;
        private System.Windows.Forms.Label label_P2P_Bitrate;
        private System.Windows.Forms.Label label_P2P_Mode;
        private System.Windows.Forms.Label label_P2P_Technology;
        private System.Windows.Forms.TextBox textBox_P2P_Bitrate;
        private System.Windows.Forms.TextBox textBox_P2P_Mode;
        private System.Windows.Forms.SaveFileDialog saveFileDialogCOMLog;
        private System.Windows.Forms.Button button_Open_Log;
        private System.Windows.Forms.OpenFileDialog openFileDialogCOMLog;
        private System.Windows.Forms.Timer timerBackground;
        private System.Windows.Forms.TextBox textBox_P2P_Received;
        private System.Windows.Forms.Label label_P2P_Receive;
        private System.Windows.Forms.TextBox textBox_P2P_Rssi;
        private System.Windows.Forms.Label label_P2P_Rssi;
        private System.Windows.Forms.GroupBox groupBox_P2P_Status;
        private System.Windows.Forms.GroupBox groupBox_P2P_Rx;
        private System.Windows.Forms.RichTextBox richTextBox_P2P_Receive;
        private System.Windows.Forms.GroupBox groupBox_P2P_Tx;
        private System.Windows.Forms.Button button_P2P_Send;
        private System.Windows.Forms.TabControl tabControl_P2P_Transmit;
        private System.Windows.Forms.TabPage tabP2PRTDText;
        private System.Windows.Forms.Label label_P2P_RTD_Text;
        private System.Windows.Forms.RichTextBox richTextBox_P2P_RTD_Text;
        private System.Windows.Forms.TabPage tabP2PRTDURI;
        private System.Windows.Forms.RichTextBox richTextBox_P2P_RTD_URI;
        private System.Windows.Forms.Label label_P2P_RTD_URI;
        private System.Windows.Forms.PictureBox pictureBox_TI_Logo;
        private System.Windows.Forms.StatusStrip statusStripConnectionStatus;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabelFWVer;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusConnectionLabel;
        private System.Windows.Forms.TabPage tabHWGuide;
        private System.Windows.Forms.Label label_Hardware_Exp_RF_Field;
        private System.Windows.Forms.Label label_Hardware_Exp_GUI;
        private System.Windows.Forms.Label label_Hardware_Exp_RX;
        private System.Windows.Forms.Label label_Hardware_Exp_TX;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.PictureBox pictureBox_Exp_Board;
        private System.Windows.Forms.Label label_Hardware_Exp_LED2;
        private System.Windows.Forms.GroupBox groupBox_RW_Config;
        private System.Windows.Forms.Button button_RW_toggle;
        private System.Windows.Forms.Panel panel_RW_Mode_Support;
        private System.Windows.Forms.Panel panel_RW_NFC_V;
        private System.Windows.Forms.RadioButton radioButton_ISO15693_26_48;
        private System.Windows.Forms.Panel panel_RW_NFC_B;
        private System.Windows.Forms.RadioButton radioButton_NFC_B_106;
        private System.Windows.Forms.RadioButton radioButton_NFC_B_212;
        private System.Windows.Forms.RadioButton radioButton_NFC_B_424;
        private System.Windows.Forms.RadioButton radioButton_NFC_B_848;
        private System.Windows.Forms.Panel panel_RW_NFC_A;
        private System.Windows.Forms.RadioButton radioButton_NFC_A_106;
        private System.Windows.Forms.RadioButton radioButton_NFC_A_212;
        private System.Windows.Forms.RadioButton radioButton_NFC_A_424;
        private System.Windows.Forms.RadioButton radioButton_NFC_A_848;
        private System.Windows.Forms.CheckBox checkBox_RW_ISO15693;
        private System.Windows.Forms.CheckBox checkBox_RW_NFC_F;
        private System.Windows.Forms.Label Label_Reader_Writer_Modes;
        private System.Windows.Forms.CheckBox checkBox_RW_NFC_B;
        private System.Windows.Forms.CheckBox checkBox_RW_NFC_A;
        private System.Windows.Forms.GroupBox groupBox_CE_Config;
        private System.Windows.Forms.Button button_CE_toggle;
        private System.Windows.Forms.Panel panel_CE_Mode_Support;
        private System.Windows.Forms.Label Label_Card_Emulation_Modes;
        private System.Windows.Forms.CheckBox checkBox_CE_T4BT;
        private System.Windows.Forms.CheckBox checkBox_CE_T4AT;
        private System.Windows.Forms.GroupBox groupBox_NFC_Compliance_Config;
        private System.Windows.Forms.Panel panel_RW_adv_config;
        private System.Windows.Forms.Panel panel_P2P_adv_config;
        private System.Windows.Forms.Label Label_P2P_Adv_Config;
        private System.Windows.Forms.CheckBox checkBox_LLCP_Enable;
        private System.Windows.Forms.ComboBox comboBox_P2P_NACK_Retry;
        private System.Windows.Forms.ComboBox comboBox_P2P_ATN_Retry;
        private System.Windows.Forms.Label label_P2P_NACK_Retry;
        private System.Windows.Forms.Label label_P2P_ATN_Retry;
        private System.Windows.Forms.CheckBox checkBox_NFC_DEP_Loopback;
        private System.Windows.Forms.Button buttonNFC_Compliance_toggle;
        private System.Windows.Forms.ComboBox comboBox_P2P_DID;
        private System.Windows.Forms.Label label_P2P_DID;
        private System.Windows.Forms.GroupBox groupBox_TRF79xx_Settings;
        private System.Windows.Forms.ComboBox comboBox_TRF_Version;
        private System.Windows.Forms.CheckBox checkBox_TRF_Ext_Amp;
        private System.Windows.Forms.Label Label_Trf_Vcc_Setting;
        private System.Windows.Forms.Button button_TRF_Update;
        private System.Windows.Forms.RadioButton radioButton_TRF_5V_supply;
        private System.Windows.Forms.RadioButton radioButton_TRF_3V_supply;
        private System.Windows.Forms.CheckBox checkBox_TRF_AutoSDD;
        private System.Windows.Forms.Label label_Trf_Version_No;
        private System.Windows.Forms.ComboBox comboBox_RW_ACK_Retry;
        private System.Windows.Forms.Label label_RW_ACK_Retry;
        private System.Windows.Forms.ComboBox comboBox_RW_DID;
        private System.Windows.Forms.Label label_RW_DID;
        private System.Windows.Forms.ComboBox comboBox_RW_WTX_Retry;
        private System.Windows.Forms.ComboBox comboBox_RW_NACK_Retry;
        private System.Windows.Forms.Label label_RW_WTX_Retry;
        private System.Windows.Forms.Label label_RW_NACK_Retry;
        private System.Windows.Forms.ComboBox comboBox_Loop_Delay;
        private System.Windows.Forms.Label label_Trf_Listen_Time;
        private System.Windows.Forms.TabPage tabReaderWriter;
        private System.Windows.Forms.TabPage tabCardEmulation;
        private System.Windows.Forms.GroupBox groupBox_CE_Read_Write_Status;
        private System.Windows.Forms.RichTextBox richTextBox_CE_Status;
        private System.Windows.Forms.GroupBox groupBox_CE_Status;
        private System.Windows.Forms.TextBox textBox_CE_Tag_Type;
        private System.Windows.Forms.TextBox textBox_CE_Technology;
        private System.Windows.Forms.Label label_CE_Rssi;
        private System.Windows.Forms.Label label_CE_Tag_Type;
        private System.Windows.Forms.Label label_CE_Technology;
        private System.Windows.Forms.TextBox textBox_CE_UID;
        private System.Windows.Forms.TextBox textBox_CE_Rssi;
        private System.Windows.Forms.Label label_CE_Bitrate;
        private System.Windows.Forms.TextBox textBox_CE_Bitrate;
        private System.Windows.Forms.Label label_CE_UID;
        private System.Windows.Forms.GroupBox groupBox_CE_Emulated_Content;
        private System.Windows.Forms.Button button_CE_Emulate;
        private System.Windows.Forms.TabControl tabControl_CE_Emulated_Content;
        private System.Windows.Forms.TabPage tabCERTDText;
        private System.Windows.Forms.Label label_CE_RTD_Text;
        private System.Windows.Forms.RichTextBox richTextBox_CE_RTD_Text;
        private System.Windows.Forms.TabPage tabCERTDURI;
        private System.Windows.Forms.RichTextBox richTextBox_CE_RTD_URI;
        private System.Windows.Forms.Label label_CE_RTD_URI;
        private System.Windows.Forms.GroupBox groupBox_RW_Tag_Content;
        private System.Windows.Forms.RichTextBox richTextBox_RW_Tag_Content;
        private System.Windows.Forms.GroupBox groupBox_RW_Status;
        private System.Windows.Forms.TextBox textBox_RW_Tag_Type;
        private System.Windows.Forms.TextBox textBox_RW_Technology;
        private System.Windows.Forms.Label label_RW_Rssi;
        private System.Windows.Forms.Label label_RW_Tag_Type;
        private System.Windows.Forms.Label label_RW_Technology;
        private System.Windows.Forms.TextBox textBox_RW_UID;
        private System.Windows.Forms.TextBox textBox_RW_Rssi;
        private System.Windows.Forms.Label label_RW_Bitrate;
        private System.Windows.Forms.TextBox textBox_RW_Bitrate;
        private System.Windows.Forms.Label label_RW_UID;
        private System.Windows.Forms.GroupBox groupBox_RW_Tag_Write;
        private System.Windows.Forms.Button button_RW_Write;
        private System.Windows.Forms.TabControl tabControl_RW_Tag_Write;
        private System.Windows.Forms.TabPage tabRWRTDText;
        private System.Windows.Forms.Label label_RW_RTD_Text;
        private System.Windows.Forms.RichTextBox richTextBox_RW_RTD_Text;
        private System.Windows.Forms.TabPage tabRWRTDURI;
        private System.Windows.Forms.RichTextBox richTextBox_RW_RTD_URI;
        private System.Windows.Forms.Label label_RW_RTD_URI;
        private System.Windows.Forms.CheckBox checkBox_NFC_F_424;
        private System.Windows.Forms.CheckBox checkBox_NFC_F_212;
        private System.Windows.Forms.Label Label_RW_Adv_Config;
        private System.Windows.Forms.ComboBox comboBox_RW_DSL_Retry;
        private System.Windows.Forms.Label label_RW_DSL_Retry;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.ComboBox hardwareSelectBox;
        private System.Windows.Forms.Label Label_Hardware;
        private System.Windows.Forms.Button autoConnectButton;
        private System.Windows.Forms.Label label_RW_Config_Help_Msg;
        private System.Windows.Forms.TabControl MainTabController;
        private System.Windows.Forms.Label label_RW_Max_Size_Msg;
        private System.Windows.Forms.TextBox textBox_RW_RTD_Text_Counter;
        private System.Windows.Forms.TextBox textBox_P2P_RTD_Text_Counter;
        private System.Windows.Forms.TextBox textBox_CE_RTD_Text_Counter;
        private System.Windows.Forms.Label label_Hardware_Exp_LEDs;
        private System.Windows.Forms.Label label_Hardware_Exp_LED3;
        private System.Windows.Forms.Label label_Hardware_Exp_LED1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox_BP_LP_Board;
        private System.Windows.Forms.Label label_Hardware_BP_LEDs;
        private System.Windows.Forms.Label label_P2P_Max_Size_Msg;
        private System.Windows.Forms.Label label_CE_Max_Size_Msg;
        private System.Windows.Forms.Button openNdefButton;
        private System.Windows.Forms.Button button_RW_TagFormat;
    }
}

