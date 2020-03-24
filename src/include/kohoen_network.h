#define MAP_SIZE 10

enum w_distribution{RANDOM_WD, GRID_WD};

float* get_example(int k);

void init_ts(int n_examples);
void set_example(int k, float* v);
void learn_ts(int epochmax);
void delete_ts();

void init_net(int ni, int map_size, enum w_distribution w_d);
void delete_net();

void set_weight_range(float wmin, float wmax);

void set_learning_range(float ini, float fin);

void set_radius_range(float ini, float fin);

void set_learn_decay(float v);

void set_radius_decay(float v);


void get_weights(int i, int j, float* w_);

float get_learning_rate();

float get_radius();
