import os
import signal
import numpy as np
from PIL import Image
import numpy as np
import argparse
import re

def _format_bits(bits):
    buff = list()
    for i in bits:
        if i > 127:
            buff.append(1)
        else:
            buff.append(0)
    return buff

def img_to_binary(img_filepath, flatten=True):
    #Load image and convert to greyscale
    img = Image.open(img_filepath)
    img = img.convert("L")

    imgData = np.asarray(img)
    imgData = imgData.T.tolist()

    buff = list()
    # Each byte is 8 vertical bits
    for bits in imgData:
        bits = _format_bits(bits)
        byte = [int("".join(map(str, bits[i:i+8])), 2) for i in range(0, len(bits), 8)]
        buff.append(byte)

    if flatten:
        buff = [item for sublist in buff for item in sublist]

    return buff

def main(args):
    img_data = img_to_binary(args.file)
    filename = os.path.basename(args.file)
    filename = re.sub("[ -.<>?!@#$%^&*\(\)]", "_", filename)    # Remove unwanted characters from variable name

    with open(args.output, 'w') as f:

        f.write(f'// Image: {filename}\n')
        f.write(f'// Reverse: {args.reverse}\n')
        f.write(f'// Bytes: {len(img_data)}\n')
        f.write(f'const char img_{filename}[] = {{')

        for i in range(len(img_data)):
            if args.reverse:
                img_data[i] = np.invert(np.array(img_data[i], dtype=np.uint8))

            if i%16==0:
                f.write('\n\t')

            f.write(f'0x{img_data[i]:02X}, ')

        f.write('\n};\n\n')


def signal_handler(signal, frame):
    print('\r\nYou pressed Ctrl+C!')
    sys.exit(0)

if __name__ == '__main__':
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGINT, signal_handler)

    parser = argparse.ArgumentParser(prog='Image to C array', description='Convert image to byte array to be used in C as image array', formatter_class=lambda prog: argparse.HelpFormatter(prog,max_help_position=80))
    parser.add_argument('-f', '--file', type=str, required=True, help='Relative or absolute path to the input image file')
    parser.add_argument('-o', '--output', type=str, default='img.c',  help='Relative or absolute path to the output file')
    parser.add_argument('-r', '--reverse', default=False, action='store_true',  help='Reverse colors (white -> black, black->white)')
    args = parser.parse_args()

    main(args)
