using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Management;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using System.Threading;

namespace TI_NFC_Tool
{
    public partial class TI_NFC_Tool
    {
        //
        // Emulate CE Button Click
        //
        private void button_CE_Configure_Click(object sender, EventArgs e)
        {
            Byte[] ui8TxPacket;
            eRTDTypes eRTD;

            if (tabControl_CE_Emulated_Content.SelectedTab == tabControl_CE_Emulated_Content.TabPages["tabCERTDText"])
            {
                ui8TxPacket = Encoding.ASCII.GetBytes(richTextBox_CE_RTD_Text.Text);
                if (ui8TxPacket.Length > 249)
                {
                    eRTD = eRTDTypes.RTD_LONG_TEXT;
                }
                else
                {
                    eRTD = eRTDTypes.RTD_TEXT;
                }

            }
            else if (tabControl_CE_Emulated_Content.SelectedTab == tabControl_CE_Emulated_Content.TabPages["tabCERTDURI"])
            {
                ui8TxPacket = Encoding.ASCII.GetBytes(richTextBox_CE_RTD_URI.Text);

                eRTD = eRTDTypes.RTD_URI;
            }
            else
            {
                return;
            }

            emulateCENDEF(ui8TxPacket, eRTD);
        }

        //
        // Sends NDEF message based on which RTD Tab is open.
        //
        private void emulateCENDEF(byte[] ui8Payload, eRTDTypes eRTDType)
        {
            Byte[] ui8TxPacket;

            switch (eRTDType)
            {
                case eRTDTypes.RTD_TEXT:
                    ui8TxPacket = new Byte[ui8Payload.Length + 7];


                    // MB : Message Begin (1 bit)
                    // ME : Message End (1 bit)
                    // CF : Chunk Flag (1 bit)
                    // SR : Short Record (1 bit)
                    // IL : ID Length (1 bit)
                    // TNF : Type Name Format (3 bits)
                    //     :  Empty                 -- 0x00
                    //     :  NFC Forum well-known  -- 0x01 (default)
                    //     :  Media-Type            -- 0x02
                    //     :  Absolute-URI          -- 0x03
                    //     :  NFC Forum external    -- 0x04
                    //     :  Unknown               -- 0x05
                    //     :  Unchanged             -- 0x06
                    //     :  Reserved              -- 0x07

                    // ________________________________________________________
                    // |___7___|____6___|___5____|____4___|___3____|____2:0___|
                    // |MB = 1 | ME = 1 | CF = 0 | SR = 1 | IL = 0 | TNF = 001|
                    ui8TxPacket[0] = 0xD1;
                    // Type Length 
                    ui8TxPacket[1] = 0x01;
                    // Payload Length - Payload Length + 3 bytes for language
                    ui8TxPacket[2] = (byte)(ui8Payload.Length + 3);
                    // NDEF Record Type
                    // Sp ---- Smart Poster
                    // T ----- Text (default)
                    // U ----- URI
                    // Gc ---- Generic Control
                    // Hr ---- Handover Request
                    // Hs ---- Handover Select
                    // Hc ---- Handover Carrier
                    // Sig --- Signature
                    ui8TxPacket[3] = (byte)'T';

                    // Payload 
                    // Language Length 
                    ui8TxPacket[4] = 0x02;
                    ui8TxPacket[5] = (byte)'e';
                    ui8TxPacket[6] = (byte)'n';
                    Array.Copy(ui8Payload, 0, ui8TxPacket, 7, ui8Payload.Length);

                    SendCommand(ui8TxPacket, eNFCHostCommands.CE_NDEF_CONFIG, ui8TxPacket.Length);

                    break;
                case eRTDTypes.RTD_LONG_TEXT:
                    break;
                case eRTDTypes.RTD_URI:
                    ui8TxPacket = new Byte[ui8Payload.Length + 5];

                    // MB : Message Begin (1 bit)
                    // ME : Message End (1 bit)
                    // CF : Chunk Flag (1 bit)
                    // SR : Short Record (1 bit)
                    // IL : ID Length (1 bit)
                    // TNF : Type Name Format (3 bits)
                    //     :  Empty                 -- 0x00
                    //     :  NFC Forum well-known  -- 0x01 (default)
                    //     :  Media-Type            -- 0x02
                    //     :  Absolute-URI          -- 0x03
                    //     :  NFC Forum external    -- 0x04
                    //     :  Unknown               -- 0x05
                    //     :  Unchanged             -- 0x06
                    //     :  Reserved              -- 0x07

                    // |MB = 1 | ME = 1 | CF = 0 | SR = 1 | IL = 0 | TNF = 001|
                    ui8TxPacket[0] = 0xD1;
                    // Type Length 
                    ui8TxPacket[1] = 0x01;
                    // Payload Length - Payload Length + 1 byte for URL code
                    ui8TxPacket[2] = (byte)(ui8Payload.Length + 1);
                    // NDEF Record Type
                    // Sp ---- Smart Poster
                    // T ----- Text
                    // U ----- URI (default)
                    // Gc ---- Generic Control
                    // Hr ---- Handover Request
                    // Hs ---- Handover Select
                    // Hc ---- Handover Carrier
                    // Sig --- Signature
                    ui8TxPacket[3] = (byte)'U';

                    // Payload 
                    // URL Code 
                    ui8TxPacket[4] = (byte)eNDEF_URIRecord_IDCode.UNABRIDGE;

                    Array.Copy(ui8Payload, 0, ui8TxPacket, 5, ui8Payload.Length);

                    SendCommand(ui8TxPacket, eNFCHostCommands.CE_NDEF_CONFIG, ui8TxPacket.Length);

                    break;
                case eRTDTypes.RTD_SMARTPOSTER:
                    break;
                case eRTDTypes.RTD_MIME:
                    break;
            }

        }

        private void richTextBox_CE_Status_TextChanged(object sender, EventArgs e)
        {
            richTextBox_CE_Status.ScrollToCaret();
        }

        private void richTextBox_CE_RTD_Text_TextChanged(object sender, EventArgs e)
        {
            textBox_CE_RTD_Text_Counter.Text = "Message Length: " + richTextBox_CE_RTD_Text.TextLength + "/246";
        }

    }
}
