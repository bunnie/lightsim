#include <QtWidgets>

#include "lightsim.h"
#include "libfixmath/libfixmath/fixmath.h"


static void ledSetRGB(void *ptr, int x, uint8_t r, uint8_t g, uint8_t b, uint8_t shift);

#define NUMLED 16
static uint8_t simfb[NUMLED*3];

typedef struct Color Color;
struct Color {
  uint8_t g;
  uint8_t r;
  uint8_t b;
};

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

static effects_config fx_config;
static uint8_t shift = 0;  // start a little bit dimmer

static uint32_t bump_amount = 0;
static uint8_t bumped = 0;
static unsigned int bumptime = 0;
static unsigned long reftime = 0;
static unsigned long reftime_tau = 0;
static unsigned long offset = 0;
static unsigned int waverate = 10;
static unsigned int waveloop = 0;
static unsigned int patternChanged = 0;

static uint32_t timeMS = 0;  // simulate chVTGetSystemTime() call using QT

static int wavesign = -1;

static void ledSetRGB(void *ptr, int x, uint8_t r, uint8_t g, uint8_t b, uint8_t shift) {
  uint8_t *buf = ((uint8_t *)ptr) + (3 * x);
  buf[0] = g >> shift;
  buf[1] = r >> shift;
  buf[2] = b >> shift;
}

static uint32_t chVTGetSystemTime() {
    return timeMS;
}

static void init_fx() {
    led_config.max_pixels = NUMLED;
    led_config.pixel_count = NUMLED;
    led_config.fb = simfb;

    fx_config.hwconfig = &led_config;
    fx_config.count = led_config.pixel_count;
    fx_config.loop = 0;
}

LightSim::LightSim(QWidget *parent)
    : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(35);

    setWindowTitle(tr("lightsim"));
    resize(400, 400);

    init_fx();
}

// alpha blend, scale the input color based on a value from 0-255. 255 is full-scale, 0 is black-out.
// uses fixed-point math.
Color alphaPix( Color c, uint8_t alpha ) {
  Color rc;
  uint32_t r, g, b;

  r = c.r * alpha;
  g = c.g * alpha;
  b = c.b * alpha;

  rc.r = (r / 255) & 0xFF;
  rc.g = (g / 255) & 0xFF;
  rc.b = (b / 255) & 0xFF;

  return( rc );
}

static Color Wheel(uint8_t wheelPos) {
  Color c;

  if (wheelPos < 85) {
    c.r = wheelPos * 3;
    c.g = 255 - wheelPos * 3;
    c.b = 0;
  }
  else if (wheelPos < 170) {
    wheelPos -= 85;
    c.r = 255 - wheelPos * 3;
    c.g = 0;
    c.b = wheelPos * 3;
  }
  else {
    wheelPos -= 170;
    c.r = 0;
    c.g = wheelPos * 3;
    c.b = 255 - wheelPos * 3;
  }
  return c;
}

static void ledSetColor(void *ptr, int x, Color c, uint8_t shift) {
  uint8_t *buf = ((uint8_t *)ptr) + (3 * x);
  buf[0] = c.g >> shift;
  buf[1] = c.r >> shift;
  buf[2] = c.b >> shift;
}

static Color ledGetColor(void *ptr, int x) {
  Color c;
  uint8_t *buf = ((uint8_t *)ptr) + (3 * x);

  c.g = buf[0];
  c.r = buf[1];
  c.b = buf[2];

  return c;
}

static void shootPatternFB(struct effects_config *config) {
  uint8_t *fb = config->hwconfig->fb;
  int count = config->count;
  int loop = config->loop;

  int i;

  //loop = (loop >> 3) % count;
  loop = loop % count;
  for (i = 0; i < count; i++) {
    if (loop == i)
      ledSetRGB(fb, i, 255, 255, 255, shift);
    else
      ledSetRGB(fb, i, 0, 0, 0, shift);
  }
}

