#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include "fbdev_my.h"

int g_fbfd = -1;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *g_fbp = 0;
size_t screensize = 0;


int init_fb()
{
	// Open the file for reading and writing
    g_fbfd = open("/dev/fb0", O_RDWR);
    if (g_fbfd == -1) {
#ifdef MSG_DEBUG
        perror("Error: cannot open framebuffer device");
#endif
        return -1;
    }
#ifdef MSG_DEBUG
    else printf("The framebuffer device was opened successfully.\n");
#endif
    
    // Get fixed screen information
    if (ioctl(g_fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {

#ifdef MSG_DEBUG
        perror("Error reading fixed information");
#endif
        return -2;
    }

    // Get variable screen information
    if (ioctl(g_fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
#ifdef MSG_DEBUG
        perror("Error reading variable information");
#endif
        return -3;
    }
#ifdef MSG_DEBUG
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
#endif

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
#ifdef MSG_DEBUG
    printf("screen size %ld\n", screensize);
#endif

    // Map the device to memory
    g_fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fbfd, 0);
    if (g_fbp == MAP_FAILED) {
#ifdef MSG_DEBUG
        perror("Error: failed to map framebuffer device to memory");
#endif
        return -4;
    }
#ifdef MSG_DEBUG
    printf("The framebuffer device was mapped to memory successfully.\n");
#endif
    return 0;
}

void deinit_fb()
{
	munmap(g_fbp, screensize);
	g_fbp = 0;
	
	close(g_fbfd);
	g_fbfd = -1;
}

struct fb_var_screeninfo get_var_resolution() //struct fb_var_screeninfo v_info)
{
	return vinfo;
}

void write_to_fb_rect3(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xend, int yend)
{
	unsigned int x = 0, y = 0;
    long int location = 0;
    
    unsigned int x_start = (x_pos < 0) ? 0 : x_pos;
    unsigned int y_start = (y_pos < 0) ? 0 : y_pos;
    
    unsigned int x_end = xend > vinfo.xres ?  vinfo.xres : xend;
    unsigned int y_end = yend > vinfo.yres ?  vinfo.yres : yend;
    unsigned int x_res = x_end - x_start;
    
    
    for (y = y_start; y < y_end; y++)
		for (x = x_start; x < x_end; x++) {
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
    
	// Figure out where in memory to put the pixel
	//for (y = y_pos; y < yend; y++)
		//for (x = x_pos; x < xend; x++) {
			//location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
				   //(y+vinfo.yoffset) * finfo.line_length;

			//if (vinfo.bits_per_pixel == 32) {
				//*(g_fbp + location) = b;// blue
				//*(g_fbp + location + 1) = g;// green
				//*(g_fbp + location + 2) = r;// red
				//*(g_fbp + location + 3) = 0;  // No transparency
			//} else  { //assume 16bpp
				//unsigned short int t = r<<11 | g << 5 | b;
				//*((unsigned short int*)(g_fbp + location)) = t;
			//}
		//}
}

void write_to_fb_rect(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xres, int yres)
{
	unsigned int x = 0, y = 0;
    long int location = 0;
	// Figure out where in memory to put the pixel
	for (y = y_pos; y < y_pos+yres; y++)
		for (x = x_pos; x < x_pos+xres; x++) {
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

void write_to_fb_rect2(SRect_t rect)
{
	int x = 0, y = 0;
    long int location = 0;
	// Figure out where in memory to put the pixel
	for (y = rect.y_pos; y < (rect.y_pos + rect.yres); y++)
		for (x = rect.x_pos; x < (rect.x_pos + rect.xres); x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
				   (y+vinfo.yoffset) * finfo.line_length;

			if (vinfo.bits_per_pixel == 32) {
				*(g_fbp + location) = rect.b;// blue
				*(g_fbp + location + 1) = rect.g;// green
				*(g_fbp + location + 2) = rect.r;// red
				*(g_fbp + location + 3) = rect.a;  //transparency
			} else  { //assume 16bpp
				unsigned short int t = rect.r<<11 | rect.g << 5 | rect.b;
				*((unsigned short int*)(g_fbp + location)) = t;
			}
		}
}

void read_data_fbuffer(fb_data_type *data, int x_pos, int y_pos, int xres, int yres)
{
	int x = 0, y = 0;
    long int location = 0;
    
    int x_start = (x_pos < 0) ? 0 : x_pos;
    int y_start = (y_pos < 0) ? 0 : y_pos;
    
    int x_end = (x_pos + xres) > vinfo.xres ?  vinfo.xres : (x_pos + xres);
    int y_end = (y_pos + yres) > vinfo.yres ?  vinfo.yres : (y_pos + yres);
    
    if (x_end < 0 || y_end < 0) return;
    
    int x_res = x_end - x_start;    
	
	// Figure out where in memory to put the pixel
	//for (int y = y_start; y < y_end; y++) {
		//location = (x_start + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
				   //(y + vinfo.yoffset) * finfo.line_length;
		//memcpy(data + (y * x_res), g_fbp + location, x_res * sizeof(fb_data_type));
	//}
	// Working
	for (y = y_start; y < y_end; y++)
		for (x = x_start; x < x_end; x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
				   (y+vinfo.yoffset) * finfo.line_length;
				   
			*data++ = *((fb_data_type*)(g_fbp + location));
		}
}

void write_data_fbuffer(fb_data_type *data, int x_pos, int y_pos, int xres, int yres)
{
	int x = 0, y = 0;
    long int location = 0;  
    
    int x_start = (x_pos < 0) ? 0 : x_pos;
    int y_start = (y_pos < 0) ? 0 : y_pos;
    
    // FIXME
//    int x_end = (x_start + xres) > 800 ?  800 : (x_start + xres);
//    int y_end = (y_start + yres) > 600 ?  600 : (y_start + yres);
    
    int x_end = (x_start + xres) > vinfo.xres ?  vinfo.xres : (x_start + xres);
    int y_end = (y_start + yres) > vinfo.yres ?  vinfo.yres : (y_start + yres);
    if (x_end < 0 || y_end < 0) return;
    
    int x_res = x_end - x_start;

    // Figure out where in memory to put the line    
    //for (int y = y_start; y < y_end; y++) {
		//location = (x_start + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
				   //(y + vinfo.yoffset) * finfo.line_length;
		//memcpy(g_fbp + location, data + (y * x_res), x_res * sizeof(fb_data_type));
	//}
	
	// Working
	for (y = y_start; y < y_end; y++)
		for (x = x_start; x < x_end; x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
				   (y+vinfo.yoffset) * finfo.line_length;			
			
			*((fb_data_type*)(g_fbp + location)) = *data++;
		}
}

void new_write(fb_data_type *data, size_t size)
{
	long int location = 0;
	memcpy(g_fbp, data, size);
}

void write_buffer(const unsigned char *pixel_data, size_t size)
{
	// TODO check g_fbp
	memcpy(g_fbp, pixel_data, size); //(640 * 480 * 2 + 1));
}

void write_to_fb_fullscreen(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	write_to_fb_rect(r, g, b, a, 0, 0, vinfo.xres, vinfo.yres);
}

void draw_cross(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x_pos, int y_pos, int xend, int yend, int tickness)
{
	if (xend < 0 || yend < 0) return;
	write_to_fb_rect3(r, g, b, a, x_pos, ((y_pos+yend)/2)-(tickness/2), xend, ((y_pos+yend)/2)+(tickness/2));
	write_to_fb_rect3(r, g, b, a, ((x_pos+xend)/2)-(tickness/2), y_pos, ((x_pos+xend)/2)+(tickness/2), yend);	
}
