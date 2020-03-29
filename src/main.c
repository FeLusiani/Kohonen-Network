#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "display_settings.h"
#include "KN_settings.h"
#include "include/kohoen_network.h"

#define MAX_IPOINTS 700
float input_points[MAX_IPOINTS][2]; // 2 --> x and y of each point
int i_p_count = 0; //input points counter

// returns position at which it printed the info
float print_info(ALLEGRO_FONT* font, ALLEGRO_COLOR c, int line_n, const char* string){
	al_draw_text(font, c, 2*p_off_w+graph_w, p_off_h+line_n*font_size, 0, string);
	return p_off_h+line_n*font_size;
}

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
	if (i_p_count == 0)
		return;
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
	float vertex[2]; // tmp vector for weight
	float vertex_2[2]; // tmp vector for weight connected to the previous one
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

void draw_u_matrix(int map_size, int max_dist, int matrix_x, int matrix_y, int cell_size){
	float vertex[2]; // tmp vector for weight
	float vertex_2[2]; // tmp vector for weight connected to the previous one
	float cell_x; // tmp var for x of cell to draw
	float cell_y; // tmp var for y of cell to draw
	float dist; // tmp var
	ALLEGRO_COLOR c; //tmp var

	c = al_map_rgb(250,250,250);
	// draw frame of matrix
	al_draw_rectangle(matrix_x, matrix_y,
					  matrix_x+(map_size+2)*cell_size,
					  matrix_y+(map_size+2)*cell_size,
					  c, cell_size);
	matrix_x += cell_size;
	matrix_y += cell_size;
	for (int i=0; i<map_size; i++)
	for (int j=0; j<map_size; j++){
		get_weights(i, j, &vertex[0]);
		cell_x = matrix_x + j*cell_size;
		cell_y = matrix_y + i*cell_size;

		// draw cell representing distance to the next output unit going right in the grid
		cell_x += cell_size;
		if (j != map_size-1){
			get_weights(i, j+1, &vertex_2[0]);
			dist = 0;
			dist += (vertex[0] - vertex_2[0])*(vertex[0] - vertex_2[0]);
			dist += (vertex[1] - vertex_2[1])*(vertex[1] - vertex_2[1]);
			dist = sqrt(dist) * 255 / max_dist;
			c = al_map_rgb(dist, dist, dist);
			al_draw_filled_rectangle(cell_x, cell_y, cell_x+cell_size, cell_y+cell_size, c);
		}

		// draw cell representing distance to the next output unit going down in the grid
		cell_x -= cell_size;
		cell_y += cell_size;
		if (i != map_size-1){
			get_weights(i+1, j, &vertex_2[0]);
			dist = 0;
			dist += (vertex[0] - vertex_2[0])*(vertex[0] - vertex_2[0]);
			dist += (vertex[1] - vertex_2[1])*(vertex[1] - vertex_2[1]);
			dist = sqrt(dist) * 255 / max_dist;
			c = al_map_rgb(dist, dist, dist);
			al_draw_filled_rectangle(cell_x, cell_y, cell_x+cell_size, cell_y+cell_size, c);
		}
	}
}

// KOHOEN NETWORK
int map_size = MAP_SIZE;
void initialize_kohoen_net(){
	set_weight_range(10,graph_h-10);
	set_learning_range(0.1,0);
	set_learn_decay(LEARN_DECAY);
	set_radius_range(MAX_RADIUS, MIN_RADIUS);
	set_radius_decay(RADIUS_DECAY);
	init_net(2, map_size, (enum w_distribution)WEIGHT_DISTRIBUTION);
}

void load_training_set(){
	init_ts(i_p_count);
	for (int i=0; i<i_p_count; i++)
		set_example(i, input_points[i]);
}

void reset(){
	delete_ts();
	delete_net();
	initialize_kohoen_net();
	load_training_set();
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
			if(keys[ALLEGRO_KEY_T] & KEY_DOWN) train = true; // TRAIN
			if(keys[ALLEGRO_KEY_L] & KEY_PRESSED) load_training_set();
			if(keys[ALLEGRO_KEY_R] & KEY_PRESSED) reset();

			if(keys[ALLEGRO_KEY_A] & KEY_PRESSED) learn_ts(1); // TRAIN (1 STEP)

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
    // allegro stuff init
    assert_init(al_init(), "allegro");
	assert_init(al_init_font_addon(), "font addon");
	assert_init(al_init_ttf_addon(), "ttf addon");
    assert_init(al_init_primitives_addon(), "primitives addon");
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
    ALLEGRO_FONT* font = al_load_ttf_font("font/UbuntuMono-R.ttf", font_size, 0);
	assert_init(font, "font");

    // events
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

	initialize_kohoen_net();

	// infinite loop
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
			display_weights(map_size, p_off_w, p_off_h, 4, color_accent2);
			display_input_points(p_off_w, p_off_h, 6, color_accent);
			// testo
			int line = 0;
			char tmp[100];
			sprintf(tmp, "FIXED VALUES:");
			print_info(font, color_text2, line++, tmp);
			sprintf(tmp, "Learning rate decay = %.2f", LEARN_DECAY);
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "Radius decay = %.2f", RADIUS_DECAY);
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "DYNAMIC VALUES:");
			print_info(font, color_text3, line++, tmp);
			sprintf(tmp, "Learning rate = %.2f", get_learning_rate());
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "Radius = %.2f", get_radius());
			print_info(font, color_text, line++, tmp);
			line++; // "line break"
			sprintf(tmp, "CONTROLS");
			print_info(font, color_accent2, line++, tmp);
			sprintf(tmp, "Click or drag the mouse");
			print_info(font, color_accent, line++, tmp);
			sprintf(tmp, "to create input elements");
			print_info(font, color_accent, line++, tmp);
			sprintf(tmp, "D -> delete input elements");
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "L -> load input elements as training set");
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "T -> train the net 1 epoch per frame");
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "A -> train the net once (1 epoch)");
			print_info(font, color_text, line++, tmp);
			sprintf(tmp, "R -> reset the net");
			print_info(font, color_text, line++, tmp);
			line++; // "line break"
			sprintf(tmp, "˅˅˅˅˅ U-MATRIX ˅˅˅˅˅");
			float pos_y = print_info(font, color_accent2, line++, tmp);

			int cell_size = u_matrix_size / (map_size+2);
			draw_u_matrix(map_size, U_M_FACTOR*graph_h / map_size, 2*p_off_w+graph_w, pos_y+font_size*2, cell_size);

            al_flip_display();
            redraw = false;
        }

		if (train){
			learn_ts(1);
			train = false;
		}
    }

	delete_ts();
	delete_net();
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
