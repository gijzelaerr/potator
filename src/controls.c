
#include "controls.h"
#include <SDL/SDL.h>

uint8 controls_state;


void controls_init() {
    printf( "controls: init\n");
}

void controls_done() {
    printf( "controls: done\n");
}

void controls_reset() {
    printf( "controls: reset\n");
    controls_state = 0x00;
}

void controls_write(uint32 addr, uint8 data) {
}

void controls_state_write(uint8 type, uint8 data) {
    if (controls_state == data)
        return;
    else
        controls_state = 0;

    if (type)
        controls_state |= data;
    else
        controls_state = data;
}


uint8 controls_read(uint32 addr) {
    return (controls_state ^ 0xff);
}

BOOL controls_update(void) {
    controls_state = 0;


/*
	uint8 *keystate = SDL_GetKeyState(NULL);

	if (keystate[SDLK_RIGHT])	controls_state|=0x01;
	if (keystate[SDLK_LEFT])	controls_state|=0x02;
	if (keystate[SDLK_DOWN])	controls_state|=0x04;
	if (keystate[SDLK_UP])		controls_state|=0x08;
	if (keystate[SDLK_x])		controls_state|=0x10;
	if (keystate[SDLK_c])		controls_state|=0x20;
	if (keystate[SDLK_F1])		controls_state|=0x80;
	if (keystate[SDLK_F2])		controls_state|=0x40;
*/

    SDL_Event event;

    // Check for events
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                    case SDLK_RIGHT:
                        controls_state|=0x01;
                        break;
                    case SDLK_LEFT:
                        controls_state|=0x02;
                        break;
                    case SDLK_DOWN:
                        controls_state|=0x04;
                        break;
                    case SDLK_UP:
                        controls_state|=0x08;
                        break;
                    case SDLK_x:
                        controls_state|=0x10;
                        break;
                    case SDLK_c:
                        controls_state|=0x20;
                        break;
                    case SDLK_F1:
                        controls_state|=0x40;
                        break;
                    case SDLK_F2:
                        controls_state|=0x80;
                        break;
                    default:
                        break;
                }
            break;
        }
    }

    return (TRUE);
}
