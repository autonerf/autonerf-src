// Project dependencies
#include <autonerf/io/gpio.h>
#include <autonerf/io/servo.h>
#include <autonerf/io/control.h>

struct servo_t {
    uint8_t  io;
    float    position;
};

struct servo_t pan;
struct servo_t tilt;

void
control_init(void)
{
  pan.io        = gpio_calculate(0, 22);
  tilt.io       = gpio_calculate(2, 16);

  pan.position  = 0.0;
  tilt.position = 0.0;

  servo_init(pan.io);
  servo_init(tilt.io);
}

void
control_set_pan(float position)
{
    if (position > 180.0) { position = 180.0; }
    if (position < 0.0)   { position = 0.0; }

    pan.position = position;

    servo_set_position(pan.io, pan.position);
}

void
control_set_tilt(float position)
{
    if (position > 180.0) { position = 180.0; }
    if (position < 0.0)   { position = 0.0; }

    tilt.position = position;

    servo_set_position(tilt.io, tilt.position);
}

void
control_set_pan_delta(float delta)
{
    control_set_pan(pan.position + delta);
}

void
control_set_tilt_delta(float delta)
{
    control_set_tilt(delta + delta);
}

