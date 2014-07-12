#include "thread.h"
#include "led-matrix.h"

#include "ImageScroller.h"
#include "ImageFlasher.h"
#include "getopt.h"
#include <math.h>
#include <string>


using std::min;
using std::max;


int main(int argc, char *argv[]) {

   const struct option longopts[] =
   {
      {"time",       required_argument,  0, 't'},
      {"image",     required_argument,  0, 'i'},
      //{"version",   no_argument,        0, 'v'},
      //{"help",      no_argument,        0, 'h'},
      //{"cflags",    no_argument,        0, 'c'},
      //{"ldflags",   no_argument,        0, 'd'},
      //{"int",       no_argument,        0, 'i'},
      //{"grid",      no_argument,        0, 'g'},
      {0,0,0,0}
   };

   int index = 0;
   int iarg  = 0;
   opterr    = 1; //turn off getopt error message
   int flash_time = 100;
   std::string image_file = "";

   while(iarg != -1) {
      iarg = getopt_long(argc, argv, "t:i:", longopts, &index);
      switch (iarg)
      {
         case 't':
            flash_time = atoi(optarg);
            break;

         case 'i':
            image_file = optarg;
            break;

         //case 'h':
         //   print_usage();
         //   exit(0);
         //   break;

         //case 'v':
         //   print_version();
         //   exit(0);
         //   break;

         //case 'l':
         //   print_libs();
         //   break;

         //case 'c':
         //   print_cflags();
         //   break;
         //case 'd':
         //   print_ldflags();
         //   break;

         //case 'g':
         //   print_grid();
         //   break;
      }
   }

   std::string theRest  = "";
   for (int i = optind; i < argc; i++) {
      theRest        += argv[i];
   }

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

   ImageFlasher *flasher = new ImageFlasher(&m);
   if (!flasher->LoadPPM(image_file.c_str()))
      return 1;
   image_gen = flasher;

   if (image_gen == NULL)
      return 1;

   RGBMatrixManipulator *updater = new DisplayUpdater(&m);
   updater->Start(10);  // high priority

   image_gen->Start();

   // Things are set up. Just wait for <RETURN> to be pressed.
   //printf("Press <RETURN> to exit and reset LEDs\n");
   //getchar();
   usleep(flash_time*1000);

   // Stopping threads and wait for them to join.
   delete image_gen;
   delete updater;

   // Final thing before exit: clear screen and update once, so that
   // we don't have random pixels burn
   m.ClearScreen();
   m.UpdateScreen();

   return 0;
}
