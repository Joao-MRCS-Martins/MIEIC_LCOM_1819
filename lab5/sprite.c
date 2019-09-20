#include <lcom/lcf.h>

#include "sprite.h"

/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *(create_sprite)(const char *pic[], int x, int y, int xspeed, int yspeed) {

  //allocate space for the "object"
  Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
  if( sp == NULL )
    return NULL;
  // read the sprite pixmap
  sp->map = read_xpm(pic, &(sp->width), &(sp->height));
  if( sp->map == NULL ) {
    free(sp);
    return NULL;
  }

  sp->x = x;
  sp->y = y;
  sp->xspeed=xspeed;
  sp->yspeed=yspeed;

  return sp;
}

void (destroy_sprite)(Sprite *sp) {
  if( sp == NULL )
    return;
  if( sp ->map )
    free(sp->map);

  free(sp);
  sp = NULL;
}
