# 2D Computer Graphics
This software was the final project of 2DCG summer course at IMPA 2020, it's a Vector Graphics renderer with some features. 
All the source files starting with "rvg-" were provided by professor Diego Nehab and my work is completely inside "hadryan" namespace, so I'm not the author of any other file.

## Vector Graphics Renderer

![alt text](https://github.com/hadryans/CG2D-IMPA/blob/master/pngs/paris.png)

This software is able to render vector graphics scenes described by the custom extension "rvg", supporting the following elements:

- Primitive shapes (lines, triangles, polygons and ellipses)
- Bezier paths (quadratic, cubic and rational quadratic)
- Anti aliasing
- Textures
- Color transparency
- Color gradients (linear and radial)

The implementation of the project also had some optimization goals:

- Implicit tests for Bezier paths
- Acceleration structure

We studied two acceleration structures, the Regular Grid and the Shortcut Tree. Any of these could been chosen to the final assignment. I did the Shortcut Tree, because I was interested in implementing an adaptative method.

Despite the project was primarily intended to be implemented in Lua language loaded by C++ framework, Diego also gave the tools to work with C++ (e.g. makefile). Develop this project in C++ gave me the opportunity to (besides rendering much faster) include support to use parallel computing via OpenMP.

### Extras:
I also developed some tools to auxiliate my tests:
- Script to render all the rvgs files and comparing them to previous versions of the same rendered images, allowing to track any new bug introduced in opmitization stages.
- Video-creating script to test a sequence of translations in some scene

![Alt Text](https://github.com/hadryans/CG2D-IMPA/blob/master/pngs/output.gif)

## Building

Mount the docker image of [https://hub.docker.com/r/diegonehab/vg](https://hub.docker.com/r/diegonehab/vg) inside my folder directory and run

	make -f hadryan_salles_makefile

Or for a "hardcore" method, you'll need to have Lua compiled with C++, then follow the instructions inside hadryan_salles_makefile.

## Running 
The software works with shared libraries invoked by a Lua script. The command to render any RVG file is:

	luapp process.lua driver.hadryan_salles <file.rvg> <out.png> 

With support to the following flags:

	-tx <float x-axis translation>
	-ty <float y-axis translation>
	-pattern <int (1,8,16,32 or 64) samples per pixel>
	-j <int number of threads to be used by OpenMP>

## References
- Shortcut Tree: Ganacim, F.; Lima, R. S.; de Figueiredo, L. H.; Nehab, D. [“Massively-parallel vector graphics”](http://www.impa.br/~diego/publications/GanEtAl14.pdf), _ACM Transactions on Graphics (Proceedings of the ACM SIGGRAPH Asia 2014)_, 36(6):229, 2014.
- Regular Grid: Nehab, D.; Hoppe, H. [“Random-access rendering of general vector graphics”](http://www.impa.br/~diego/publications/NehHop08.pdf), _ACM Transactions on Graphics (Proceedings of the ACM SIGGRAPH Asia 2008)_, 27(5):135, 2008.
