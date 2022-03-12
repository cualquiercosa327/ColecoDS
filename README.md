# ColecoDS
ColecoDS - A Colecovision Emulator for the DS/DSi

_**Your Vision Is Our Vision...  COLECOVISION**_

To run requires a coleco.rom BIOS to 
be in the same directory as the emulator
or else in /roms/bios or /data/bios

Because the chips used in the Colecovision Hardware 
were so common in that era, other systems tended
to be very close to the CV in terms of hardware. 
Often only the IO/Memory was different. As such,
ColecoDS also allows cartridge (not disk or
cassette) games from "cousin" systems to be played - 
namely the Sord M5, the SG-1000 and the MSX1.

If you want to play Sord M5 games, you will
also need an 8k sordm5.rom bios in the same
directory as the coleco.rom BIOS.

SG-1000 emulation does not need a BIOS.

MSX emulation does not need a BIOS (C-BIOS used) 
but you can put the 32k msx.rom in /roms/bios or /data/bios 
and the emulator will use that instead.

ADAM computer emulation requires eos.rom and writer.rom 

Features :
-----------------------
* Colecovision game support (.rom or .col files)
* Super Game Module support including AY sound chip.
* Megacart Bankswitching support (up to 512K).
* Coleco ADAM game support (.ddp or .dsk files)
* Full Controller button mapping and touch-screen input.
* High-Score support - 10 scores per game.
* Save/Load Game State (one slot).
* Video Blend Mode (see below) and Vertical Sync.
* LCD Screen Swap (press and hold L+R+X during gameplay).
* Overlay support for the few games that need them.
* Super Action Controller, Spinner and Roller Controller (Trackball) mapping.
* Sega SG-1000 game support (.sg roms)
* Sord M5 game support (.m5 roms) - requires sordm5.rom BIOS.
* MSX1 game support (.msx or .rom) up to 512K 
* Full speed, full sound and full frame-rate even on older hardware.

Copyright :
-----------------------
ColecoDS Phoenix-Edition is Copyright (c) 2021-2022 Dave Bernazzani (wavemotion-dave)

Copying and distribution of this emulator, it's source code
and associated readme files, with or without modification, 
are permitted in any medium without royalty provided this 
copyright notice is used and wavemotion-dave (Phoenix-Edition),
Alekmaul (original port) and Marat Fayzullin (ColEM core) are 
thanked profusely.

The ColecoDS emulator is offered as-is, without any warranty.

Credits :
-----------------------
Thanks to Alekmaul who provided the 
baseline code to work with and to lobo
for the menu graphical design.

Thanks to Reesy for the DrZ80 core.

Thanks to Flubba for the SN76496 sound core.

Thanks to the C-BIOS team for the open 
source MSX BIOS (see cbios.txt)

Special thanks to  Marat Fayzullin, as the 
author of ColEM which is the code for the 
core emulation (specifically TMS9918 VDP
and the CZ80 CPU core).  I think the original 
port was circa ColEM 2.1 with some fixes and 
updated Sprite/Line handling from ColEM 5.6

