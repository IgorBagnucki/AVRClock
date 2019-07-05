all:
	avr-gcc -O2 -mmcu=atmega328p -std=c99 main.c dbg.c -o main
	avr-objcopy -O ihex main main.hex
	avrdude -c usbasp -p ATMEGA328P -U flash:w:main.hex:i