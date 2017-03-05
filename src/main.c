#include <genesis.h>
#include "../res/resources.h"



int main() {
    VDP_drawText("Hello world!", 10, 13);

    // Sup
    VDP_setPalette(PAL0, PAL_Sup.data);
    VDP_loadTileSet(&sup, TILE_USERINDEX, TRUE);
    VDP_fillTileMapRectInc(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USERINDEX),
                           0, 0, 16, 16);
    VDP_setBackgroundColor(32);

    // Angus
    VDP_setPalette(PAL1, PAL_Angus.data);
    SPR_init(0, 0, 0);
    Sprite* angusSprite = SPR_addSprite(&angus, 10, 10, TILE_ATTR(PAL1, 0, 0, 0));
    int i = 0;
    while(1) {
        i += 1;
        SPR_setPosition(angusSprite, i, 10);
        SPR_update();
        VDP_waitVSync();
    }
    return 0;
}
