from PIL import Image
import sys
import os

WIDTH = 296
HEIGHT = 128

if len(sys.argv) != 2:
    print('Usage: bitmap.py <filename>')
    sys.exit(1)
filename = sys.argv[1]

img = Image.open(filename)
if not img:
    print('Error: can\'t open', filename)
    sys.exit(1)
if (img.width, img.height) != (WIDTH, HEIGHT):
    print('Error: image size should be 296 x 128 but is:', img.width, 'x', img.height)
    sys.exit(1)
bitmap = []
for x in range(0, img.height):
    for y in range(0, img.height, 8):
        byte = 0
        for bit in range(0, 8):
            byte <<= 1
            if img.getpixel((x, y)) == (0, 0, 0):
                byte |= 0
            elif img.getpixel((x, y)) == (0xFF, 0xFF, 0xFF):
                byte |= 1
            else:
                print('Error: image size should be monohrome but pixel x:', x, 'y:', y, 'is not')
                sys.exit(1)
        bitmap.append(bitmap)
out = open(os.path.splitext(filename)[1] + '.cpp', 'wt')
out.write(str(bitmap))