Known Issues :
-----------------------
* Borders are not correctly rendered - only a few games utilize them and are still fully playable without this.
* Games that utilize voice samples (Squish Em Sam, Wizard of Wor, etc) will not play the speech.
* 64K Activision PCB carts have no EEPROM support (Black Onyx, Boxxle - both playable without saves).
* The original 2011 release of StarForce will crash - this is a known bug. There is a patched version of the game StarForce on Atariage.
* Sord M5 games are not sound accurate due to CTC emulation issues.
* MSX envelope sound and noise emulation not accurate (close enough).
* MSX Konami SCC sound chip is not emulated (Gradius 2/3, Salamander, etc. won't have music)

ADAM Compatibility :
-----------------------
The emulated ADAM is not completely bug-free but generally will run most tape images (.ddp) or disk images (.dsk).
The emulated ADAM is a 128K system (64K internal memory and 64K expanded RAM) - enough for most any game.
Sometimes when loading an ADAM game the system doesn't run... just hit RESET and it will probably load.
You can turn on the full ADAM keyboard with the Configuration of Overlays (choose 'ADAM KEYBOARD').
The tape or disk images do NOT automatically write back to your SD card... you have to hit the little Cassette icon
to make that happen (and only whenthe tape/disk is idle - it won't save if the tape/disk is busy reading/writing).

MSX Compatibility :
-----------------------
Considering this is a Colecovision emulator, the MSX1 support and compatibility is reasonably high.  In Game Options you will notice that
the default MSX Mapper is set to "GUESS" which does a fairly good job loading the ROM - especially for 32K or smaller games. However, if a game doesn't run, you can try these suggestions:
* A small number of games don't work with the open-source C-BIOS. In this case you would need a real msx.rom BIOS. You can set this up in Game Options.
* Most 64K games use the ASC16 memory mapper - so you can try that one... but a few (e.g. Mutants from the Deep) are linear mapped from 0-64K and you will need to pick LINEAR64 in Game Options. 
* The auto-detection on KONAMI8 and KONAMI-SCC mappers is pretty good... but many other games using ASCII8 or ASCII16 don't detect well - you should try those mappers if the "larger than 64K" game won't run.
* Some of the really big games (128K or larger) run slow - if you're not getting full frame rate, you can try switching to the DrZ80 core (not all games will run with this) and/or you can disable Vertical Sync and/or enable Frame Skip.  Recommend you turn off Vertical Sync at a minimum to get the most speed from large MSX games.
* Occasionally one ROM won't run but an alternate dump might. For example, the 384K version of R-Type is a bit of a mess for the emulator to handle, but someone made a clean 512K version that loads and runs great.
* Some MSX games take 4 or 5 seconds to initialize and start running due to checking various slots and subslots for memory - be patient. 
* With a little diligence in trying different mapping/BIOS combinations, you should be able to achieve a 98% run rate on MSX1 games. 
* MSX2 games are not supported and will not run.

The MSX memory is laid out as follows:
```
 SLOT0:  MSX BIOS (first 32K... 0xFF after that)
 SLOT1:  Cartridge Slot (this is where the game ROM lives)
 SLOT2:  Empty (0xFF always)
 SLOT3:  RAM (64K)
```

Controllers :
-----------------------
You can map buttons to either P1 or P2 controllers. 
There is full support for Spinner X (P1) and Spinner Y (P2) or map 
both of them to get support for trackball games. These also work
for games like Turbo steering.  You can change the spinner sensitivity
to one of five different settings (Normal, Fast, Fastest, Slow, Slowest).

For the MSX emulation, the colecovision keypad is mapped as follows:
```
  1   2   3
  
  4   5  STOP
  
  M1  M2  M3
  
  SPC 0   RET
```
That should be enough to get most MSX1 cart games running...  In Game Options you can also override the '5' key to be any mappable MSX key. For the few games that still require the MSX arrows to play - you can emulate that via the D-PAD in Game Options.
As of version 6.1 there is also a custom overlay for "MSX Full" keyboard.

Blend Mode (DSi) :
-----------------------
A huge change is the new "blend mode" which I borrowed from my scheme on StellaDS. In this mode, 
two frames are blended together - this is really useful when playing games like Space Fury or Galaxian 
where the bullets on screen are only 1 pixel wide and the DSi LCD just doesn't hold onto the pixels 
long enough to be visible. These games were designed to run on an old tube TV with phosphor which 
decays slowly so your eye will see slight traces as the image fades. This emulates that (crudely).
On the DSi using this new mode renders those games really bright and visible.

The DSi XL/LL has a slower refresh on the LCD and it more closely approximates the old tube TVs... 
so blend mode is not needed for the XL/LL models.

However! Using blend mode comes at at 25% CPU cost!! The DSi can handle it... the DS-LITE/PHAT might
struggle a bit on more complicated games. 

So my recommendation is as follows:
* DSi non XL/LL - use Blend Mode for the games that benefit from it (Space Fury, Galaxian, etc).
* DSi XL/LL - don't bother... the XL/LL screen decay is slower and games look great as-is.
* DS-LITE/PHAT - you can try it but the framerate might drop below 60 on some games.

To enable this new blend mode, pick your game and go into the "Game Options" sub-menu and turn it on.

Vertical Sync :
-----------------------
Vertical sync will force the update (refresh) of the screen when the DS goes into the vertical blank.
This reduces tearing and minor graphical artifacts but comes at a cost of speed. The DSi can handle
it for almost all games (Princess Quest is one game where you might turn it off) but the DS can only
handle it for the more simple games. So by default it's enabled for DSi and disabled for DS-LITE/PHAT.
You can toggle this in the "Game Options" (and START=SAVE it out as you wish). 

A Tale of Two Cores :
-----------------------
ColecoDS supports 2 different Z80 CPU cores. 
DrZ80 is very fast but is not 100% accurate so some games don't run right.
CZ80 is 10% slower but is much closer to 100% accurate and games generally run great.
For the DSi and above, the CZ80 core is the default.
For the DS-LITE/PHAT, the DrZ80 core is the default.
You can toggle this (and save on a per-game basis) in GAME OPTIONS.
If you want to use the CZ80 core but need a bit more speed to make the
games playable - you can also play with Vertical Sync and Frame Skip settings.

The Need For Speed :
-----------------------
If a game just isn't running at the right speed or has periods of slowdown (not
attributed to the actual game), here are the things you can try in the order I 
would personally try them:
* Turn off Vertical Sync
* Turn on Frame Skip
* Set Max Sprites to 4
* Switch to the DrZ80 fast CPU core

Versions :
-----------------------
V6.3: 12-Mar-2022 by wavemotion-dave
* ADAM Computer support is added! Play .ddp and .dsk games (requires eos.rom and writer.rom).
* Other minor cleanups and fixes as time permitted.

V6.2: 29-Jan-2022 by wavemotion-dave
* Increased AY noise frequency dynamic range - improves MSX and CV-SGM sounds.
* Increased AY tone frequency dynamic range - improves MSX and CV-SGM sounds.
* Diagnoal d-pad mapping now available on SG-1000.
* Other minor cleanups as time permitted.

V6.1: 25-Jan-2022 by wavemotion-dave
* Added full MSX keyboard overlay - Choose "MSX Full" in Game Options.
* Improved ASC8 mapper so Bomber King, Batman Rovin and others now playable.
* Added SRAM support to make Hydlide II, Dragon Slayer II (Xanadu) and Deep Dungeon 2 are now playable.
* Added 'beeper music' sounds so games like Avenger, Batman - The Movie and Masters of the Universe have sound.

V6.0: 21-Jan-2022 by wavemotion-dave
* Improved MSX compatabilty - more playable games.
* Added D-PAD to emulate MSX arrow keys.
* Added D-PAD diagonals emulation.

V5.9: 19-Jan-2022 by wavemotion-dave
* Refactor of memory to gain another 128K of fast VRAM to improve Coleco MegaCart and MSX games.

V5.8: 16-Jan-2022 by wavemotion-dave
* Improved DMA memory handling of MSX to bump mega ROM games speed by up to 10%
* Improved loading databaes so more games detect memory mapper correctly.
* Faster RAM swapping for improved loading speed on games.
* Other cleanups and improvements under the hood.

V5.7: 12-Jan-2022 by wavemotion-dave
* Fixed RESET of Colecovision games.
* Added MSX keypad template and configurable MSX key '5'
* Improved detection of 32K MSX basic games for better compatibility.
* Allow 64K ROMs to be loaded in linear memory.
* Allow 48K ROMs to be memory mapped.
* Allow 32K ROMs to be loaded at 0K, 4K or 8K.

V5.6: 11-Jan-2022 by wavemotion-dave
* Improved loading of MSX 8K, 16K and 32K ROMs for higher compatibility.
* Fixed so we only return joystick input for Port 1.
* Fixed bug in memory write to Slot 1 (rare).
* Minor cleanups across the board.

V5.5: 09-Jan-2022 by wavemotion-dave
* Major improvement in speed for MSX megaROM games. 
* Fixed RESET of MSX megaROM games.
* Slight optmizations to all emulation cores.

V5.4: 07-Jan-2022 by wavemotion-dave
* AY Envelope sound handler improved - more games sound right!
* MSX, SG-1000 and Sord M5 SAVE/LOAD states working.
* Press X on ROM selection to force-load MSX game cart (in case auto-detect fails).
* Faster audio processing to gain us almost 5% speed boost across the board.
* Fix controls when launched from TWL++
* More cleanups and minor improvements under the hood. 

V5.3: 06-Jan-2022 by wavemotion-dave
* Added MSX config to set BIOS on per game basis.
* Added MSX config to set mapper type.
* Upgraded Config Database to 800 entries.
* Upgrade High Score Database to 575 entries.
* Upgrade roms per directory to 1024 entries.
* Autodetect between CV and MSX .rom files.
* Revised MSX controller map for better game support.
* AY optmization so MSX games run faster.

V5.2: 04-Jan-2022 by wavemotion-dave
* MSX1 now supports 256K and 512K mega ROMs. 
* AY Sound core re-written so noise and envelopes work (not perfectly accurate but good enough).
* Optional you can use msx.rom BIOS if found in the usual places.

V5.1: 03-Jan-2022 by wavemotion-dave
* MSX1 emulation now supports some of the common Mappers - some of the 128K games work but you'll probably have to turn off Vert Sync and turn on Frame Skip to get it to run full speed.
* MSX1 emulation is now 64K Main RAM 

V5.0: 02-Jan-2022 by wavemotion-dave
* MSX1 game support up to 32K Standard Loader (.msx format)
* New 3/4 Frameskip (show 3 of 4 frames) to help DS-LITE

V4.9: 30-Dec-2021 by wavemotion-dave
* Improved SG-1000 game support.
* Preliminary support for Sord M5 games (.m5 format)

V4.8: 26-Dec-2021 by wavemotion-dave
* Preliminary support for SG-1000 games (.sg format)
* Cleanup across the board - a bit more speed gained.

V4.7: 23-Dec-2021 by wavemotion-dave
* Major speed improvements in the new CZ80 core. 
* Installed new CZ80 core as the default for DSi and above. 

V4.6: 22-Dec-2021 by wavemotion-dave
* New CZ80 core added to solve compatibility problems with the remaining games.

V4.5: 21-Dec-2021 by wavemotion-dave
* Full support for Spinner/Trackball. Map SpinX, SpinY in key settings.
* Five different sensitivities for the Spinner/Trackball in Game Options.
* Added ability to Quit Emulator (will return to TWL++ or power off depending on launcher).
* Unified handling of UI in Redefine Keys and Game Options so they work the same.
* Many small fixes and tweaks under the hood - Sudoku fixed.

V4.4: 18-Dec-2021 by wavemotion-dave
* Added option for "Max Sprites" to set to original HW limit of 4 vs 32.
* New Vertical Sync option (default ON for DSi and above) to reduce tearing.
* Fixed Save/Load state so it doesn't break on every new release.
* Slight adjustment to Z80 CPU timing for better accuracy.

V4.3: 16-Dec-2021 by wavemotion-dave
* More overlays added.
* Adjust CPU timing slightly to fix Spy Hunter (cars now spawn) and Frantic.

V4.2: 14-Dec-2021 by wavemotion-dave
* Allow mapping of Super Action Controller buttons.
* Overlay support - five games supported (for now) + Generic.
* Minor cleanups to VDP and better commenting.

V4.1: 11-Dec-2021 by wavemotion-dave
* VDP Timing fixes - Fathom now runs.
* Fixed loading of coleco.rom bios if in same directory as emulator.
* Minor Z80 cleanups for more accurate timing.

V4.0: 09-Dec-2021 by wavemotion-dave
* Fix GhostBlaster homebrew.
* Fix for graphical issues in Meteoric Shower
* Improved DrZ80 core from various web sources
* Improved VDP handling to more closely mimic real TMS9918a
* Many small touch-ups and improvements under the hood

V3.9: 06-Dec-2021 by wavemotion-dave
* Fix for Pitfall II Arcade Homebrew
* Improved memory management
* Shorter keyclick for more responsive keypad touches
* Improved UI key handling
* Other cleanups and improvements under the hood
* Saved states changed again ... sorry!

V3.8: 04-Dec-2021 by wavemotion-dave
* L+R+X to swap LCD Screens
* New light keyclick for feedback of touch controller.
* Other minor cleanups and improvements under the hood.

V3.7: 27-Nov-2021 by wavemotion-dave
* Super DK and Super DK Jr prototypes work now.
* Max Game ROM filename extended to 128 bytes.
* Tries to start in /roms or /roms/coleco if possible.
* Slight tweaks to main menu graphics to clean them up.
* Code cleanup and commenting of source files.

V3.6: 25-Nov-2021 by wavemotion-dave
* New game options for frame skip, frame blend, auto fire, etc.
* Fixed pop noise on some of the SGM-AY games.
* Minor menu cleanup for better visual presentation.

V3.5: 24-Nov-2021 by wavemotion-dave
* Sound finally fixed with use of maxmod library!
* Updated CRC computation to match real crc32.
* High scores, save states and key options all changed - sorry!
* Lots of cleanups as timer permitted.

V3.4: 23-Nov-2021 by wavemotion-dave
* Save key map on a per-game basis.
* English is now the only language option.
* More cleanups and tweaks under the hood.

V3.3: 22-Nov-2021 by wavemotion-dave
* AY sound channels are now independent - for a CV total of 6 channels.
* Added MC/AY/SGM indicators on-screen for enhanced carts.
* Other cleanups and minor improvements across the board.

V3.2: 20-Nov-2021 by wavemotion-dave
* More AY sound improvements for the Super Game Module.
* Slight optmization of VDP rendering.
* Improved display of Game Titles - slower scroll and centered.
* Increase in contrast on game selection.
* Other cleanups and minor improvements across the board.

V3.1: 19-Nov-2021 by wavemotion-dave
* Fixed noise sound handling on AY/SGM games.
* Fixed audio pop going into first game.
* Optimized video rendering for speed improvement across the board.
* More robust VDP handling to avoid memory overflow.

V3.0: 18-Nov-2021 by wavemotion-dave
* Ressurected from the ashes - ported to the latest LIBNDS and DEVKIT PRO development tools.
* Sound core updated to latest SN76496.
* CPU core DrZ80 updated to latest.
* Added Super Game Module support with AY sound handling.
* Added MegaCart and Activision PCB cart support for larger games.
* Speed improvements and optmizations across the board.
