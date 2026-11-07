/* Camada de tela, relógio e entrada do Doomgeneric para o SaguiOS. */
#include <doom_sagui.h>
#include <abi.h>
#include <doomgeneric.h>
#include <doomkeys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TELA_LARG 640
#define TELA_ALT 400
#define FILA 64

static int janela = -1;
static uint32_t *pixels;
static unsigned short fila[FILA];
static unsigned ini, fim;
static unsigned char mapa_antigo[32];
static unsigned inicio_ms;
static int fechada;

static int apertada(const unsigned char *m, unsigned cv)
{ return (m[cv >> 3] >> (cv & 7)) & 1; }

static unsigned char doom_key(unsigned cv)
{
    if (cv & 0x80) {
        switch (cv & 0x7f) {
        case 0x48: return KEY_UPARROW;
        case 0x50: return KEY_DOWNARROW;
        case 0x4b: return KEY_LEFTARROW;
        case 0x4d: return KEY_RIGHTARROW;
        case 0x47: return KEY_HOME;
        case 0x4f: return KEY_END;
        case 0x49: return KEY_PGUP;
        case 0x51: return KEY_PGDN;
        case 0x52: return KEY_INS;
        case 0x53: return KEY_DEL;
        case 0x1d: return KEY_RCTRL;
        case 0x38: return KEY_RALT;
        default: return 0;
        }
    }
    if (cv == 0x1c) return KEY_ENTER;
    if (cv == 0x01) return KEY_ESCAPE;
    if (cv == 0x0f) return KEY_TAB;
    if (cv == 0x0e) return KEY_BACKSPACE;
    if (cv == 0x1d) return KEY_FIRE;
    if (cv == 0x2a || cv == 0x36) return KEY_RSHIFT;
    if (cv == 0x38) return KEY_RALT;
    if (cv == 0x39) return KEY_USE;
    if (cv >= 0x02 && cv <= 0x0b) return cv == 0x0b ? '0' : (unsigned char)('1' + cv - 0x02);
    if (cv >= 0x10 && cv <= 0x19) { static const char r[]="qwertyuiop"; return r[cv-0x10]; }
    if (cv >= 0x1e && cv <= 0x26) { static const char r[]="asdfghjkl"; return r[cv-0x1e]; }
    if (cv >= 0x2c && cv <= 0x32) { static const char r[]="zxcvbnm"; return r[cv-0x2c]; }
    switch (cv) {
    case 0x0c: return '-'; case 0x0d: return '='; case 0x1a: return '[';
    case 0x1b: return ']'; case 0x27: return ';'; case 0x28: return '\'';
    case 0x29: return '`'; case 0x2b: return '\\'; case 0x33: return ',';
    case 0x34: return '.'; case 0x35: return '/'; case 0x37: return '*';
    case 0x3b: return KEY_F1; case 0x3c: return KEY_F2; case 0x3d: return KEY_F3;
    case 0x3e: return KEY_F4; case 0x3f: return KEY_F5; case 0x40: return KEY_F6;
    case 0x41: return KEY_F7; case 0x42: return KEY_F8; case 0x43: return KEY_F9;
    case 0x44: return KEY_F10; case 0x45: return KEY_NUMLOCK;
    default: return 0;
    }
}

static void enfileirar(int down, unsigned char key)
{
    if (!key) return;
    unsigned prox = (fim + 1) % FILA;
    if (prox == ini) return;
    fila[fim] = (unsigned short)((down ? 0x100 : 0) | key);
    fim = prox;
}

void DG_Init(void)
{
    janela = sagui_janela("Doom", TELA_LARG, TELA_ALT, &pixels);
    if (janela < 0 || !pixels) {
        janela = -1;
        fputs("Doom: nao foi possivel criar a janela.\n", stderr);
        exit(1);
    }
    info_arquivo_t wad;
    if (sagui_info("/doom1.wad", &wad) < 0 || wad.tipo != TIPO_ARQUIVO) {
        fputs("Doom: /doom1.wad ausente no ramdisk.\n", stderr);
        exit(1);
    }
    memset(pixels, 0, TELA_LARG * TELA_ALT * sizeof(uint32_t));
    sagui_atualizar(janela);
    inicio_ms = sagui_tempo();
}

void DG_DrawFrame(void)
{
    if (janela < 0 || fechada) return;
    evento_t ev;
    while (sagui_evento(janela, &ev) > 0)
        if (ev.tipo == EV_FECHAR) fechada = 1;
    if (!fechada && pixels && DG_ScreenBuffer) {
        memcpy(pixels, DG_ScreenBuffer, TELA_LARG * TELA_ALT * sizeof(uint32_t));
        sagui_atualizar(janela);
    }
}

void DG_SleepMs(uint32_t ms) { if (ms) (void)sagui_dormir(ms); }
uint32_t DG_GetTicksMs(void) { return sagui_tempo() - inicio_ms; }

int DG_GetKey(int *pressed, unsigned char *key)
{
    unsigned char atual[32];
    (void)sagui_teclas(atual);
    for (unsigned cv = 0; cv < 256; cv++) {
        int a = apertada(atual, cv), b = apertada(mapa_antigo, cv);
        if (a != b) {
            unsigned char k = doom_key(cv);
            enfileirar(a, k);
        }
    }
    memcpy(mapa_antigo, atual, sizeof mapa_antigo);
    if (ini == fim) return 0;
    unsigned short v = fila[ini];
    ini = (ini + 1) % FILA;
    *pressed = !!(v & 0x100);
    *key = (unsigned char)v;
    return 1;
}

void DG_SetWindowTitle(const char *title)
{ if (janela >= 0 && title) (void)sagui_titulo(janela, title); }

int doom_sagui_fechada(void) { return fechada; }
void doom_sagui_fechar(void) { if (janela >= 0) (void)sagui_fechar_janela(janela); }
