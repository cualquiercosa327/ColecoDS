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

// ------------------------------------------------------------------------------------
// The 'Fake' AY handler simply turns AY sound register access into corresponding
// SN sound chip calls. There is some loss of fidelity and we have to handle the
// volume envelopes in a very crude way... but it works and is good enough for now.
//
// The AY register map is as follows:
// Reg      Description
// 0-5      Tone generator control for channels A,B,C
// 6        Noise generator control
// 7        Mixer control-I/O enable
// 8-10     Amplitude control for channels A,B,C
// 11-12    Envelope generator period
// 13       Envelope generator shape
// 14-15    I/O ports A & B (MSX Joystick mapped in here - otherwise unused)
// ------------------------------------------------------------------------------------

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../colecoDS.h"
#include "../../colecomngt.h"
#include "../../colecogeneric.h"

#include "SN76496.h"
#include "Fake_AY.h"

extern u8 sgm_enable;
extern u8 ay_reg_idx;
extern u16 sgm_low_addr;
extern SN76496 aycol;

static const u8 Volumes[16] = { 15,15,13,12,11,10,9,8,7,6,5,4,3,2,1,0 };
u16 envelope_period __attribute__((section(".dtcm"))) = 0;
u16 envelope_counter __attribute__((section(".dtcm"))) = 0;

