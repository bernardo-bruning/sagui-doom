/* compat.c - runtime freestanding mínimo do Doom no anel 3 do SaguiOS. */
#include <doom_sagui.h>
#include <abi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

int errno;

int sagui_syscall(unsigned n, unsigned a, unsigned b, unsigned c, unsigned d)
{
    int r;
    __asm__ volatile("int $0x80" : "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c), "S"(d) : "memory");
    return r;
}

int sagui_escrever(int fd, const void *buf, unsigned n)
{ return sagui_syscall(SIS_ESCREVER, (unsigned)fd, (unsigned)buf, n, 0); }
int sagui_abrir(const char *p, int m) { return sagui_syscall(SIS_ABRIR, (unsigned)p, (unsigned)m, 0, 0); }
int sagui_fechar(int fd) { return sagui_syscall(SIS_FECHAR, (unsigned)fd, 0, 0, 0); }
int sagui_ler(int fd, void *b, unsigned n) { return sagui_syscall(SIS_LER, (unsigned)fd, (unsigned)b, n, 0); }
int sagui_posicionar(int fd, int d, int o) { return sagui_syscall(SIS_POSICIONAR, (unsigned)fd, (unsigned)d, (unsigned)o, 0); }
int sagui_info(const char *p, void *i) { return sagui_syscall(SIS_INFO_ARQUIVO, (unsigned)p, (unsigned)i, 0, 0); }
unsigned sagui_tempo(void) { return (unsigned)sagui_syscall(SIS_TEMPO, 0, 0, 0, 0); }
int sagui_dormir(unsigned ms) { return sagui_syscall(SIS_DORMIR, ms, 0, 0, 0); }
int sagui_janela(const char *t, int l, int a, uint32_t **px)
{ return sagui_syscall(SIS_CRIAR_JANELA, (unsigned)t, (unsigned)(l | (a << 16)), (unsigned)px, 0); }
int sagui_atualizar(int id) { return sagui_syscall(SIS_ATUALIZAR_JANELA, (unsigned)id, 0, 0, 0); }
int sagui_evento(int id, void *ev) { return sagui_syscall(SIS_PROXIMO_EVENTO, (unsigned)id, (unsigned)ev, 0, 0); }
int sagui_teclas(unsigned char m[32]) { return sagui_syscall(SIS_TECLAS_SEGURADAS, (unsigned)m, 0, 0, 0); }
int sagui_titulo(int id, const char *t) { return sagui_syscall(SIS_TITULO_JANELA, (unsigned)id, (unsigned)t, 0, 0); }
int sagui_fechar_janela(int id) { return sagui_syscall(SIS_FECHAR_JANELA, (unsigned)id, 0, 0, 0); }

typedef struct { unsigned n; } cabecalho_t;
void *malloc(size_t n)
{
    if (!n) n = 1;
    n = (n + 15u) & ~15u;
    unsigned total = (unsigned)n + sizeof(cabecalho_t);
    unsigned char *p = (unsigned char *)sagui_syscall(SIS_AUMENTAR_MEM, total, 0, 0, 0);
    if ((int)p < 0) { errno = ENOMEM; return NULL; }
    ((cabecalho_t *)p)->n = (unsigned)n;
    return p + sizeof(cabecalho_t);
}
void free(void *p) { (void)p; }
void *calloc(size_t n, size_t z)
{
    if (z && n > 0xFFFFFFFFu / z) return NULL;
    size_t bytes = n * z;
    void *p = malloc(bytes);
    if (p) memset(p, 0, bytes);
    return p;
}
void *realloc(void *p, size_t n)
{
    if (!p) return malloc(n);
    if (!n) return NULL;
    cabecalho_t *h = (cabecalho_t *)((unsigned char *)p - sizeof(cabecalho_t));
    void *q = malloc(n);
    if (q) memcpy(q, p, h->n < n ? h->n : n);
    return q;
}

void *memcpy(void *d, const void *s, size_t n) { unsigned char *a=d; const unsigned char *b=s; for(size_t i=0;i<n;i++) a[i]=b[i]; return d; }
void *memmove(void *d, const void *s, size_t n) { unsigned char *a=d; const unsigned char *b=s; if(a<b) for(size_t i=0;i<n;i++) a[i]=b[i]; else if(a>b) while(n) { --n; a[n]=b[n]; } return d; }
void *memset(void *d, int c, size_t n) { unsigned char *p=d; while(n--) *p++=(unsigned char)c; return d; }
int memcmp(const void *a, const void *b, size_t n) { const unsigned char *x=a,*y=b; while(n--) { if(*x!=*y) return *x<*y?-1:1; x++; y++; } return 0; }
size_t strlen(const char *s) { size_t n=0; while(s[n]) n++; return n; }
size_t strnlen(const char *s,size_t n) { size_t i=0; while(i<n&&s[i]) i++; return i; }
char *strcpy(char *d,const char *s) { char *r=d; while((*d++=*s++)); return r; }
char *strncpy(char *d,const char *s,size_t n) { char *r=d; size_t i=0; for(;i<n&&s[i];i++)d[i]=s[i]; for(;i<n;i++)d[i]=0; return r; }
char *strcat(char *d,const char *s) { strcpy(d+strlen(d),s); return d; }
int strcmp(const char *a,const char *b) { while(*a&&*a==*b){a++;b++;} return (unsigned char)*a-(unsigned char)*b; }
int strncmp(const char *a,const char *b,size_t n) { while(n&&*a&&*a==*b){a++;b++;n--;} return n?(unsigned char)*a-(unsigned char)*b:0; }
int strcasecmp(const char *a,const char *b) { while(*a&&tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;} return tolower((unsigned char)*a)-tolower((unsigned char)*b); }
int strncasecmp(const char *a,const char *b,size_t n) { while(n&&*a&&tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;n--;} return n?tolower((unsigned char)*a)-tolower((unsigned char)*b):0; }
char *strchr(const char *s,int c) { do { if(*s==(char)c) return (char *)s; } while(*s++); return NULL; }
char *strrchr(const char *s,int c) { const char *r=NULL; do { if(*s==(char)c)r=s; } while(*s++); return (char *)r; }
char *strstr(const char *s,const char *q) { size_t n=strlen(q); if(!n)return (char *)s; for(;*s;s++)if(!strncmp(s,q,n))return (char *)s; return NULL; }
char *strdup(const char *s) { size_t n=strlen(s)+1; char *p=malloc(n); return p?memcpy(p,s,n):NULL; }
int isspace(int c) { return c==' '||(c>='\t'&&c<='\r'); }
int isdigit(int c) { return c>='0'&&c<='9'; }
int isalpha(int c) { c=tolower(c); return c>='a'&&c<='z'; }
int tolower(int c) { return c>='A'&&c<='Z'?c+('a'-'A'):c; }
int toupper(int c) { return c>='a'&&c<='z'?c-('a'-'A'):c; }
int abs(int n) { return n<0?-n:n; }
double atof(const char *s) { (void)s; return 0.0; }
long atol(const char *s) { return strtol(s,NULL,10); }
int atoi(const char *s) { return (int)atol(s); }
long strtol(const char *s,char **end,int base)
{
    while(isspace((unsigned char)*s))s++;
    int neg=0; if(*s=='+'||*s=='-'){neg=*s=='-';s++;}
    if(!base)base=10;
    if(base==16&&s[0]=='0'&&(s[1]=='x'||s[1]=='X'))s+=2;
    const char *start=s; unsigned long v=0;
    for(;;s++){int d=isdigit((unsigned char)*s)?*s-'0':tolower((unsigned char)*s)-'a'+10; if(d<0||d>=base)break;v=v*base+d;}
    if(end)*end=(char *)(s==start?start:s);
    return neg?-(long)v:(long)v;
}
void qsort(void *base,size_t n,size_t z,int (*cmp)(const void *,const void *))
{
    unsigned char *p=base; if(n<2||!z)return;
    for(size_t i=1;i<n;i++){size_t j=i; while(j&&cmp(p+(j-1)*z,p+j*z)>0){for(size_t k=0;k<z;k++){unsigned char t=p[(j-1)*z+k];p[(j-1)*z+k]=p[j*z+k];p[j*z+k]=t;}j--;}}
}

/* GCC emite estas rotinas para divisão inteira de 64 bits no alvo i386. */
unsigned long long __udivmoddi4(unsigned long long n,unsigned long long d,unsigned long long *rem)
{
    unsigned long long q=0,r=n;
    if(!d){if(rem)*rem=0;return ~0ULL;}
    for(int bit=63;bit>=0;bit--){
        if(d <= (r >> bit)){
            unsigned long long parte=d << bit;
            if(parte<=r){r-=parte;q|=1ULL<<bit;}
        }
    }
    if(rem)*rem=r;
    return q;
}
unsigned long long __udivdi3(unsigned long long a,unsigned long long b)
{ return __udivmoddi4(a,b,NULL); }
unsigned long long __umoddi3(unsigned long long a,unsigned long long b)
{ unsigned long long r; (void)__udivmoddi4(a,b,&r); return r; }
long long __divdi3(long long a,long long b)
{
    int neg=(a<0)^(b<0);
    unsigned long long x=a<0?0ULL-(unsigned long long)a:(unsigned long long)a;
    unsigned long long y=b<0?0ULL-(unsigned long long)b:(unsigned long long)b;
    unsigned long long q=__udivmoddi4(x,y,NULL);
    return neg?-(long long)q:(long long)q;
}
long long __moddi3(long long a,long long b)
{
    int neg=a<0;
    unsigned long long x=a<0?0ULL-(unsigned long long)a:(unsigned long long)a;
    unsigned long long y=b<0?0ULL-(unsigned long long)b:(unsigned long long)b;
    unsigned long long r; (void)__udivmoddi4(x,y,&r);
    return neg?-(long long)r:(long long)r;
}

typedef struct { int fd, flags, eof, err; unsigned pos; } arquivo_t;
static arquivo_t arquivos[8];
static FILE fluxos[3]={{0,1,0,0,0},{1,2,0,0,0},{2,2,0,0,0}};
FILE *stdin=&fluxos[0], *stdout=&fluxos[1], *stderr=&fluxos[2];
FILE *fopen(const char *path,const char *mode)
{
    int flags=ABRIR_LER;
    if(mode&&mode[0]=='w')flags=ABRIR_ESCREVER|ABRIR_CRIAR|ABRIR_TRUNCAR;
    else if(mode&&mode[0]=='a')flags=ABRIR_ESCREVER|ABRIR_CRIAR|ABRIR_ACRESCENTAR;
    int fd=sagui_abrir(path,flags); if(fd<0)return NULL;
    for(unsigned i=0;i<8;i++)if(arquivos[i].fd==0){arquivos[i].fd=fd;arquivos[i].flags=flags;arquivos[i].eof=arquivos[i].err=arquivos[i].pos=0;return (FILE *)&arquivos[i];}
    sagui_fechar(fd); return NULL;
}
int fclose(FILE *f) { if(!f)return EOF; arquivo_t *a=(arquivo_t *)f; int r=sagui_fechar(a->fd); a->fd=0; return r<0?EOF:0; }
size_t fread(void *b,size_t s,size_t n,FILE *f) { if(!f||!s||!n)return 0; int r=sagui_ler(f->fd,b,(unsigned)(s*n)); if(r<0){f->error=1;return 0;} f->pos+=r; if(!r)f->eof=1; return (unsigned)r/s; }
size_t fwrite(const void *b,size_t s,size_t n,FILE *f) { if(!f||!s||!n)return 0; int r=sagui_escrever(f->fd,b,(unsigned)(s*n)); if(r<0){f->error=1;return 0;} f->pos+=r; return (unsigned)r/s; }
int fseek(FILE *f,long o,int w) { if(!f)return -1; int r=sagui_posicionar(f->fd,(int)o,w); if(r<0)return -1; f->pos=(unsigned)r; f->eof=0; return 0; }
long ftell(FILE *f) { return f?(long)f->pos:-1; }
void rewind(FILE *f) { (void)fseek(f,0,SEEK_SET); }
int feof(FILE *f) { return f?f->eof:1; }
int ferror(FILE *f) { return f?f->error:1; }
int fflush(FILE *f) { (void)f; return 0; }
int fileno(FILE *f) { return f?f->fd:-1; }
int read(int fd,void *b,unsigned n) { return sagui_ler(fd,b,n); }
int write(int fd,const void *b,unsigned n) { return sagui_escrever(fd,b,n); }
int close(int fd) { return sagui_fechar(fd); }
int usleep(unsigned u) { return sagui_dormir((u+999)/1000); }
int unlink(const char *p) { return sagui_syscall(SIS_APAGAR,(unsigned)p,0,0,0); }
int remove(const char *p) { return unlink(p); }
int system(const char *command) { (void)command; return -1; }
int rename(const char *a,const char *b) { return sagui_syscall(SIS_RENOMEAR,(unsigned)a,(unsigned)b,0,0); }
int mkdir(const char *p,unsigned m) { (void)m; return sagui_syscall(SIS_CRIAR_PASTA,(unsigned)p,0,0,0); }
int rmdir(const char *p) { return sagui_syscall(SIS_REMOVER_PASTA,(unsigned)p,0,0,0); }
int access(const char *p,int m) { (void)m; info_arquivo_t i; return sagui_info(p,&i)<0?-1:0; }
int isatty(int fd) { return fd>=0&&fd<3; }

static char format_buf[256];
static void out_char(char *out,size_t cap,size_t *n,char c) { if(cap&&*n<cap-1)out[*n]=c; (*n)++; }
static void out_text(char *out,size_t cap,size_t *n,const char *s) { while(*s)out_char(out,cap,n,*s++); }
static void out_num(char *out,size_t cap,size_t *n,unsigned long long v,unsigned base,int neg,int upper,int width,int zero,int precision)
{
    char b[32];int k=0;do{unsigned d=(unsigned)(v%base);b[k++]=(char)(d<10?'0'+d:(upper?'A':'a')+d-10);v/=base;}while(v&&k<(int)sizeof b);
    int zeros=precision>k?precision-k:0;
    int pad=width-k-zeros-neg;
    if(!zero||precision>=0)while(pad-->0)out_char(out,cap,n,' ');
    if(neg)out_char(out,cap,n,'-');
    if(zero&&precision<0)while(pad-->0)out_char(out,cap,n,'0');
    while(zeros-->0)out_char(out,cap,n,'0');
    while(k)out_char(out,cap,n,b[--k]);
}
int vsnprintf(char *out,size_t cap,const char *fmt,va_list ap)
{
    size_t n=0;
    while(*fmt){if(*fmt!='%'){out_char(out,cap,&n,*fmt++);continue;}fmt++;if(*fmt=='%'){out_char(out,cap,&n,*fmt++);continue;}
        int zero=0,width=0,neg=0,upper=0,precision=-1; if(*fmt=='0'){zero=1;fmt++;} while(isdigit((unsigned char)*fmt)){width=width*10+*fmt++-'0';if(width>80)width=80;}
        if(*fmt=='.'){fmt++;precision=0;if(*fmt=='*'){precision=va_arg(ap,int);fmt++;}else while(isdigit((unsigned char)*fmt)){precision=precision*10+*fmt++-'0';if(precision>80)precision=80;}}
        int lng=0;if(*fmt=='l'){lng=1;fmt++;if(*fmt=='l'){lng=2;fmt++;}}else if(*fmt=='z'||*fmt=='h')fmt++;
        char c=*fmt?*fmt++:0;
        if(c=='s'){const char *s=va_arg(ap,const char *);if(!s)s="(null)";if(precision<0)out_text(out,cap,&n,s);else for(int i=0;s[i]&&i<precision;i++)out_char(out,cap,&n,s[i]);}
        else if(c=='c')out_char(out,cap,&n,(char)va_arg(ap,int));
        else if(c=='d'||c=='i'){long long v=lng==2?va_arg(ap,long long):(lng?va_arg(ap,long):va_arg(ap,int));if(v<0){neg=1;v=-v;}out_num(out,cap,&n,(unsigned long long)v,10,neg,0,width,zero,precision);}
        else if(c=='u'||c=='x'||c=='X'||c=='o'){unsigned long long v=lng==2?va_arg(ap,unsigned long long):(lng?va_arg(ap,unsigned long):va_arg(ap,unsigned));upper=c=='X';out_num(out,cap,&n,v,c=='x'||c=='X'?16:c=='o'?8:10,0,upper,width,zero,precision);}
        else if(c=='p'){out_text(out,cap,&n,"0x");out_num(out,cap,&n,(uintptr_t)va_arg(ap,void *),16,0,0,width,1,precision);}
        else if(c=='f'||c=='g'||c=='e'){(void)va_arg(ap,double);out_text(out,cap,&n,"0.0");}
        else {out_char(out,cap,&n,'%');if(c)out_char(out,cap,&n,c);}
    }
    if(cap)out[n<cap?n:cap-1]=0;
    return (int)n;
}
int snprintf(char *o,size_t n,const char *f,...){va_list a;va_start(a,f);int r=vsnprintf(o,n,f,a);va_end(a);return r;}
int sprintf(char *o,const char *f,...){va_list a;va_start(a,f);int r=vsnprintf(o,0xFFFFFFFFu,f,a);va_end(a);return r;}
int vfprintf(FILE *f,const char *fmt,va_list a){vsnprintf(format_buf,sizeof format_buf,fmt,a);int n=(int)strlen(format_buf);return sagui_escrever(f?f->fd:2,format_buf,(unsigned)n);}
int fprintf(FILE *f,const char *fmt,...){va_list a;va_start(a,fmt);int r=vfprintf(f,fmt,a);va_end(a);return r;}
int printf(const char *fmt,...){va_list a;va_start(a,fmt);int r=vfprintf(stdout,fmt,a);va_end(a);return r;}
int puts(const char *s){int r=sagui_escrever(1,s,(unsigned)strlen(s));char nl='\n';sagui_escrever(1,&nl,1);return r<0?r:r+1;}
int fputs(const char *s,FILE *f){return sagui_escrever(f?f->fd:2,s,(unsigned)strlen(s));}
int putchar(int c){char ch=(char)c;return sagui_escrever(1,&ch,1)<0?EOF:(unsigned char)ch;}
int sscanf(const char *s,const char *f,...){(void)s;(void)f;return 0;}
void exit(int c){sagui_syscall(SIS_SAIR,(unsigned)c,0,0,0);for(;;)__asm__ volatile("hlt");}
void sair(int c){exit(c);}
void abort(void){exit(127);}
int atexit(void (*fn)(void)){(void)fn;return 0;}
int stat(const char *p,struct stat *s){info_arquivo_t i;if(sagui_info(p,&i)<0||!s)return -1;s->st_size=i.tamanho;s->st_mode=i.tipo==TIPO_PASTA?S_IFDIR:S_IFREG;return 0;}
