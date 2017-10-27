/*
 * main.c
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "gpio.h"


bool g_quit = false;

static void sig_int(int signo)
{
	printf("Exit with signal\n");
	g_quit = true;
}

int main(int argc, char *argv[])
{
	int res;
	float cpu_average;
	GPIOPin_t pin_25 = { -1, 60, 0, "", 0};


	// SIGKILL and SIGTERM
	if (signal(SIGINT, sig_int) == SIG_ERR) {
		return 0;
	}
	if (signal(SIGTERM, sig_int) == SIG_ERR) {
		return 0;
	}
	if (gpio_export(pin_25.pin, EXPORT) < 0) return 1;
	
	if (gpio_set_direction(pin_25.pin, pin_25.dir) < 0) {
		gpio_export(pin_25.pin, UNEXPORT);
		return 1;
	}
	
	pin_25.fd = gpio_open_pin(pin_25.pin, pin_25.dir);
	if (pin_25.fd < 0) {
		perror("open_pin");
		return 1;
	}
	FILE *f = fopen("/proc/loadavg", "r");
	if (!f) {
		perror("Open /proc/loadavg");
		gpio_close_pin(pin_25.fd);
		gpio_export(pin_25.pin, UNEXPORT);
		return 1;
	}

	float freq = 0.0;
	float time = 0;
	float sec = 1000000;
	while (!g_quit) {
		FILE *f = fopen("/proc/loadavg", "r");
		if (!f) continue;
		//rewind(f);
		fscanf(f, "%f", &cpu_average);
		printf("cpu: %.2f\n", cpu_average);
		fclose(f);
		
		freq = 1 - cpu_average;
		if (freq < 0.0) freq = 0;
		else if (freq > 1.0) freq = 1.0;
		printf("freq: %.2f\n", freq);		
		printf("time: %.2f\n", (freq * sec));
		time = freq * sec;
		// Enable LED
		gpio_write_byte(pin_25.fd, ENABLE);
		usleep(time);
		// Disable LED
		gpio_write_byte(pin_25.fd, DISABLE);
		usleep(time);
	}
	
	gpio_close_pin(pin_25.fd);
	gpio_export(pin_25.pin, UNEXPORT);
	printf("Done\n");
	return 0;
}
