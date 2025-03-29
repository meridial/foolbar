#define _POSIX_C_SOURCE 199309L

#include "stddef.h"
#include "stdio.h"
#include "fcntl.h"
#include "wayland-client-protocol.h"
#include "unistd.h"
#include "sys/mman.h"
#include "syscall.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "time.h"
// wayland-scanner generated!
#include "wlr-layer-shell-unstable-v1.h"
// these are for my widgets and shi. you wont see them in the wayland code
#include "sys/utsname.h"
#include "sys/sysinfo.h"
#include "pthread.h"
// im too lazy to allow the wm to set this automatically. pls set urself :3
static const unsigned int width = 1024;
static const unsigned int height = 10;


// took me a night to debug ts. forgor the paren
#define PIXEL(x,y) (((y) * width) + (x))
// convert from 32bit ARGB to packed 64-bit ARGBARGB
#define PACKEDCOLOR64(c) ((uint64_t)(c) << 32 | (uint64_t)(c))
// we must be inclusive
#define PACKEDCOLOUR64(c) ((uint64_t)(c) << 32 | (uint64_t)(c))
// note: the only supported formats are anything 4 bytes
// the most common on-mem format is BRGA8888. which im using
// but when using literals. you should write in ARGB (little endian)
// RED only with full alpha: 0xffff0000
// BLUE only with half alpha: 0x7f0000ff
// if you are using big endian. idfk. figure it out or smth
// also: translate from hex colors yourself :3

// required to be given by compositor:
struct wl_compositor *compositor;
struct wl_shm *shm;
struct wl_surface* surface;
struct wl_output *output;
struct zwlr_layer_shell_v1 *layer_shell;
struct zwlr_layer_surface_v1 *layer_surface;

static const size_t len = width * height;
static const size_t map_size = len * 4;
static const unsigned int stride = width * 4;

// changing this wont do anything (except increase font space)
static const unsigned int font_size = 8;
static void shit_pants(){}

// top 10 communication methods
bool configured = false;
bool should_continue = true;

static void zwlr_close(void *v, struct zwlr_layer_surface_v1* sf){
  should_continue = false;
}

static void zwlr_config(void*v, struct zwlr_layer_surface_v1* sf, uint32_t ser, uint32_t w, uint32_t h){
  zwlr_layer_surface_v1_ack_configure(sf, ser);
  configured = true;
  wl_surface_commit(surface);
}

static const struct zwlr_layer_surface_v1_listener zwlr_lisn = {
  .closed = zwlr_close,
  .configure = zwlr_config,
};

void add_handler(
  void* v,
  struct wl_registry* registry,
  uint32_t nawa,
  const char* iface,
  uint32_t ver
){
    // yandere dev pro tip:
    //printf("iface: %s :: ver: %u :: nawa :: %u\n", iface, ver, nawa);
    if (!strcmp(iface, "wl_compositor")) {
        compositor = wl_registry_bind(registry, nawa,
            &wl_compositor_interface, 3);
    } else if (!strcmp(iface, "wl_shm")) {
        shm = wl_registry_bind(registry, nawa,
            &wl_shm_interface, 1);
    } else if (!strcmp(iface, zwlr_layer_shell_v1_interface.name)){
        layer_shell = wl_registry_bind(registry, nawa, &zwlr_layer_shell_v1_interface, 3);
    }
}

void rm_handler(
  void *v,
  struct wl_registry* registry,
  uint32_t nawa
){
    printf("remove: %u\n", nawa);
}  

static const struct wl_registry_listener rlisn = {
    .global = add_handler,
    .global_remove = rm_handler,
};


static int shm_alloc(){
  // no retries needed! (probably)
  char nawa[] = "if_you-use-This-filename_For_some_dumbass_fucking_reason_i-will-scatter-your-_retarded_fucking-ass-across-the_fucking_stars___foolbar__shmfile";
  int fd = shm_open(nawa, O_RDWR | O_CREAT | O_EXCL, 0600);
  if (fd < 0)
    return fd; 
  if(ftruncate(fd, map_size) < 0){
    return -1;
    close(fd);
  }
  shm_unlink(nawa);
  return fd;
}

