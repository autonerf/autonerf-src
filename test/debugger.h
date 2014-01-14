#ifndef AUTONERF_TEST_DEBUGGER_H
#define AUTONERF_TEST_DEBUGGER_H

#define TYPE_DEFAULT 0
#define TYPE_LABELED 1

extern void debugger_save_image(const char * filename, const uint8_t * data, const size_t size, const uint8_t type);

#endif /* AUTONERF_TEST_DEBUGGER_H */
