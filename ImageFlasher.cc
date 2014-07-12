#include "ImageFlasher.h"

//_________________________________________________
bool ImageFlasher::LoadPPM(const char *filename) {
   if (image_) {
      delete [] image_;
      image_ = NULL;
   }
   FILE *f = fopen(filename, "r");
   if (f == NULL) return false;
   char header_buf[256];
   const char *line = ReadLine(f, header_buf, sizeof(header_buf));
#define EXIT_WITH_MSG(m) { fprintf(stderr, "%s: %s |%s", filename, m, line); \
   fclose(f); return false; }
   if (sscanf(line, "P6 ") == EOF)
      EXIT_WITH_MSG("Can only handle P6 as PPM type.");
   line = ReadLine(f, header_buf, sizeof(header_buf));
   if (!line || sscanf(line, "%d %d ", &width_, &height_) != 2)
      EXIT_WITH_MSG("Width/height expected");
   //printf("%d x %d", width_,height_ );
   int value;
   line = ReadLine(f, header_buf, sizeof(header_buf));
   if (!line || sscanf(line, "%d ", &value) != 1 || value != 255)
      EXIT_WITH_MSG("Only 255 for maxval allowed.");
   const size_t pixel_count = width_ * height_;
   image_ = new Pixel [ pixel_count ];
   assert(sizeof(Pixel) == 3);   // we make that assumption.
   if (fread(image_, sizeof(Pixel), pixel_count, f) != pixel_count) {
      line = "";
      EXIT_WITH_MSG("Not enough pixels read.");
   }
#undef EXIT_WITH_MSG
   fclose(f);
   fprintf(stderr, "Read image with %dx%d\n", width_, height_);
   horizontal_position_ = 0;
   return true;
}
//_________________________________________________
void ImageFlasher::Run() {
   const int screen_height = matrix_->height();
   const int screen_width  = matrix_->width();
   while (running_) {
      if (image_ == NULL) {
         usleep(100 * 1000);
         continue;
      }
      for (int x = 0; x < screen_width; ++x) {
         for (int y = 0; y < screen_height; ++y) {
            const Pixel &p = getPixel((horizontal_position_ + x) % width_, y);
            // Display upside down on my desk. Lets flip :)
            int disp_x = screen_width - x  - 1 ;
            int disp_y = screen_height - y - 1;
            matrix_->SetPixel(disp_x, disp_y, p.red, p.green, p.blue);
         }
      }
      usleep(30 * 1000);
      //horizontal_position_ += screen_width;
      //running_=false;
   }
}
//_________________________________________________

