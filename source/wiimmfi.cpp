#include "patch.h"

#include <types.h>
#include <wii/string.h>

/*
    This file was originally reverse engineered from code in LE-Code that was written by Leseratte/Wiimm.
    It's also went through several versions across different mods and patching systems, so contains some
    edits from myself, TheLordScruffy, and CLF78 at various points.
*/

extern "C" {

extern char Patch_LoginPrintHeader[], Patch_WiimmfiURLs[];
extern const char* Patch_AuthserverHosts[3];
extern void Patch_WiimmfiHook1(), Patch_WiimmfiHook2(), Patch_AuthServerOverride(), Patch_HostHeader();

extern s32 DWC_AuthServer, SSL_Initialised;
void NETSHA1Init();
void NETSHA1Update();
void NETSHA1GetDigest();

void wiimmfiAsm1(), wiimmfiAsm2();

}

u32 expectedHash[] = {0x0FFF1F07, 0x00E638C9, 0x49FBEFFA, 0x79022D3A, 0x84AB134F};

__asm__(
".global wiimmfiAsm1\n"
"wiimmfiAsm1:\n"
    // Original instruction
    "cmpwi 3, 0\n"
    
    // Return workaround
    "mflr 23\n"

    "ble w1_end\n"

    // r13 replacements
    "lis 11, DWC_AuthServer@ha\n"
    "lis 12, SSL_Initialised@ha\n"

    "lwz 3, 0xC(1)\n"
    "lwz 0, DWC_AuthServer@l (11)\n"
    "cmpwi 0, 2\n"
    "beq w1_cont\n"

    "stw 3, SSL_Initialised@l (12)\n"
    "li 0, 2\n"
    "stw 0, DWC_AuthServer@l (11)\n"
    "b w1_end\n"

    // Execute payload
"w1_cont:\n"
    "addi 4, 3, 3\n"
    "rlwinm 4, 4, 0, 0, 29\n"
    "lbz 5, 0x0(4)\n"
    "add 5, 4, 5\n"
    "dcbf 0, 5\n"
    "mtlr 5\n"
    "blrl\n"

    // Original instruction
"w1_end:\n"
    "li 3, -1\n"
    "cmpwi 3, 0\n"

    // Return workaround
    "mtlr 23\n"
    "li 23, 0\n"
    "blr\n"
);

__asm__(
".global wiimmfiAsm2\n"
"wiimmfiAsm2:\n"
    // Return workaround
	"stwu 1, -8 (1)\n"
	"mflr 3\n"
	"stw 3, 4 (1)\n"

    "lis 12, SSL_Initialised@ha\n"

    // Check if inited
    "lwz 4, SSL_Initialised@l (12)\n"
    "cmplwi 4, 1\n"
    "ble w2_nomatch\n"

    // Push stack
    "stwu 1, -0x80(1)\n"

    // Call NETSHA1Init
    "addi 3, 1, 0x20\n"
    "bl NETSHA1Init\n"

    // Call NETSHA1Update
    "addi 3, 1, 0x20\n"
    "lis 12, SSL_Initialised@ha\n"
    "lwz 4, SSL_Initialised@l (12)\n"
    "li 5, 0x554\n"
    "stw 5, 0xC4(28)\n"
    "bl NETSHA1Update\n"

    // Call NETSHA1GetDigest
    "addi 3, 1, 0x20\n"
    "addi 4, 1, 0x10\n"
    "bl NETSHA1GetDigest\n"

    // Setup loop
    "lis 3, (expectedHash-4)@h\n"
    "ori 3, 3, (expectedHash-4)@l\n"
    "addi 4, 1, 0xC\n"
    "li 5, 5\n"
    "mtctr 5\n"

    // Loop it!
"w2_loop:\n"
    "lwzu 5, 0x4(3)\n"
    "lwzu 6, 0x4(4)\n"
    "cmpw 6, 5\n"
    "bne w2_exit\n"
    "bdnz+ w2_loop\n"

    // Check if we found a match and pop the stack
"w2_exit:\n"
    "cmpw 6, 5\n"
    "addi 1, 1, 0x80\n"
    "lis 12, SSL_Initialised@ha\n"
    "lwz 4, SSL_Initialised@l (12)\n"
    "beq w2_end\n"

    // Return 0 otherwise
"w2_nomatch:\n"
    "li 4, 0\n"

"w2_end:\n"
    // Return workaround
	"lwz 3, 4 (1)\n"
	"mtlr 3\n"
	"addi 1, 1, 8\n"
	"blr\n"
);

static void patchURL(u32 offset, const char* string)
{
    wii::string::strcpy(&Patch_WiimmfiURLs[offset], string);
}

void wiimmfiPatch()
{
    wii::string::strcpy(Patch_LoginPrintHeader, "CHEESE"); // set patcher name

    Patch_AuthserverHosts[0] = "http://ca.nas.wiimmfi.de/ca";
    Patch_AuthserverHosts[1] = "http://naswii.wiimmfi.de/ac";

#if defined MKW_EU
    Patch_AuthserverHosts[2] = "http://main.nas.wiimmfi.de/pp";
#elif defined MKW_US
    Patch_AuthserverHosts[2] = "http://main.nas.wiimmfi.de/pe";
#elif defined MKW_JP
    Patch_AuthserverHosts[2] = "http://main.nas.wiimmfi.de/pj";
#elif defined MKW_KR
    Patch_AuthserverHosts[2] = "http://main.nas.wiimmfi.de/pk";
#endif

    patchURL(0xA8, "://naswii.wiimmfi.de/pr");
    patchURL(0x964, "wiimmfi.de"); // Available
    patchURL(0x10D4, "wiimmfi.de"); // GPCM
    patchURL(0x1AEC, "wiimmfi.de"); // GPSP
    patchURL(0x2C8D, "wiimmfi.de"); // Master
    patchURL(0x38A7, "wiimmfi.de"); // Natneg
    patchURL(0x38C3, "wiimmfi.de");
    patchURL(0x38DF, "wiimmfi.de");
    patchURL(0x3A2F, "wiimmfi.de"); // MS
    patchURL(0x3AB3, "wiimmfi.de"); // SAKE

    writeBranchLink(Patch_WiimmfiHook1, 0, wiimmfiAsm1);
    writeBranchLink(Patch_WiimmfiHook2, 0, wiimmfiAsm2);

    writeWord(&Patch_AuthServerOverride, 0, 0x3bc00000); // force DWC_AUTHSERVER_DEBUG
    writeWord(&Patch_HostHeader, 0, 0x60000000); // nop host header
}