#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <time.h>
#include <langinfo.h>

#define MAX_DEPTH 10

enum {
    ENTRY_NAME,
    ENTRY_PATH
};

enum{
    IMAGE,
    DIRENT_NAME,
    DIRENT_SIZE,
    DIRENT_MOD,
    DIRENT_PATH,
    DIRENT_TIME
};

GtkTreeStore *store;
GtkListStore *lstore;
GtkWidget *textview_lines, *textview_hex, *textview_ascii;

//Build the FileSystem Tree Store
int build_tree(GtkTreeStore* store) {
    GtkTreeIter iter, child;

    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set (store, &iter, ENTRY_NAME, "/", ENTRY_PATH, "/", -1);

    gtk_tree_store_append(store, &child, &iter);

    return 0;
}

//Build the Directory Entry List Tree Store
int build_list(GtkListStore* lstore){
    GtkTreeIter iter;

    gtk_list_store_append(lstore, &iter);

    return 0;
}

int build_box(GtkWidget *myBox, GtkWidget *textview_lines, GtkWidget *textview_hex, GtkWidget *textview_ascii){
    gtk_box_pack_start(GTK_BOX(myBox), textview_lines, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(myBox), textview_hex, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(myBox), textview_ascii, 1, 1, 0);


    return 0;
}

static void destroy (GtkWidget*, gpointer);


void build_treeview (GtkWidget *treeview) {

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // add the Position to the treeview
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
            "File System", renderer, "text", ENTRY_NAME,
            NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

void build_listview(GtkWidget *listview){
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

       column = gtk_tree_view_column_new();
       renderer = gtk_cell_renderer_pixbuf_new();
       gtk_tree_view_column_pack_start(column, renderer, FALSE);
       gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", IMAGE, NULL);
       renderer = gtk_cell_renderer_text_new();
       gtk_tree_view_column_pack_start(column, renderer, FALSE);
       gtk_tree_view_column_set_attributes (column, renderer, "text", DIRENT_NAME, NULL);
       gtk_tree_view_column_set_title(column, "Name");
       gtk_tree_view_append_column (GTK_TREE_VIEW (listview), column);
    /*
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
            "Name", renderer, "text", DIRENT_NAME, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (listview), column);
*/
    gtk_tree_view_column_set_sort_column_id(column, DIRENT_NAME);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
            "Size", renderer, "text", DIRENT_SIZE, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (listview), column);
    gtk_tree_view_column_set_sort_column_id(column, DIRENT_SIZE);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);


    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes (
            "Date", renderer, "text", DIRENT_MOD, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (listview), column);
    gtk_tree_view_column_set_sort_column_id(column, DIRENT_MOD);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);


}

void build_textviews(GtkWidget *textview_lines, GtkWidget *textview_hex, GtkWidget *textview_ascii){
    GtkTextIter linesIter, hexIter, asciiIter;
    char line[1000];
    char hex[1000];
    char ascii[1000];

    //Disable editing of the textviews
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_lines), 0);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_hex), 0);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_ascii), 0);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_ascii), GTK_WRAP_CHAR);

    //Obtain Buffers
    GtkTextBuffer* line_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_lines));
    GtkTextBuffer* hex_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_hex));
    GtkTextBuffer* ascii_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_ascii));

    //Obtain Buffer Iters
    gtk_text_buffer_get_iter_at_offset(line_buffer, &linesIter, 0);
    gtk_text_buffer_get_iter_at_offset(hex_buffer, &hexIter, 0);
    gtk_text_buffer_get_iter_at_offset(ascii_buffer, &asciiIter, 0);

    //Enable Overwrite mode
    gtk_text_view_set_overwrite(GTK_TEXT_VIEW(textview_lines), 1);
    gtk_text_view_set_overwrite(GTK_TEXT_VIEW(textview_hex), 1);
    gtk_text_view_set_overwrite(GTK_TEXT_VIEW(textview_ascii), 1);

    GdkRGBA color = {.75, .75, .75, 1.0};
    gtk_widget_override_background_color (textview_lines, GTK_STATE_NORMAL, &color);
}

void update_editor(char *contents, char *lines, char *ascii){
    GtkTextIter linesIter, hexIter, asciiIter;

    GtkTextBuffer* line_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_lines));
    GtkTextBuffer* hex_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_hex));
    GtkTextBuffer* ascii_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textview_ascii));

    gtk_text_buffer_set_text (ascii_buffer, ascii, -1);
    gtk_text_buffer_set_text (line_buffer, lines, -1);
    gtk_text_buffer_set_text (hex_buffer, contents, -1);
}

