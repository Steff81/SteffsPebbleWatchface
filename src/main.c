#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
// To avoid battery drain seconds are not available
// static TextLayer *s_time_layer_seconds;
static TextLayer *s_time_layer_date;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;

static GFont s_time_font;

// Battery status handling
static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "lade");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%% geladen", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

// Bluetooth connection status handling
static void handle_bluetooth(bool connected) {
  if (connected) {
      text_layer_set_text_color(s_connection_layer, GColorPictonBlue);
      text_layer_set_text(s_connection_layer, "Bluetooth: OK");
  }  else {
      text_layer_set_text_color(s_connection_layer, GColorRed);
      text_layer_set_text(s_connection_layer, "Bluetooth: --");
  }
}

// Watchface window loading
static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);  
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 45, bounds.size.w, 60));
  
  // Create the TextLayer with specific bounds
  // s_time_layer_seconds = text_layer_create(
  //    GRect(0, PBL_IF_ROUND_ELSE(105, 105), bounds.size.w, 50));
  
    // Create the TextLayer with specific bounds
  s_time_layer_date = text_layer_create(
      GRect(0, 105, bounds.size.w, 50));
  
  s_battery_layer = text_layer_create(
      GRect(0, 120, bounds.size.w, 50));  
  
  s_connection_layer = text_layer_create(
      GRect(0, 30, bounds.size.w, 34));
  
  // Improve the time layout
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorYellow);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
    // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RALEWAY_BLACK_50));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);

  // Improve the layout to be more like a watchface
  // text_layer_set_background_color(s_time_layer_seconds, GColorClear);
  // text_layer_set_text_color(s_time_layer_seconds, GColorOrange);
  // text_layer_set_text(s_time_layer_seconds, "00");
  // text_layer_set_font(s_time_layer_seconds, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  // text_layer_set_text_alignment(s_time_layer_seconds, GTextAlignmentCenter);
  
    // Improve the date layout
  text_layer_set_background_color(s_time_layer_date, GColorClear);
  text_layer_set_text_color(s_time_layer_date, GColorOrange);
  text_layer_set_text(s_time_layer_date, "Heute");
  text_layer_set_font(s_time_layer_date, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_time_layer_date, GTextAlignmentCenter);
  
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_text(s_battery_layer, "100%");  

  
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  handle_bluetooth(connection_service_peek_pebble_app_connection());
  
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  // layer_add_child(window_layer, text_layer_get_layer(s_time_layer_seconds));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_date));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
}

// Update the time and date
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  // static char s_buffer_seconds[10];
  static char s_buffer_date[80];
  
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  // strftime(s_buffer_seconds, sizeof(s_buffer_seconds), "%S", tick_time);
  
  strftime(s_buffer_date, sizeof(s_buffer_date), "%d-%b-%Y", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  // text_layer_set_text(s_time_layer_seconds, s_buffer_seconds);
  text_layer_set_text(s_time_layer_date, s_buffer_date);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// free memory
static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  // text_layer_destroy(s_time_layer_seconds);
  text_layer_destroy(s_time_layer_date);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_connection_layer);
  fonts_unload_custom_font(s_time_font);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  update_time();
  battery_state_service_subscribe(handle_battery);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
