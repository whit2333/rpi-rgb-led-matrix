#ifndef ImageFlasher_HH 
#define ImageFlasher_HH 1
#include "thread.h"
#include "led-matrix.h"

#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "RGBMatrixManipulator.h"

using std::min;
using std::max;

class ImageFlasher : public RGBMatrixManipulator {
   public:
      int   fFlashTime;
      bool  fRepeat;

   public:
      ImageFlasher(RGBMatrix *m)
         : RGBMatrixManipulator(m),fFlashTime(100),fRepeat(false), image_(NULL), horizontal_position_(0) {
         }

      // _very_ simplified. Can only read binary P6 PPM. Expects newlines in headers
      // Not really robust. Use at your own risk :)
      bool LoadPPM(const char *filename) ;

      void Run() ;

   private:
      struct Pixel {
         uint8_t red;
         uint8_t green;
         uint8_t blue;
      };

      // Read line, skip comments.
      char *ReadLine(FILE *f, char *buffer, size_t len) {
         char *result;
         do {
            result = fgets(buffer, len, f);
         } while (result != NULL && result[0] == '#');
         return result;
      }

      const Pixel &getPixel(int x, int y) {
         static Pixel dummy;
         if (x < 0 || x > width_ || y < 0 || y > height_) return dummy;
         return image_[x + width_ * y];
      }

      int width_;
      int height_;
      Pixel *image_;
      uint32_t horizontal_position_;
};

#endif

