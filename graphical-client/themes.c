#include <gtk/gtk.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include "main.h"
#include "../common/utils.h"
#include "../common/map.h"
#include "themes.h"

#define ANIMATING_ON 1
#define ANIMATING_PAUSED 0
#define ANIMATING_OFF -1

map_element* currently_animated;  

struct gtk_data_storage {
  GtkWidget* widget;
  char theme_name[PATH_MAX];
};



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
      result[i] = text[i] - 32; //Work some ascii magic to capitalize the first letter of every word. 
      new_word = 0;
    } else if (text[i] == '_') {
        result[i] = ' '; //Replace underscored with spaces
        new_word = 1;
    } else {
      result[i] = text[i];
    }
  }
  return result;
}

GdkPixbuf* get_pixbuf_standard_size(char* path) {
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_scale(path, 300, 230, 0, NULL);
  return pixbuf;
}

GtkWidget* get_optimal_theme_image(char* theme_name) {
  char path[PATH_MAX];
  sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-", theme_name);
  int optimal_number = (find_highest_file_number(path, 0, ".png") / 4);
  path[0] = '\0';
  sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-%i.png", theme_name, optimal_number);
  GdkPixbuf* pixbuf = get_pixbuf_standard_size(path);
  GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf); 
  gtk_widget_set_size_request(image, 300, 300);
  return image;
}

void* animate_plymouth_theme_async(void* args) {
  char path[PATH_MAX];

  //Transmute the arguments back into the original struct form.
  struct gtk_data_storage* data = (struct gtk_data_storage*) args;
  char theme_name[1000];
  strcpy(theme_name, data->theme_name);
   
  GtkWidget* image =  (GtkWidget*) data->widget;
  
  sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-", theme_name);
  int highest_num = find_highest_file_number(path, 0, ".png");
  for(int i = 0; i < (highest_num - 1); i++) {   
    if(map_find(currently_animated, theme_name) == ANIMATING_ON) {
      sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-%i.png", theme_name, i);
      gtk_image_set_from_pixbuf( GTK_IMAGE(image), get_pixbuf_standard_size(path));

    }  
    sleep(0.8);
  }
  sprintf(path, "/usr/share/plymouth-manager/themes/%s/progress-%i.png", theme_name, highest_num/4);
  gtk_image_set_from_pixbuf( GTK_IMAGE(image), get_pixbuf_standard_size(path));
  map_modify(currently_animated, theme_name, ANIMATING_OFF);
  pthread_exit(NULL);
}

static void on_image_mouseover(GtkEventController* controller, gpointer theme_name) {
  struct gtk_data_storage data;
  data.widget = gtk_event_controller_get_widget(controller);
  strcpy(data.theme_name, (char*) theme_name);
  void* args = (void*) &data;
  pthread_t thread_id;

  if(map_find(currently_animated, (char*) theme_name) == ANIMATING_OFF) { 
    printf("Starting thread with name %s\n", (char*) theme_name); 
    pthread_create(&thread_id, NULL, animate_plymouth_theme_async, args);
  } 
  map_modify(currently_animated, (char*) theme_name, ANIMATING_ON);
}

static void on_image_mouseout(GtkEventController* controller, gpointer theme_name) {
 map_modify(currently_animated, (char*) theme_name, ANIMATING_PAUSED);
}



GtkWidget* build_theme_frame(char* theme_name) {
  map_add(currently_animated, theme_name, ANIMATING_OFF);
  GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget* image = get_optimal_theme_image(theme_name);
  GtkWidget* title = gtk_label_new(theme_name);
  GtkWidget* apply_button = gtk_button_new_with_label("Use Theme");
  
  GtkEventController* controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller(image, controller);

  g_signal_connect(apply_button, "clicked", G_CALLBACK(on_install_button_click), theme_name);
  g_signal_connect(controller, "enter", G_CALLBACK(on_image_mouseover), theme_name);
  g_signal_connect(controller, "leave", G_CALLBACK(on_image_mouseout), theme_name);

  gtk_box_append( GTK_BOX(main_box), title);
  gtk_box_append( GTK_BOX(main_box), image);
  gtk_box_append( GTK_BOX(main_box), apply_button);

  GtkWidget* frame = gtk_frame_new(NULL);
  gtk_frame_set_child( GTK_FRAME(frame), main_box);

  gtk_widget_set_size_request (frame, 340, 350);
  return frame;

}


GtkWidget* fetch_all_themes() {
  currently_animated = malloc(1000 * sizeof(map_element));
  map_init(currently_animated, 1000);

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