void display (GtkWidget *treeview, GtkWidget *listview, GtkWidget *myBox) {

    // create the window
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "File Browser");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (window, 750, 600);
    g_signal_connect (window, "delete_event", gtk_main_quit, NULL);
    g_signal_connect (G_OBJECT (window), "destroy",
            G_CALLBACK (destroy), NULL);


    GtkWidget* hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    GtkWidget* vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

    //Create the Scrollable Containers
    GtkWidget* scroller = gtk_scrolled_window_new (NULL, NULL);
    GtkWidget* list_scroller = gtk_scrolled_window_new (NULL, NULL);
    GtkWidget* hex_scroller = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (list_scroller),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (hex_scroller),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);


    gtk_container_add (GTK_CONTAINER (scroller), treeview);
    gtk_container_add (GTK_CONTAINER (list_scroller), listview);
    gtk_container_add (GTK_CONTAINER (hex_scroller), myBox);

    gtk_paned_pack1 (GTK_PANED (hpaned), scroller, TRUE, FALSE);
    gtk_paned_pack2 (GTK_PANED (hpaned), vpaned, TRUE, FALSE);
    gtk_paned_pack1 (GTK_PANED (vpaned), list_scroller, TRUE, FALSE);
    gtk_paned_pack2 (GTK_PANED (vpaned), hex_scroller, TRUE, FALSE);

    //Set the initial spacing between the panes
    gtk_paned_set_position(GTK_PANED(hpaned), 200);
    gtk_paned_set_position(GTK_PANED(vpaned), 400);


    gtk_container_add (GTK_CONTAINER (window), hpaned);
    gtk_widget_show_all (window);

}

int get_file_subsystem(GtkWidget *selection, gpointer data){

    GtkTreeModel *model;
    GtkTreeIter iter, child, children, grandchild;
    int files;
    int i, met;
    struct dirent **entries;
    struct stat meta;
    DIR *dp;
    char pathname[MAXPATHLEN];
    char dirpath[MAXPATHLEN];


    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), &model, &iter)){
        gchar *path;

        gtk_tree_model_get (model, &iter, ENTRY_PATH, &path, -1);

        while(gtk_tree_model_iter_has_child(model, &iter)){
            gtk_tree_model_iter_children(model, &children, &iter);
            gtk_tree_store_remove(store, &children);
        }

        if (path == NULL){
            return -1;
        }

        files = scandir(path, &entries, NULL, alphasort);
        if(files > 0){
            for (i=0; i < files; i++){
                if(strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0){
                    continue;
                }

                sprintf(dirpath, "%s%s", path, entries[i]->d_name);

                if (met = (stat(dirpath, &meta) == 0)){
                    if (S_ISDIR(meta.st_mode)){
                        sprintf(pathname, "%s%s%s", path, entries[i]->d_name, "/");
                        gtk_tree_store_append(store, &child, &iter);
                        gtk_tree_store_set(store, &child, ENTRY_NAME, entries[i]->d_name, ENTRY_PATH, pathname, -1);
                        gtk_tree_store_append(store, &grandchild, &child);
                    }
                }
            }
        }
    }

    return 0;
}

int update_dir_listing(GtkWidget *selection, gpointer data){
    GtkTreeModel *model;
    GtkTreeIter iter, child, children, grandchild;
    int files, filesize;
    int i, met;
    struct dirent **entries;
    struct stat meta;
    DIR *dp;
    char pathname[MAXPATHLEN];
    char dirpath[MAXPATHLEN];
    char dirpath2[MAXPATHLEN];
    char modification_date[1024];
    char modification_time[1024];
    char fsize[1024];
    struct tm time;
    struct tm time2;
    GError *error = NULL;
    GtkIconTheme *icon_theme;
    GdkPixbuf* pixbuf;

    icon_theme = gtk_icon_theme_get_default ();
    pixbuf = gtk_icon_theme_load_icon (icon_theme, "text-x-generic", 48, 0, &error);
    //gdk_pixbuf_new_from_file("icon.png", &error);

    gtk_list_store_clear(lstore);

    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), &model, &iter)){
        gchar *path;
        gchar *name;

        gtk_tree_model_get (model, &iter, ENTRY_PATH, &path, ENTRY_NAME, &name, -1);

        if (path == NULL){
            return -1;
        }

        if (name == NULL){
            return -1;
        }

        sprintf(dirpath, "%s%s%s", path, name, "/" );
        files = scandir(path, &entries, NULL, alphasort);

        if(files > 0){
            for (i=0; i < files; i++){
                if(strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0){
                    continue;
                }

                sprintf(dirpath2, "%s%s", path, entries[i]->d_name);
                if (met = (stat(dirpath2, &meta) == 0)){

                    if ((S_ISDIR(meta.st_mode))){
                        pixbuf = gtk_icon_theme_load_icon (icon_theme, "folder", 16, 0, &error);
                    }else{
                        pixbuf = gtk_icon_theme_load_icon (icon_theme, "text-x-generic", 16, 0, &error);
                    }

                    filesize = (int)meta.st_size;
                    sprintf(fsize, "%d B", filesize);
                    localtime_r(&meta.st_mtime, &time);
                    localtime_r(&meta.st_mtime, &time2);
                    strftime(modification_date, sizeof(modification_date), "%D %I:%M %p", &time);
                    strftime(modification_time, sizeof(modification_time), "%Y%m%d%I%M", &time2);
                    sprintf(pathname, "%s%s%s", path, entries[i]->d_name, "/");
                    gtk_list_store_append(lstore, &child);
                    gtk_list_store_set(lstore, &child, IMAGE, pixbuf, DIRENT_NAME, entries[i]->d_name,
                            DIRENT_SIZE, fsize, DIRENT_MOD, modification_date,
                            DIRENT_PATH, dirpath2, DIRENT_TIME, modification_time, -1);
                }
            }
        }
    }
}


