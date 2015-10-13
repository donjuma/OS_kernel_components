#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#define MAX_LINE 1000
#define MAX_DEPTH 10

enum {
    POSITION = 0,
    FIRST_NAME,
    LAST_NAME,
    ROOM,
    COLUMNS
};

int build_tree (char* filename, GtkTreeStore* store) {
   
    FILE* input;
    if ((input = fopen(filename, "r")) == NULL) {
        printf("File %s not found\n", filename);
        return 1;
    }
    
    char line[MAX_LINE];
    char data[COLUMNS][MAX_LINE];
    char* token;
    int col;
    
    GtkTreeIter iter[MAX_DEPTH];
    
    // for each line of input
    while (fgets(line, MAX_LINE, input)) {
        line[strlen(line)-1] = '\0';
        token = strtok(line, "/");
        col = 0;
        
        // for each string in the line
        while (token != NULL) {
            strcpy(data[col], token);
            token = strtok(NULL, "/");
            col++;
        }
        
        // count the number of > characters at the start of the line
        int level;
        for (level=0; data[POSITION][level] == '>'; level++);
        
        // add the data as a new row in the tree store
        if (level == 0)
            gtk_tree_store_append (store, &(iter[level]), NULL);
        else
            gtk_tree_store_append (store, &(iter[level]), &(iter[level-1]));
            
        gtk_tree_store_set (store, &(iter[level]), 
                            POSITION, &(data[POSITION][level]),
                            FIRST_NAME, data[FIRST_NAME], 
                            LAST_NAME, data[LAST_NAME],
                            ROOM, data[ROOM],
                            -1);        
    }
    return 0;
    
}


void build_treeview (GtkWidget *treeview) {
    
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // add the Position to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "Position", renderer, "text", POSITION,
                NULL);    
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    // add the First name to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "First name", renderer, "text", FIRST_NAME,
                NULL);    
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    // add the Last name to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "Surname", renderer, "text", LAST_NAME,
                NULL);    
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    // add the room number to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
                "Room", renderer, "text", ROOM,
                NULL);    
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column); 

}

void display (GtkWidget *treeview) {
    
    // create the window
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "WWU Org Chart");
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

/***********************************************************************/
/*                added for selection handling                         */

void item_selected (GtkWidget *selection, gpointer data) {
    
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), 
        &model, &iter)) {
        
        gchar *name;
        gtk_tree_model_get (model, &iter, LAST_NAME, &name, -1);
        g_message("selected %s\n", name);
    }
}
/************************************************************************/

int main (int argc, char* argv[]) {

    // expect the input file name on the command line
    if (argc < 2) {
        printf("Usage: listdemo infile\n");
        return 1;
    }
    
    gtk_init (&argc, &argv);
    
    // build the list store from the file data
    GtkTreeStore *store = gtk_tree_store_new (COLUMNS, G_TYPE_STRING,
                                              G_TYPE_STRING, G_TYPE_STRING,
                                              G_TYPE_STRING);
                                              
    if (build_tree(argv[1], store) != 0) {
        printf("Error building tree from data\n");
        return 1;
    }
    
    // create the tree view of the list
    GtkWidget *treeview = gtk_tree_view_new ();
    build_treeview(treeview);
 
    /*****************************************************************/
    /*************** added for selection handling ********************/
    // set the tree selection object
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(treeview));
    
    // connect the selection callback function
    g_signal_connect (G_OBJECT(selection), "changed", 
                      G_CALLBACK(item_selected), NULL);

    /******************************************************************/
    
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