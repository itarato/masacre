import png
import argparse
import numpy as np
from perlin_numpy import generate_perlin_noise_2d

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Set image dimensions from command line or use defaults.")
    parser.add_argument('-s', '--size', type=int, default=2048, help='Image size (default: 1024)')
    args = parser.parse_args()

    size = args.size
    pixel_size = 3

    pixels_image = []
    pixels_texture = []

    for _ in range(0, size):
        pixels_image.append([255] * size * pixel_size)
        pixels_texture.append([255] * size * pixel_size)

    np.random.seed(0)
    noise_mask = generate_perlin_noise_2d((size, size), (8, 8))
    noise_terrain = generate_perlin_noise_2d((size // 16, size // 16), (64, 64))

    for y in range(0, size):
        for x in range(0, size):
            if noise_mask[y][x] > 0.3:
                v = int(((noise_terrain[y // 16][x // 16] + 1.0) / 2.0) * 20.0)
                pixels_texture[y][x * pixel_size:x * pixel_size + 3] = [50 + v, 50 + v, 50 + v]
            elif noise_mask[y][x] > 0.2:
                pixels_texture[y][x * pixel_size:x * pixel_size + 3] = [0, 0, 0]
            else:
                v = int(((noise_terrain[y // 16][x // 16] + 1.0) / 2.0) * 20.0)
                pixels_texture[y][x * pixel_size:x * pixel_size + 3] = [215 + v, 199 + v, 160 + v]


            if noise_mask[y][x] > 0.2:
                pixels_image[y][x * pixel_size:x * pixel_size + 3] = [0, 0, 0]

    png.from_array(pixels_image, 'RGB').save("map_image.png")
    png.from_array(pixels_texture, 'RGB').save("map_texture.png")
