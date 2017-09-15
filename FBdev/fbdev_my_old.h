#pragma once

#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <string.h>

int g_fbfd = -1;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *g_fbp = 0;
size_t screensize = 0;

#define BRD_SIZE 5

void init_fb()
{	
	// Open the file for reading and writing
    g_fbfd = open("/dev/fb0", O_RDWR);
    if (g_fbfd == -1) {
        perror("Error: cannot open framebuffer device");        
        exit(1);
    } else printf("The framebuffer device was opened successfully.\n");
    
    // Get fixed screen information
    if (ioctl(g_fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(g_fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screen size %ld\n", screensize);

    // Map the device to memory
    g_fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fbfd, 0);
    if (g_fbp == MAP_FAILED) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
}

struct fb_var_screeninfo GetVarResolution() //struct fb_var_screeninfo v_info)
{
	return vinfo;
}


void deinit_fb()
{
	munmap(g_fbp, screensize);
}

void write_to_fb(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xend, int yend)
{
	int x = 0, y = 0;
    long int location = 0;
	// Figure out where in memory to put the pixel
	for (y = y_pos; y < yend; y++)
		for (x = x_pos; x < xend; x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
				   (y+vinfo.yoffset) * finfo.line_length;

			if (vinfo.bits_per_pixel == 32) {
				*(g_fbp + location) = b;// blue
				*(g_fbp + location + 1) = g;// green
				*(g_fbp + location + 2) = r;// red
				*(g_fbp + location + 3) = 0;  // No transparency
			} else  { //assume 16bpp
				unsigned short int t = r<<11 | g << 5 | b;
				*((unsigned short int*)(g_fbp + location)) = t;
			}
		}
}

void write_buffer(const unsigned char *pixel_data)
{
	memcpy(g_fbp, pixel_data, (640 * 480 * 2 + 1));
}

void write_to_fb_fullscreen(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	write_to_fb(r, g, b, a, 0, 0, vinfo.xres, vinfo.yres);
}

