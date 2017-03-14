## Official Website

http://www.mupuf.org/project/synema.html


## Dependencies
* libcairo2-dev
* libdbus-glib-1-dev
* libgtk2.0-dev
* libglib2.0-dev
* librsvg2-dev
* libtar-dev
* libpcre3-dev
* ploticus


## Build Dependencies
* make
* cmake


## Documentation Dependencies
* doxygen
* graphviz


## Extra Tools Used by Developers
The following tools are used to generate source code in this project:

*  dbus-binding-tool
*  gob2

See the Makefile for how to generate the source code.


## Easy Build and Run
Use `./quick-build.sh` to build the application and `./quick-run.sh` to run it.


## Detailed Build Steps
### 1. Make a build directory

    mkdir BUILD && cd BUILD

### 2. Use cmake to generate a Makefile

    cmake .. -DBUILD_PLUGIN_<plugin name>=ON

`<plugin_name>` is the name of the plugin you want, in capital letters. See the
CMakeLists.txt file for a complete list. You can also build the application
without plugins if you want.

### 3. Build the source for real

    make

### 4. Optional: install the application on your system

    make install

If you don't install the application, you may need to use environment variables
to tell the application where to look for data, configuration, machine and
plugin files.
You can use `cmake .. -DCMAKE_INSTALL_PREFIX=/your/prefix` to choose a particular
install prefix.


## Bug Reports
Please report bugs here on Github. Doc is partially broken, no need to report it. Code builds with many deprecations as of March 2017.
We are aware of some bugs but have no time to fix them, however them being documented remains useful to prospective users and contributors.
