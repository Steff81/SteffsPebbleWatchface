#include <pebble.h>
#include "color.h"

static Window *s_main_window;

// time and date layers
static TextLayer *s_time_layer;
// To avoid battery drain seconds are not available
// static TextLayer *s_time_layer_seconds;
static TextLayer *s_time_layer_date;

// information layers
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;

//weather layer
static TextLayer *s_weather_layer;

enum {
  KEY_TEMPERATURE = 0,
  KEY_CONDITIONS = 1,
  KEY_CLOCK_COLOR = 2,
  KEY_BG_COLOR = 3
};

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

// change the watchfaces colors when called
void change_color() {
  
  char color[2];
  persist_read_string(KEY_CLOCK_COLOR, color, sizeof(color));
  APP_LOG(APP_LOG_LEVEL_INFO, "Change color to: %s", color);

  if (strcmp("1", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorYellow);
  } 
  if (strcmp("2", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorRed);
  } 
  if (strcmp("3", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorBlue);
  } 
  if (strcmp("4", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorWhite);
  }   
  if (strcmp("5", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorMalachite);
  }
  if (strcmp("6", color) == 0) {
    text_layer_set_text_color(s_time_layer, GColorVividViolet);
  }   
  
  char bgcolor[2];
  persist_read_string(KEY_BG_COLOR, bgcolor, sizeof(bgcolor));
  APP_LOG(APP_LOG_LEVEL_INFO, "Change bg color to: %s", bgcolor);
  
  if (strcmp("1", bgcolor) == 0) {
    // set bg color and change all text colors so they are readable
    window_set_background_color(s_main_window, GColorBlack);
    // Style the Battery text
    text_layer_set_text_color(s_battery_layer, GColorWhite);
    // Style the date layer
    text_layer_set_text_color(s_time_layer_date, GColorOrange);
    // Style the weather layer
    text_layer_set_text_color(s_weather_layer, GColorWhite);

  } 
  if (strcmp("2", bgcolor) == 0) {
    window_set_background_color(s_main_window, GColorWhite);
    // Style the Battery text
    text_layer_set_text_color(s_battery_layer, GColorBlack);
    // Style the date layer
    text_layer_set_text_color(s_time_layer_date, GColorBlue);
    // Style the weather layer
    text_layer_set_text_color(s_weather_layer, GColorBlack); 
  } 
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
  
  // Create battery layer 
  s_battery_layer = text_layer_create(
      GRect(0, 125, bounds.size.w, 50)); 
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(
      GRect(0, 145, bounds.size.w, 25));
  
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
  
  // Style the Battery text
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_text(s_battery_layer, "100%");  

  // Style the Connection text
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  handle_bluetooth(connection_service_peek_pebble_app_connection());

  // Style the weather text
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Lade...");  
  
  // call the color change method
  change_color(); 
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  // layer_add_child(window_layer, text_layer_get_layer(s_time_layer_seconds));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_date));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer)); 
  
  //make sure battery indicator is always up to date
  handle_battery(battery_state_service_peek());
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
    
  // Get weather update only every 60 minutes
  if(tick_time->tm_min % 60 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }  
}

// free memory
static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  // text_layer_destroy(s_time_layer_seconds);
  text_layer_destroy(s_time_layer_date);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_connection_layer);
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_time_font);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  int temperature_int = 0;

    // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  
  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    temperature_int =  (int)temp_tuple->value->int32;
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d C", temperature_int);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  }
  
  // adjust clock color depending on the settings
  Tuple *temp_color = dict_find(iterator, KEY_CLOCK_COLOR);
  static char color_buffer[2];
  
  if (temp_color) {
     snprintf(color_buffer, sizeof(color_buffer), "%s", temp_color->value->cstring);
     persist_write_string(KEY_CLOCK_COLOR, color_buffer); 
  }
  
  Tuple *temp_bgcolor = dict_find(iterator, KEY_BG_COLOR);
  static char bgcolor_buffer[3];
  
  if (temp_bgcolor) {
     snprintf(bgcolor_buffer, sizeof(bgcolor_buffer), "%s", temp_bgcolor->value->cstring);
     persist_write_string(KEY_BG_COLOR, bgcolor_buffer); 
  }  
  // call the color change method
  change_color(); 
  
  // adjust colors depending on the temperature
  if (temperature_int < 5)
  {
     text_layer_set_text_color(s_weather_layer, GColorVividCerulean);   
  } else if (temperature_int < 18 && temperature_int > 5) {
     text_layer_set_text_color(s_weather_layer, GColorElectricUltramarine); 
  } else if (temperature_int > 16 && temperature_int < 26) {
     text_layer_set_text_color(s_weather_layer, GColorChromeYellow ); 
  } else if (temperature_int > 26 ) {
     text_layer_set_text_color(s_weather_layer, GColorRed); 
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
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
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
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
