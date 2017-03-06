#include <genesis.h>
#include "../res/resources.h"

#define SPRITE_WIDTH(value) (value.animations[0]->frames[0]->w)
#define SPRITE_HEIGHT(value) (value.animations[0]->frames[0]->h)

typedef struct
{
    Sprite* sprite;
    fix16 x;
    fix16 y;
    fix16 velX;
    fix16 velY;
} AngusSprite;

AngusSprite* Angus_new(fix16 x, fix16 y) {
    fix16 centerX = x - FIX16(SPRITE_WIDTH(angus)/2);
    fix16 centerY = y - FIX16(SPRITE_HEIGHT(angus)/2);

    VDP_setPalette(PAL1, PAL_Angus.data);
    Sprite* spr = SPR_addSprite(&angus,
                                fix16ToRoundedInt(centerX), fix16ToRoundedInt(centerY),
                                TILE_ATTR(PAL1, 0, 0, 0));

    AngusSprite* angusSprite = MEM_alloc(sizeof(AngusSprite));
    *angusSprite = (AngusSprite){
            .sprite=spr,
            .x = centerX,
            .y = centerY,
            .velX=-32 + (random() & 0x003f),
            .velY=-32 + (random() & 0x003f)
    };

    return angusSprite;
}

void Angus_update(AngusSprite* angusSprite) {
    angusSprite->x += angusSprite->velX;
    angusSprite->y += angusSprite->velY;

    fix16 width = FIX16(SPRITE_WIDTH(angus));
    fix16 height = FIX16(SPRITE_HEIGHT(angus));
    fix16 right = angusSprite->x + width;
    fix16 bottom = angusSprite->y + height;
    fix16 screenRight = FIX16(VDP_getScreenWidth());
    fix16 screenBottom = FIX16(VDP_getScreenHeight());

    if (angusSprite->x < 0) {
        angusSprite->x *= -1;
        angusSprite->velX *= -1;
    } else if (right > screenRight) {
        angusSprite->x = screenRight - width - (right - screenRight);
        angusSprite->velX *= -1;
    }
    if (angusSprite->y < 0) {
        angusSprite->y *= -1;
        angusSprite->velY *= -1;
    } else if (bottom > screenBottom) {
        angusSprite->y = screenBottom - height - (bottom - screenBottom);
        angusSprite->velY *= -1;
    }
    SPR_setPosition(angusSprite->sprite,
                    fix16ToRoundedInt(angusSprite->x),
                    fix16ToRoundedInt(angusSprite->y));
}

int main() {
    VDP_drawText("Hello world!", 10, 13);
    SPR_init(0, 0, 0);

    // Sup
    VDP_setPalette(PAL0, PAL_Sup.data);
    VDP_loadTileSet(&sup, TILE_USERINDEX, TRUE);
    VDP_fillTileMapRectInc(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USERINDEX),
                           0, 0, 16, 16);
    VDP_setBackgroundColor(32);

    setRandomSeed(1);
    AngusSprite* angusSprites[80];
    for (int i = 0; i < 80; i++) {
        angusSprites[i] = Angus_new(FIX16(VDP_getScreenWidth()/2),
                                    FIX16(VDP_getScreenHeight()/2));

    }
    while(1) {
        for (int i = 0; i < 80; i++) {
            Angus_update(angusSprites[i]);
        }
        SPR_update();
        VDP_waitVSync();
    }
    return 0;
}