void* map;

// find a way to use +=8. it wont work for some fucking reason
void fill(uint64_t koloro){
  for(size_t i = 0; i < (len/2); i++){
    *((uint64_t*)map+i) = koloro;
  }
}

// note. start drawing right->left ->up
// reference font is from TempleOS (commodore petscii) (with some changes)
static const uint64_t char_map[] = {
  [0x0] = 0b0000000100000001000000010000000100000001000000010000000100000001, // guideline
  ['!'] = 0b0001100000000000000110000001100000111100001111000011110000011000,
  ['"'] = 0b0000000000000000000000000000000001100110011001100110011001100110,
  ['#'] = 0b0110011001100110111111111111111101100110111111101111111101100110,
  ['$'] = 0b0000000000011000011111101101000001111100000101101111110000011000,
  ['%'] = 0b0000000001100000011001100000110000011000001100000110011000000110,
  ['&'] = 0b0000000011011100011001101011011000011100001101100011011000011100,
 ['\''] = 0b0000000000000000000000000000000000011000000110000001100000011000,
  ['('] = 0b0011000000011000000011000000110000001100000011000001100000110000,
  [')'] = 0b0000110000011000001100000011000000110000001100000001100000001100,
  ['*'] = 0b0000000000000000000110000111111000111100011111100001100000000000,
  ['+'] = 0b0000000000011000000110000111111001111110000110000001100000000000,
  [','] = 0b0000110000011000000110000000000000000000000000000000000000000000,
  ['-'] = 0b0000000000000000000000001111111111111111000000000000000000000000,
  ['.'] = 0b0001100000011000000000000000000000000000000000000000000000000000,
  ['/'] = 0b0000000000000110000011000001100000110000011000001100000000000000,
  //        0000000100000001000000010000000100000001000000010000000100000001
  ['0'] = 0b0000000000111100011001100110111001111110011101100110011000111100,
  ['1'] = 0b0000000001111110000110000001100000011000000110000001110000011000,
  ['2'] = 0b0000000001111110000011000001100000110000011000000110011000111100,
  ['3'] = 0b0000000000111100011001100110000000111000011000000110011000111100,
  ['4'] = 0b0000000000110000001100000111111000110110001111000011100000110000,
  ['5'] = 0b0000000000111100011001100110000001100000001111100000011001111110,
  ['6'] = 0b0000000000111100011001100110011000111110000001100000110000111000,
  ['7'] = 0b0000000000001100000011000000110000011000001100000110000001111110,
  ['8'] = 0b0000000000111100011001100110011000111100011001100110011000111100,
  ['9'] = 0b0000000000011100001100000110000001111100011001100110011000111100,
  [':'] = 0b0000000000011000000110000000000000011000000110000000000000000000,
  [';'] = 0b0000110000011000000110000000000000011000000110000000000000000000,
  ['<'] = 0b0000000000110000000110000000110000000110000011000001100000110000,
  ['='] = 0b0000000000000000000000000111111000000000011111100000000000000000,
  ['>'] = 0b0000000000001100000110000011000001100000001100000001100000001100,
  ['?'] = 0b0000000000011000000000000001100000011000001100000110011000111100,
  ['@'] = 0b0000000000111100000001100111011001010110011101100110011000111100, // haha funny snake
  //        0000000100000001000000010000000100000001000000010000000100000001
  ['A'] = 0b0000000001100110011001100110011001111110011001100110011000111100,
  ['B'] = 0b0000000000111110011001100110011000111110011001100110011000111110,
  ['C'] = 0b0000000000111100011001100000011000000110000001100110011000111100,
  ['D'] = 0b0000000000011110001101100110011001100110011001100011011000011110,
  ['E'] = 0b0000000001111110000001100000011000111110000001100000011001111110,
  ['F'] = 0b0000000000000110000001100000011000111110000001100000011001111110,
  ['G'] = 0b0000000000111100011001100110011001110110000001100110011000111100,
  ['H'] = 0b0000000001100110011001100110011001111110011001100110011001100110,
  ['I'] = 0b0000000001111110000110000001100000011000000110000001100001111110,
  ['J'] = 0b0000000000011100001101100011000000110000001100000011000001111100,
  ['K'] = 0b0000000001100110001101100001111000001110000111100011011001100110,
  ['L'] = 0b0000000001111110000001100000011000000110000001100000011000000110,
  ['M'] = 0b0000000011000110110001101101011011010110111111101110111011000110,
  ['N'] = 0b0000000001100110011001100111011001111110011011100110011001100110,
  ['O'] = 0b0000000000111100011001100110011001100110011001100110011000111100,
  ['P'] = 0b0000000000000110000001100000011000111110011001100110011000111110,
  ['Q'] = 0b0000000001101100001101100101011001100110011001100110011000111100,
  ['R'] = 0b0000000001100110011001100011011000111110011001100110011000111110,
  ['S'] = 0b0000000000111100011001100110000000111100000001100110011000111100,
  //        0000000100000001000000010000000100000001000000010000000100000001
  ['T'] = 0b0000000000011000000110000001100000011000000110000001100001111110,
  ['U'] = 0b0000000000111100011001100110011001100110011001100110011001100110,
  ['V'] = 0b0000000000011000001111000110011001100110011001100110011001100110,
  ['W'] = 0b0000000011000110111011101111111011010110110101101100011011000110,
  ['X'] = 0b0000000001100110011001100011110000011000001111000110011001100110,
  ['Y'] = 0b0000000000011000000110000001100000111100011001100110011001100110,
  ['Z'] = 0b0000000001111110000001100000110000011000001100000110000001111110,
  //        0000000100000001000000010000000100000001000000010000000100000001
  ['['] = 0b0000000000111110000001100000011000000110000001100000011001111110,
 ['\\'] = 0b0000000001100000001100000001100000001100000001100000001100000000,
  [']'] = 0b0000000001111100011000000110000001100000011000000110000001111110,
  ['^'] = 0b0000000000000000000000000000000000000000010000100010010000011000,
  ['_'] = 0b1111111100000000000000000000000000000000000000000000000000000000,
  ['`'] = 0b0000000000000000000000000000000000000000001100000001100000001100,
  ['a'] = 0b0000000001111100011001100111110001100000001111000000000000000000,
  ['b'] = 0b0000000000111110011001100110011001100110001111100000011000000110,
  ['c'] = 0b0000000000111100011001100000011001100110001111000000000000000000,
  ['d'] = 0b0000000001111100011001100110011001100110011111000110000001100000,
  ['e'] = 0b0000000000111100000001100111111001100110001111000000000000000000,
  ['f'] = 0b0000000000001100000011000000110000111110000011000000110000111000,
  ['g'] = 0b0011110001100000011111000110011001100110011111000000000000000000,
  ['p'] = 0b0000011000000110001111100110011001100110001111100000000000000000,
  ['h'] = 0b0000000001100110011001100110011001100110001111100000011000000110,
  ['i'] = 0b0000000000111100000110000001100000011000000111000000000000011000,
  ['j'] = 0b0000111000011000000110000001100000011000000111000000000000011000,
  ['k'] = 0b0000000001100110001101100001111000110110011001100000011000000110,
  ['l'] = 0b0000000000111100000110000001100000011000000110000001100000011100,
  ['m'] = 0b0000000011000110110101101101011011111110011011000000000000000000,
  ['n'] = 0b0000000001100110011001100110011001100110001111100000000000000000,
  ['o'] = 0b0000000000111100011001100110011001100110001111000000000000000000,
  ['q'] = 0b1110000001100000011111000110011001100110011111000000000000000000,
  ['r'] = 0b0000000000000110000001100000011001101110001101100000000000000000,
  //        0000000100000001000000010000000100000001000000010000000100000001
  ['s'] = 0b0000000000111110011000000011110000000110011111000000000000000000,
  ['t'] = 0b0000000000111000000011000000110000001100001111100000110000001100,
  ['u'] = 0b0000000001111100011001100110011001100110011001100000000000000000,
  ['v'] = 0b0000000000011000001111000110011001100110011001100000000000000000,
  ['w'] = 0b0000000001101100111111101101011011010110110001100000000000000000,
  ['x'] = 0b0000000001100110001111000001100000111100011001100000000000000000,
  ['z'] = 0b0000000001111110000011000001100000110000011111100000000000000000,
  ['{'] = 0b0000000000110000000110000001100000001110000110000001100000110000,
  ['|'] = 0b0000000000011000000110000001100000011000000110000001100000011000,
  ['}'] = 0b0000000000001100000110000001100001110000000110000001100000001100,
  ['~'] = 0b0000000000000000000000000000000000000000011000101101011010001100,
  [0xff] = 0,
};


