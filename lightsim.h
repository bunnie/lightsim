#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <QWidget>

class LightSim : public QWidget
{
    Q_OBJECT

public:
    LightSim(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
};

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

typedef struct Color Color;
struct Color {
  uint8_t g;
  uint8_t r;
  uint8_t b;
};

typedef struct HsvColor {
  uint8_t h;
  uint8_t s;
  uint8_t v;
} HsvColor;

typedef struct RgbColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RgbColor;

typedef struct effects_config {
  struct led_config *hwconfig;
  uint32_t count;
  uint32_t loop;
} effects_config;

static struct led_config {
  uint8_t       *fb; // effects frame buffer
  uint32_t      pixel_count;  // generated pixel length
  uint32_t      max_pixels;   // maximal generation length
} led_config;

Color alphaPix( Color c, uint8_t alpha );
void ledSetColor(void *ptr, int x, Color c, uint8_t shift);
Color ledGetColor(void *ptr, int x);
void ledSetRGB(void *ptr, int x, uint8_t r, uint8_t g, uint8_t b, uint8_t shift);

RgbColor HsvToRgb(HsvColor hsv);
HsvColor RgbToHsv(RgbColor rgb);

#endif
