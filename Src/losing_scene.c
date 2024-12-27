#include <allegro5/allegro.h>
#include "losing_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static ALLEGRO_BITMAP* loseImage; // Declare bitmap for the losing image

static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    // Load the losing image
    loseImage = al_load_bitmap("Assets/panda_lose.png");
    if (!loseImage) {
        game_log("Failed to load losing image!");
    }
}

static void update(void) {
    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }
}

static void draw(void) {
    // Draw the losing image scaled and centered
    if (loseImage) {
        float scale_x = 5.0; // Scale the image by 5x in the x direction
        float scale_y = 5.0; // Scale the image by 5x in the y direction
        float image_width = al_get_bitmap_width(loseImage) * scale_x;
        float image_height = al_get_bitmap_height(loseImage) * scale_y;

        al_draw_scaled_bitmap(
            loseImage,
            0, 0,                                // Source x, y
            al_get_bitmap_width(loseImage),      // Source width
            al_get_bitmap_height(loseImage),     // Source height
            (SCREEN_W - image_width) / 2,        // Destination x (centered)
            (SCREEN_H - image_height) / 2,       // Destination y (centered)
            image_width,                         // Destination width
            image_height,                        // Destination height
            0                                    // Flags
        );
    }

    // Draw button
    draw_button(backButton);

    // Button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
}

static void destroy(void) {
    destroy_button(&backButton);
    if (loseImage) {
        al_destroy_bitmap(loseImage); // Destroy the bitmap to free memory
    }
}

Scene create_losing_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "losing";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
