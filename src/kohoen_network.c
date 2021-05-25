#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "include/kohoen_network.h"
#include "KN_settings.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct out_unit out_unit;
struct out_unit {
	float o; // output
	float* w; // weights vector
};

static out_unit map[MAP_SIZE][MAP_SIZE];
static float** ts = NULL; // training examples [n_examples][input_dim]
static float* ki; // input vector [input_dim]
static int input_dim; // number of input units
// static out_unit** map; // vector 2D of out_units
static int map_size = 0; // width and heigth of map (is square)

// static int topology; // output map topology
static int nex = 0; // number of examples
static float k_radius; // current neighborhood radius
static float k_r_ini, k_r_fin; // initial, final radius
static float k_r_norm; // normal. radius in [0,1]
static float k_alpha; // current dynamic learning rate
static float k_a_ini, k_a_fin; // initial, final l. rate
static float k_a_norm; // normal. l.rate in [0,1]
static float learn_decay; // learning rate decay
static float radius_decay; // radius decay
static float w_min, w_max; // initial weight range

float frand(int xmin, int xmax){
    float range;
    range = xmax-xmin;
    return (xmin + range * rand()/(float)RAND_MAX);
}

float distance(float *x, float *w){
    int i;
    float d, sum = 0.0;
    for (i=0; i<input_dim; i++) {
        d = w[i] - x[i];
        sum = sum + d*d;
    }
    return sum;
}

void set_weight_range(float wmin, float wmax){
    w_min = wmin;
    w_max = wmax;
}

void set_learning_range(float ini, float fin){
    k_a_ini = ini;
    k_a_fin = fin;
}

void set_radius_range(float ini, float fin){
    k_r_ini = ini;
    k_r_fin = fin;
}

void set_learn_decay(float v){
    learn_decay = v;
}

void set_radius_decay(float v){
    radius_decay = v;
}

float get_radius(){
    return k_radius;
}

float get_norm_radius(){
    return k_r_norm;
}


float get_learning_rate(){
    return k_alpha;
}

float *get_example(int k){
    return ts[k];
}

int get_n_examples(){
    return nex;
}

float get_input(int i){
    return ki[i];
}

void init_ts(int n_examples){
	nex = n_examples;
	ts = (float**) malloc(nex*sizeof(float*));
	for (int i=0; i<nex; i++)
		ts[i] = (float*) malloc(input_dim*sizeof(float));
}

void set_example(int k, float* v){
	memcpy(ts[k], v, sizeof(float)*input_dim);
}

void delete_ts(){
	for (int i=0; i<nex; i++)
		free(ts[i]);
    if (ts != NULL){
	    free(ts);
        ts = NULL;
    }
	nex = 0;
}

void init_net(int ni, int map_s, enum w_distribution w_d){
    input_dim = ni; // set net variables
    map_size = map_s;
	map_size = MAP_SIZE;
    ki = (float*) malloc(ni*sizeof(float));
    // map = (out_unit**)	malloc(map_s*map_s*sizeof(out_unit));

    k_alpha = k_a_ini;
    k_radius = k_r_ini;
    k_a_norm = 1.0;
    k_r_norm = 1.0;
    srand(time(NULL));
	// init weights
    for (int i=0; i<map_size; i++)
	for (int j=0; j<map_size; j++){
    	map[i][j].w = (float*)malloc(input_dim*sizeof(float));
		for (int z=0; z<input_dim; z++){
			if (w_d == RANDOM_WD)
				map[i][j].w[z] = frand(w_min, w_max);
			else if (w_d == GRID_WD){
				if (z == 0)
					map[i][j].w[0] = w_min + j*(float)(w_max - w_min)/map_size;
				else if (z == 1)
					map[i][j].w[1] = w_min + i*(float)(w_max - w_min)/map_size;
			}
		}
	}
}

void delete_net(){
	free(ki);
	// free weights
    for (int i=0; i<map_size; i++)
		for (int j=0; j<map_size; j++)
	    	free(map[i][j].w);

	// free(map);
	map_size = 0;
	input_dim = 0;
}

void set_input(float *v){
    memcpy(ki, v, sizeof(float)*input_dim);
}

// output neuron N input in = 1 / exp (sqr_dist(N,in) / mean_sqr_dist)
void compute_norm_output(){
    double mean_sqr_dist = 0.0;
	for (int i=0; i<map_size; i++)
    for (int j=0; j<map_size; j++){
        map[i][j].o = distance(ki, map[i][j].w);
        mean_sqr_dist += map[i][j].o;
    }

    mean_sqr_dist = mean_sqr_dist / map_size*map_size;
	for (int i=0; i<map_size; i++)
	for (int j=0; j<map_size; j++){
		map[i][j].o = exp(- map[i][j].o / mean_sqr_dist);
	}
}

void select_winner(int* win_x_, int* win_y_){
    float min, d;
	int winx, winy;
    min = distance(ki, map[0][0].w);
    winx = winy = 0;

	for (int i=0; i<map_size; i++)
	for (int j=0; j<map_size; j++){
		d = distance(ki, map[i][j].w);
		if (d <= min) {
            min = d;
            winx = i;
			winy = j;
        }
	}
	*win_x_ = winx;
	*win_y_ = winy;
}

void update_weights(int win_x, int win_y)
{
    int dist2, r2;
    int rmin_x, rmax_x, rmin_y, rmax_y; // neighborhood range
    float phi; // distance function
	int int_r = (int)k_radius;
    rmin_x = MAX(win_x-int_r, 0);
	rmax_x = MIN(win_x+int_r+1, map_size);
    rmin_y = MAX(win_y-int_r, 0);
	rmax_y = MIN(win_y+int_r+1, map_size);
    r2 = int_r*int_r;

	for (int i=rmin_x; i<rmax_x; i++)
	for (int j=rmin_y; j<rmax_y; j++){
		dist2 = (win_x-i)*(win_x-i) + (win_y-j)*(win_y-j); // sqr dist in the topology (grid)
		if (dist2 > r2) continue;
		if (r2 == 0)
			phi = 1;
		else
			phi = exp(-1.5*dist2 / r2); // dist = 0 -> phi=1, dist = radius -> phi ~= 0.2
		for (int z=0; z<input_dim; z++)
			map[i][j].w[z] += phi*k_alpha*(ki[z]-map[i][j].w[z]);
	}
}

void get_weights(int i, int j, float* w_){
    memcpy(w_, map[i][j].w, sizeof(float)*input_dim);
}

void reduce_learning(){
    k_a_norm = k_a_norm * learn_decay;
    k_alpha = k_a_fin + k_a_norm*(k_a_ini - k_a_fin);
}
void reduce_radius(){
    k_r_norm = k_r_norm * radius_decay;
    k_radius = k_r_fin + k_r_norm*(k_r_ini - k_r_fin);
}

void learn_example(float *v){
    set_input(v);
    // compute_output();
	int win_i, win_j;
    select_winner(&win_i, &win_j);
    update_weights(win_i, win_j);
}

void learn_ts(int epochmax)
{
    int k, epoch = 0;
    float *p;
    while (epoch < epochmax)
    {
        for (k=0; k<nex; k++) {
            p = get_example(k);
            learn_example(p);
        }
        epoch++;
        reduce_radius();
        reduce_learning();
    }
}
