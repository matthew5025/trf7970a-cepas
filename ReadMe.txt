---------------------NFCLink Stand Alone User Experience-----------------------
Ver.: 1.03.00
November 2016
Texas Instruments, Inc.
--------------------------------------------------------------------------------

CONTENTS

I.   DESCRIPTION
II.  PROJECT & FILE CONFIGURATION
III. PROJECT IMPORT IN CCS
IV.  HISTORY

I. DESCRIPTION
-----------------------------------

The Near Field Communication (NFC) market is emerging into multiple fields including
Medical, Consumer, Retail, Industrial, Automotive and Smart Grid. Card Emulation is
one of the three operational modes supported by the TRF7970A. When using Card
Emulation, the user can configure the TRF7970A to emulate a Type 4A or Type 4B Tag
Platform. When emulating either of the Tag Platforms, Record Type Definitions (RTD)
can be used such as Text, URI, Smart Poster, or V-Card. It should also be noted here
that Card Emulation on the TRF7970A is not limited to NFC applications. Other
applications could include payment systems, access control, proprietary customer
systems, and other ISO 14443-4 applications.

The hardware required to validate this reference design must be one of the 
following evaluation platforms:
1. MSP430F5529 LaunchPad + DLP-7970ABP
2. MSP430F5529 Experimenter Board + TRF7970ATB
3. MSP432P401R LaunchPad + DLP-7970ABP
There is a CCS project for each of these platforms supporting all three NFC 
modes: Reader/Writer, Peer to Peer, and Card Emulation.

For the Card Emulation mode the TRF7970A can emulate a ISO14443 Type A / B Tag.
There are multiple configurations supported natively for the NDEF:
1. Text (TI NFC Tool GUI supports short record) 
2. URI (TI NFC Tool GUI supports short record)
3. Smart Poster
4. V-Card
5. Bluetooth Handover
6. MIME image

For the Reader/Writer mode four different tag types can be selected to be read
in a switching or individually format: 
1. Type 2 (ISO14443A)
2. Type 3 (FeliCa/ISO18092)
3. Type 4A (ISO14443A)
4. Type 4B (ISO14443B, including RF430CL33xH Transponders)
5. Type V (ISO15693, including RF430FRL15xH Transponders)

For the Peer-to-Peer mode the TRF7970A can be an initiator or a target
supporting the following over-the-air data rates:
1. 106kbps
2. 212kbps
3. 424kbps
Peer-to-Peer mode also supports the following protocol(s) to exchange
NFC Data Exchange Format (NDEF):
1. Simple NDEF Exchange Protocol (SNEP)

II. PROJECT & FILE CONFIGURATION
-----------------------------------

PROJECT ROOT  - TRF7970A_RW_1.03.00
|   README                  <This file>
+---[doc]
|	|---APIGuide_ver_01_01.pdf
+---[driverlib430]			<MSP430 driverlib>
+---[driverlib432]			<MSP432 driverlib>
+---[examples]
|   \---[boards]
|       \---[MSP-EXP430F5529]		<MSP430F5529 Experimenters Board>
|            \---[F5529EXP_TRF7970A_ALL_NFC_MODES]		<RW, P2P, CE>
|       \---[MSP-EXP430F5529LP]		<MSP430F5529 LaunchPad Board>		
|            \---[F5529LP_TRF7970A_ALL_NFC_MODES]		<RW, P2P, CE>
|       \---[MSP-MSP-EXP432P401R]	<MSP432P401R LaunchPad Board>
|            \---[P401R_TRF7970A_ALL_NFC_MODES]			<RW, P2P, CE>
+---[nfclink]		<NFC source + .lib files used on the CCS projects>
|   \---[Source]		<TRF7970A drivers, and NFC stack Files>
+---[tools] 			<TI NFC Tool V1.8>  
\---[usblib430]			<MSP430 USB drivers>  
            
Legend: [Directory]     
           |---filename 

           
III. PROJECT IMPORT IN CCS 
---------------------------
  1. Project Import in CCS  
       a. Open CCS.
       b. Select a new project workspace outside of the project folder*           
       c. Select Project-->Import Existing Project 
       d. Browse to the installation file\examples\boards\
	   e. Select correct HW platform
       f. Select Finish 
       
       *Ideally, workspace should be in completely independent folder, not
       containing or contained by the project/package folder.
       
       !Note: For CCS, while project root is in the outer directory, the CCS 
       project files are located inside CCS. To enable the portability of
       the project, the file macros.ini is created to define the root. 
       Additionally, all project code files (*.c, *.h) are added as linked 
       resources with their relative path to the project root.

IV. HISTORY 
-----------------------------------
Ver. 1.00.14
- Initial firmware release for Card Emulation, including updated support for
Peer-to-Peer mode.
Ver. 1.03.00
-Updated release with NFC Stack seperation implemented to parse out
functionality for Reader/Writer, Peer-to-Peer, and Card Emulation as needed.