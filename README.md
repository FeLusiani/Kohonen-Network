# Project presentation

Here follows an in-depth presentation of the project.

## The Kohoen Network

The projects aims to implement an interactive **Kohoen Network**, a type of **SOM** ([Self Organizing Map](https://en.wikipedia.org/wiki/Self-organizing_map)). Through the GUI the user can insert data, and visualize the network as it learns.  
Kohoen Networks are usually used to perform dimensionality reduction and normalization on data. In this project, the network maps a 2D input on a grid topology of nodes (therefore, it performs no dimensionality reduction, just data normalization).

[**Follow this link**](https://youtu.be/hWig_1mXETk) to watch the presentation (in italian) of the application.

User draws data distribution (red dots):  
<img src="img/KN_neurons.png" width="75%" height="75%">

After training:  
<img src="img/KN_neurons_tr.png" width="75%" height="75%">


## Implementation details

+ Everything has been implemented from scratch by me, with the only exception of graphics libraries (Allegro).
+ Everything has been implemented in C.

### Code structure

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

### Files & directories
**bin** -> contains the executable binaries after building  
**build** -> contains the .o object files generated during the compilation  
**conf** -> contains the configuration .h files  
**font** -> contains few fonts used by the application  
**img** -> contains the images used here  
**makefile** -> just a makefile  
**README** -> this file  
**src** -> contains the source code files  
**report** -> contains an in-depth report on the project.

## Installation

First of all, download this repository with:
```
git clone https://www.github.com/FeLusiani/Kohonen-Network.git
```

Then, you need to install Allegro5. The following steps apply to Debian/Ubuntu and are based on the official [wiki](https://wiki.allegro.cc/index.php?title=Install_Allegro5_From_Git/Linux/Debian). Installation on other distros is similar, yet some dependencies may differ. For instance, Centos/RHEL users shall refer to the relative [documentation](https://wiki.allegro.cc/index.php?title=Install_Allegro5_From_Git/Linux/Centos).
Honestly, I'm not sure which deps are strictly necessary and which are not, but unless you have limited storage capacity, I advise to download them all.

First, setup your repo list
```
cd /etc/apt/
sudo gedit sources.list
```
and add `contrib` and `free` at the end of those lines starting with deb or deb-src. Then update:

```sudo apt-get update```

Now install required dependencies:
```
sudo apt-get install build-essential git cmake cmake-curses-gui xorg-dev libgl1-mesa-dev libglu1-mesa-dev
```
and "optional" ones:
```
sudo apt-get install -y cmake g++ freeglut3-dev libxcursor-dev libpng-dev libjpeg-dev libfreetype6-dev libgtk2.0-dev libasound2-dev libpulse-dev libopenal-dev libflac-dev libdumb1-dev libvorbis-dev libphysfs-dev
```

From the Kohonen-Network directory, clone the Allegro5 git repository:
```
git clone https://github.com/liballeg/allegro5.git
```
and switch to version 5.2.4:
```
git checkout 5.2.4
```
Setup the compilation:
```
mkdir build
cd build
ccmake -DCMAKE_INSTALL_PREFIX=/usr ..
```
Inside the cmake environment, press 'C' (configure) and 'E' if it complains about few missing libraries (don't worry, it still works). Then press 'G' (generate).

Here comes the fun, compilation:
```
make
```
You can optionally add the -j option to parallelize (speed up) the compilation on multiple cores.  
Finally, install:
```
make install
```

If all the previous steps went fine, Allegro5 is properly installed and setup.  
All you need now is to compile the Autoparking application, which is very easy. Switch back to the Kohonen-Network directory, and run:
```
make run
```

## Disclaimer

This work has been carried out as final project for the 'Neural Networks' exam at SSSUP. Anyway, anyone is encouraged to fork, modify or extend it for non-commercial purposes, as long as the original author (that's me) is explicitly credited. Feel free to contact me for any doubt!
