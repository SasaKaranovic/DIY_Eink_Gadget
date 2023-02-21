import os
import re
import signal
import argparse
from eink_image import eink_image


def main(args):
    filename = args.file
    img = eink_image(filename)
    if args.size > 0:
        img.resize(args.size, args.size)

    if args.tricolor:
        img_data_bw, img_data_red = img.convert_red_black_image(args.inverse, args.reverse)
    else:
        img_data_bw = img.convert_black_and_white_image(args.inverse)

    img_width, img_height = img.get_image_size()

    icon_name = os.path.basename(args.file)
    icon_name = re.sub("[ -.<>?!@#$%^&*\(\)]", "_", icon_name)    # Remove unwanted characters from variable name

    with open(args.output, 'w') as f:
        # Write .h file header section
        f.write(f'#ifndef __ICON_{icon_name.upper()}_H_\n')
        f.write(f'// Prevent circular/multiple import\n')
        f.write(f'#define __ICON_{icon_name.upper()}_H_\n\n')
        f.write(f'// Image: {filename}\n')
        f.write(f'// Inverse: {args.inverse}\n')
        f.write(f'// Size: {img_width}x{img_height}\n')
        f.write(f'// Bytes: {len(img_data_bw)}\n')

        f.write(f'const char icon_buff_{icon_name}[] = {{')
        for i in range(len(img_data_bw)):
            if i%16==0:
                f.write('\n\t')
            f.write(f'0x{img_data_bw[i]:02X}, ')
        f.write('\n};\n\n\n')

        if args.tricolor:
            f.write(f'// Image: {filename}\n')
            f.write(f'// Inverse: {args.reverse}\n')
            f.write(f'// Size: {img_width}x{img_height}\n')
            f.write(f'// Bytes: {len(img_data_red)}\n')
            f.write(f'const char icon_buff_red_{icon_name}[] = {{')
            for i in range(len(img_data_red)):
                if i%16==0:
                    f.write('\n\t')
                f.write(f'0x{img_data_red[i]:02X}, ')
            f.write('\n};\n\n')

        f.write(f'const gfx_icon_typedef icon_{icon_name} = {{\n')
        f.write(f'\t (uint8_t *)icon_buff_{icon_name}, \n')
        f.write(f'\t {img_width},\t// Width\n')
        f.write(f'\t {img_height},\t// Height \n')
        f.write('\n};\n')

        if args.tricolor:
            f.write(f'\nconst gfx_icon_typedef icon_red_{icon_name} = {{\n')
            f.write(f'\t (uint8_t *)icon_buff_red_{icon_name}, \n')
            f.write(f'\t {img_width},\t// Width\n')
            f.write(f'\t {img_height},\t// Height \n')
            f.write('\n};\n')



        f.write('#endif\n')

def signal_handler(signal, frame):
    print('\r\nYou pressed Ctrl+C!')
    sys.exit(0)

if __name__ == '__main__':
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGINT, signal_handler)

    parser = argparse.ArgumentParser(prog='Image to C array', description='Convert image to byte array to be used in C as image array', formatter_class=lambda prog: argparse.HelpFormatter(prog,max_help_position=80))
    parser.add_argument('-f', '--file', type=str, required=True, help='Relative or absolute path to the input image file')
    parser.add_argument('-o', '--output', type=str, default='img.h',  help='Relative or absolute path to the output file')
    parser.add_argument('-i', '--inverse', default=False, action='store_true',  help='inverse colors (white -> black, black->white)')
    parser.add_argument('-s', '--size', type=int, default=-1,  help='Image size (will resize the image to be square of size-by-size)')
    parser.add_argument('-t', '--tricolor', default=False, action='store_true',  help='Tri-color display (ie. Black White Red)')
    parser.add_argument('-r', '--reverse', default=False, action='store_true',  help='Inverse red colors (white->red, red->white)')


    args = parser.parse_args()

    main(args)
