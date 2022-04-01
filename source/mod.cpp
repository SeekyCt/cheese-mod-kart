#include "assets.h"
#include "patch.h"
#include "wiimmfi.h"

#include <types.h>
#include <wii/gx.h>
#include <wii/tpl.h>

namespace mod {

// Image formats to leave untouched
u32 fmtMask[] = {
    0x2, // IA4
    0x3, // IA8
    0x5, // RGB5A3
    0x6, // RGBA8
};

// Checks if an image format should be replaced
static bool checkFmt(u32 fmt)
{
    fmt &= 0xf;
    for (u32 i = 0; i < sizeof(fmtMask) / sizeof(fmtMask[0]); i++)
    {
        if (fmtMask[i] == fmt)
            return false;
    }
    return true;
}

// Cheese GXTexObj
wii::gx::GXTexObj texObj;

// Override GXLoadTexObj to use custom cheese texture
static void (*GXLoadTexObjReal)(wii::gx::GXTexObj * obj, u32 id);
static void loadTexObj(wii::gx::GXTexObj * obj, u32 id)
{
    // Check if override is needed
    if (checkFmt(obj->texFmt))
        obj = &texObj;

    // Call original function
    GXLoadTexObjReal(obj, id);
}

void main()
{
    // Init texture
    wii::tpl::TPLBind(&cheeseTpl);
    wii::tpl::TPLGetGXTexObjFromPalette(&cheeseTpl, &texObj, 0);

    // Replace required function
    GXLoadTexObjReal = patch::hookFunction(wii::gx::GXLoadTexObj, loadTexObj);

    // Wiimmfi patch
    wiimmfiPatch();
}

}
