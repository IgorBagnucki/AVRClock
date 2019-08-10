
def create_list_from_img(img):
    bitmap = []
    for y in range(0, img.height, 8):
        bitmap.append([])
        for x in range(0, img.width):
            byte = 0
            a = []
            for bit in range(0, 8):
                byte <<= 1
                (red, green, blue) = (img.getpixel((x, y + bit))[0], img.getpixel((x, y + bit))[1], img.getpixel((x, y + bit))[2])
                if (red, green, blue) != (0xFF, 0xFF, 0xFF):
                    byte |= 0
                else:
                    byte |= 1
            bitmap[-1].append(byte)
    return bitmap

def create_cpp_array_from_list(img, name):
    code = f'uint8_t {name}[{len(img)}][{len(img[0])}] PROGMEM = '
    code += '{\n'
    for vertical_line in img:
        code += '    {'
        for byte in vertical_line:
            code += f'{hex(byte)}, '
        code += '},\n'
    code += '};\n'
    return code

def create_cpp_code(juice):
    code = '#pragma once\n'
    code += '#include <avr/pgmspace.h>\n\n'
    code += juice
    return code
