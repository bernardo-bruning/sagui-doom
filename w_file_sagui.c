/* Backend de WAD: leitura posicional pelo sistema de arquivos do SaguiOS. */
#include <doom_sagui.h>
#include <abi.h>
#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

extern wad_file_class_t stdc_wad_file;

typedef struct { wad_file_t wad; int fd; } sagui_wad_t;

static wad_file_t *abrir_wad(char *path)
{
    int fd = sagui_abrir(path, ABRIR_LER);
    if (fd < 0) return NULL;
    info_arquivo_t info;
    if (sagui_info(path, &info) < 0 || info.tamanho > 0xFFFFFFFFu) {
        sagui_fechar(fd);
        return NULL;
    }
    sagui_wad_t *f = Z_Malloc(sizeof(*f), PU_STATIC, NULL);
    f->wad.file_class = &stdc_wad_file;
    f->wad.mapped = NULL;
    f->wad.length = info.tamanho;
    f->fd = fd;
    return &f->wad;
}

static void fechar_wad(wad_file_t *wad)
{
    sagui_wad_t *f=(sagui_wad_t *)wad;
    sagui_fechar(f->fd);
    Z_Free(f);
}

static size_t ler_wad(wad_file_t *wad,unsigned offset,void *buf,size_t n)
{
    sagui_wad_t *f=(sagui_wad_t *)wad;
    int r=sagui_posicionar(f->fd,(int)offset,POS_INICIO);
    if(r<0)return 0;
    r=sagui_ler(f->fd,buf,(unsigned)n);
    return r<0?0:(size_t)r;
}

wad_file_class_t stdc_wad_file={abrir_wad,fechar_wad,ler_wad};
