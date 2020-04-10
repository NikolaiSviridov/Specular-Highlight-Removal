from PIL import Image
from PIL.ExifTags import TAGS
# import sys

def convert(input, output):
    im = Image.open(input)
    # box = (100, 100, 1000, 1000)
    # im = im.crop(box)
    im.save(output)


def resolution(input):
    img = Image.open(input)
    info = img._getexif()
    exifObj = {}
    if info != None:
        for tag, value in info.items():
            decoded = TAGS.get(tag, tag)
            exifObj[decoded] = value
    print(exifObj)
    print(exifObj['XResolution'][0]/exifObj['XResolution'][1])



if __name__ == "__main__":
    f1 = "fish.ppm"
    f2 = "_fish.jpg"
    f3 = "__fish.ppm"
    # convert(f1, f2)
    # convert(f2, f3)
    resolution(f1)
    resolution(f2)
    resolution(f3)

