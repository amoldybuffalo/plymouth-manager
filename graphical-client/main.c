#include <gtk/gtk.h>
#include "themes.h"
#include "main.h"

GtkWidget* window;
GtkWidget* footer;
GtkWidget* main_grid;

static GtkWidget* build_ui() {
  GtkWidget* top_level_box =  gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  
  GtkWidget* scrolled_window = gtk_scrolled_window_new();
  gtk_widget_set_size_request(scrolled_window, 1280, 650);
  

  main_grid = fetch_all_themes();
  gtk_widget_set_halign(main_grid, GTK_ALIGN_CENTER);
  gtk_grid_set_column_spacing(GTK_GRID(main_grid), 5);
  gtk_grid_set_row_spacing(GTK_GRID(main_grid), 3);

  footer = gtk_label_new(NULL);

  gtk_scrolled_window_set_child( GTK_SCROLLED_WINDOW (scrolled_window), main_grid);
  gtk_box_append(GTK_BOX(top_level_box), scrolled_window);
  gtk_box_append(GTK_BOX(top_level_box), footer);
  return top_level_box;

}

static void
activate (GtkApplication* app, gpointer user_data) {
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 1280, 720);
  GtkWidget* top_level_box = build_ui();
  gtk_window_set_child(GTK_WINDOW(window), top_level_box);
  gtk_widget_show(window);

}

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.amoldybuffalo.plymouth-manager", 0);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}