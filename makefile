all:
	python3 tools/bitmap.py baloons
	python3 tools/bitmap.py hearts
	python3 tools/bitmap.py kuce
	python3 tools/bitmap.py ring
	python3 tools/big_font.py
	python3 tools/small_font.py
	avr-gcc -O3 -mmcu=atmega328p -std=c99 twi.c clock.c spi.c disp.c main.c -o main
	avr-objcopy -O ihex main main.hex
	avrdude -c usbasp -p ATMEGA328P -U flash:w:main.hex:i
