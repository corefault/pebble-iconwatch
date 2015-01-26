#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static GBitmap *icon;
static BitmapLayer* bitmap_layer;
static PropertyAnimation *s_animation = 0;
static char  theTime[10];

// definition of one item
typedef struct t_config {
   uint8_t   hour;
   uint32_t  icon;
   uint8_t   height;
} Config;

Config data[] = {
   {.hour = 0, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 1, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 2, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 3, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 4, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 5, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 6, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 7, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 8, .icon = RESOURCE_ID_IMAGE_GOWORK, .height = 64},
   {.hour = 9, .icon = RESOURCE_ID_IMAGE_COFFEE, .height = 64},
   {.hour = 10, .icon = RESOURCE_ID_IMAGE_WORK, .height = 64},
   {.hour = 11, .icon = RESOURCE_ID_IMAGE_WORK, .height = 64},
   {.hour = 12, .icon = RESOURCE_ID_IMAGE_LUNCH, .height = 40},
   {.hour = 13, .icon = RESOURCE_ID_IMAGE_WORK, .height = 64},
   {.hour = 14, .icon = RESOURCE_ID_IMAGE_WORK, .height = 64},
   {.hour = 15, .icon = RESOURCE_ID_IMAGE_COFFEE, .height = 64},
   {.hour = 16, .icon = RESOURCE_ID_IMAGE_WORK, .height = 64},
   {.hour = 17, .icon = RESOURCE_ID_IMAGE_DINNER, .height = 64},
   {.hour = 18, .icon = RESOURCE_ID_IMAGE_TV, .height = 63},
   {.hour = 19, .icon = RESOURCE_ID_IMAGE_TV, .height = 63},
   {.hour = 20, .icon = RESOURCE_ID_IMAGE_TV, .height = 63},
   {.hour = 21, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 22, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83},
   {.hour = 23, .icon = RESOURCE_ID_IMAGE_SLEEP, .height = 83}
};
//----------------------------------------------------------------------------
void create_animation_enter(Animation *animation, bool finished, void *data) {
   text_layer_set_text(text_layer, theTime);
   GRect   from = GRect(144, 119, 144, 30);
   GRect   to   = GRect(0, 119, 144, 30);
   
   property_animation_destroy(s_animation);
   
   s_animation = property_animation_create_layer_frame((Layer*)text_layer, &from, &to);
   animation_set_duration ((Animation*) s_animation, 500);
   
   animation_schedule((Animation*) s_animation);
}
//----------------------------------------------------------------------------
void create_animation_leave() {
   GRect   from = GRect(0, 119, 144, 30);
   GRect   to   = GRect(-144, 119, 144, 30);
   
   s_animation = property_animation_create_layer_frame((Layer*)text_layer, &from, &to);
   animation_set_duration ((Animation*) s_animation, 500);
   
   animation_set_handlers((Animation*) s_animation, (AnimationHandlers) {
    .started = NULL,
    .stopped = (AnimationStoppedHandler) create_animation_enter,
   }, NULL);
   animation_schedule((Animation*) s_animation);
}
//----------------------------------------------------------------------------
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
   // get current time
   time_t             now = time(NULL);
   struct tm *        currentTime = localtime(&now);
   static uint8_t     hour  = 30;
   
   snprintf(theTime, 10, "%02d:%02d", currentTime->tm_hour, currentTime->tm_min);

   if (hour != currentTime->tm_hour) {
      hour = currentTime->tm_hour;
      if (icon != 0) {
         gbitmap_destroy(icon);
      }
      icon = gbitmap_create_with_resource(data[hour].icon);
      bitmap_layer_set_bitmap(bitmap_layer, icon);
   }

   create_animation_leave();
}
//----------------------------------------------------------------------------
static void init() {
   window = window_create();
   window_stack_push(window, true);
   window_set_background_color(window, GColorBlack);

   // empty bitmap layer
   icon = 0;

   // create canvas
   bitmap_layer = bitmap_layer_create((GRect) {.origin = {40, 19}, .size = {64,90}});
   layer_add_child(window_get_root_layer (window), bitmap_layer_get_layer(bitmap_layer));
   
   // text layer stays on same position
   text_layer = text_layer_create((GRect) { .origin = { 0, 119 }, .size = { 144, 30 } });
   text_layer_set_text(text_layer, "--:--");
   text_layer_set_text_color(text_layer, GColorWhite);
   text_layer_set_background_color(text_layer, GColorClear);
   text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));  
   text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
   layer_add_child(window_get_root_layer (window), text_layer_get_layer(text_layer));
   
   handle_minute_tick(NULL, 0);
   tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}
//----------------------------------------------------------------------------
static void deinit() {
   window_destroy(window);
   text_layer_destroy(text_layer);
   bitmap_layer_destroy(bitmap_layer);
}
//----------------------------------------------------------------------------
int main(void) {
  init();
  app_event_loop();
  deinit();
}