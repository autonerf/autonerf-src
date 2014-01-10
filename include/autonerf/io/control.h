#ifndef AUTONERF_IO_CONTROL_H
#define AUTONERF_IO_CONTROL_H

/**
 Initialize the pan and tilt servo motots
 */
extern void control_init(void);

/**
 Set the position of the panning servo to a specific `position`

 @param position The new position of the pan servo (between 0 and 180 degrees)
 */
extern void control_set_pan(float position);

/**
 Set the position of the tilt servo to a specific `position`

 @param position The new position of the tilt servo (between 0 and 180 degrees)
 */
extern void control_set_tilt(float position);

/**
 Set the position of the panning servo using a `delta`

 @param delta The delta for the new position (both negative and positive)
 */
extern void control_set_pan_delta(float delta);

/**
 Set the position of the tilting servo using a `delta`

 @param delta The delta for the new position (both negative and positive)
 */
extern void control_set_tilt_delta(float delta);

#endif /* AUTONERF_IO_CONTROL_H */