static inline void fill_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint64_t koloro){
  unsigned int nx = w*h;
  for(unsigned int i = 0; i < nx; i+=2){
    ldiv_t d = ldiv(i, w);
    *(uint64_t*)(map+PIXEL((x+d.rem), y+d.quot)*4) = koloro;
  }
}

static inline void paint_char(uint64_t c, unsigned int x, unsigned int y, uint32_t koloro){
  for(size_t i = 0; i < 64; i++){
    if((c >> i) & 1){
      ldiv_t d = ldiv(i, 8);
      *(uint32_t*)(map+PIXEL(x+d.rem, y+d.quot)*4) = koloro;
    }
  }
}

// slow as FUCK
// literal scale. size is 8 * scale
// did i mention that its so fucking slow?
static inline void paint_char_scale(uint64_t c, unsigned int x, unsigned int y, uint64_t koloro, unsigned int scale){
  for(size_t i = 0; i < 64; i++){
    if((c >> i) & 1){
      ldiv_t d = ldiv(i, 8);
      fill_rect(x+d.rem*scale, y+d.quot*scale, scale, scale, koloro);
    }
  }
}

static inline void paint_str(const char* str, size_t len, unsigned int x, unsigned int y, uint32_t koloro){
  for(unsigned int i = 0; i < len; i++){
    paint_char(*(char_map+*(str+i)), x+(i*font_size), y, koloro);
  }
}


