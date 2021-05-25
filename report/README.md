# Project presentation

Here follows an in-depth presentation of the project.

## The Kohoen Network

The projects aims to implement an interactive **Kohoen Network**, a type of **SOM** ([Self Organizing Map](https://en.wikipedia.org/wiki/Self-organizing_map)). Through the GUI the user can insert data, and visualize the network as it learns.  
Kohoen Networks are usually used to perform dimensionality reduction and normalization on data. In this project, the network maps a 2D input on a grid topology of nodes (therefore, it performs no dimensionality reduction, just data normalization).

[**Follow this link**](https://youtu.be/hWig_1mXETk) to watch the presentation (in italian) of the application.



## Implementation details

+ The code in `src/kohoen_network.c` implements the Kohoen Network functionality. Some settings are configured in the `cong/KN_settings.h` configuration file. See the following sub-section for an explanation of some constants.
+ The code in `src/main` implements the application (GUI functionality) using the `Allegro` library. Some settings are configured in the `cong/display_settings.h` configuration file.

### KN configuration
Following is an explanation of the constants that can be set in the configuration file `cong/KN_settings.h`, regarding the functioning of the Kohoen Network:

**MAP_SIZE** -> determines the size of the network (*n. of units = MAP_SIZE^2*).  
**LEARN_DECAY** -> learning decay factor for the KN (determines how fast the network stabilizes).  
**RADIUS_DECAY** -> radius decay factor for the KN (determines how fast each neuron becomes insensible to its neighbours' updates).  
**MAX_RADIUS** -> max (starting) radius value.  
**MIN_RADIUS** -> min (final) radius value.  
**WEIGHT_DISTRIBUTION** -> determines the starting distrubition of the weights. If set to `1`, the weights will be initialized equally spaced across the map (creating a grid pattern). If set to `0`, each weight will be initialized at random across the map.  

