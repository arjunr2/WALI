#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Open the X display
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    // Create a simple window
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 400, 300, 1, BlackPixel(display, screen), WhitePixel(display, screen));

    // Set window title
    XStoreName(display, window, "Hello World");

    // Create a graphics context (for drawing)
    GC gc = XCreateGC(display, window, 0, NULL);

    // Set the foreground color to black (for the text)
    XSetForeground(display, gc, BlackPixel(display, screen));

    // Show the window
    XMapWindow(display, window);
    XFlush(display);

    // Draw "Hello, World!" in the window
    XDrawString(display, window, gc, 50, 50, "Hello, World!", 13);

    // Wait for the window to close
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            break;
        }
    }

    // Clean up and close the display
    XFreeGC(display, gc);
    XCloseDisplay(display);

    return 0;
}