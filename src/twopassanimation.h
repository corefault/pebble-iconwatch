#pragma once

typedef struct t_twopassanimation {
   PropertyAnimation *first;
   PropertyAnimation *last;
   Layer* refLayer;
   char*  refText;
} TwoPassAnimation;

extern TwoPassAnimation animation;

void tpa_init();
void tpa_run(Layer* la, char* update);
void tpa_deinit();