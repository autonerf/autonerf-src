// Project dependencies
#include <autonerf/io/gpio.h>
#include <autonerf/io/servo.h>
#include <autonerf/io/control.h>

struct servo_t {
    uint8_t  io;
    float    position;
    uint32_t min;
    uint32_t max;
};

struct servo_t pan;
struct servo_t tilt;

void
control_init(void)
{
  pan.io        = gpio_calculate(0, 22);
  tilt.io       = gpio_calculate(2, 16);

  pan.position  = 90.0;
  tilt.position = 90.0;

  pan.min  =  700000; //Tested; OK
  pan.max  = 2200000; //Tested; OK
  tilt.min =  555092; //Tested; OK
  tilt.max = 2033333; //Tested; OK

  servo_init(pan.io, ((pan.max - pan.min) / 2) + pan.min);
  servo_init(tilt.io, ((tilt.max - tilt.min) / 2) + tilt.min);
}

void
control_set_pan(float position)
{
    if (position > 180.0) { position = 180.0; }
    if (position < 0.0)   { position = 0.0; }

    pan.position = position;
    // printf("New position: %f\n", position);

    servo_set_position(pan.io, pan.position, pan.min, pan.max);
}

void
control_set_tilt(float position)
{
    if (position > 180.0) { position = 180.0; }
    if (position < 0.0)   { position = 0.0; }

    tilt.position = position;
    printf("New position: %f\n", position);

    servo_set_position(tilt.io, tilt.position, tilt.min, tilt.max);
}

void
control_set_pan_delta(float delta)
{
    pan.position += delta;

    control_set_pan(pan.position);
}

void
control_set_tilt_delta(float delta)
{
    tilt.position += delta;

    control_set_tilt(tilt.position);
}

