#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static GBitmap *icon;
static BitmapLayer* bitmap_layer;
static PropertyAnimation *s_animation = 0;
static char  theTime[10];

// definition of one item

uint32_t d = RESOURCE_ID_IMAGE_SLEEP;

uint32_t  we_data[] = {
   RESOURCE_ID_IMAGE_SLEEP,  //0
   RESOURCE_ID_IMAGE_SLEEP,  //1
   RESOURCE_ID_IMAGE_SLEEP,  //2
   RESOURCE_ID_IMAGE_SLEEP,  //3
   RESOURCE_ID_IMAGE_SLEEP,  //4
   RESOURCE_ID_IMAGE_SLEEP,  //5
   RESOURCE_ID_IMAGE_SLEEP,  //6
   RESOURCE_ID_IMAGE_SLEEP,  //7
   RESOURCE_ID_IMAGE_SLEEP,  //8
   RESOURCE_ID_IMAGE_WAKEUP,  //9
   RESOURCE_ID_IMAGE_COFFEE,  //10
   RESOURCE_ID_IMAGE_HOME,  //11
   RESOURCE_ID_IMAGE_LUNCH,  //12
   RESOURCE_ID_IMAGE_SHOPPING,  //13
   RESOURCE_ID_IMAGE_SHOPPING,  //14
   RESOURCE_ID_IMAGE_COFFEE,  //15
   RESOURCE_ID_IMAGE_SHOPPING,  //16
   RESOURCE_ID_IMAGE_SHOPPING,  //17
   RESOURCE_ID_IMAGE_SHOPPING,  //18
   RESOURCE_ID_IMAGE_LUNCH,  //19
   RESOURCE_ID_IMAGE_LUNCH,  //20
   RESOURCE_ID_IMAGE_GLASS,  //21
   RESOURCE_ID_IMAGE_GLASS,  //22
   RESOURCE_ID_IMAGE_SLEEP  //23
};
uint32_t  data[] = {
   RESOURCE_ID_IMAGE_SLEEP,  //0
   RESOURCE_ID_IMAGE_SLEEP,  //1
   RESOURCE_ID_IMAGE_SLEEP,  //2
   RESOURCE_ID_IMAGE_SLEEP,  //3
   RESOURCE_ID_IMAGE_SLEEP,  //4
   RESOURCE_ID_IMAGE_SLEEP,  //5
   RESOURCE_ID_IMAGE_WAKEUP,  //6
   RESOURCE_ID_IMAGE_HOME,  //7
   RESOURCE_ID_IMAGE_CAR,  //8
   RESOURCE_ID_IMAGE_COFFEE,  //9
   RESOURCE_ID_IMAGE_WORK2,  //10
   RESOURCE_ID_IMAGE_WORK,  //11
   RESOURCE_ID_IMAGE_LUNCH,  //12
   RESOURCE_ID_IMAGE_LAB,  //13
   RESOURCE_ID_IMAGE_HEADPHONES,  //14
   RESOURCE_ID_IMAGE_COFFEE,  //15
   RESOURCE_ID_IMAGE_MUSIC,  //16
   RESOURCE_ID_IMAGE_TRAIN,  //17
   RESOURCE_ID_IMAGE_LUNCH,  //18
   RESOURCE_ID_IMAGE_LAPTOP,  //19
   RESOURCE_ID_IMAGE_GAME,  //20
   RESOURCE_ID_IMAGE_GLASS,  //21
   RESOURCE_ID_IMAGE_SLEEP,  //22
   RESOURCE_ID_IMAGE_SLEEP  //23
};//----------------------------------------------------------------------------
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
      
      // check weekend or not
      if (currentTime->tm_wday == 0 || currentTime->tm_wday == 6) {
         icon = gbitmap_create_with_resource(we_data[hour]);
      } else {
         icon = gbitmap_create_with_resource(data[hour]);
      }
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
   property_animation_destroy(s_animation);
}
//----------------------------------------------------------------------------
int main(void) {
  init();
  app_event_loop();
  deinit();
}