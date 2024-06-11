#include "bn_core.h"
#include "bn_display.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_math.h"
#include "bn_log.h"

#include "bn_music_items.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_bg.h"
#include "bn_sound_items.h"

#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_paddle.h"
#include "bn_sprite_items_ball.h"

#include "common_info.h"
#include "common_variable_8x8_sprite_font.h"
#include "bn_sprite_text_generator.h"

int main()
{
    // this is to initialize
    bn::core::init();

    // here we start playing the bg music
    bn::music_items::amayadori.play(1);

    // here we create the background
    bn::regular_bg_ptr bg = bn::regular_bg_items::bg.create_bg(0, 0);

    // paddles
    bn::sprite_ptr left_paddle = bn::sprite_items::paddle.create_sprite(-140, 0);
    bn::sprite_ptr right_paddle = bn::sprite_items::paddle.create_sprite(140, 0);

    // flip the right paddle
    right_paddle.set_horizontal_flip(true);

    // ball
    bn::sprite_ptr ball = bn::sprite_items::ball.create_sprite(0, 0);

    // important variables
    int score = 0;
    bool enemy_going_up = false;

    int delta_x = 0;
    int delta_y = 0;

    // randomness
    bn::random random;

    // set up font
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // a vector for letters
    bn::vector<bn::sprite_ptr, 16> text_sprites;

    // write some stuff
    text_generator.generate(-6 * 16, -68, "(Press A to start)", text_sprites);

    while (true)
    {
        // up-down left paddle logic
        if (bn::keypad::up_held() && left_paddle.y() > -48)
        {
            left_paddle.set_y(left_paddle.y() - 1);
        }

        else if (bn::keypad::down_held() && left_paddle.y() < 48)
        {
            left_paddle.set_y(left_paddle.y() + 1);
        }

        // right paddle logic
        if (enemy_going_up)
        {
            if (right_paddle.y() > -48)
            {
                right_paddle.set_y(right_paddle.y() - 1);
            }
            else
            {
                enemy_going_up = false;
            }
        }
        else
        {
            if (right_paddle.y() < 48)
            {
                right_paddle.set_y(right_paddle.y() + 1);
            }
            else
            {
                enemy_going_up = true;
            }
        }

        if (bn::keypad::a_pressed() && delta_x == 0 && delta_y == 0)
        {
            text_sprites.clear();

            bn::string<32> txt_score = "Score: " + bn::to_string<32>(score);
            text_generator.generate(-6 * 16, -68, txt_score, text_sprites);

            while (delta_x == 0 || delta_y == 0)
            {
                delta_x = (random.get_int() % 5) - 2;
                delta_y = (random.get_int() % 5) - 2;
            }

            bn::sound_items::pong.play();
        }

        ball.set_x(ball.x() + delta_x);
        ball.set_y(ball.y() + delta_y);

        if (ball.x() < -100)
        {
            if (bn::abs(ball.y() - left_paddle.y()) < 32)
            {
                delta_x = delta_x * -1;
                bn::sound_items::pong.play();
            }
            else
            {
                score--;
                ball.set_position(0, 0);
                delta_x = 0;
                delta_y = 0;

                text_sprites.clear();
                bn::string<32> txt_score = "Score: " + bn::to_string<32>(score) + " (Press A)";
                text_generator.generate(-6 * 16, -68, txt_score, text_sprites);
            }
        }

        else if (ball.x() > 100)
        {
            if (bn::abs(ball.y() - right_paddle.y()) < 32)
            {
                delta_x = delta_x * -1;
                bn::sound_items::pong.play();
            }
            else
            {
                score++;
                ball.set_position(0, 0);
                delta_x = 0;
                delta_y = 0;
                text_sprites.clear();
                bn::string<32> txt_score = "Score: " + bn::to_string<32>(score) + " (Press A)";
                text_generator.generate(-6 * 16, -68, txt_score, text_sprites);
            }
        }

        if (ball.y() < -64)
        {
            delta_y = delta_y * -1;
            bn::sound_items::ping.play();
        }
        else if (ball.y() > 64)
        {
            delta_y = delta_y * -1;
            bn::sound_items::ping.play();
        }

        if (bn::keypad::b_pressed())
        {
            BN_LOG(score);
        }

        bn::core::update();
    }
}
