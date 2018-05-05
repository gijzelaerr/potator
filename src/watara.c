
#include "supervision.h"
#include "sound.h"
#include <string.h>
#include "gpu.h"

static M6502 m6502_registers;

byte Loop6502(register M6502 *R) {
    return (INT_QUIT);
}

void supervision_init(void) {
    printf("supervision: init\n");
#ifndef DEBUG
    //iprintf("supervision: init\n");
#endif

    memorymap_init();
    io_init();
    gpu_init();
    timer_init();
    controls_init();
    sound_init();
    interrupts_init();
}

BOOL supervision_load(uint8 *rom, uint32 romSize) {
    //uint32 supervision_programRomSize;
    //uint8 *supervision_programRom = memorymap_rom_load(szPath, &supervision_programRomSize);
#ifdef DEBUG
    //iprintf("supervision: load\n");
#endif

    memorymap_load(rom, romSize);
    supervision_reset();

    return (TRUE);
}


void supervision_reset(void) {
    //printf( "supervision: reset\n");


    memorymap_reset();
    io_reset();
    gpu_reset();
    timer_reset();
    controls_reset();
    sound_reset();
    interrupts_reset();

    Reset6502(&m6502_registers);
}


void supervision_reset_handler(void) {
    //printf( "supervision: reset handler\n");
}


void supervision_done(void) {
    //printf( "supervision: done\n");
    memorymap_done();
    io_done();
    gpu_done();
    timer_done();
    controls_done();
    sound_done();
    interrupts_done();
}


void supervision_set_colour_scheme(int sv_colourScheme) {
    gpu_set_colour_scheme(sv_colourScheme);
}


M6502 *supervision_get6502regs(void) {
    return (&m6502_registers);
}

BOOL supervision_update_input(void) {
    return (controls_update());
}


void supervision_exec(int16 *backbuffer, BOOL bRender) {
    uint32 supervision_scanline, scan1 = 0;

    for (supervision_scanline = 0; supervision_scanline < 160; supervision_scanline++) {
        m6502_registers.ICount = 512;
        timer_exec(m6502_registers.ICount);
#ifdef GP2X
        if(currentConfig.enable_sound) sound_exec(11025/160);
#else
        //sound_exec(22050/160);
#endif
        Run6502(&m6502_registers);
#ifdef NDS
        gpu_render_scanline(supervision_scanline, backbuffer);
        backbuffer += 160+96;
#else
        //gpu_render_scanline(supervision_scanline, backbuffer);
        gpu_render_scanline_fast(scan1, backbuffer);
        backbuffer += 160;
        scan1 += 0x30;
#endif
    }

    if (Rd6502(0x2026) & 0x01)
        Int6502(supervision_get6502regs(), INT_NMI);
}


void supervision_turnSound(BOOL bOn) {
    audio_turnSound(bOn);
}


int sv_loadState(char *statepath, int id) {
    FILE *fp;
    char newPath[256];

    strcpy(newPath, statepath);
    sprintf(newPath + strlen(newPath) - 3, ".s%d", id);

    fp = fopen(newPath, "rb");

    if (fp) {
        fread(&m6502_registers, 1, sizeof(m6502_registers), fp);
        fread(memorymap_programRom, 1, sizeof(memorymap_programRom), fp);
        fread(memorymap_lowerRam, 1, 0x2000, fp);
        fread(memorymap_upperRam, 1, 0x2000, fp);
        fread(memorymap_lowerRomBank, 1, sizeof(memorymap_lowerRomBank), fp);
        fread(memorymap_upperRomBank, 1, sizeof(memorymap_upperRomBank), fp);
        fread(memorymap_regs, 1, 0x2000, fp);
        fclose(fp);
    }



    return (1);
}


int sv_saveState(char *statepath, int id) {
    FILE *fp;
    char newPath[256];

    strcpy(newPath, statepath);
    sprintf(newPath + strlen(newPath) - 3, ".s%d", id);

    fp = fopen(newPath, "wb");

    if (fp) {
        fwrite(&m6502_registers, 1, sizeof(m6502_registers), fp);
        fwrite(memorymap_programRom, 1, sizeof(memorymap_programRom), fp);
        fwrite(memorymap_lowerRam, 1, 0x2000, fp);
        fwrite(memorymap_upperRam, 1, 0x2000, fp);
        fwrite(memorymap_lowerRomBank, 1, sizeof(memorymap_lowerRomBank), fp);
        fwrite(memorymap_upperRomBank, 1, sizeof(memorymap_upperRomBank), fp);
        fwrite(memorymap_regs, 1, 0x2000, fp);
        fflush(fp);
        fclose(fp);
    }
    return (1);
}
