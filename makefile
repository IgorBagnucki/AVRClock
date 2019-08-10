all:
	python tools\bitmap.py baloons
	python tools\big_font.py
	python tools\small_font.py
	avr-gcc -O3 -mmcu=atmega328p -std=c99 dbg.c twi.c clock.c spi.c disp.c main.c -o main
	avr-objcopy -O ihex main main.hex
	avrdude -c usbasp -p ATMEGA328P -U flash:w:main.hex:i