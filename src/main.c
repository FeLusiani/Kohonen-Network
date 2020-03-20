#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "settings.h"


void assert_init(bool test, const char *description)
{
    if(test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    // sistemi
    assert_init(al_init(), "allegro");
    assert_init(al_init_primitives_addon(), "primitives");
    assert_init(al_init_image_addon(), "image addon");
    assert_init(al_install_keyboard(), "keyboard");
    assert_init(al_install_mouse(), "mouse");

    // timer
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    assert_init(timer, "timer");
    // event queue
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    assert_init(queue, "queue");

    // display
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    assert_init(disp, "display");
    // font
    ALLEGRO_FONT* font = al_create_builtin_font();
    assert_init(font, "font");

    // images
    ALLEGRO_BITMAP* mysha = al_load_bitmap("img/mysha.png");
    assert_init(mysha, "mysha");

    // events
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int mx = 0;
    int my = 0;

    #define KEY_PRESSED 1 // first bit: the key has been pressed since last frame
    #define KEY_DOWN 2 // second bit: the key is currently down

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_ESCAPE] & KEY_PRESSED)
                    done = true;

                if(key[ALLEGRO_KEY_RIGHT] & KEY_DOWN)
                    mx ++;

                // clear the KEY_PRESSED bit
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_DOWN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                mx = event.mouse.x;
                my = event.mouse.y;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_PRESSED | KEY_DOWN;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_PRESSED; //clear the KEY_DOWN bit
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(color_backg);
            al_draw_text(font, color_text, mx, my, 0, "Hello world!");
            al_draw_text(font, color_text2, page_offset_w, page_offset_h+10, 0, "Hello world!");
            al_draw_text(font, color_text3, page_offset_w, page_offset_h+20, 0, "Hello world!");
            al_draw_text(font, color_accent, page_offset_w, page_offset_h+30, 0, "Hello world!");
            al_draw_text(font, color_accent2, page_offset_w, page_offset_h+40, 0, "Hello world!");

            al_flip_display();
            fflush(stdout);

            redraw = false;
        }
    }

    al_destroy_bitmap(mysha);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
