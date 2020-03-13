from PIL import Image
from os import listdir
from os.path import isfile, join
from natsort import natsorted
import sys

path = sys.argv[0]
result_name = sys.argv[1]
images_fn = natsorted([f for f in listdir(path) if isfile(join(path, f))])

images = []
for image in images_fn:
    im = Image.open(path + "\\" + image)
    images.append(im)

images[0].save(result_name + "_slow" + ".gif", save_all=True, append_images=images[1:], optimize=False, duration=200)
images[0].save(result_name + ".gif", save_all=True, append_images=images[1:], optimize=False)
