// =====================================================================================
// Copyright (c) 2021 Dave Bernazzani (wavemotion-dave)
//
// Copying and distribution of this emulator, it's source code and associated 
// readme files, with or without modification, are permitted in any medium without 
// royalty provided this copyright notice is used and wavemotion-dave (Phoenix-Edition),
// Alekmaul (original port) and Marat Fayzullin (ColEM core) are thanked profusely.
//
// The ColecoDS emulator is offered as-is, without any warranty.
// =====================================================================================
#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fat.h>

#include "colecoDS.h"
#include "AdamNet.h"
#include "FDIDisk.h"
#include "CRC32.h"
#include "cpu/z80/Z80_interface.h"
#include "colecomngt.h"
#include "colecogeneric.h"
#include "MTX_BIOS.h"
#define NORAM 0xFF


// ---------------------------------------------------------------------
// Memotech MTX IO Port Read - just VDP, Joystick/Keyboard and Z80-CTC
// ---------------------------------------------------------------------
unsigned char cpu_readport_memotech(register unsigned short Port) 
{
  // MTX ports are 8-bit
  Port &= 0x00FF; 

  if (Port == 0x00) return IOBYTE;
  else if (Port >= 0x08 && Port <= 0x0B)      // Z80-CTC Area
  {
      return ctc_timer[Port-0x08];
  }
  else if ((Port == 0x01))  // VDP Area
  {
      return(RdData9918());
  }
  else if ((Port == 0x02))  // VDP Area
  {
      return(RdCtrl9918());
  }
  else if ((Port == 0x03))  // Per MEMO - drive 0x03
  {
      return(0x03);
  }
  else if ((Port == 0x05))
  {
      u8 joy1 = 0x00;
      if (myConfig.dpad == DPAD_DIAGONALS)
      {
          if (JoyState & JST_UP)    joy1 |= (0x01 | 0x08);
          if (JoyState & JST_DOWN)  joy1 |= (0x02 | 0x04);
          if (JoyState & JST_LEFT)  joy1 |= (0x04 | 0x01);
          if (JoyState & JST_RIGHT) joy1 |= (0x08 | 0x02);
      }
      else
      {
          if (JoyState & JST_UP)    joy1 |= 0x01;
          if (JoyState & JST_DOWN)  joy1 |= 0x02;
          if (JoyState & JST_LEFT)  joy1 |= 0x04;
          if (JoyState & JST_RIGHT) joy1 |= 0x08;
      }          

      // -----------------------------------------------------------
      // We are at the top of the scan loop... if we have buffered 
      // keys, we insert them into the stream now...
      // -----------------------------------------------------------
      if (MTX_KBD_DRIVE == 0xFD)
      {
          if (key_shift_hold > 0) {key_shift = 1; key_shift_hold--;}
          if (BufferedKeysReadIdx != BufferedKeysWriteIdx)
          {
              msx_key = BufferedKeys[BufferedKeysReadIdx];
              BufferedKeysReadIdx = (BufferedKeysReadIdx+1) % 32;
              if (msx_key == KBD_KEY_SHIFT) key_shift_hold = 1;
          }
      }
      
      if ((JoyState == 0) && (msx_key == 0) && (key_shift == 0)) return 0xFF;
      
      if (MTX_KBD_DRIVE == 0xFD)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_ESC)   key1 = 0x01;
              if (msx_key == '2')           key1 = 0x02;
              if (msx_key == '4')           key1 = 0x04;
              if (msx_key == '6')           key1 = 0x08;
              if (msx_key == '8')           key1 = 0x10;
              if (msx_key == '0')           key1 = 0x20;
          }          
          return (~key1 & 0xFF);
      }
      
      if (MTX_KBD_DRIVE == 0xFE)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == '1')           key1 = 0x01;
              if (msx_key == '3')           key1 = 0x02;
              if (msx_key == '5')           key1 = 0x04;
              if (msx_key == '7')           key1 = 0x08;
              if (msx_key == '9')           key1 = 0x10;
              return (~key1 & 0xFF);
          }          
      }

      if (MTX_KBD_DRIVE == 0xFB)
      {
          u8 key1 = 0x00;
          if (joy1 & 0x01)                  key1 = 0x80;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_CTRL)  key1 = 0x01;
              if (msx_key == 'W')           key1 = 0x02;
              if (msx_key == 'R')           key1 = 0x04;
              if (msx_key == 'Y')           key1 = 0x08;
              if (msx_key == 'I')           key1 = 0x10;
              if (msx_key == 'P')           key1 = 0x20;
              if (msx_key == '[')           key1 = 0x40;
              if (msx_key == KBD_KEY_UP)    key1 = 0x80;
          }          
          return (~key1 & 0xFF);
      }
      if (MTX_KBD_DRIVE == 0xF7)
      {
          u8 key1 = 0x00;
          if (joy1 & 0x04)                  key1 = 0x80;
          if (msx_key)
          {
              if (msx_key == 'Q')           key1 = 0x01;
              if (msx_key == 'E')           key1 = 0x02;
              if (msx_key == 'T')           key1 = 0x04;
              if (msx_key == 'U')           key1 = 0x08;
              if (msx_key == 'O')           key1 = 0x10;
              if (msx_key == '@')           key1 = 0x20;
              if (msx_key == KBD_KEY_LEFT)  key1 = 0x80;              
          }          
          return (~key1 & 0xFF);
      }
      
      
      if (MTX_KBD_DRIVE == 0xEF)
      {
          u8 key1 = 0x00;
          if (joy1 & 0x08)                  key1 = 0x80;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_SHIFT) key1 = 0x01;
              if (msx_key == 'S')           key1 = 0x02;
              if (msx_key == 'F')           key1 = 0x04;
              if (msx_key == 'H')           key1 = 0x08;
              if (msx_key == 'K')           key1 = 0x10;
              if (msx_key == ';')           key1 = 0x20;
              if (msx_key == ']')           key1 = 0x40;
              if (msx_key == KBD_KEY_RIGHT) key1 = 0x80;              
          }          
          return (~key1 & 0xFF);
      }
      if (MTX_KBD_DRIVE == 0xDF)
      {
          u8 key1 = 0x00;
          if (JoyState & JST_FIRER)         key1 = 0x80;
          if (JoyState & JST_FIREL)         key1 = 0x80;
          if (msx_key)
          {
              if (msx_key == 'A')           key1 = 0x01;
              if (msx_key == 'D')           key1 = 0x02;
              if (msx_key == 'G')           key1 = 0x04;
              if (msx_key == 'J')           key1 = 0x08;
              if (msx_key == 'L')           key1 = 0x10;
              if (msx_key == ':')           key1 = 0x20;
              if (msx_key == KBD_KEY_RET)   key1 = 0x40;
          }          
          return (~key1 & 0xFF);
      }


      if (MTX_KBD_DRIVE == 0xBF)
      {
          u8 key1 = 0x00;
          if (joy1 & 0x02)                  key1 = 0x80;
          if (key_shift)                    key1 = 0x01;    // SHIFT key
          
          if (msx_key)
          {
              if (msx_key == 'X')           key1 = 0x02;
              if (msx_key == 'V')           key1 = 0x04;
              if (msx_key == 'N')           key1 = 0x08;
              if (msx_key == ',')           key1 = 0x10;
              if (msx_key == '/')           key1 = 0x20;
              if (msx_key == KBD_KEY_DOWN)  key1 = 0x80;              
          }          
         return (~key1 & 0xFF);
      }
      if (MTX_KBD_DRIVE == 0x7F)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == 'Z')           key1 = 0x01;
              if (msx_key == 'C')           key1 = 0x02;
              if (msx_key == 'B')           key1 = 0x04;
              if (msx_key == 'M')           key1 = 0x08;
              if (msx_key == '.')           key1 = 0x10;
          }          
          return (~key1 & 0xFF);
      }
  }    
  else if ((Port == 0x06))
  {
      if (MTX_KBD_DRIVE == 0xFD)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_STOP)  key1 = 0x01;    // Backspace key on Memotech
              if (msx_key == KBD_KEY_F5)    key1 = 0x02;    // F5
          }          
          return (~key1 & 0xFF);
      }
      else if (MTX_KBD_DRIVE == 0xFE)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_CTRL)  key1 = 0x01;    // BREAK key on Memotech
              if (msx_key == KBD_KEY_F1)    key1 = 0x02;    // F1
          }          
          return (~key1 & 0xFF);
      }

      else if (MTX_KBD_DRIVE == 0xFB)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_F2)    key1 = 0x02;    // F2
          }          
          return (~key1 & 0xFF);
      }
      else if (MTX_KBD_DRIVE == 0xF7)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_F6)    key1 = 0x02;    // F6
          }          
          return (~key1 & 0xFF);
      }
      
      
      else if (MTX_KBD_DRIVE == 0xEF)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_F7)   key1 = 0x02;    // F7
          }          
          return (~key1 & 0xFF);
      }
      else if (MTX_KBD_DRIVE == 0xDF)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_F3)    key1 = 0x02;    // F3
          }          
          return (~key1 & 0xFF);
      }
      

      else if (MTX_KBD_DRIVE == 0xBF)
      {
          u8 key1 = 0x00;
          if (msx_key)
          {
              if (msx_key == KBD_KEY_F8)    key1 = 0x02;    // F8
          }          
          return (~key1 & 0xFF);
      }
      else if (MTX_KBD_DRIVE == 0x7F)
      {
          u8 key1 = 0x00;
          if (JoyState & JST_BLUE)          key1 = 0x01;    // Map the alternate 2 buttons to 'space' as some games make use of this as a 2nd button
          if (JoyState & JST_PURPLE)        key1 = 0x01;          
          if (msx_key)
          {
              if (msx_key == ' ')           key1 = 0x01;
              if (msx_key == KBD_KEY_F4)    key1 = 0x02;    // F4
          }          
          return (~key1 & 0xFF);
      }
  }

  // No such port
  return(NORAM);
}

