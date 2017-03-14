#include <genesis.h>
#include "../res/resources.h"

#define SPRITE_WIDTH(value) (value.animations[0]->frames[0]->w)
#define SPRITE_HEIGHT(value) (value.animations[0]->frames[0]->h)
#define NUM_SPRITES 20
#define FORCE FIX16(0.2)


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

void Angus_force(AngusSprite* angusSprite, fix16 x, fix16 y) {
    angusSprite->velX += x;
    angusSprite->velY += y;
}

u16 controllerState = 0;

void inputHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) controllerState = state;
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
    AngusSprite* angusSprites[NUM_SPRITES];
    for (int i = 0; i < NUM_SPRITES; i++) {
        angusSprites[i] = Angus_new(FIX16(VDP_getScreenWidth()/2 - 128 + (random() & 0x00FF)),
                                    FIX16(VDP_getScreenHeight()/2 - 128 + (random() & 0x00FF)));

    }

    XGM_startPlay(BGM_bgm);

    JOY_init();
    JOY_setEventHandler(&inputHandler);

    fix16 forceX, forceY;
    while(1) {
        forceX = forceY = 0;
        if (controllerState & BUTTON_LEFT) forceX -= FORCE;
        if (controllerState & BUTTON_RIGHT) forceX += FORCE;
        if (controllerState & BUTTON_UP) forceY -= FORCE;
        if (controllerState & BUTTON_DOWN) forceY += FORCE;

        for (int i = 0; i < NUM_SPRITES; i++) {
            Angus_force(angusSprites[i], forceX, forceY);
            Angus_update(angusSprites[i]);
        }
        SPR_update();
        VDP_waitVSync();
    }
}