static inline void paint_str_scale(const char* str, size_t len, unsigned int x, unsigned int y, uint64_t koloro, unsigned int scale){
  for(unsigned int i = 0; i < len; i++){
    paint_char_scale(*(char_map+*(str+i)), x+((i*font_size)*scale), y, koloro, scale);
  }
}

// the following functions are solely for widgets
// the following code is also extremely unsound. hope u dont mind some segfaults as a treat :3


static const uint32_t bc = 0xfffffadc;
static const uint32_t fc = 0xffffcaba;
static const uint32_t tc = 0xff8a5752;

static const uint64_t bc_packed = PACKEDCOLOR64(bc);
static const uint64_t fc_packed = PACKEDCOLOUR64(fc);
static const uint64_t tc_packed = PACKEDCOLOR64(tc);


char batt_fmt_buffer[32];
static const char* batt_charge_full_path= "/sys/class/power_supply/BAT1/charge_full";
static const char* batt_charge_now_path="/sys/class/power_supply/BAT1/charge_now";
static const char* batt_status_path = "/sys/class/power_supply/BAT1/status";
char charge_full_buf[16];
char charge_now_buf[16];
char charge_status_buf[16];
unsigned int batt_acc;
uint64_t batt_lowpwr_color_cycle[] = {
  fc_packed,
  PACKEDCOLOR64(0xff1a0000)
};
unsigned int batt_lowpwr_cycle_counter = 0;
void* batt_draw(void* v){
  int full_fd = open(batt_charge_full_path, O_RDONLY);
  int now_fd = open(batt_charge_now_path, O_RDONLY);
  int status_fd = open(batt_status_path, O_RDONLY);
  if(full_fd < 0 || 0 > now_fd || status_fd < 0){
    printf("couldn't open one of batt status paths\n");
    return 0;
  }
  while(1){  
    read(now_fd, charge_now_buf, 15);
    read(full_fd, charge_full_buf, 15);
    read(status_fd, charge_status_buf, 15);
    // i fucking love null terminated strings
    char *nptr;
    long fnum = strtol(charge_full_buf, &nptr, 10);
    long nnum = strtol(charge_now_buf, &nptr, 10);
    double s = (double)nnum/(double)fnum;
    double p = s * 100.0;
    // damn just give me a number
    int is_eating_biscuits = strcmp(charge_status_buf, "Discharging\n");
    int len = snprintf(batt_fmt_buffer, 16, "batt%c[%0.3f]", is_eating_biscuits ? 'c' : 'd', p);  
    batt_acc = len*font_size;
    if(!is_eating_biscuits && s < 0.12){
      batt_lowpwr_cycle_counter ^= 1;
      fill_rect(0, 0, batt_acc, height , batt_lowpwr_color_cycle[batt_lowpwr_cycle_counter]);
    } else {
      fill_rect(0, 0, batt_acc, height, fc_packed);
    }
    paint_str(batt_fmt_buffer, len, 0, 1, tc);
    // reset file descriptors
    lseek(full_fd, 0, SEEK_SET);
    lseek(now_fd, 0, SEEK_SET);
    lseek(status_fd, 0, SEEK_SET);
    sleep(7);    
  }
  return 0;
}

