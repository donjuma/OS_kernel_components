#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#define MAX_LINE 100

enum {
    W_NUMBER = 0,
    FIRST_NAME,
    LAST_NAME,
    CS_MAJOR,
    GPA,
    IMAGE,
    COLUMNS
};


int build_list (char* filename, GtkListStore* store) {
   
    FILE* input;
    if ((input = fopen(filename, "r")) == NULL) {
        printf("File %s not found\n", filename);
        return 1;
    }
    
    char line[MAX_LINE];
    char data[COLUMNS][MAX_LINE];
    char* token;
    int col;
    gfloat gpa;
    gboolean ismajor;
    GError *error = NULL;
    GdkPixbuf* image1 = gdk_pixbuf_new_from_file("icon.png", &error);
    GdkPixbuf* image2 = gdk_pixbuf_new_from_file("icon2.png", &error);
    GdkPixbuf* image;
    
    GtkTreeIter iter;
    
    // for each line of input
    while (fgets(line, MAX_LINE, input)) {
        line[strlen(line)-1] = '\0';
        token = strtok(line, " ");
        col = 0;
        
        // for each string in the line
        while (token != NULL) {
            strcpy(data[col], token);
            token = strtok(NULL, " ");
            col++;
        }
        
        // add the data as a new row in the list store
        gtk_list_store_append (store, &iter);
        sscanf (data[GPA], "%f", &gpa);
        ismajor = (data[CS_MAJOR][0] == 'Y') ? TRUE : FALSE;
        image = (data[CS_MAJOR][0] == 'Y') ? image1 : image2;
        gtk_list_store_set (store, &iter, W_NUMBER, data[W_NUMBER],
                            FIRST_NAME, data[FIRST_NAME], 
                            LAST_NAME, data[LAST_NAME],
                            CS_MAJOR, ismajor,
                            IMAGE, image,
                            GPA, gpa, -1);        
    }
    return 0;
    
}

void gpa_cell_display (GtkTreeViewColumn* col, 
                       GtkCellRenderer* renderer,
                       GtkTreeModel* model,
                       GtkTreeIter* iter,
                       gpointer data )  {
    gfloat gpa;
    gchar buf[20];
    
    gtk_tree_model_get (model, iter, GPA, &gpa, -1);
    g_snprintf (buf, 20, "%.2f", gpa);
    g_object_set (renderer, "text", buf, NULL);
}


void build_treeview (GtkWidget *treeview) {
    
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // add the W-number to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "W-number", renderer, "text", W_NUMBER,
                NULL); 
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    // add the first name to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "First name", renderer, "text", FIRST_NAME,
                NULL); 
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    // add the last name to the treeview, with an icon
    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", IMAGE, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer, "text", LAST_NAME,
                NULL); 
    
    gtk_tree_view_column_set_title(column, "Surname");
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    //****************************************************added for sorting
    // set the sort column and indicator
    gtk_tree_view_column_set_sort_column_id(column, LAST_NAME);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
    //***************************************************
    
     // add the GPA to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_title (column, "GPA");
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func (column, renderer, gpa_cell_display,
                                             NULL, NULL);

    // add the CS major indicator to the treeview as a checkbox
    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes (
                "CS Major?", renderer, "active", CS_MAJOR,
                NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);
}

void display (GtkWidget *treeview) {
    
    // create the window
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Student List");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 200, 200);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL);
    
    // create a scrolled window
    GtkWidget* scroller = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    
    // pack the containers
    gtk_container_add (GTK_CONTAINER (scroller), treeview);
    gtk_container_add (GTK_CONTAINER (window), scroller);
    gtk_widget_show_all (window);
}

//****************************************************added for sorting
// define sort order on names
gint sort_last_names (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {
    
    gchar *name1, *name2;
    gtk_tree_model_get (model, a, LAST_NAME, &name1, -1);
    gtk_tree_model_get (model, b, LAST_NAME, &name2, -1);
    
    int order = strcmp(name1, name2);
    g_free(name1);
    g_free(name2);
    
    return -order;
}
//*************************************************************************


int main (int argc, char* argv[]) {

    // expect the input file name on the command line
    if (argc < 2) {
        printf("Usage: listdemo infile\n");
        return 1;
    }
    
    gtk_init (&argc, &argv);
    
    // build the list store from the file data
    GtkListStore *store = gtk_list_store_new (COLUMNS, G_TYPE_STRING,
                                              G_TYPE_STRING, G_TYPE_STRING,
                                              G_TYPE_BOOLEAN, G_TYPE_FLOAT, 
                                              GDK_TYPE_PIXBUF                                            );
    //****************************************************added for sorting
    // make the treeview sortable on last name
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE (store);
    
    gtk_tree_sortable_set_sort_func (sortable, LAST_NAME, sort_last_names, 
                                     GINT_TO_POINTER (LAST_NAME), NULL);
    
    gtk_tree_sortable_set_sort_column_id (sortable, LAST_NAME,
                                          GTK_SORT_ASCENDING);
    //****************************************************************
    
    if (build_list(argv[1], store) != 0) {
        printf("Error building list from data\n");
        return 1;
    }
    
    // create the tree view of the list
    GtkWidget *treeview = gtk_tree_view_new ();
    build_treeview(treeview);
    
    // add the tree model to the tree view
    gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
    
    // unreference the model so that it will be destroyed when the tree
    // view is destroyed
    g_object_unref (store);
    
    // display the tree view
    display (treeview);
    
    gtk_main ();

    return 0;
}