// ------------------------------------------------------------------------------------

// Sord Memotech MTX IO Port Write - Need to handle SN sound, VDP and the Z80-CTC chip
// ------------------------------------------------------------------------------------
void cpu_writeport_memotech(register unsigned short Port,register unsigned char Value) 
{
    // M5 ports are 8-bit
    Port &= 0x00FF;

    if (Port == 0x00)   // This is where the memory bank map magic happens for the MTX
    {
        IOBYTE = Value;
        if (lastIOBYTE != IOBYTE)
        {
            // -----------------------------------------------------------------------
            // We are using very simplified logic for the MTX... real bank handling 
            // is more complex than is required just to play a few more games...
            // -----------------------------------------------------------------------
            if ((IOBYTE & 0x80) == 0)  // ROM Mode...
            {
                if (memotech_RAM_start == 0x0000)
                {
                    FastMemCopy(pColecoMem+0x0000, (u8 *)(0x6820000+0x0000), 0x2000);   // Copy mtx_os[] rom into memory
                    pColecoMem[0x0aae] = 0xed; pColecoMem[0x0aaf] = 0xfe; pColecoMem[0x0ab0] = 0xc9;  // Patch for .MTX tape access      
                }
                
                if ((IOBYTE & 0x70) == 0x00)       
                {
                    FastMemCopy(pColecoMem+0x2000, (u8 *)(0x6820000+0x2000), 0x2000);   // Copy mtx_basic[] rom into memory
                    memotech_RAM_start = 0x4000;
                }
                else if ((IOBYTE & 0x70) == 0x10)  
                {
                    FastMemCopy(pColecoMem+0x2000, (u8 *)(0x6820000+0x4000), 0x2000);   // Copy mtx_assem[] rom into memory
                    memotech_RAM_start = 0xC000;                                        // We only allow access to upper RAM here
                }
                else 
                {
                    memset(pColecoMem+0x2000,0xFF,0x2000);      // Nothing lives here...
                    memotech_RAM_start = 0xC000;
                }
            }
            else                      // RAM Mode
            {
                memotech_RAM_start = 0x0000;        // We're emulating a 64K machine
            }
            lastIOBYTE = IOBYTE;
        }
    }
    // ----------------------------------------------------------------------
    // Z80-CTC Area
    // This is only a partial implementation of the CTC logic - just enough
    // to handle the VDP and Sound Generation and very little else. This is
    // NOT accurate emulation - but it's good enough to render the Memotech
    // games as playable in this emulator.
    // ----------------------------------------------------------------------
    else if (Port >= 0x08 && Port <= 0x0B)
    {
        Port &= 0x03;
        if (ctc_latch[Port])    // If latched, we now have the countdown timer value
        {
            ctc_time[Port] = Value;     // Latch the time constant and compute the countdown timer directly below.
            ctc_timer[Port] = ((((ctc_control[Port] & 0x20) ? 256 : 16) * (ctc_time[Port] ? ctc_time[Port]:256)) / MTX_CTC_SOUND_DIV) + 1;
            ctc_latch[Port] = 0x00;     // Reset the latch - we're back to looking for control words
        }
        else
        {
            if (Value & 1) // Control Word
            {
                ctc_control[Port] = Value;      // Keep track of the control port 
                ctc_latch[Port] = Value & 0x04; // If the caller wants to set a countdown timer, the next value read will latch the timer
            }
            else
            {
                if (Port == 0x00) // Channel 0, bit0 clear is special - this is where the 4 CTC vector addresses are setup
                {
                    ctc_vector[0] = (Value & 0xf8) | 0;     // VDP Interrupt
                    ctc_vector[1] = (Value & 0xf8) | 2;     // 
                    ctc_vector[2] = (Value & 0xf8) | 4;     // 
                    ctc_vector[3] = (Value & 0xf8) | 6;     // 
                    sordm5_irq = ctc_vector[0];             // When the VDP interrupts the CPU, it's channel 0 on the CTC
                }
            }
        }
    }
    else if ((Port == 0x01) || (Port == 0x02))  // VDP Area
    {
        if ((Port & 1) != 0) WrData9918(Value);
        else if (WrCtrl9918(Value)) CPU.IRequest=sordm5_irq;    // Memotech MTX must get vector from the Z80-CTC. Only the CZ80 core works with this.
    }
    else if (Port == 0x05) MTX_KBD_DRIVE = Value;
    else if (Port == 0x06) sn76496W(Value, &sncol);
}

