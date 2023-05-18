#include <SDL.h>
#include <SDL_image.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/*#define MANDELBROT*/
#define BURNING_SHIP

#define STARTING_POS 0

#define THREADS 12

#define RADIUS 0.9
#define ZOOM 6
#define THRESHOLD 4
#define CNST_X 0.3
#define CNST_Y -0.3

#define LOG2 0.69314718055994528622676398299518041312694549560546875
#define LOGPOINT5 -0.69314718055994528622676398299518041312694549560546875
#define TAU 6.28318530717958647692528676655900576839433879875021164

/*#define TEST*/

#ifdef TEST

#define RESX (7680 / 2)
#define RESY (4320 / 2)

#define COLOUR_DEPTH (size_t)30000

#define ITERATIONS 100

#define ROTATE_AMOUNT 0.01

#else

#define RESX 3840
#define RESY 2160

#define COLOUR_DEPTH (size_t)65536

#define ITERATIONS (8192 * 2)

#define ROTATE_AMOUNT 0.0004363323129985824

#endif

typedef struct tinfo {
  pthread_t pid;
  size_t tid;
  long double re;
  long double im;
} tinfo_t;

uint32_t *framebuffer = NULL;

uint32_t colours[COLOUR_DEPTH];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *surface = NULL;

static void *render(void *_inf) {
  tinfo_t *inf = _inf;

  long double real = inf->re;
  long double imag = inf->im;

  for (size_t i = inf->tid; i < RESY; i += THREADS)
    for (size_t j = 0; j < RESX; j++) {
      long double zx = (j - RESX / 2.0) * ZOOM / RESX;
      long double zy = (i - RESY / 2.0) * ZOOM / RESX;

      size_t k = 0;

      while (k < ITERATIONS && zx * zx + zy * zy <= THRESHOLD) {
#ifdef MANDELBROT
        long double nx = zx * zx - zy * zy + real;
        long double ny = 2 * zx * zy + imag;
#endif
     
#ifdef BURNING_SHIP
        long double nx = zx * zx - zy * zy + real;
        long double ny = 2 * fabsl(zx * zy) + imag;
#endif

        zx = nx;
        zy = ny;

        k++;
      }

      if (k == ITERATIONS)
        framebuffer[i * RESX + j] = 0;
      else {
        long double smooth_colour =
          k + 1 - ((LOGPOINT5 + log(log(zx * zx + zy * zy))) / (LOG2));
        framebuffer[i * RESX + j] = colours[(
          size_t)(smooth_colour / (long double)ITERATIONS * COLOUR_DEPTH)];
      }
    }

  pthread_exit(NULL);
}

static inline void save_img(char *name) {
  SDL_UpdateTexture(texture, NULL, framebuffer, RESX * sizeof(uint32_t));
  SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels,
                       surface->pitch);
  IMG_SavePNG(surface, name);
}

static void sig(int p) {
  (void)p;
  exit(0);
}

int main() {
  mkdir("./pics", 0755); // drwxr-xr-x

  signal(SIGINT, sig);

  window =
    SDL_CreateWindow("Julia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                     RESX, RESY, SDL_WINDOW_HIDDEN);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
                              SDL_TEXTUREACCESS_TARGET, RESX, RESY);
  surface = SDL_CreateRGBSurface(0, RESX, RESY, 32, 0, 0, 0, 0);

  SDL_SetRenderTarget(renderer, texture);

  framebuffer = calloc(sizeof(uint32_t), RESX * RESY);

  for (size_t i = 0; i < COLOUR_DEPTH; i++) {
    long double k = (long double)(i * ITERATIONS) / (long double)COLOUR_DEPTH;
    long double n1 = sin(k * 0.1) * 0.5 + 0.5;
    long double n2 = cos(k * 0.1) * 0.5 + 0.5;

    uint8_t r = n1 * 0xff;
    uint8_t g = n2 * 0xff;
    uint8_t b = 1 * 0xff;

    colours[i] = (r << 16) | (g << 8) | (b << 0);
  }

  char bufname[1024];
  tinfo_t ts[THREADS] = {0};

  size_t roll = STARTING_POS;

  while (1) {
    long double curr_rotation = roll * ROTATE_AMOUNT;
    
    if (curr_rotation > TAU)
      exit(0);

    for (size_t i = 0; i < THREADS; i++) {
      ts[i] = (tinfo_t){
        .re = RADIUS * cos(curr_rotation) + CNST_X,
        .im = RADIUS * -sin(curr_rotation) + CNST_Y,
        .tid = i,
      };

      pthread_create(&ts[i].pid, NULL, render, &ts[i]);
    }

    for (size_t i = 0; i < THREADS; i++)
      pthread_join(ts[i].pid, NULL);

    snprintf(bufname, 1024, "pics/img%07zu.png", roll++);
    save_img(bufname);
  }

  return 0;
}
