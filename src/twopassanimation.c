#include <pebble.h>
#include "twopassanimation.h"

TwoPassAnimation   animation;

void tpa_init () {
   animation.first = 0;
   animation.last = 0;
   animation.refLayer = 0;
   animation.refText = 0;
}

void tpa_makeFirst() {
   GRect   from = GRect(0, 119, 144, 30);
   GRect   to   = GRect(-144, 119, 144, 30);
   animation.first = property_animation_create_layer_frame(animation.refLayer, &from, &to);
   animation_set_curve((Animation*)animation.first, AnimationCurveEaseInOut);
   animation_set_duration ((Animation*) animation.first, 500);
   animation_schedule((Animation*) animation.first);
}
void tpa_lastStarted(struct Animation *anim, void *context) {
   text_layer_set_text( (TextLayer*)animation.refLayer, animation.refText);
}

void tpa_makeLast() {
   GRect   from = GRect(144, 119, 144, 30);
   GRect   to   = GRect(0, 119, 144, 30);
   animation.last = property_animation_create_layer_frame(animation.refLayer, &from, &to);
   animation_set_delay((Animation*) animation.last, 600);
   animation_set_duration ((Animation*) animation.last, 500);
   animation_set_curve((Animation*)animation.last, AnimationCurveEaseInOut);
   animation_set_handlers((Animation*) animation.last, (AnimationHandlers) {
    .started = (AnimationStartedHandler) tpa_lastStarted,
    .stopped = NULL
   }, NULL);

   animation_schedule((Animation*) animation.last);
}


void tpa_run(Layer* la, char* update) {
   tpa_deinit();

   animation.refLayer = la;
   animation.refText  = update;
   
   tpa_makeFirst();
   tpa_makeLast();
}

void tpa_deinit() {
   if (animation.first) {
      property_animation_destroy(animation.first), animation.first = 0;
   }
   if (animation.last) {
      property_animation_destroy(animation.last), animation.last = 0;
   }
}