// ---------------------------------------------------------
// The Memotech MTX has CTC plus some memory handling stuff
// ---------------------------------------------------------
void memotech_reset(void)
{
    if (memotech_mode)
    {
        // Reset the Z80-CTC stuff...
        memset(ctc_control, 0x00, 4);       // Set Software Reset Bit (freeze)
        memset(ctc_time, 0x00, 4);          // No time value set
        memset(ctc_timer, 0x00, 8);         // No timer value set
        memset(ctc_vector, 0x00, 4);        // No vectors set
        memset(ctc_latch, 0x00, 4);         // No latch set
        sordm5_irq = 0xFF;                  // No IRQ set

        memotech_RAM_start = 0x4000;
        IOBYTE = 0x00;
        MTX_KBD_DRIVE = 0x00;
        lastIOBYTE = 99;
        tape_pos = 0;
    }
}

void MTX_HandleCassette(register Z80 *r)
{
    // Memotech MTX Tape Patch
    if ( r->PC.W-2 == 0x0AAE )
    {
        word base   = r->HL.W;
        word length = r->DE.W;
        //word calcst = pColecoMem[0xfa81] + (pColecoMem[0xfa82] * 256);

        if ( pColecoMem[0xfd68] == 0 )
        /* SAVE */
        {
            // Not supported yet...
        }
        else if ( pColecoMem[0xfd67] != 0 )
        /* VERIFY.
           Normally, if verification fails, the MTX BASIC ROM
           stops the tape, cleans up and does rst 0x28.
           That rst instruction is at 0x0adb. */
        {
            if ( base == 0xc011 && length == 18 )
            {
                tape_pos = 0;
            }
            tape_pos += length;
        }
        else
        {
            /* LOAD */
            if ( base == 0xc011 && length == 18 )
            /* Load header, so read whole file */
            {
                // File is in romBuffer[]
                tape_pos = 0;
            }

            /* Then return chunks as requested */
            memcpy(pColecoMem + base, romBuffer+tape_pos, length);
            tape_pos += length;
        }

        cpu_writeport_memotech(0x08, 0x00);
        cpu_writeport_memotech(0x08, 0xf0);
        cpu_writeport_memotech(0x08, 0x03);
        cpu_writeport_memotech(0x09, 0x03);
        cpu_writeport_memotech(0x0A, 0x03);
        cpu_writeport_memotech(0x0B, 0x03);

        RdCtrl9918();

        /* Then re-enables the video interrupt */
        cpu_writeport_memotech(0x08, 0xa5);
        cpu_writeport_memotech(0x08, 0x7d);    
    }    
}
// End of file