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
    case 0x04:
        // Ignore writes to gate bit to mute voices.
        if (isMuted[0]) data &= 0xfe;
        tw0_control = data;
        if (isTWEnabled) 
		{   // Don't set wave to sawtooth if write:
			// - has no wave selected.
			// - has noise bit set.
			// - is for tri+pulse wave with ringmod and no sync.
			// - is for triangle wave with no sync.
			if (((data & 0xf0) != 0) && !(data & 0x80) && ((data & 0xf6) != 0x54) && ((data & 0xf2) != 0x10))
			{   // Set wave to sawtooth.
				data = (data & 0x0f) | 0x20;
			}
        }
        break;
    case 0x0b:
        if (isMuted[1]) data &= 0xfe; 
        tw0_control = data;     
        if (isTWEnabled) 
		{
			if (((data & 0xf0) != 0) && !(data & 0x80) && ((data & 0xf6) != 0x54) && ((data & 0xf2) != 0x10))
			{
				data = (data & 0x0f) | 0x20;
			}
        }
        break;
    case 0x12:
        if (isMuted[2]) data &= 0xfe;
        tw0_control = data;     
        if (isTWEnabled) 
		{
			if (((data & 0xf0) != 0) && !(data & 0x80) && ((data & 0xf6) != 0x54) && ((data & 0xf2) != 0x10))
			{
				data = (data & 0x0f) | 0x20;
			}
        }
        break;
    case 0x17:
        // Ignore writes to filter register to disable filter.
        if (isFilterDisabled) data &= 0xf0;
        break;
    case 0x18:
        // Set master volume permanently to max to mute its output.
        if (isMuted[3]) data |= 0x0f;
        break;
    }

    write(addr, data);

    twdata(addr, isTWEnabled, tw0_control);
}

void sidemu::voice(unsigned int voice, bool mute)
{
    if (voice < 4)
        isMuted[voice] = mute;
}

void sidemu::triggerwaves(bool enable)
{
    isTWEnabled = enable;
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
