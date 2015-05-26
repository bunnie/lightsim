#include <QtWidgets>

#include "libfixmath/libfixmath/fixmath.h"
#include "lightsim.h"
#include <math.h>

// all values stored as binary. Prior to transmission and sex,
// values are encoded in gray scale for the addition of mutations
// and then decoded back into binary for storage internally
typedef struct haploid {
  uint8_t cd_period;
  uint8_t cd_rate;
  uint8_t sat;
  uint8_t hue_ratedir;
  uint8_t hue_gamut;
} haploid;

typedef struct dipliod {
  // genetic
  struct haploid A;
  struct haploid B;

  // epigenetics
  uint8_t status;  // epigenetic; contains info like "locked"
  uint32_t playtime;  // duration, in seconds, that an individual has been shown
  char name[12]; // a user-assignable name to an individual. Picked at random initially.
}

char badgeName[16];  // name of the person who owns this badge

#define MAX_ANCESTORS  64   // for a 1k array of names
typedef struct lineage {
  char **ancestors;  // array of names of people who had sex with a particular badge
  uint8_t num_ancestors;
}

extern uint8_t shift;

uint8_t gray_encode(uint8_t n) {
  return n ^ (n >> 1);
}

uint8_t gray_decode(uint8_t n) {
  uint8_t p = n;
  while (n >>= 1)
    p ^= n;
  return p;
}

void lightGeneFB(struct effects_config *config) {
  uint8_t *fb = config->hwconfig->fb;
  int count = config->count;
  uint8_t loop = config->loop & 0xFF;
  HsvColor hsvC;
  RgbColor rgbC;
  
  int i;
  int count_mask;

  for (i = 0; i < count; i++) {
    hsvC.h = loop + (i * (256 / count));
    hsvC.s = 192 + (uint8_t) (63.0 * sin(4.0 * 3.14159 * ((float)loop / 255.0) -
					  (6.0 * 3.14159 * (float) i / 16.0) ) );
    hsvC.v = 128 + (uint8_t) (127.0 * sin(2.0 * 3.14159 * ((float)loop / 255.0) -
					  (2.0 * 3.14159 * (float) i / 16.0) ) );
    rgbC = HsvToRgb(hsvC);
    ledSetRGB(fb, i, rgbC.r, rgbC.g, rgbC.b, shift);
  }
}
