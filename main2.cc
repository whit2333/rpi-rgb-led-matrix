#include "thread.h"
#include "led-matrix.h"

#include "ImageScroller.h"
#include "ImageFlasher.h"

using std::min;
using std::max;

// Simple generator that pulses through RGB and White.
class ColorPulseGenerator : public RGBMatrixManipulator {
public:
  ColorPulseGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    uint32_t count = 0;
    while (running_) {
      usleep(5000);
      ++count;
      int color = (count >> 9) % 6;
      int value = count & 0xFF;
      if (count & 0x100) value = 255 - value;
      int r, g, b;
      switch (color) {
      case 0: r = value; g = b = 0; break;
      case 1: r = g = value; b = 0; break;
      case 2: g = value; r = b = 0; break;
      case 3: g = b = value; r = 0; break;
      case 4: b = value; r = g = 0; break;
      default: r = g = b = value; break;
      }
      for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
          matrix_->SetPixel(x, y, r, g, b);
    }
  }
};

class SimpleSquare : public RGBMatrixManipulator {
public:
  SimpleSquare(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    // Diagonaly
    const int nbox   = width/height;
    for( int n = 0; n < nbox; ++n) {
    for (int x = 0; x < width; ++x) {
        matrix_->SetPixel(n*height, x, 255, 255, 255);
        if(n>0) matrix_->SetPixel(n*height-1, x, 255, 255, 255);
    }
    }
    for (int x = 0; x < width; ++x) {
        matrix_->SetPixel(x, x, 255, 255, 255);
        matrix_->SetPixel(height -1 - x, x, 255, 0, 255);
    }
    for (int x = 0; x < width; ++x) {
        matrix_->SetPixel(x, x, 255, 255, 255);
        matrix_->SetPixel(height -1 - x, x, 255, 0, 255);
    }
    for (int x = 0; x < width; ++x) {
      matrix_->SetPixel(x, 0, 255, 0, 0);
      matrix_->SetPixel(x, height - 1, 255, 255, 0);
    }
    for (int y = 0; y < height; ++y) {
      matrix_->SetPixel(0, y, 0, 0, 255);
      matrix_->SetPixel(width - 1, y, 0, 255, 0);
    }
  }
};

// Simple class that generates a rotating block on the screen.
class RotatingBlockGenerator : public RGBMatrixManipulator {
public:
  RotatingBlockGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}

  uint8_t scale_col(int val, int lo, int hi) {
    if (val < lo) return 0;
    if (val > hi) return 255;
    return 255 * (val - lo) / (hi - lo);
  }

  void Run() {
    const int cent_x = matrix_->width() / 2;
    const int cent_y = matrix_->height() / 2;

    // The square to rotate (inner square + black frame) needs to cover the
    // whole area, even if diagnoal.
    const int rotate_square = min(matrix_->width(), matrix_->height()) * 1.41;
    const int min_rotate = cent_x - rotate_square / 2;
    const int max_rotate = cent_x + rotate_square / 2;

    // The square to display is within the visible area.
    const int display_square = min(matrix_->width(), matrix_->height()) * 0.7;
    const int min_display = cent_x - display_square / 2;
    const int max_display = cent_x + display_square / 2;

    const float deg_to_rad = 2 * 3.14159265 / 360;
    int rotation = 0;
    while (running_) {
      ++rotation;
      usleep(15 * 1000);
      rotation %= 360;
      for (int x = min_rotate; x < max_rotate; ++x) {
        for (int y = min_rotate; y < max_rotate; ++y) {
          float disp_x, disp_y;
          Rotate(x - cent_x, y - cent_y,
                 deg_to_rad * rotation, &disp_x, &disp_y);
          if (x >= min_display && x < max_display &&
              y >= min_display && y < max_display) { // within display square
            matrix_->SetPixel(disp_x + cent_x, disp_y + cent_y,
                              scale_col(x, min_display, max_display),
                              255 - scale_col(y, min_display, max_display),
                              scale_col(y, min_display, max_display));
          } else {
            // black frame.
            matrix_->SetPixel(disp_x + cent_x, disp_y + cent_y, 0, 0, 0);
          }
        }
      }
    }
  }

private:
  void Rotate(int x, int y, float angle,
              float *new_x, float *new_y) {
    *new_x = x * cosf(angle) - y * sinf(angle);
    *new_y = x * sinf(angle) + y * cosf(angle);
  }
};


int main(int argc, char *argv[]) {
  int demo = 0;
  if (argc > 1) {
    demo = atoi(argv[1]);
  }
  fprintf(stderr, "Using demo %d\n", demo);

  GPIO io;
  if (!io.Init())
    return 1;

  RGBMatrix m(&io);
    
  RGBMatrixManipulator *image_gen = NULL;
  switch (demo) {
  case 0:
    image_gen = new RotatingBlockGenerator(&m);
    break;

  case 1:
    if (argc > 2) {
      ImageScroller *scroller = new ImageScroller(&m);
      if (!scroller->LoadPPM(argv[2]))
        return 1;
      image_gen = scroller;
    } else {
      fprintf(stderr, "Demo %d Requires PPM image as parameter", demo);
      return 1;
    }
    break;

  case 2:
    image_gen = new SimpleSquare(&m);
    break;

  case 3:
    if (argc > 2) {
      ImageFlasher *flasher = new ImageFlasher(&m);
      if (!flasher->LoadPPM(argv[2]))
        return 1;
      image_gen = flasher;
    } else {
      fprintf(stderr, "Demo %d Requires PPM image as parameter", demo);
      return 1;
    }
    break;

  default:
    image_gen = new ColorPulseGenerator(&m);
    break;
  }

  if (image_gen == NULL)
    return 1;

  RGBMatrixManipulator *updater = new DisplayUpdater(&m);
  updater->Start(10);  // high priority

  image_gen->Start();

  // Things are set up. Just wait for <RETURN> to be pressed.
  printf("Press <RETURN> to exit and reset LEDs\n");
  getchar();

  // Stopping threads and wait for them to join.
  delete image_gen;
  delete updater;

  // Final thing before exit: clear screen and update once, so that
  // we don't have random pixels burn
  m.ClearScreen();
  m.UpdateScreen();

  return 0;
}
