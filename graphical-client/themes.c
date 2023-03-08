#include <gtk/gtk.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include "main.h"
#include "../common/utils.h"
#include "themes.h"

/* This is all done asynchronously so that the information can be updated live.*/
void* on_install_button_click_async(void* data) {
  char* theme_name = (char*) data;
  char* install_str = strjoin("pkexec /home/testing/Documents/plymouth-manager/target/pm install ", theme_name);
  FILE* cmd = popen(install_str, "r");
  char buffer[PATH_MAX];
  while((fgets(buffer, PATH_MAX, cmd)) != NULL) {
      gtk_label_set_text(GTK_LABEL(footer), buffer);
  }
}

static void on_install_button_click(GtkWidget* widget, gpointer theme_name) {
  pthread_t thread_id; 
  pthread_create(&thread_id, NULL, on_install_button_click_async, theme_name);
}
/* *****************************************************************************/

char* fancify_text(char* text) {
  char* result = malloc(strlen(text));
  int new_word = 1;
  for(int i = 0; i < strlen(text); i++) {
    if(new_word == 1 && text[i] >= 'a' && text[i] <= 'z') {
      result[i] = text[i] - 32; //Work some ascii magic.
      new_word = 0;
    } else if (text[i] == '_') {
        result[i] = ' ';
        new_word = 1;
    } else {
      result[i] = text[i];
    }
  }
  return result;
}


GtkWidget* get_optimal_theme_image(char* theme_name) {
  char path[PATH_MAX];
  sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-2.png", theme_name);
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_scale(path, 300, 230, 0, NULL);
  GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf); 
  gtk_widget_set_size_request(image, 300, 300);
  return image;
}


GtkWidget* build_theme_frame(char* theme_name) {
  GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget* image = get_optimal_theme_image(theme_name);
  GtkWidget* title = gtk_label_new(fancify_text(theme_name));
  GtkWidget* apply_button = gtk_button_new_with_label("Use Theme");

  g_signal_connect(apply_button, "clicked", G_CALLBACK(on_install_button_click), theme_name);

  gtk_box_append( GTK_BOX(main_box), title);
  gtk_box_append( GTK_BOX(main_box), image);
  gtk_box_append( GTK_BOX(main_box), apply_button);

  GtkWidget* frame = gtk_frame_new(NULL);
  gtk_frame_set_child( GTK_FRAME(frame), main_box);

  gtk_widget_set_size_request (frame, 340, 350);
  return frame;

}


GtkWidget* fetch_all_themes() {
  
  GtkWidget* theme_grid = gtk_grid_new();

  DIR* theme_dir = opendir("/usr/share/plymouth-manager/themes");
  struct dirent* entry;
  if(theme_dir) {
    int entry_count = 0;
    while((entry = readdir(theme_dir)) != NULL) {    
      char* path = strjoin("/usr/share/plymouth-manager/themes/", entry->d_name);
      if(isDir(path) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        GtkWidget* child = build_theme_frame(entry->d_name);
        gtk_grid_attach(GTK_GRID(theme_grid), child, entry_count % 3, entry_count / 3, 1, 1);\
        entry_count++;
      }
    }
  } else {
    printf("Theme directory does not exist.");
  } 

  return theme_grid;
  
}