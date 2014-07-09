#Â!/bin/bash

## ParÃametros de config del puerto serie/usb
SERIALPORT=/dev/ttyUSB0
SERIALSPEED=9600

## Parametros de configuracion del servidor de tratamiento zigbee.bin
ZIGBEEPORT=1234

echo "Iniciando Lighttpd"
lighttpd -f lighttpd.conf

echo "Iniciando servidor C zigbee.bin"
sudo ./Zigbee/zigbee.bin $ZIGBEEPORT $SERIALPORT $SERIALSPEED

echo "Matando el server lighttpd"
killall lighttpd