unsigned char Envelopes[16][32] __attribute__((section(".dtcm"))) =
{
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
    {15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

u8 AY_RegisterMasks[] __attribute__((section(".dtcm"))) = {0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0x1F, 0xFF, 
                                                           0x1F, 0x1F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

u16 noise_period __attribute__((section(".dtcm"))) = 0;

u8 a_idx __attribute__((section(".dtcm"))) = 0;
u8 b_idx __attribute__((section(".dtcm"))) = 0;
u8 c_idx __attribute__((section(".dtcm"))) = 0;


void UpdateNoiseAY(void);
void UpdateTonesAY(void);

// ---------------------------------------------------------------------------------------------
// We handle envelopes here... the timing is nowhere near exact but so few games utilize this 
// and so accuracy isn't all that critical. The sound will be a little off - but it will be ok.
// ---------------------------------------------------------------------------------------------
void FakeAY_Loop(void)
{
    //if (++envelope_counter > envelope_period)  - for speed, the counter is handled by the caller
    {
        u8 bUpdateVols = false;
        envelope_counter = 0;
        u8 shape = ay_reg[0x0D] & 0x0F;
        
        // ---------------------------------------
        // If Envelope is enabled for Channel A 
        // ---------------------------------------
        if ((ay_reg[0x08] & 0x10))
        {
            u8 vol = Envelopes[shape][a_idx]; 
            if (vol != (ay_reg[0x08] & 0x0F))
            {
                ay_reg[0x08] = (ay_reg[0x08] & 0xF0) | vol;
                bUpdateVols = true;
            }
            if (++a_idx > 31)
            {
                if ((shape & 0x09) == 0x08) a_idx = 0; else a_idx=31;   // Decide if we continue the shape or hold 
            }
        }
        
        // ---------------------------------------
        // If Envelope is enabled for Channel B
        // ---------------------------------------
        if ((ay_reg[0x09] & 0x10))
        {
            u8 vol = Envelopes[shape][b_idx]; 
            if (vol != (ay_reg[0x09] & 0x0F))
            {
                ay_reg[0x09] = (ay_reg[0x09] & 0xF0) | vol;
                bUpdateVols = true;
            }
            if (++b_idx > 31)
            {
                if ((shape & 0x09) == 0x08) b_idx = 0; else b_idx=31;   // Decide if we continue the shape or hold 
            }
        }

        // ---------------------------------------
        // If Envelope is enabled for Channel C
        // ---------------------------------------
        if ((ay_reg[0x0A] & 0x10))
        {
            u8 vol = Envelopes[shape][c_idx]; 
            if (vol != (ay_reg[0x0A] & 0x0F))
            {
                ay_reg[0x0A] = (ay_reg[0x0A] & 0xF0) | vol;
                bUpdateVols = true;
            }
            if (++c_idx > 31)
            {
                if ((shape & 0x09) == 0x08) c_idx = 0; else c_idx=31;   // Decide if we continue the shape or hold 
            }
        }
        
        if (bUpdateVols)
        {
            UpdateTonesAY();
            UpdateNoiseAY();
        }
    }
}

// ------------------------------------------------------------------
// Noise is a bit more complicated on the AY chip as we have to
// check each A,B,C channel to see if we should be mixing in noise. 
// ------------------------------------------------------------------
void UpdateNoiseAY(void)
{
      // Output the noise for the first channel it's enbled on...
      if      (!(ay_reg[0x07] & 0x08) && ((ay_reg[0x08]&0xF) != 0)) sn76496W(0xF0 | Volumes[ay_reg[0x08]&0xF], &aycol);
      else if (!(ay_reg[0x07] & 0x10) && ((ay_reg[0x09]&0xF) != 0)) sn76496W(0xF0 | Volumes[ay_reg[0x09]&0xF], &aycol);
      else if (!(ay_reg[0x07] & 0x20) && ((ay_reg[0x0A]&0xF) != 0)) sn76496W(0xF0 | Volumes[ay_reg[0x0A]&0xF], &aycol);
      else sn76496W(0xFF, &aycol);  // Otherwise Noise is OFF
}

void UpdateToneA(void)
{
    u16 freq=0;
    
    // ----------------------------------------------------------------------
    // If Channel A tone is enabled - set frequency and update SN sound core
    // ----------------------------------------------------------------------
    if (!(ay_reg[0x07] & 0x01))
    {
        freq = (ay_reg[0x01] << 8) | ay_reg[0x00];
        freq = ((freq & 0x0C00) ? 0x3FF : freq&0x3FF);
        sn76496W(0x80 | (freq & 0xF), &aycol);
        sn76496W((freq >> 4) & 0x3F, &aycol);
        if (freq > 0)
            sn76496W(0x90 | Volumes[(ay_reg[0x08] & 0x0F)], &aycol);
        else 
            sn76496W(0x9F, &aycol); // Turn off tone sound on Channel A
    }
    else
    {
        sn76496W(0x9F, &aycol); // Turn off tone sound on Channel A
    }    
}

void UpdateToneB(void)
{
    u16 freq=0;
    
    // ----------------------------------------------------------------------
    // If Channel B tone is enabled - set frequency and update SN sound core
    // ----------------------------------------------------------------------
    if (!(ay_reg[0x07] & 0x02))
    {
        freq = (ay_reg[0x03] << 8) | ay_reg[0x02];
        freq = ((freq & 0x0C00) ? 0x3FF : freq&0x3FF);
        sn76496W(0xA0 | (freq & 0xF), &aycol);
        sn76496W((freq >> 4) & 0x3F, &aycol);
        if (freq > 0)
            sn76496W(0xB0 | Volumes[(ay_reg[0x09] & 0x0F)], &aycol);
        else 
            sn76496W(0xBF, &aycol); // Turn off tone sound on Channel B
    }
    else
    {
        sn76496W(0xBF, &aycol); // Turn off tone sound on Channel B
    }    
}


void UpdateToneC(void)
{
    u16 freq=0;
    
    // ----------------------------------------------------------------------
    // If Channel C tone is enabled - set frequency and update SN sound core
    // ----------------------------------------------------------------------
    if (!(ay_reg[0x07] & 0x04))
    {
        freq = (ay_reg[0x05] << 8) | ay_reg[0x04];
        freq = ((freq & 0x0C00) ? 0x3FF : freq&0x3FF);
        sn76496W(0xC0 | (freq & 0xF), &aycol);
        sn76496W((freq >> 4) & 0x3F, &aycol);
        if (freq > 0)
            sn76496W(0xD0 | Volumes[(ay_reg[0x0A] & 0x0F)], &aycol);
        else 
            sn76496W(0xDF, &aycol); // Turn off tone sound on Channel C
    }
    else
    {
        sn76496W(0xDF, &aycol); // Turn off tone sound on Channel C
    }    
}

// -----------------------------------------------------------------------
// Check if any of the Tone Channels A, B or C needs to be updated/output
// -----------------------------------------------------------------------
void UpdateTonesAY(void)
{
    UpdateToneA();
    UpdateToneB();
    UpdateToneC();
}

// ------------------------------------------------------------------------------------------------------------------
// Writing AY data is where the magic mapping happens between the AY chip and the standard SN colecovision chip.
// This is a bit of a hack... and it reduces the sound quality a bit on the AY chip but it allows us to use just
// one sound driver for the SN audio chip for everything in the system. On a retro-handheld, this is good enough.
// ------------------------------------------------------------------------------------------------------------------
void FakeAY_WriteData(u8 Value)
{
      extern u8 bDontResetEnvelope;
    
      // ----------------------------------------------------------------------------------------
      // This is the AY sound chip support... we're cheating here and just mapping those sounds
      // onto the original Colecovision SN sound chip. Not perfect but good enough.
      // ----------------------------------------------------------------------------------------
      //Value &= AY_RegisterMasks[ay_reg_idx & 0x0F];  // Not strictly necessary so we save the CPU time
    
      u8 prevVal = ay_reg[ay_reg_idx];
      ay_reg[ay_reg_idx]=Value;
      
      switch (ay_reg_idx)
      {
          // Channel A tone frequency (period) - low and high
          case 0x00:
          case 0x01:
              UpdateToneA();
              break;
              
          // Channel B tone frequency (period) - low and high
          case 0x02:
          case 0x03:
              UpdateToneB();
              break;

          // Channel C tone frequency (period) - low and high
          case 0x04:
          case 0x05:
              UpdateToneC();
              break;
              
          // Noise Period     
          case 0x06:
              noise_period = Value & 0x1F;
              if      (noise_period > 24) sn76496W(0xE2 | 0x04, &aycol);   // E2 is the lowest frequency (highest period)
              else if (noise_period > 12) sn76496W(0xE1 | 0x04, &aycol);   // E1 is the middle frequency (middle period)
              else                        sn76496W(0xE0 | 0x04, &aycol);   // E0 is the highest frequency (lowest period)              
              UpdateNoiseAY();  // Update the Noise output
              break;
              
          // Global Sound Enable/Disable Register
          case 0x07:
              UpdateTonesAY();  // Tones may have turned on/off
              UpdateNoiseAY();  // Update the Noise output
              break;
              
          // -------------------------------------------------------
          // Volume and Envelope Enable Registers are below...
          // -------------------------------------------------------
          case 0x08:
              if (Value & 0x10) // Is Envelope Mode for Channel A active?
              {
                  if (bDontResetEnvelope == false) a_idx = 0;
                  ay_reg[0x08] &= 0xF0 | (prevVal & 0x0F);
                  envelope_counter = 0xF000;    // Force first state change immediately
                  AY_EnvelopeOn = true;
              }
              else
              {
                  AY_EnvelopeOn = (((ay_reg[0x08] & 0x10) || (ay_reg[0x09] & 0x10) || (ay_reg[0x0A] & 0x10))  ? true : false);
                  UpdateToneA();
                  UpdateNoiseAY();
              }
              break;
              
          case 0x09:
              if (Value & 0x10)  // Is Envelope Mode for Channel B active?
              {
                  if (bDontResetEnvelope == false) b_idx = 0;
                  ay_reg[0x09] &= 0xF0 | (prevVal & 0x0F);
                  envelope_counter = 0xF000;    // Force first state change immediately
                  AY_EnvelopeOn = true;
              }
              else
              {
                  AY_EnvelopeOn = (((ay_reg[0x08] & 0x10) || (ay_reg[0x09] & 0x10) || (ay_reg[0x0A] & 0x10))  ? true : false);
                  UpdateToneB();
                  UpdateNoiseAY();
              }
              break;
              
          case 0x0A:
              if (Value & 0x10)   // Is Envelope Mode for Channel C active?
              {
                  if (bDontResetEnvelope == false) c_idx = 0;
                  ay_reg[0x0A] &= 0xF0 | (prevVal & 0x0F);
                  envelope_counter = 0xF000;    // Force first state change immediately
                  AY_EnvelopeOn = true;
              }
              else
              {
                  AY_EnvelopeOn = (((ay_reg[0x08] & 0x10) || (ay_reg[0x09] & 0x10) || (ay_reg[0x0A] & 0x10))  ? true : false);
                  UpdateToneC();
                  UpdateNoiseAY();
              }
              break;
             
          // -----------------------------
          // Envelope Period Register
          // -----------------------------
          case 0x0B:
          case 0x0C:
              envelope_period = ((ay_reg[0x0C] << 8) | ay_reg[0x0B]);
              envelope_period = envelope_period / 8;  // This gets us "close"
              break;
              
          case 0x0D:
              a_idx=0; b_idx=0; c_idx=0;
              break;
      }
}

// End of file