// time_draw and date_draw uses this. updated by time_draw
struct tm* lt;
char time_fmt_buffer[32];
void* time_draw(void* v){
  while(should_continue){
    time_t t = time(0);
    lt = localtime(&t);
    int time_len = snprintf(time_fmt_buffer, 32, "%02i%02i%02i", lt->tm_sec, lt->tm_min, lt->tm_hour);
    int acc = time_len*font_size;
    int time_x = width-acc;
    fill_rect(time_x, 0, acc, height, fc_packed);
    paint_str(time_fmt_buffer, time_len, time_x, 1, tc);
    sleep(1);
  }
  return 0;
}

char date_fmt_buffer[32];
void* date_draw(void *v){
  // it'll sleep long so doesnt really matter
  while (1){
    size_t date_len = strftime(date_fmt_buffer, 64, "%a %d %B %Y", lt);
    size_t acc = date_len*font_size;
    size_t date_x = (width/2)-(acc/2);
    fill_rect(date_x, 0, acc, height, bc_packed);
    paint_str(date_fmt_buffer, date_len, date_x, 1, tc);
    sleep(120);
  }
  return 0;
}



// stuff that are drawn every frame go here
// other stuff with different intervals go in threads

#define nonfreeuse_size 640
char non_freeuse_scrolltext_buffer[nonfreeuse_size];
static const unsigned int viewport_chars = 32;
static const unsigned int viewport_px = viewport_chars*font_size;
int vps = 0;
int ce = 0;
int utsname(){
  struct utsname u;
  uname(&u);
  return snprintf(non_freeuse_scrolltext_buffer+ce, sizeof(non_freeuse_scrolltext_buffer)-ce, "%s-%s :: %s %s ", u.sysname, u.release, u.machine, u.nodename);
}

int sc_sysinfo(){
  struct sysinfo si;
  sysinfo(&si);
  return snprintf(non_freeuse_scrolltext_buffer+ce, sizeof(non_freeuse_scrolltext_buffer)-ce, "FREERAM=%08lu FREESWAP=%08lu PROCS=%u UPTIME=%08lu ", si.freeram, si.freeswap, si.procs, si.uptime);
}

// update this value when adding functions! (idk how to get length of arrays)
#define sflist_len 2
int (*sflist[])() = {
  utsname,
  sc_sysinfo
};

