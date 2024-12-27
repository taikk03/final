#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);

Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    player.speed = 4;
    player.health = 50;
    
    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }
    
    return player;
}

void update_player(Player * player, Map* map){
    if (player->status == PLAYER_DYING) {
        player->animation_tick = (player->animation_tick + 1) % 64;
        return;
    }
    Point original = player->coord;
    
    // Knockback effect
    if(player->knockback_CD > 0){

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){next_x, player->coord.y};
        
        if(!isCollision(player, map)){
            player->coord = next;
        }
        
        next = (Point){player->coord.x, next_y};
        if(!isCollision(player, map)){
            player->coord = next;
        }
    }
    
    /*
        [TODO HACKATHON 1-1] 
        
        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */
    player->status = PLAYER_IDLE;
    if (keyState[ALLEGRO_KEY_W]) {
        player->coord.y -= player->speed;
        player->direction = UP;
        player->status = PLAYER_WALKING;
    }
    if (keyState[ALLEGRO_KEY_S]) {
        player->coord.y += player->speed;
        player->direction = DOWN;
        player->status = PLAYER_WALKING;
    }
    
    // if Collide, snap to the grid to make it pixel perfect
    if(isCollision(player, map)){
        player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO HACKATHON 1-2] 
        
        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */
    if (keyState[ALLEGRO_KEY_A]) {
        player->coord.x -= player->speed;
        player->direction = LEFT;
        player->status = PLAYER_WALKING;
    }
    if (keyState[ALLEGRO_KEY_D]) {
        player->coord.x += player->speed;
        player->direction = RIGHT;
        player->status = PLAYER_WALKING;
    }
    
    if(isCollision(player, map)){
        player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO Homework] 
        
        Calculate the animation tick to draw animation later
    */
    player->animation_tick = (player->animation_tick + 1) % 64;
}
int flag = 0;
void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    
    /*
        [TODO Homework] 
        
        Draw Animation of Dying, Walking, and Idle
    */
    // Define offsets and tint variables
    int spriteOffsetX = 0;
    int spriteOffsetY = 0;
    int tintRedValue = 225;
    ALLEGRO_COLOR tintColor = al_map_rgb(255, 255, 255); // Default white tint

    if (player->damage_tint_duration > 0) {
        tintColor = al_map_rgb(255, 0, 0); // Red tint when damaged
        player->damage_tint_duration--; // Decrease tint duration
    }


    // Determine animation state
    if (player->status == PLAYER_IDLE) {
        spriteOffsetX = 32 * (player->animation_tick / (64 / 2));
        spriteOffsetX = (spriteOffsetX > 32 * (2 - 1)) ? 32 * (2 - 1) : spriteOffsetX;
        spriteOffsetY = 0;
        tintRedValue = (player->knockback_CD > 0) ? 0 : 255;
    }
    else if (player->status == PLAYER_WALKING) {
        spriteOffsetX = 32 * (player->animation_tick / (64 / 4));
        spriteOffsetX = (spriteOffsetX > 32 * (4 - 1)) ? 32 * (4 - 1) : spriteOffsetX;
        spriteOffsetY = 32;

        // Set direction flag based on key input
        if (keyState[ALLEGRO_KEY_D]) {
            flag = 1; // Facing right
        }
        if (keyState[ALLEGRO_KEY_A]) {
            flag = 0; // Facing left
        }

        tintRedValue = (player->knockback_CD > 0) ? 0 : 255;
    }
    else if (player->status == PLAYER_DYING) {
        spriteOffsetX = 32 * (player->animation_tick / (64 / 4));
        spriteOffsetX = (spriteOffsetX > 32 * (4 - 1)) ? 32 * (4 - 1) : spriteOffsetX;
        spriteOffsetY = 64;
        if (spriteOffsetX >= 32 * 3) {
            player->status = PLAYER_DEAD;
        }
    }

    // Draw the player sprite with tint
    al_draw_tinted_scaled_bitmap(
        player->image,
        tintColor,
        spriteOffsetX, spriteOffsetY, 32, 32,
        dx, dy, TILE_SIZE, TILE_SIZE,
        flag
    );

    
#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif
    
}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map){

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*

        [TODO HACKATHON 2-1] 
    
        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;

    */
    int left = player->coord.x / TILE_SIZE;
    int top = player->coord.y / TILE_SIZE;
    int right = (player->coord.x + TILE_SIZE - 1) / TILE_SIZE;
    int bottom = (player->coord.y + TILE_SIZE - 1) / TILE_SIZE;

    if (!isWalkable(map->map[top][left])) return true;
    if (!isWalkable(map->map[top][right])) return true;
    if (!isWalkable(map->map[bottom][left])) return true;
    if (!isWalkable(map->map[bottom][right])) return true;

    return false;
}

void hitPlayer(Player * player, Point enemy_coord, int damage){
    if(player->knockback_CD == 0){
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /*
            [TODO Homework]
            
            Game Logic when the player get hit or die

            player->knockback_angle = angle;
            player->knockback_CD = 32;

            player->health = ...
            if(player->health <= 0){

                player->health = 0;
                player->status = ...

                ...
            }
        */
        player->knockback_angle = angle;
        player->knockback_CD = 32;

        player->health -= damage;
        player->damage_tint_duration = 32; // Set tint duration to 32 frames (or adjust as needed)

            if (player->health <= 0) {

                player->health = 0;
                player->status = PLAYER_DYING;
                player->animation_tick = 0;
            }
    }
}
