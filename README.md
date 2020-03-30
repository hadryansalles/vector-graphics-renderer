# 2D Computer Graphics
This application was the final project of 2DCG summer course at IMPA 2020, it implements a Vector Graphics renderer with some features for purely educational purposes. 
All the source files starting with "rvg-" were provided by professor Diego Nehab and my work is completely inside "hadryan_salles.h/cpp", so I'm not the author of any other file.

## Vector Graphics Renderer

![alt text](https://github.com/hadryans/CG2D-IMPA/blob/master/pngs/paris.png)

This software is able to render vector graphics scenes described by the custom extension "rvg", with support to the following features:

- Primitive shapes (lines, triangles, polygons and ellipses)
- Bezier paths (quadratic, cubic and rational quadratic)
- Anti aliasing
- Textures
- Color transparency
- Color gradients (linear and radial)

The implementation of the project had some goals:

- Implicit tests for Bezier paths
- Acceleration structure

We studied two acceleration structures, the Regular Grid and the Shortcut Tree and any of these could been chosen to the final assignment. I did the Shortcut Tree, mainly because the desire to learn an adaptative aproach.

Despite the project was primarily intended to be implemented in Lua language loaded by C++ framework, Diego also give the tools to work with C++ (e.g. makefile).  Develop that project in C++ gave me the opportunity to (besides rendering much faster :P) include support to OpenMP.

### Extras:
I developed some tools to auxiliate my tests, among them:
- Script to test all the rvgs files comparing with previous versions of the same rendered images, allowing to track any new bug introduced in opmitization stages.
- Video-creating script to test a sequence of translations in some scene

![Alt Text](https://github.com/hadryans/CG2D-IMPA/blob/master/pngs/output.gif)

## Building

I'm not guessing that anyone would be interested in build that software, but if you want, you could just mount the docker image on [https://hub.docker.com/r/diegonehab/vg](https://hub.docker.com/r/diegonehab/vg) inside my folder directory and run

	make -f hadryan_salles_makefile

Or, for a "hardcore" method, you'll need a Lua program compiled with C++ and follow the instructions inside hadryan_salles_makefile.

## Running 
The software works with driver shared objects invoked by a Lua script. The command to render any RVG file is:

	luapp process.lua driver.hadryan_salles <file.rvg> <out.png> 

With support to the following flags:

	-tx <float x-axis translation>
	-ty <float y-axis translation>
	-pattern <int (1,8,16,32 or 64) samples per pixel>
	-j <int number of threads to be used by OpenMP>

## References
- Shortcut Tree: Ganacim, F.; Lima, R. S.; de Figueiredo, L. H.; Nehab, D. [“Massively-parallel vector graphics”](http://www.impa.br/~diego/publications/GanEtAl14.pdf), _ACM Transactions on Graphics (Proceedings of the ACM SIGGRAPH Asia 2014)_, 36(6):229, 2014.
- Regular Grid: Nehab, D.; Hoppe, H. [“Random-access rendering of general vector graphics”](http://www.impa.br/~diego/publications/NehHop08.pdf), _ACM Transactions on Graphics (Proceedings of the ACM SIGGRAPH Asia 2008)_, 27(5):135, 2008.