static void calmPatternFB(struct effects_config *config) {
  uint8_t *fb = config->hwconfig->fb;
  int count = config->count;
  int loop = config->loop;

  int i;
  int count_mask;
  Color c;

  count_mask = count & 0xff;
  loop = loop % (256 * 5);
  for (i = 0; i < count; i++) {
    c = Wheel( (i * (256 / count_mask) + loop) & 0xFF );
    ledSetRGB(fb, i, c.r, c.g, c.b, shift);
  }
}


#define VU_X_PERIOD 3  // number of waves across the entire band
#define VU_T_PERIOD 2500  // time to complete 2pi rotation, in integer milliseconds
#define TAU 1000

static void waveRainbowFB(struct effects_config *config) {
  uint8_t *fb = config->hwconfig->fb;
  int count = config->count;

  unsigned long curtime;
  int i;
  uint32_t c;
  uint16_t colorrate = 1;

  curtime = chVTGetSystemTime() + offset;
  if ((curtime - reftime) > VU_T_PERIOD)
    reftime = curtime;

  if ((curtime - reftime_tau) > TAU) {
    reftime_tau = curtime;
    waverate -= 4;
    if (waverate < 10)
      waverate = 10;

    if (colorrate > 1)
      colorrate -= 1;
  }

  if (bumped) {
    bumped = 0;
    waverate += 20;
    colorrate += 1;
    if (waverate > 300)
      waverate = 300;
    if (colorrate > 10)
      colorrate = 10;
  }

  offset += waverate;
  if (offset > 0x80000000) {
    offset = 0;
    curtime = chVTGetSystemTime();
    reftime = curtime;
    reftime_tau = curtime;
  }

  waveloop += colorrate;
  if (waveloop == (256 * 5)) {
    waveloop = 0;
  }
  for (i = 0; i < count; i++) {
    fix16_t count_n = fix16_from_int(i * VU_X_PERIOD);
    fix16_t count_d = fix16_from_int(count - 1);
    fix16_t time_n = fix16_from_int(curtime - reftime);
    fix16_t time_d = fix16_from_int(VU_T_PERIOD);
    fix16_t ratios = fix16_add(
                fix16_div(count_n, count_d), fix16_div(time_n, time_d));

    /* 'ratios' now goes from 0 to 2, depending on where we are in the cycle */

    ratios = fix16_mul(ratios, fix16_from_int(2));
    ratios = fix16_mul(ratios, fix16_pi);
    fix16_t v = fix16_sin(ratios);

    /* Normalize, go from [-1, 1] to [0, 256] */
    v = fix16_mul(fix16_add(v, fix16_from_int(1)), fix16_from_int(127));

    c = fix16_to_int(v);

    /* Quick and dirty nonlinearity */
    c = c * c;
    c = (c >> 8) & 0xFF;

    ledSetColor(fb, i, alphaPix(Wheel(((i * 256 / count) + waveloop) & 255), (uint8_t) c), shift);
  }
}

static void draw_pattern(void) {

    fx_config.loop++;
    if(bump_amount != 0) {
        fx_config.loop += bump_amount;
        bump_amount = 0;
    }
    waveRainbowFB(&fx_config);

}



void LightSim::paintEvent(QPaintEvent *)
{
    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };

    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);

//    painter.save();
//    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
//    painter.drawConvexPolygon(hourHand, 3);
//    painter.restore();

    QColor  ledColor(16,16,16);
    painter.setPen(ledColor);
    painter.setBrush(ledColor);
    painter.drawRect(-100,-100,200,200);

    timeMS = time.msecsSinceStartOfDay();
    draw_pattern();

    for (int i = 0; i < 16; ++i) {
        ledColor.setRgb(simfb[i*3+2],simfb[i*3],simfb[i*3+1]); // buffer order is gbr
        painter.setPen(ledColor);
        painter.setBrush(ledColor);

        painter.drawEllipse(80,0,18,18);
    //      painter.drawLine(88, 0, 96, 0);
        painter.rotate(-22.5);
    }


}
