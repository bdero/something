#include <genesis.h>
#include "../res/resources.h"

#define SPRITE_WIDTH(value) (value.animations[0]->frames[0]->w)
#define SPRITE_HEIGHT(value) (value.animations[0]->frames[0]->h)
#define NUM_SPRITES 20
#define FORCE FIX16(0.2)
#define REGISTER_SOUND(id, sound) (register_sound(id, sound, sizeof(sound)))
#define SOUND_BANK 0x80

u16 soundChannel = 0;

void register_sound(u8 id, const u8 *sound, u32 size) {
    XGM_setPCM(SOUND_BANK | id, sound, size);
}

void play_sound(u8 id) {
    XGM_startPlayPCM(SOUND_BANK | id, 5, soundChannel++);
    if (soundChannel > 3) soundChannel = 1;
}

typedef struct
{
    Sprite* sprite;
    fix16 x;
    fix16 y;
    fix16 velX;
    fix16 velY;
} BubbleSprite;

BubbleSprite* Bubble_new(fix16 x, fix16 y) {
    fix16 centerX = x - FIX16(SPRITE_WIDTH(bubble)/2);
    fix16 centerY = y - FIX16(SPRITE_HEIGHT(bubble)/2);

    VDP_setPalette(PAL1, PAL_Bubble.data);
    Sprite* spr = SPR_addSprite(&bubble,
                                fix16ToRoundedInt(centerX), fix16ToRoundedInt(centerY),
                                TILE_ATTR(PAL1, 0, 0, 0));

    BubbleSprite* bubbleSprite = MEM_alloc(sizeof(BubbleSprite));
    *bubbleSprite = (BubbleSprite){
            .sprite=spr,
            .x = centerX,
            .y = centerY,
            .velX=-32 + (random() & 0x003f),
            .velY=-32 + (random() & 0x003f)
    };

    return bubbleSprite;
}

void Bubble_update(BubbleSprite* bubbleSprite) {
    bubbleSprite->x += bubbleSprite->velX;
    bubbleSprite->y += bubbleSprite->velY;

    fix16 width = FIX16(SPRITE_WIDTH(bubble));
    fix16 height = FIX16(SPRITE_HEIGHT(bubble));
    fix16 right = bubbleSprite->x + width;
    fix16 bottom = bubbleSprite->y + height;
    fix16 screenRight = FIX16(VDP_getScreenWidth());
    fix16 screenBottom = FIX16(VDP_getScreenHeight());

    s8 bump = FALSE;
    if (bubbleSprite->x < 0) {
        bump = TRUE;
        bubbleSprite->x *= -1;
        bubbleSprite->velX *= -1;
    } else if (right > screenRight) {
        bump = TRUE;
        bubbleSprite->x = screenRight - width - (right - screenRight);
        bubbleSprite->velX *= -1;
    }
    if (bubbleSprite->y < 0) {
        bump = TRUE;
        bubbleSprite->y *= -1;
        bubbleSprite->velY *= -1;
    } else if (bottom > screenBottom) {
        bump = TRUE;
        bubbleSprite->y = screenBottom - height - (bottom - screenBottom);
        bubbleSprite->velY *= -1;
    }

    if (bump) {
        play_sound(0);
    }

    SPR_setPosition(bubbleSprite->sprite,
                    fix16ToRoundedInt(bubbleSprite->x),
                    fix16ToRoundedInt(bubbleSprite->y));
}

void Bubble_force(BubbleSprite* bubbleSprite, fix16 x, fix16 y) {
    bubbleSprite->velX += x;
    bubbleSprite->velY += y;
}

u16 controllerState = 0;

void inputHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) controllerState = state;
}

s16 frame = 0;

int main() {
    VDP_setPlanSize(32, 32);

    VDP_drawText("Hello world!", 10, 13);
    SPR_init(0, 0, 0);

    // Tiled background
    VDP_setPalette(PAL0, PAL_Sup.data);
    VDP_loadTileSet(&sup, TILE_USERINDEX, TRUE);
    for (u16 i = 0; i < 4; i++) {
        VDP_fillTileMapRectInc(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USERINDEX),
                               i%2 * 16, i/2 * 16, 16, 16);
    }
    VDP_setBackgroundColor(32);

    setRandomSeed(1);
    BubbleSprite* bubbleSprites[NUM_SPRITES];
    for (int i = 0; i < NUM_SPRITES; i++) {
        bubbleSprites[i] = Bubble_new(FIX16(VDP_getScreenWidth()/2 - 128 + (random() & 0x00FF)),
                                    FIX16(VDP_getScreenHeight()/2 - 128 + (random() & 0x00FF)));

    }

    REGISTER_SOUND(0, SFX_Bump);

    XGM_startPlay(BGM_bgm);

    JOY_init();
    JOY_setEventHandler(&inputHandler);

    fix16 forceX, forceY;
    while(1) {
        frame += 1;

        forceX = forceY = 0;
        if (controllerState & BUTTON_LEFT) forceX -= FORCE;
        if (controllerState & BUTTON_RIGHT) forceX += FORCE;
        if (controllerState & BUTTON_UP) forceY -= FORCE;
        if (controllerState & BUTTON_DOWN) forceY += FORCE;

        for (int i = 0; i < NUM_SPRITES; i++) {
            Bubble_force(bubbleSprites[i], forceX, forceY);
            Bubble_update(bubbleSprites[i]);
        }

        VDP_setHorizontalScroll(PLAN_B, frame*2);
        VDP_setVerticalScroll(PLAN_B, -frame/2);

        SPR_update();
        VDP_waitVSync();
    }
}
