/* Entrada por transições de teclas seguradas entregues pelo ABI do SaguiOS. */
#include "doomtype.h"
#include "d_event.h"
#include "doomgeneric.h"

int vanilla_keyboard_mapping = 1;

void I_InitInput(void) { }

void I_GetEvent(void)
{
    int pressed;
    unsigned char key;
    event_t ev;
    while (DG_GetKey(&pressed, &key)) {
        ev.type = pressed ? ev_keydown : ev_keyup;
        ev.data1 = key;
        ev.data2 = ev.data3 = 0;
        D_PostEvent(&ev);
    }
}
