#define MAP_SIZE 40
#define LEARN_DECAY 0.999
#define RADIUS_DECAY 0.95
#define MAX_RADIUS (MAP_SIZE / 2.0)
#define MIN_RADIUS 0.9
// 1 (GRID distribution) or 0 (RANDOM distribution)
#define WEIGHT_DISTRIBUTION 1

// a positive floating value (like 5.0)
// if lower, the same distances in the weights will produce lighter cells in the U-Matrix
#define U_M_FACTOR 5
