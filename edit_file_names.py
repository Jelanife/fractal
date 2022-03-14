import os
import sys
import os

def main():
    dir = "/home/jelani/sandbox/mandelbrot/pics"
    file_names = os.listdir(dir)

    for i, file in enumerate(sorted(file_names)):
        number = file[4:8]
        new_file = 'img_' + str(i).zfill(4) + '.bmp'
        os.rename(os.path.join(dir, file), os.path.join(dir, new_file))

if __name__ == "__main__":
    main()