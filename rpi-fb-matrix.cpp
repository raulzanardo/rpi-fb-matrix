// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Program to copy the contents of the Raspberry Pi primary display to LED matrices.
// Author: Tony DiCola
#include <iostream>
#include <stdexcept>

// BCM specific libraries.
#include <fcntl.h>
#include <led-matrix.h>

#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

// X11 libraries for screen capture.
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "Config.h"
#include "GridTransformer.h"

using namespace std;
using namespace rgb_matrix;

// Global to keep track of if the program should run.
// Will be set false by a SIGINT handler when ctrl-c is
// pressed, then the main loop will cleanly exit.
volatile bool running = true;

// SIGINT handler to cleanly exit the program.
static void sigintHandler(int s) {
  running = false;
}

static void usage(const char* progname) {
    std::cerr << "Usage: " << progname << " [flags] [config-file]" << std::endl;
    std::cerr << "Flags:" << std::endl;
    rgb_matrix::RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_options;
    runtime_options.drop_privileges = -1;  // Need root
    rgb_matrix::PrintMatrixFlags(stderr, matrix_options, runtime_options);
}

int main(int argc, char** argv) {
  try {
    // Initialize from flags.
    rgb_matrix::RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_options;
    runtime_options.drop_privileges = -1;  // Need root
    if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                           &matrix_options, &runtime_options)) {
      usage(argv[0]);
      return 1;
    }

    // Read additional configuration from config file if it exists
    Config config(&matrix_options, argc >= 2 ? argv[1] : "/dev/null");
    cout << "Using config values: " << endl
         << " display_width: " << config.getDisplayWidth() << endl
         << " display_height: " << config.getDisplayHeight() << endl
         << " panel_width: " << config.getPanelWidth() << endl
         << " panel_height: " << config.getPanelHeight() << endl
         << " chain_length: " << config.getChainLength() << endl
         << " parallel_count: " << config.getParallelCount() << endl;

    // Set screen capture state depending on if a crop region is specified or not.
    // When not cropped grab the entire screen and resize it down to the LED display.
    // However when cropping is enabled instead grab the entire screen (by
    // setting the capture_width and capture_height to -1) and specify an offset
    // to the start of the crop rectangle.
    int capture_width = config.getDisplayWidth();
    int capture_height = config.getDisplayHeight();
    int x_offset = 0;
    int y_offset = 0;
    if (config.hasCropOrigin()) {
      cout << " crop_origin: (" << config.getCropX() << ", " << config.getCropY() << ")" << endl;
      capture_width = -1;
      capture_height = -1;
      x_offset = config.getCropX();
      y_offset = config.getCropY();
    }


    // Initialize matrix library.
    // Create canvas and apply GridTransformer.
    RGBMatrix *canvas = CreateMatrixFromOptions(matrix_options, runtime_options);
    if (config.hasTransformer()) {
      canvas->SetTransformer(new GridTransformer(config.getGridTransformer()));
    }
    canvas->Clear();

    // Initialize BCM functions and display capture class.
    //bcm_host_init();
    //BCMDisplayCapture displayCapture(capture_width, capture_height);

    // Loop forever waiting for Ctrl-C signal to quit.
    signal(SIGINT, sigintHandler);
    cout << "Press Ctrl-C to quit..." << endl;
    while (running) {
      // Capture the current display image.
      //displayCapture.capture();
      // Loop through the frame data and set the pixels on the matrix canvas.
      for (int y=0; y<config.getDisplayHeight(); ++y) {
        for (int x=0; x<config.getDisplayWidth(); ++x) {
          uint8_t red, green, blue;
          //displayCapture.getPixel(x+x_offset, y+y_offset, &red, &green, &blue);
          //canvas->SetPixel(x, y, red, green, blue);
        }
      }
      // Sleep for 25 milliseconds (40Hz refresh)
      usleep(25 * 1000);
    }
    canvas->Clear();
    delete canvas;
  }
  catch (const exception& ex) {
    cerr << ex.what() << endl;
    usage(argv[0]);
    return -1;
  }
  return 0;
}
