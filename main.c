/* Entrada do programa Doom do SaguiOS. */
#include <doom_sagui.h>
#include <abi.h>
#include <stdio.h>
#include "doomgeneric.h"

extern int doom_sagui_fechada(void);
extern void doom_sagui_fechar(void);
void D_DoomMain(void);

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    static char nome[]="doom", iwad[]="-iwad", caminho[]="/doom1.wad", som[]="-nosound";
    char *args[]={nome,iwad,caminho,som,NULL};
    doomgeneric_Create(4,args);
    while (!doom_sagui_fechada()) {
        doomgeneric_Tick();
        DG_SleepMs(1);
    }
    doom_sagui_fechar();
    return 0;
}
