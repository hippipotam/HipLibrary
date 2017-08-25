ls /sys/class/gpio
echo "echo 21 > /sys/class/gpio/export"
echo 21 > /sys/class/gpio/export
ls /sys/class/gpio
echo "echo out > /sys/class/gpio/gpio21/direction"
echo out > /sys/class/gpio/gpio21/direction

echo "ls /sys/class/gpio/gpio21"
ls /sys/class/gpio/gpio21
