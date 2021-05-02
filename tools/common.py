
def create_list_from_img(img):
    bitmap = []
    for y in range(0, img.height, 8):
        for x in range(0, img.width):
            byte = 0
            for bit in range(0, 8):
                byte <<= 1
                (red, green, blue) = (img.getpixel((x, y + bit))[0], img.getpixel((x, y + bit))[1], img.getpixel((x, y + bit))[2])
                if (red, green, blue) != (0xFF, 0xFF, 0xFF):
                    byte |= 0
                else:
                    byte |= 1
            bitmap.append(byte)
    return bitmap

def compress_list(input):
    compressed = [0, input[0]]
    for element in input:
        if compressed[-1] == element and compressed[-2] < 0xFF:
            compressed[-2] += 1
        else:
            compressed.append(1)
            compressed.append(element)
    return compressed


def create_cpp_array_from_list(img, name):
    code = f'const uint8_t {name}[{len(img)}] PROGMEM = '
    code += '{\n'
    for element in img:
        code += f'{hex(element)}, '
    code += '};\n'
    return code

def create_cpp_code(juice):
    code = '#pragma once\n'
    code += '#include <avr/pgmspace.h>\n\n'
    code += juice
    return code
