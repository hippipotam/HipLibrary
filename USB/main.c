#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <libusb-1.0/libusb.h>

//#include <linux_my/linux/usb.h>

#define DEV_VID 0x0951
#define DEV_PID 0x1642
#define DEV_CONFIG 1
#define DEV_INTF 0
#define EP_IN 0x81
#define EP_OUT 0x01



int main(int argc, char *argv[])
{

	libusb_device_handle *handle;

	/* инициализация библиотеки */
	libusb_init(NULL);

	/* определяет насколько информативной будет программа в случае ошибки
	 * установлен максимальный уровень
	 */
	libusb_set_debug(NULL, 3);

	/* ищет на шине устройство USB по заданным значениям Vendor ID и Product ID
	 * и открывает первое найденное для работы
	 */
	handle = libusb_open_device_with_vid_pid(NULL, DEV_VID, DEV_PID);

	if(handle == NULL) {
		printf("Cannot find device!\n");
		libusb_exit(NULL);
		return 0;
	}
	/* определяет, доступен ли заданный интерфейс */
	if (libusb_kernel_driver_active(handle,DEV_INTF)) {
			/* отцепляет от него драйвер операционной системы */
			printf("Detach kernel driver.\n");
			libusb_detach_kernel_driver(handle, DEV_INTF);
	}

	/* прицепляет драйвер обратно */
	printf("Attach kernel driver\n");
	libusb_attach_kernel_driver(handle, DEV_INTF);

	libusb_close(handle);
	libusb_exit(NULL);
	printf("Done.\n");

	return 0;
}
