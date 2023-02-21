import os
import numpy as np
from PIL import Image
import numpy as np


class eink_image:
    image_data = []
    image_filepath = None
    width = 0
    height = 0

    def __init__(self, img_filepath):
        self.img_filepath = img_filepath
        self.img = Image.open(self.img_filepath)
        self.get_image_size()

    def resize(self, width, height):
        self.img = self.img.resize((width, height))
        self.width = width
        self.height = height

    def get_image_size(self):
            self.width, self.height = self.img.size
            return self.width, self.height

    def _format_bits(self, bits):
        buff = list()
        for i in bits:
            if i > 127:
                buff.append(1)
            else:
                buff.append(0)
        return buff

    def img_data_to_binary(self, img_data, reverse):
        buff = list()
        # Each byte is 8 vertical bits
        for bits in img_data:
            bits = self._format_bits(bits)
            byte = [int("".join(map(str, bits[i:i+8])), 2) for i in range(0, len(bits), 8)]

            if reverse:
                byte = np.invert(np.array(byte, dtype=np.uint8))
            buff.append(byte)

        buff = [item for sublist in buff for item in sublist]

        self.image_data = buff
        return self.image_data


    def convert_black_and_white_image(self, reverse=False):
        img = self.img.convert("L")
        img_data = np.asarray(img)
        img_data = img_data.T.tolist()

        return self.img_data_to_binary(img_data, reverse)

    # Very hacky, quick and dirty solution
    def convert_red_black_image(self, reverse_bw=False, reverse_red=False):
        threshold = 128

        img_data_bw = list()
        img_data_red = list()

        width, height = self.img.size

        for i in range(width):
            row_bw = list()
            row_red = list()
            for j in range(height):
                color = (self.img.getpixel((i, j)))
                # White ie (255, 255, 255)
                if color[0]>threshold and color[1]>threshold and color[2]>threshold:
                    row_bw.append(255)
                    row_red.append(0)
                # Black ie (0, 0, 0)
                elif color[0]<threshold and color[1]<threshold and color[2]<threshold:
                    row_bw.append(0)
                    row_red.append(0)
                # Any other color combination will be treated as red (255, 0, 0)
                else:
                    row_bw.append(0)
                    row_red.append(255)

            img_data_bw.append(row_bw)
            img_data_red.append(row_red)

        return (self.img_data_to_binary(img_data_bw, reverse_bw), self.img_data_to_binary(img_data_red, reverse_red))
