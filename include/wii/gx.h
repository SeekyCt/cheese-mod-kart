#pragma once

#include <types.h>

namespace wii::gx {

struct GXTexObj
{
    u8 unknown_0x0[0x14 - 0x0];
    u32 texFmt;
    u8 unknown_0x18[0x20 - 0x18];
};
static_assert(sizeof(GXTexObj) == 0x20);

extern "C" {

void GXLoadTexObj(GXTexObj * obj, u32 id);

}

}
