#include <pebble.h>
#include "configuration.h"
#include "twopassanimation.h"

typedef struct t_main {
   Window *window;
   TextLayer *text_layer;
   GBitmap *icon;
   BitmapLayer* bitmap_layer;
   char  theTime[10];
} MainData;

MainData   app;

//----------------------------------------------------------------------------
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
   // get current time
   time_t             now = time(NULL);
   struct tm *        currentTime = localtime(&now);
   static uint8_t     hour  = 30;
   
   snprintf(app.theTime, 10, "%02d:%02d", currentTime->tm_hour, currentTime->tm_min);

   if (hour != currentTime->tm_hour) {
      hour = currentTime->tm_hour;
      if (app.icon != 0) {
         gbitmap_destroy(app.icon);
      }
      
      // check weekend or not
      if (currentTime->tm_wday == 0 || currentTime->tm_wday == 6) {
         app.icon = gbitmap_create_with_resource(we_data[hour]);
      } else {
         app.icon = gbitmap_create_with_resource(data[hour]);
      }
      bitmap_layer_set_bitmap(app.bitmap_layer, app.icon);
   }

   tpa_run((Layer*)app.text_layer, app.theTime);
}
//----------------------------------------------------------------------------
static void init() {
   app.window = window_create();
   window_stack_push(app.window, true);
   window_set_background_color(app.window, GColorBlack);

   // empty bitmap layer
   app.icon = 0;

   // create canvas
   app.bitmap_layer = bitmap_layer_create((GRect) {.origin = {40, 19}, .size = {64,90}});
   layer_add_child(window_get_root_layer (app.window), bitmap_layer_get_layer(app.bitmap_layer));
   
   // text layer stays on same position
   app.text_layer = text_layer_create((GRect) { .origin = { 0, 119 }, .size = { 144, 30 } });
   text_layer_set_text(app.text_layer, "--:--");
   text_layer_set_text_color(app.text_layer, GColorWhite);
   text_layer_set_background_color(app.text_layer, GColorClear);
   text_layer_set_font(app.text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));  
   text_layer_set_text_alignment(app.text_layer, GTextAlignmentCenter);
   layer_add_child(window_get_root_layer (app.window), text_layer_get_layer(app.text_layer));
   
   tpa_init();
   
   handle_minute_tick(NULL, 0);
   tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}
//----------------------------------------------------------------------------
static void deinit() {
   window_destroy(app.window);
   text_layer_destroy(app.text_layer);
   bitmap_layer_destroy(app.bitmap_layer);
   tpa_deinit();
}
//----------------------------------------------------------------------------
int main(void) {
  init();
  app_event_loop();
  deinit();
}