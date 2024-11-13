/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2024 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2000-2001 Simon White
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "sidemu.h"

namespace libsidplayfp
{

const char sidemu::ERR_UNSUPPORTED_FREQ[] = "Unable to set desired output frequency.";
const char sidemu::ERR_INVALID_SAMPLING[] = "Invalid sampling method.";
const char sidemu::ERR_INVALID_CHIP[]     = "Invalid chip model.";

void sidemu::writeReg(uint_least8_t addr, uint8_t data)
{
    switch (addr)
    {

    case 0x02:
        if (isTgrWavesEnabled) data = 0;
    case 0x03:
        if (isTgrWavesEnabled) data = 0x08;
    case 0x04:
        // Ignore writes to gate bit to mute voices
        if (isMuted[0]) data &= 0xfe;
        // Check and manipulate writes to the control register
        // If saw-combined wave, set the sawcon flag    
        sawcon = ((data & 0x20) && (data >= 0x30));
        if (isTgrWavesEnabled) {
            // If pulse wave, disable pulse and enable saw
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            // If tri+pulse wave with no ringmod, disable pulse
            if ((data & 0xf4) == 0x50) data &= 0xbf;
            // If saw-combined wave, disable pulse and tri
            if (data & 0x20) data &= 0xaf;
            // If non-noise wave with sync, set wave to saw
            if ((data & 0x82) == 0x02) data = (data & 0x0f) | 0x20;
        }
        break;

    case 0x09:
        if (isTgrWavesEnabled) data = 0;
    case 0x0a:
        if (isTgrWavesEnabled) data = 0x08;
    case 0x0b:
        if (isMuted[1]) data &= 0xfe; 
        sawcon = ((data & 0x20) && (data >= 0x30));
        if (isTgrWavesEnabled) {
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0xf4) == 0x50) data &= 0xbf;
            if (data & 0x20) data &= 0xaf;
            if ((data & 0x82) == 0x02) data = (data & 0x0f) | 0x20;
        }
        break;

    case 0x10:
        if (isTgrWavesEnabled) data = 0;
    case 0x11:
        if (isTgrWavesEnabled) data = 0x08;
    case 0x12:
        if (isMuted[2]) data &= 0xfe; 
        sawcon = ((data & 0x20) && (data >= 0x30));
        if (isTgrWavesEnabled) {
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0xf4) == 0x50) data &= 0xbf;
            if (data & 0x20) data &= 0xaf;
            if ((data & 0x82) == 0x02) data = (data & 0x0f) | 0x20;
        }
        break;

    case 0x17:
        // Ignore writes to filter register to disable filter
        if (isFilterDisabled) data &= 0xf0;
        break;

    case 0x18:
        // Ignore writes to volume register to mute samples
        // Works only for volume-based digis
        // Trick suggested by LMan
        if (isMuted[3]) data |= 0x0f;
        break;
    }

    write(addr, data);

    wavegenflags(addr, sawcon, isTgrWavesEnabled);
}

void sidemu::voice(unsigned int voice, bool mute)
{
    if (voice < 4)
        isMuted[voice] = mute;
}

void sidemu::tgrwaves(bool enable)
{
    isTgrWavesEnabled = enable;
}

void sidemu::filter(bool enable)
{
    isFilterDisabled = !enable;
}

bool sidemu::lock(EventScheduler *scheduler)
{
    if (isLocked)
        return false;

    isLocked  = true;
    eventScheduler = scheduler;

    return true;
}

void sidemu::unlock()
{
    isLocked  = false;
    eventScheduler = nullptr;
}

}