gint sort_dir_names(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data){
    gchar *name1, *name2;
    gtk_tree_model_get(model, a, DIRENT_NAME, &name1, -1);
    gtk_tree_model_get (model, b, DIRENT_NAME, &name2, -1);

    int order = strcmp(name1, name2);
    g_free(name1);
    g_free(name2);

    return -order;
}
gint sort_dir_dates(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data){
    gchar *name1, *name2;
    gtk_tree_model_get(model, a, DIRENT_TIME, &name1, -1);
    gtk_tree_model_get (model, b, DIRENT_TIME, &name2, -1);

    long long first = atoll(name1);
    long long second = atoll(name2);

    g_free(name1);
    g_free(name2);

    if (first < second){
        return -1;
    }
    if (first > second){
        return 1;
    }
    return 0;
}
gint sort_dir_sizes(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data){
    gchar *name1, *name2;
    char newName1[32] = "";
    char newName2[32] = "";
    gtk_tree_model_get(model, a, DIRENT_SIZE, &name1, -1);
    gtk_tree_model_get (model, b, DIRENT_SIZE, &name2, -1);

    memcpy(newName1, name1, strlen(name1) - 2);
    memcpy(newName2, name2, strlen(name2) - 2);

    newName1[strlen(name1) - 2] = '\0';
    newName2[strlen(name2) - 2] = '\0';


    int size1 = atoi(newName1);
    int size2 = atoi(newName2);

    g_free(name1);
    g_free(name2);

    if (size1 < size2){
        return -1;
    }
    if (size1 > size2){
        return 1;
    }
    return 0;
}

int ascii_to_hex(char c){
        int num = (int) c;
        if(num < 58 && num > 47)
        {
                return num - 48;
        }
        if(num < 103 && num > 96)
        {
                return num - 87;
        }
        return num;
}

void file_selected(GtkWidget *selection, gpointer data) {
    GtkTreeModel *model;
    GtkTreeIter iter, child;
    char *contents, *lines, *ascii;
    FILE *fp;
    int c;
    int n = 0;
    int linecount = 0;
    int wordcount = 0;
    int temp;
    char counter[1024];
    char tempHex[1024];
    struct stat meta;
    unsigned int fp_size;
    int met;
    int i = 0;
    unsigned char sum, c1, c2;

    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection),
                &model, &iter)) {

        gchar *name;
        gchar *path;
        gtk_tree_model_get (model, &iter, DIRENT_NAME, &name, -1);
        gtk_tree_model_get (model, &iter, DIRENT_PATH, &path, -1);


        if (met = (stat(path, &meta) == 0)){
            if (!(S_ISDIR(meta.st_mode))){
                fp = fopen(path, "r");
                if(fp == NULL)
                {
                    perror("Error in opening file");
                    return;
                }
                fseek(fp, 0, SEEK_END);
                fp_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                contents = (char *)malloc(3 * sizeof(char) * fp_size*8+20);
                lines = (char *)malloc(3 * sizeof(char) * fp_size*8+20);
                ascii = (char *)malloc(3 * sizeof(char) * fp_size*8+20);

                //while(c = fgetc(fp)  != EOF){
                memset(counter, 0, sizeof(counter));
                memset(lines, 0, sizeof(lines));
                memset(contents, 0, sizeof(contents));
                memset(ascii, 0, sizeof(ascii));
                sprintf(counter, "00000000\n");
                strcat(lines, counter);
                for (i=0; i<fp_size/2;i++){
                    c = fgetc(fp);
                    if ((c > 32) && (c < 127)){
                        sprintf(tempHex, "%c", c);
                        strcat(ascii, tempHex);
                        //memset(&tempHex[0], 0, sizeof(tempHex));
                    }else{
                        strcat(ascii, ".");
                    }
                    c1 = ascii_to_hex(c);

                    c = fgetc(fp);
                    if ((c > 32) && (c < 127)){
                        sprintf(tempHex, "%c", c);
                        strcat(ascii, tempHex);
                        //memset(&tempHex[0], 0, sizeof(tempHex));
                    }else{
                        strcat(ascii, ".");
                    }
                    c2 = ascii_to_hex(c);

                    sum = c1<<4 | c2;
                    sprintf(tempHex, "%02x ", sum);
                    strcat(contents, tempHex);
                    wordcount++;
                    if (wordcount > 15){
                        wordcount = 0;
                        linecount = (linecount + 16);
                        strcat(contents, "\n");
                        memset(counter, 0, sizeof(counter));
                        sprintf(counter, "%08x\n", linecount);
                        strcat(lines, counter);
                    }
                }

                fclose(fp);
                update_editor(contents, lines, ascii);
            }
        }
    }
}

