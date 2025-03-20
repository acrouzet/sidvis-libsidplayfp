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
    OS_data = data;

    switch (addr)
    {
    case 0x04:
        // Force envelope on and gate off to mute voices
        if (isMuted[0]) {
            doEnvDisable = false;
            data &= 0xfe;
        } else if (isEnvDisabled) doEnvDisable = true;
        if (isTwEnabled) {
            // If saw is on, disable tri and pulse
            if (data & 0x20) data &= 0xaf;
            // If only pulse is on, disable pulse and enable saw
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            // If tri and pulse are on, disable pulse
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x0b:
        if (isMuted[1]) {
            doEnvDisable = false;
            data &= 0xfe;
        } else if (isEnvDisabled) doEnvDisable = true;
        if (isTwEnabled) {
            if (data & 0x20) data &= 0xaf;
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x12:
        if (isMuted[2]) {
            doEnvDisable = false;
            data &= 0xfe;
        } else if (isEnvDisabled) doEnvDisable = true;
        if (isTwEnabled) {
            if (data & 0x20) data &= 0xaf;
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x17:
        OS_res_filt = OS_data;
        // Ignore writes to filter registers to disable filter
        if (isFilterDisabled) data = 0x00;
        break;
    case 0x18:
        // Force max volume to mute D418 volume-based digis
        if (isMuted[3]) data |= 0x0f;
        break;
    }

    write(addr, data);
    OS_write(addr, OS_data);

    sidvis(addr, doEnvDisable, isKinkDisabled, isTwEnabled);
}

void sidemu::voice(unsigned int voice, bool mute)
{
    if (voice < 4)
        isMuted[voice] = mute;
}

void sidemu::envelope(bool enable)
{
    isEnvDisabled = !enable;
}

void sidemu::kinkdac(bool enable);
}
    isKinkDisabled = !enable;
}

void sidemu::triggerwaves(bool enable)
{
    isTwEnabled = enable;
}

void sidemu::triggerfilter(bool enable)
{
    isTfEnabled = enable;
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
