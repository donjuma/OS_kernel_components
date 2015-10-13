#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#define MAX_LINE 1000

void display (GtkWidget* textview) {
    
    // create the window
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Text View Demo");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 200, 200);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL);
    
    // create a scrolled window
    GtkWidget* scroller = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    
    // pack the containers
    gtk_container_add (GTK_CONTAINER (scroller), textview);
    gtk_container_add (GTK_CONTAINER (window), scroller);
    gtk_widget_show_all (window);
}

int fill_buffer (GtkTextBuffer* buffer, gchar* filename) {
  
    GtkTextIter iter;  
    char line[MAX_LINE];
    FILE* input;
    
    if ((input = fopen(filename, "r")) == NULL) {
        printf("File %s not found\n", filename);
        return 1;
    }

    // initialize the buffer's iterator
    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
    
    // add each line from the file to the buffer
    while (fgets(line, MAX_LINE, input)) 
       gtk_text_buffer_insert (buffer, &iter, line, -1);
    
    return 0;
}

int main (int argc, char* argv[]) {

    // expect the input file name on the command line
    if (argc < 2) {
        printf("Usage: listdemo infile\n");
        return 1;
    }
       
    gtk_init (&argc, &argv);
    
    // establish the text view and its buffer
    GtkWidget* textview = gtk_text_view_new();
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview));  
    
    // fill the buffer with data from the file specified on command line
    if (fill_buffer(buffer, argv[1]) != 0) {
        printf("Error filling text view buffer\n");
        return 1;
    }
    
    // display the text view
    display(textview);
       
    gtk_main ();
    return 0;
}