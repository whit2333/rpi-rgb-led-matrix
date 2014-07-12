#!/usr/bin/python
import os, sys, getopt
from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw

def main(argv):
    inputfile = ''
    outputfile = 'test'
    outputtext = ''
    try:
        opts, args = getopt.getopt(argv,"ht:i:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print 'make_message.py -i <inputfile> -o <outputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'make_message.py -i <inputfile> -o <outputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
        elif opt in ("-t", "--tfile"):
            outputtext = arg
    
    outputfile+='.ppm'
    print 'Input file is "', inputfile
    print 'Output file is "', outputfile

    text = ((outputtext, (255, 0, 0)), ("", (0, 255, 0)), ("", (0, 0, 255)))
     
    font = ImageFont.truetype("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16)
    all_text = ""
    for text_color_pair in text:
        t = text_color_pair[0]
        all_text = all_text + t
     
    print(all_text)
    width, ignore = font.getsize(all_text)
    print(width)

    horizBuf = 0
    if width < 64:
        horizBuf = (64-width)/2
    im = Image.new("RGB", (64, 16), "black")
    #im = Image.new("RGB", (width + 30, 16), "black")
    draw = ImageDraw.Draw(im)
     
    x = 0;
    for text_color_pair in text:
        t = text_color_pair[0]
        c = text_color_pair[1]
        print("t=" + t + " " + str(c) + " " + str(x))
        draw.text((x, 0), t, c, font=font)
        x = x + font.getsize(t)[0]
     
    im.save(outputfile)
    os.system("/home/pi/bin/led-matrix 1 test.ppm")

if __name__ == "__main__":
   main(sys.argv[1:])
 