static const struct wl_callback_listener surface_frame_lisn;
// if this variable reaches 8. move forward 1 char
unsigned int slide_tab = 0;
int fmtlen = 0;
void monet(void* brick, struct wl_callback* callback, uint32_t delta){
  wl_callback_destroy(callback);
  callback = wl_surface_frame(surface);
  wl_callback_add_listener(callback, &surface_frame_lisn, brick);

  // this part is the scrolling text. very expensive
  while(ce <= vps+viewport_chars){
    fmtlen = sflist[rand()%sflist_len]();
    ce+=fmtlen;
  }
  if(fmtlen < 0 || ce+fmtlen >= nonfreeuse_size || vps+viewport_chars >= nonfreeuse_size) {
    ce = 0;
    vps = 0;;
  }
  fill_rect(batt_acc, 0, viewport_px, height, bc_packed);
  // -slide_tab for smooth scrolling
  //printf("%i", viewport_startx);
  paint_str(non_freeuse_scrolltext_buffer+vps, viewport_chars, batt_acc+font_size-slide_tab, 1, tc);
  fill_rect(batt_acc + viewport_px - font_size, 0, 16, height, bc_packed);
  if (slide_tab >= 8){
    slide_tab=0;
    vps++;
  }
  slide_tab++;
  
  wl_surface_attach(surface, brick, 0, 0);
  wl_surface_damage(surface, 0, 0, INT32_MAX, INT32_MAX); // wayland-book set me up. dont use damge_buffer
  wl_surface_commit(surface);
  return;
}

static const struct wl_callback_listener surface_frame_lisn = {
  .done = monet
};

#define sagit 3
static void* (*sagit_fn[sagit])(void*) = {
  time_draw,
  date_draw,
  batt_draw,
};

pthread_t draw_fn_threads[sagit];

static const struct timespec sl = {
  .tv_sec = 0,
  .tv_nsec = 77777777,
};

int main(){
  struct wl_display* parad = wl_display_connect(NULL);
  if(!parad){
    fprintf(stderr, "cant connect to display. ya sure yer opened one? siya\n");
  }
  struct wl_registry* registary = wl_display_get_registry(parad);
  wl_registry_add_listener(registary, &rlisn, 0);
  wl_display_roundtrip(parad);
  if (!(compositor && shm && layer_shell))
    {fprintf(stderr, "couldn't get the required interfaces. update your WM or smth idk. siya\n"); return 1;}
  int fd = shm_alloc();
  if (fd < 0)
    {fprintf(stderr, "damn\n"); return 1;}
  map = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(map == MAP_FAILED)
    {fprintf(stderr, "damn (1)\n"); return 1;}
  struct wl_shm_pool* shm_pool = wl_shm_create_pool(shm, fd, map_size);
  struct wl_buffer* brick = wl_shm_pool_create_buffer(shm_pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
  close(fd);
  surface = wl_compositor_create_surface(compositor);
  layer_surface = zwlr_layer_shell_v1_get_layer_surface(layer_shell, surface, output, ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM, "panel");
  zwlr_layer_surface_v1_set_anchor(layer_surface ,ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT); 
  zwlr_layer_surface_v1_add_listener(layer_surface, &zwlr_lisn, 0);
  zwlr_layer_surface_v1_set_size(layer_surface, 0, height);
  zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, height);
  wl_surface_commit(surface);
  while(wl_display_dispatch(parad) > 0 && !configured){}
  wl_surface_attach(surface, brick, 0, 0);
  wl_surface_commit(surface);
  struct wl_callback* callback = wl_surface_frame(surface);
  wl_callback_add_listener(callback, &surface_frame_lisn, brick);
  wl_surface_damage(surface, 0, 0, INT32_MAX, INT32_MAX); // wayland-book set me up. dont use damge_buffer
  wl_surface_commit(surface);

  // init rand for scrolling text
  srand(0);
  // no repeating fills
  fill(bc_packed);
  // create seperate draw threeads
  int i = 0;
  while(i < sagit){  
    pthread_create(draw_fn_threads+i, 0, sagit_fn[i], 0);
    i++;
  }
  while(wl_display_dispatch(parad) > 0 && should_continue){
    nanosleep(&sl, 0);
  }
  wl_surface_destroy(surface);
  wl_display_disconnect(parad);
  return 0;
}