void item_selected(GtkWidget *selection, gpointer data) {

    GtkTreeModel *model;
    GtkTreeIter iter, child;

    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection),
                &model, &iter)) {

        gchar *name;
        gchar *path;
        gtk_tree_model_get (model, &iter, ENTRY_NAME, &name, -1);
        gtk_tree_model_get (model, &iter, ENTRY_PATH, &path, -1);


        get_file_subsystem(selection, data);
        update_dir_listing(selection, data);

    }
}

void onRowExpanded(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer selection){
  //  GtkTreeSelection *selection;
//    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(user_data));
    gtk_tree_selection_select_path(GTK_TREE_SELECTION(selection), path);
    gtk_tree_selection_select_iter(GTK_TREE_SELECTION(selection), iter);
}

void onRowActivated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data){
    gboolean result = gtk_tree_view_expand_row(GTK_TREE_VIEW(tree_view), path, 0);
    //gtk_tree_selection_select_path(GTK_TREE_SELECTION(user_data), path);
}

int main (int argc, char *argv[]){

    GtkWidget *window, *hpaned, *vpaned;
    GtkTreeIter   iter, child;

    gtk_init (&argc, &argv);

    //Create the FileSystem Tree, Directory Listing
    store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    lstore = gtk_list_store_new(6, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    //Make the Directory Listing Sortable
    GtkTreeSortable *sortable = GTK_TREE_SORTABLE (lstore);

    gtk_tree_sortable_set_sort_func(sortable, DIRENT_NAME, sort_dir_names, GINT_TO_POINTER(DIRENT_NAME), NULL);
    gtk_tree_sortable_set_sort_func(sortable, DIRENT_MOD, sort_dir_dates, GINT_TO_POINTER(DIRENT_MOD), NULL);
    gtk_tree_sortable_set_sort_func(sortable, DIRENT_SIZE, sort_dir_sizes, GINT_TO_POINTER(DIRENT_SIZE), NULL);

    gtk_tree_sortable_set_sort_column_id(sortable, DIRENT_NAME, GTK_SORT_DESCENDING);

    GtkWidget *treeview = gtk_tree_view_new();
    GtkWidget *listview = gtk_tree_view_new();
    GtkWidget *myBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    textview_lines = gtk_text_view_new();
    textview_hex = gtk_text_view_new();
    textview_ascii = gtk_text_view_new();

    //BUILD THE MODELS
    build_tree(store);
    build_list(lstore);
    build_box(myBox, textview_lines, textview_hex, textview_ascii);


    build_treeview(treeview);
    build_listview(listview);
    build_textviews(textview_lines, textview_hex, textview_ascii);

    gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(treeview), TRUE);

    /*****************************************************************/
    /*************** added for selection handling ********************/
    // set the tree selection object
    GtkTreeSelection *selection;
    GtkTreeSelection *dir_selection;
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(treeview));
    dir_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(listview));


    //g_signal_connect(treeview, "clicked", G_CALLBACK(item_selected), selection);
    g_signal_connect(treeview, "row-activated", G_CALLBACK(onRowActivated), selection);
    g_signal_connect(treeview, "row-expanded", G_CALLBACK(onRowExpanded), selection);
    // connect the selection callback function

    g_signal_connect (G_OBJECT(selection), "changed",
            G_CALLBACK(item_selected), store);

    g_signal_connect (G_OBJECT(dir_selection), "changed",
            G_CALLBACK(file_selected), NULL);

    /******************************************************************/

    // add the tree models to the tree views
    gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
    gtk_tree_view_set_model (GTK_TREE_VIEW (listview), GTK_TREE_MODEL (lstore));

    // unreference the model so that it will be destroyed when the tree
    // view is destroyed
    g_object_unref (store);
    g_object_unref (lstore);

    // display the views
    display (treeview, listview, myBox);

    gtk_main ();
    return 0;
}

    static void
destroy (GtkWidget *window,
        gpointer data)
{
    gtk_main_quit ();
}
