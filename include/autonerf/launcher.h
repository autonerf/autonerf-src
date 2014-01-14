#ifndef AUTONERF_LAUNCHER_H
#define AUTONERF_LAUNCHER_H

/**
 Initialize the launcher
 */
extern void launcher_init(void);

/**
 Aim the launcher to a specific X/Y offset

 @param x The offset of the X-axis
 @param y The offset of the Y-axis
 */
extern void launcher_aim(const float x, const float y);

/**
 Shoot a missile (if one is present)

 @return Returns 1 if the launcher has fired, 0 otherwise
 */
extern int  launcher_shoot(void);

/**
 Indicate that the launcher is reloaded
 */
extern void launcher_reload(void);

/**
 Check if the launcher is loaded

 @return Returns 1 if the launcher is loaded, 0 otherwise
 */
extern int launcher_is_loaded(void);

/**
 Deinitialize the launcher
 */
extern void launcher_deinit(void);

#endif /* AUTONERF_LAUNCHER_H */
