#ifndef DOOM_SAGUI_H
#define DOOM_SAGUI_H
#include <stdint.h>
#include <stddef.h>

int sagui_syscall(unsigned n, unsigned a, unsigned b, unsigned c, unsigned d);
int sagui_escrever(int fd, const void *buf, unsigned n);
int sagui_abrir(const char *path, int mode);
int sagui_fechar(int fd);
int sagui_ler(int fd, void *buf, unsigned n);
int sagui_posicionar(int fd, int offset, int origin);
int sagui_info(const char *path, void *info);
unsigned sagui_tempo(void);
int sagui_dormir(unsigned ms);
int sagui_janela(const char *title, int width, int height, uint32_t **pixels);
int sagui_atualizar(int id);
int sagui_evento(int id, void *event);
int sagui_teclas(unsigned char map[32]);
int sagui_titulo(int id, const char *title);
int sagui_fechar_janela(int id);

void doom_sagui_registrar(const char *s);
#endif
