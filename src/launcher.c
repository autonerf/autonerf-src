// Standard C dependencies
#include <stdio.h>
#include <unistd.h>

// Project dependencies
#include <autonerf/launcher.h>
#include <autonerf/io/gpio.h>

uint8_t _loaded[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t _chamber[10][3];
uint8_t _feedback;
uint8_t _disabled;
uint8_t _reload_led;
uint8_t _reload_switch;

int
launcher_shoot_next(void)
{
    uint8_t i = 0;

    for (; i < 10; i++) {
        _loaded[i] ? break : continue;
    }

    if (i == 10) {
        return -1;
    }

    _loaded[i] = 0;

    gpio_write(_chamber[i], 1);
    usleep(10000);
    gpio_write(_chamber[i], 0);

    if (i == 9) {
        gpio_write(_reload_led, 1);

        while (gpio_read(_feedback) == 1) {
            gpio_write(_disabled, 0);
            usleep(10);
            gpio_write(_disabled, 1);
            usleep(10);
        }
    }
}

void
launcher_init(void)
{
    uint8_t i = 0;

    _chamber[0]     = gpio_calculate(2, 14);    gpio_init(_chamber[0]); gpio_write(_chamber[0], 0);
    _chamber[1]     = gpio_calculate(2, 12);    gpio_init(_chamber[1]); gpio_write(_chamber[1], 0);
    _chamber[2]     = gpio_calculate(2, 10);    gpio_init(_chamber[2]); gpio_write(_chamber[2], 0);
    _chamber[3]     = gpio_calculate(2, 8);     gpio_init(_chamber[3]); gpio_write(_chamber[3], 0);
    _chamber[4]     = gpio_calculate(2, 6);     gpio_init(_chamber[4]); gpio_write(_chamber[4], 0);
    _chamber[5]     = gpio_calculate(2, 7);     gpio_init(_chamber[5]); gpio_write(_chamber[5], 0);
    _chamber[6]     = gpio_calculate(2, 9);     gpio_init(_chamber[6]); gpio_write(_chamber[6], 0);
    _chamber[7]     = gpio_calculate(2, 11);    gpio_init(_chamber[7]); gpio_write(_chamber[7], 0);
    _chamber[8]     = gpio_calculate(2, 13);    gpio_init(_chamber[8]); gpio_write(_chamber[8], 0);
    _chamber[9]     = gpio_calculate(2, 15);    gpio_init(_chamber[9]); gpio_write(_chamber[9], 0);

    _disabled       = gpio_calculate(0, 9);     gpio_init(_disabled);
    _feedback       = gpio_calculate(0, 8);     gpio_init(_feedback);
    _reload_led     = gpio_calculate(2, 5);     gpio_init(_reload_led);
    _reload_switch  = gpio_calculate(0, 2);     gpio_init(_reload_switch);

    gpio_write(_disabled, 1);
    gpio_write(_reload_led, 0);
}

void
launcher_shoot(void)
{
    return (launcher_is_loaded()) ? launcher_shoot_next() : -1;
}

void
launcher_reload(void)
{
    uint8_t i = 0;

    if (gpio_read(_reload_switch) == 0) {
        gpio_write(_reload_led, 0);

        for (; i < 10; i++) {
            _loaded[i] = 1;
        }
    }
}

void
launcher_deinit(void)
{
    uint8_t i = 0;

    for (; i < 10; i++) {
        gpio_unexport(_chamber[i]);
    }
}

int
launcher_is_loaded(void)
{
    uint8_t i = 0;

    for (; i < 10; i++) {
        if (_loaded[i]) {
            return 1;
        }
    }

    return 0;
}
