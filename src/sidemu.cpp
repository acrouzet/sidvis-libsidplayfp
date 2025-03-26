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
    case 0x04: // Channel 1 Control
        // Force envelope on and gate off to mute voices
        if (isMuted[0]) {
            disableEnvelopes = false;
            data &= 0xfe;
        } else if (isNoEnvelopesEnabled) {
            disableEnvelopes = true;
        }
        if (isTriggerWavesEnabled) {
            // If saw is on, disable tri and pulse
            if  (data & 0x20)          data &= 0xaf;
            // If only pulse is on, disable pulse and enable saw
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            // If tri and pulse are on, disable pulse
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x0b: // Channel 2 Control
        if (isMuted[1]) {
            disableEnvelopes = false;
            data &= 0xfe;
        } else if (isNoEnvelopesEnabled) {
            disableEnvelopes = true;
        }
        if (isTriggerWavesEnabled) {
            if  (data & 0x20)          data &= 0xaf;
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x12: // Channel 3 Control
        if (isMuted[2]) {
            disableEnvelopes = false;
            data &= 0xfe;
        } else if (isNoEnvelopesEnabled) {
            disableEnvelopes = true;
        }
        if (isTriggerWavesEnabled) {
            if  (data & 0x20)          data &= 0xaf;
            if ((data & 0xf0) == 0x40) data ^= 0x60;
            if ((data & 0x50) == 0x50) data &= 0xbf;
        }
        break;

    case 0x17: // Resonance / Filter Channels
        if (isFilterDisabled) data  = 0xf0;
        if (isNotFiltered[0]) data &= 0xfe;
        if (isNotFiltered[1]) data &= 0xfd;
        if (isNotFiltered[2]) data &= 0xfb;
        break;
    case 0x18: // Filter Mode / Master Volume
        // Force max volume to mute D418 volume-based digis
        if (isMuted[3]) data |= 0x0f;
        break;
    }

    write(addr, data);
    OS_write(addr, OS_data);

    sidvis(addr, disableEnvelopes, isTriggerWavesEnabled, isNoKinksEnabled);
}

void sidemu::voice(unsigned int voice, bool mute)
{
    if (voice < 4)
        isMuted[voice] = mute;
}

void sidemu::filter(bool enable)
{
    isFilterDisabled = !enable;
}

void sidemu::dontfilter(unsigned int voice, bool enable)
{
    if (voice < 3)
        isNotFiltered[voice] = enable;
}

void sidemu::noenvelopes(bool enable)
{
    isNoEnvelopesEnabled = enable;
}

void sidemu::triggerwaves(bool enable)
{
    isTriggerWavesEnabled = enable;
}

void sidemu::nokinks(bool enable)
{
    isNoKinksEnabled = enable;
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
