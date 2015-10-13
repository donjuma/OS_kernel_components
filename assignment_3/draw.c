#include <gtk/gtk.h>
#include <math.h>

// dimensions of the window
#define WIDTH   640
#define HEIGHT  480

// Xoom factor for the graph.  Zoom in with larger numbers
#define ZOOM_X  100.0
#define ZOOM_Y  100.0

// Graph resolution: pixels between points
#define DX  2.0
#define DY  2.0

// the function to be plotted
gfloat f (gfloat x) {
    return sin (x);
}

// the call-back function for the draw event
void on_draw_event (GtkWidget *widget, gpointer user_data) {

    // Create the drawing context
    cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(widget));

    // variables for function points and increments
    gdouble dx = DX, dy = DY;
    gdouble x;

    // range of values to be displayed
    gdouble min_x, min_y, max_x, max_y;

    /* Determine window dimensions */
    GdkRectangle da;
    gdk_window_get_geometry (gtk_widget_get_window(widget),
            &da.x,
            &da.y,
            &da.width,
            &da.height);

    /* Define a clipping zone  */
    cairo_rectangle (cr, da.x, da.y, da.width, da.height);
    cairo_clip (cr);

    /* Draw a pale background on the clipping zone */
    cairo_set_source_rgb (cr, 0.9, 1.0, 1.0);
    cairo_paint (cr);

    /* move the graph origin to the center of the area */
    cairo_translate (cr, da.width/3, da.height/2);

    // scale by the zoom factor, inverting Y axis
    // so that Y values increase up the screen
    cairo_scale (cr, ZOOM_X, -ZOOM_Y);

    // get the minimum and maximum values of x and y
    // after origin translation and scaling
    cairo_clip_extents (cr, &min_x, &min_y, &max_x, &max_y);

    // apply the scale factor from x, y values to pixels
    cairo_device_to_user_distance (cr, &dx, &dy);

    // draw x and y axis
    cairo_set_line_width (cr, 2 * dx);
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, min_x, 0.0);
    cairo_line_to (cr, max_x, 0.0);
    cairo_move_to (cr, 0.0, min_y);
    cairo_line_to (cr, 0.0, max_y);
    cairo_stroke (cr);

    /* Link the data points */
    for (x = min_x; x < max_x; x += dx)
        cairo_line_to (cr, x, f (x));

    /* Draw the curve */
    cairo_set_line_width (cr, dx);
    cairo_set_source_rgb (cr, 1.0, 0.0, 0.8);
    cairo_stroke (cr);

    cairo_destroy (cr);
}


int main (int argc, char **argv) {

    GtkWidget *window;
    GtkWidget *da;

    gtk_init (&argc, &argv);

    // set up the GTK window
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_window_set_title (GTK_WINDOW (window), "Graph drawing");
    g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    // establish a drawing area in the window
    da = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (window), da);

    // connect a callback function to the draw event
    g_signal_connect (G_OBJECT (da),
            "draw",
            G_CALLBACK (on_draw_event),
            NULL);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
