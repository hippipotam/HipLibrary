#pragma once

#include <inttypes.h>

#define BRD_SIZE 5

#ifdef MACHINE_32BIT
	typedef uint32_t fb_data_type;
#else 
	typedef uint16_t fb_data_type;
#endif

typedef struct SRect {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	int x_pos;
	int y_pos;
	int xres;
	int yres;
} SRect_t;

/* Init framebuffer */
int init_fb();

/* Deinitialize framebuffer */
void deinit_fb();

/* Get screen info */
struct fb_var_screeninfo get_var_resolution();

/* Write rect to framebuffer (with start and end) */
void write_to_fb_rect3(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xend, int yend);

/* Write rect to framebuffer */
void write_to_fb_rect(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xres, int yres);

/* Write to framebuffer */
void write_to_fb_rect2(SRect_t rect);

/* Write to framebuffer */
void write_buffer(const unsigned char *pixel_data, size_t size);

/* Write to framebuffer fullscreen color */
void write_to_fb_fullscreen(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/* Read data from framebuffer */
void read_data_fbuffer(fb_data_type *data, int x_pos, int y_pos, int xres, int yres);

/* Write data to framebuffer */
void write_data_fbuffer(fb_data_type *data, int x_pos, int y_pos, int xres, int yres);


void new_write(fb_data_type *data, size_t size);

void draw_cross(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xend, int yend, int tickness);

