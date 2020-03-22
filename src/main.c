#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "display_settings.h"
#include "include/kohoen_network.h"

#define MAX_IPOINTS 700
float input_points[MAX_IPOINTS][2]; // 2 --> x and y of each point
int i_p_count = 0; //input points counter

// 0 if point is outside of graph boundaries
bool inside_graph(float _x, float _y){
 	return ( 0 < _x && _x < graph_w && 0 < _y && _y < graph_h);
}

void add_input_point(float _x, float _y){
	if (!inside_graph(_x, _y)) return;
	if (i_p_count == MAX_IPOINTS) return;
	input_points[i_p_count][0] = _x;
	input_points[i_p_count][1] = _y;
	i_p_count ++;
}

void add_input_point_cloud(float _x, float _y, float _r, int n){
	if (!inside_graph(_x, _y)) return;
	float tmp_x, tmp_y;
	for (int i=0; i<n; i++){
		tmp_x = _x + ((double)rand() / RAND_MAX)*_r - _r/2;
		tmp_y = _y + ((double)rand() / RAND_MAX)*_r - _r/2;
		add_input_point(tmp_x, tmp_y);
	}
}

void del_input_point(){
	if (i_p_count == 0){
		// printf("No input points to delete\n");
		return;
	}
	i_p_count --;
}

void display_input_points(int graph_x, int graph_y, int size, ALLEGRO_COLOR c){
	int vertex_x, vertex_y;
	for (int i=0; i<i_p_count; i++){
		if (!inside_graph(input_points[i][0], input_points[i][1])) continue;
		vertex_x = graph_x+input_points[i][0] - size;
		vertex_y = graph_y+input_points[i][1] - size;
		al_draw_filled_rectangle(vertex_x, vertex_y, vertex_x+size, vertex_y+size, c);
	}
}

void display_weights(int map_size, int graph_x, int graph_y, int size, ALLEGRO_COLOR c){
	float vertex[2];
	float vertex_2[2];
	for (int i=0; i<map_size; i++)
	for (int j=0; j<map_size; j++){
		// if (!inside_graph(input_points[i][0], input_points[i][1])) continue;
		get_weights(i, j, &vertex[0]);
		vertex[0] += graph_x - size;
		vertex[1] += graph_y - size;
		al_draw_filled_rectangle(vertex[0], vertex[1], vertex[0]+size, vertex[1]+size, c);

		// draw line connecting to the next output unit going right in the grid
		if (j != map_size-1){
			get_weights(i, j+1, &vertex_2[0]);
			vertex_2[0] += graph_x - size;
			vertex_2[1] += graph_y - size;
			al_draw_line(vertex[0], vertex[1], vertex_2[0], vertex_2[1], c, 0.5);
		}

		// draw line connecting to the next output unit going down in the grid
		if (i != map_size-1){
			get_weights(i+1, j, &vertex_2[0]);
			vertex_2[0] += graph_x - size;
			vertex_2[1] += graph_y - size;
			al_draw_line(vertex[0], vertex[1], vertex_2[0], vertex_2[1], c, 0.5);
		}


	}


}

void move_input_points(float dx, float dy){
	for (int i=0; i<i_p_count; i++){
		input_points[i][0] += dx;
		input_points[i][1] += dy;
	}
}

void assert_init(bool test, const char *description)
{
    if(test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

bool redraw = true;
bool done = false;
bool train = false;
#define KEY_PRESSED 1 // first bit: the key has been pressed since last frame
#define KEY_DOWN 2 // second bit: the key is currently down
unsigned char keys[ALLEGRO_KEY_MAX] = {0};
unsigned char mouse_button = 0;

void handle_event(ALLEGRO_EVENT event){
	static int last_added_x, last_added_y = 0; // coords of last added input
	switch(event.type)
	{
		case ALLEGRO_EVENT_TIMER:
			if(keys[ALLEGRO_KEY_ESCAPE] & KEY_PRESSED) done = true;
			if(keys[ALLEGRO_KEY_T] & KEY_DOWN) train = true;
			if(keys[ALLEGRO_KEY_L] & KEY_PRESSED){
				init_ts(i_p_count, 2);
				for (int i=0; i<i_p_count; i++)
					set_example(i, input_points[i]);
			}

			if(keys[ALLEGRO_KEY_A] & KEY_PRESSED) learn_ts(1);

			if(keys[ALLEGRO_KEY_UP]	& KEY_DOWN) move_input_points(0, -3);
			if(keys[ALLEGRO_KEY_RIGHT] & KEY_DOWN) move_input_points(3, 0);
			if(keys[ALLEGRO_KEY_DOWN] & KEY_DOWN) move_input_points(0, 3);
			if(keys[ALLEGRO_KEY_LEFT] & KEY_DOWN) move_input_points(-3, 0);

			if(keys[ALLEGRO_KEY_D] & (KEY_PRESSED | KEY_DOWN)) del_input_point();


			// clear the KEY_PRESSED bit
			for(int i = 0; i < ALLEGRO_KEY_MAX; i++) keys[i] &= KEY_DOWN;
			mouse_button &= KEY_DOWN;

			redraw = true;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			mouse_button = KEY_PRESSED | KEY_DOWN;
			if(keys[ALLEGRO_KEY_S] & KEY_DOWN)
				add_input_point_cloud(event.mouse.x-p_off_w, event.mouse.y-p_off_h, 60, 5);
			add_input_point(event.mouse.x-p_off_w, event.mouse.y-p_off_h);
			last_added_x = event.mouse.x;
			last_added_y = event.mouse.y;
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			mouse_button &= KEY_PRESSED; //clear the KEY_DOWN bit
			break;

		case ALLEGRO_EVENT_KEY_DOWN:
			keys[event.keyboard.keycode] = KEY_PRESSED | KEY_DOWN;
			break;
		case ALLEGRO_EVENT_KEY_UP:
			keys[event.keyboard.keycode] &= KEY_PRESSED; //clear the KEY_DOWN bit
			break;

		case ALLEGRO_EVENT_MOUSE_AXES:
			if (mouse_button & KEY_DOWN)
				if (abs(event.mouse.x - last_added_x) > 20 || abs(event.mouse.y - last_added_y) > 20){
					last_added_x = event.mouse.x;
					last_added_y = event.mouse.y;
					add_input_point(event.mouse.x-p_off_w, event.mouse.y-p_off_h);
				}
			break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			done = true;
			break;
	}
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
    ALLEGRO_DISPLAY* disp = al_create_display(disp_w, disp_h);
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

	// KOHOEN NETWORK
	int map_size = MAP_SIZE;
	set_weight_range(10,graph_h-10);
	set_learning_range(0.1,0);
	set_learn_decay(0.95);
	set_radius_range(map_size / 2, 1);
	set_radius_decay(0.95);
	init_net(2, map_size, RANDOM_WD);

    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);
		handle_event(event);
        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(color_backg);
			al_draw_filled_rectangle(p_off_w, p_off_h, p_off_w+graph_w, p_off_h+graph_h, color_black);
			display_input_points(p_off_w, p_off_h, 6, color_accent);
			display_weights(map_size, p_off_w, p_off_h, 4, color_accent2);

            al_flip_display();
            fflush(stdout);

            redraw = false;
        }

		if (train){
			learn_ts(1);
			train = false;
		}
    }

	delete_ts();
	delete_net();
    al_destroy_bitmap(mysha);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
