UNAME:=$(shell uname)

# ??D Separate intermediary and output files by UNAME?

# Lua version in use
V?=53

#DEF?=-DRVG_THICKEN_WITH_CUBICS

#---
# Darwin instructions for MacPorts on El Capitan
# Run: port install lua libpng libb64 freetype boost harfbuzz-icu lapack
# Tweaks:
# b64.pc mising.
#   Adjust "prefix" and copy to /opt/local/lib/pkgconfig
#---

SOLDFLAGS_Darwin:= -L/opt/local/lib -L/opt/local/lib/libomp -bundle -undefined dynamic_lookup
INC_Darwin= -I/opt/local/include
CP_Darwin=gcp
CC_Darwin=clang-mp-8.0
CXX_Darwin=clang++-mp-8.0
LP_INC_Darwin=$(shell PKG_CONFIG_PATH=/opt/local/lib/lapack/pkgconfig $(PKG) --cflags lapacke)
LP_LIB_Darwin=$(shell PKG_CONFIG_PATH=/opt/local/lib/lapack/pkgconfig $(PKG) --libs lapack)

#---
# Linux instructions for Ubuntu 18.04
# Run: apt-get install libboost-dev libharfbuzz-dev libb64-dev libfreetype6-dev liblapacke-dev
# Tweaks:
# b64.pc missing.
#   Adjust "prefix" and copy to /usr/lib/x86_64-linux-gnu/pkgconfig/b64.pc
#---

SOLDFLAGS_Linux:= -shared -fpic
CP_Linux=cp
CC_Linux=gcc
CXX_Linux=g++
LP_INC_Linux=$(shell $(PKG) --cflags lapacke)
LP_LIB_Linux=$(shell $(PKG) --libs lapack)

#---
# Subset of targets to build
#---
vg_build_strokers?=no
vg_build_cairo?=no
vg_build_qt5?=no
vg_build_skia?=no
vg_build_distroke?=no
vg_build_nvpr?=no
vg_build_tests?=no
vg_build_gperftools?=no

#---
# Probably no need to change anything below here
#---
CP=$(CP_$(UNAME))
CC=$(CC_$(UNAME))
CXX=$(CXX_$(UNAME))

# Load includes and libs from pkg-config
PKG:=pkg-config
LUA_INC:=$(shell $(PKG) --cflags luapp$V)
FT_INC:=$(shell $(PKG) --cflags freetype2)
FT_LIB:=$(shell $(PKG) --libs freetype2)
PNG_INC:=$(shell $(PKG) --cflags libpng16)
PNG_LIB:=$(shell $(PKG) --libs libpng16)
B64_LIB=$(shell $(PKG) --libs b64)
B64_INC=$(shell $(PKG) --cflags b64)
HB_LIB=$(shell $(PKG) --libs harfbuzz-icu)
HB_INC=$(shell $(PKG) --cflags harfbuzz-icu)
#LP_INC=$(LP_INC_$(UNAME))
#LP_LIB=$(LP_LIB_$(UNAME))

ifeq ($(vg_build_gperftools),yes)
GPERFTOOLS_LIB=-lprofiler
endif

OMP_LIB= -fopenmp -lgomp

ifeq ($(vg_build_cairo),yes)
CAIRO_LIB=$(shell $(PKG) --libs cairo)
CAIRO_INC=$(shell $(PKG) --cflags cairo)
endif

ifeq ($(vg_build_qt5),yes)
QT5_INC=$(shell $(PKG) --cflags Qt5Gui)
QT5_LIB=$(shell $(PKG) --libs Qt5Gui)
endif

ifeq ($(vg_build_skia),yes)
SKIA_INC=$(shell $(PKG) --cflags skia)
SKIA_LIB=$(shell $(PKG) --libs skia)
endif

EGL_LIB:=-lGLEW -lEGL -lGL
EGL_INC:=

strokers/rvg-lua-strokers.o: INC += $(LUA_INC)

ST_OBJ:=
ST_LIB:= $(B64_LIB) $(PNG_LIB)

ST_DEF:= -DSTROKER_RVG
ST_RVG_OBJ:= strokers/rvg-stroker-rvg.o
ST_OBJ+= $(ST_RVG_OBJ)

ifeq ($(vg_build_strokers),yes)
ST_DEF+= -DSTROKER_AGG
ST_AGG_INC:=$(shell $(PKG) --cflags agg-stroker)
ST_AGG_LIB:=$(shell $(PKG) --libs agg-stroker)
ST_AGG_OBJ:= strokers/agg/rvg-stroker-agg.o
ST_OBJ+= $(ST_AGG_OBJ)
ST_LIB+= $(ST_AGG_LIB)

ST_DEF+= -DSTROKER_CAIRO
ST_CAIRO_LIB=$(shell $(PKG) --libs cairo-stroker)
ST_CAIRO_INC=$(shell $(PKG) --cflags cairo-stroker)
ST_CAIRO_OBJ:= strokers/cairo/rvg-stroker-cairo.o
ST_OBJ+= $(ST_CAIRO_OBJ)
ST_LIB+= $(ST_CAIRO_LIB)

ST_DEF+= -DSTROKER_GS
ST_GS_LIB=$(shell $(PKG) --libs ghostscript-stroker)
ST_GS_INC=$(shell $(PKG) --cflags ghostscript-stroker)
ST_GS_OBJ:= strokers/gs/rvg-stroker-gs.o strokers/gs/rvg-gs.o
ST_OBJ+= $(ST_GS_OBJ)
ST_LIB+= $(ST_GS_LIB)

ST_DEF+= -DSTROKER_MUPDF
ST_MUPDF_LIB=$(shell $(PKG) --libs mupdf-stroker)
ST_MUPDF_INC=$(shell $(PKG) --cflags mupdf-stroker)
ST_MUPDF_OBJ:= strokers/mupdf/rvg-stroker-mupdf.o
ST_OBJ+= $(ST_MUPDF_OBJ)
ST_LIB+= $(ST_MUPDF_LIB)

ST_DEF+= -DSTROKER_QT5
ST_QT5_INC:=$(shell $(PKG) --cflags Qt5Gui)
ST_QT5_LIB:=$(shell $(PKG) --libs Qt5Gui)
ST_QT5_OBJ:= strokers/qt5/rvg-stroker-qt5.o
ST_OBJ+= $(ST_QT5_OBJ)
ST_LIB+= $(ST_QT5_LIB)

ST_DEF+= -DSTROKER_SKIA
ST_SKIA_INC:=$(shell $(PKG) --cflags skia)
ST_SKIA_LIB:=$(shell $(PKG) --libs skia)
ST_SKIA_OBJ:= strokers/skia/rvg-stroker-skia.o
ST_OBJ+= $(ST_SKIA_OBJ)
ST_LIB+= $(ST_SKIA_LIB)

ST_DEF+= -DSTROKER_LIVAROT
ST_LIVAROT_INC:=$(shell $(PKG) --cflags livarot-stroker)
ST_LIVAROT_LIB:=$(shell $(PKG) --libs livarot-stroker)
ST_LIVAROT_OBJ:= strokers/livarot/rvg-stroker-livarot.o
ST_OBJ+= $(ST_LIVAROT_OBJ)
ST_LIB+= $(ST_LIVAROT_LIB)

ST_DEF+= -DSTROKER_OPENVG_RI
ST_OPENVGRI_OBJ:= strokers/openvg-ri/rvg-stroker-openvg-ri.o strokers/openvg-ri/riMath.o strokers/openvg-ri/riPath.o
ST_OBJ+= $(ST_OPENVGRI_OBJ)

ifeq ($(UNAME),Darwin)
ST_DEF+= -DSTROKER_QUARTZ
ST_QUARTZ_OBJ:= strokers/quartz/rvg-stroker-quartz.o
ST_OBJ+= $(ST_QUARTZ_OBJ)
ST_LIB+= -framework CoreGraphics
endif
endif

# Automatically generate dependencies
DEPFLAGS=-MT $@ -MMD -MP -MF $*.d
CXXFLAGS=-fopenmp -O2 -g -W -std=c++14 -pedantic -Wall -fpic -fvisibility=hidden $(DEPFLAGS) $(ST_DEF)
#CXXFLAGS+=-ftemplate-backtrace-limit=0
SOLDFLAGS:=$(SOLDFLAGS_$(UNAME))

INC := -I./
rvg-lua-image.o: INC += $(LUA_INC)
rvg-lua.o: INC += $(LUA_INC)
rvg-lua-facade.o: INC += $(LUA_INC)
rvg-lua-path-data.o: INC += $(LUA_INC)
rvg-lua-filter.o: INC += $(LUA_INC)
rvg-lua-path-filters.o: INC += $(LUA_INC)
rvg-lua-scene-filters.o: INC += $(LUA_INC) -Istrokers
rvg-lua-scene-data.o: INC += $(LUA_INC)
rvg-lua-triangle-data.o: INC += $(LUA_INC)
rvg-lua-circle-data.o: INC += $(LUA_INC)
rvg-lua-rect-data.o: INC += $(LUA_INC)
rvg-lua-polygon-data.o: INC += $(LUA_INC)
rvg-lua-patch-data.o: INC += $(LUA_INC)
rvg-lua-patch.o: INC += $(LUA_INC)
rvg-lua-xform.o: INC += $(LUA_INC)
rvg-lua-bbox.o: INC += $(LUA_INC)
rvg-lua-stroke-style.o: INC += $(LUA_INC)
rvg-lua-shape.o: INC += $(LUA_INC)
rvg-lua-rgba.o: INC += $(LUA_INC)
rvg-lua-spread.o: INC += $(LUA_INC)
rvg-lua-winding-rule.o: INC += $(LUA_INC)
rvg-lua-color-ramp.o: INC += $(LUA_INC)
rvg-lua-linear-gradient-data.o: INC += $(LUA_INC)
rvg-lua-radial-gradient-data.o: INC += $(LUA_INC)
rvg-lua-texture-data.o: INC += $(LUA_INC)
rvg-lua-paint.o: INC += $(LUA_INC)
rvg-lua-scene.o: INC += $(LUA_INC)
rvg-xform-svd.o: INC += $(LP_INC)

rvg-lua-chronos.o: INC += $(LUA_INC)
rvg-lua-util.o: INC += $(LUA_INC)
ifeq ($(vg_build_gperftools),yes)
rvg-lua-util.o: DEF += -DUSE_GPERFTOOLS
endif
rvg-lua-base64.o: INC += $(LUA_INC) $(B64_INC)
rvg-lua-harfbuzz.o: INC += $(LUA_INC) $(FT_INC) $(HB_INC)
rvg-lua-freetype.o: INC += $(LUA_INC) $(FT_INC)
rvg-lua-freetype-typeface.o: INC += $(LUA_INC) $(FT_INC)

rvg-pngio.o: INC += $(PNG_INC)
rvg-base64.o: INC += $(B64_INC)
rvg-util.o: INC += $(LP_INC)

rvg-driver-distroke.o: INC += $(LUA_INC)
rvg-driver-svg.o: INC += $(LUA_INC)
rvg-driver-eps.o: INC += $(LUA_INC)
rvg-driver-png.o: INC += $(LUA_INC)
rvg-driver-rvg-lua.o: INC += $(LUA_INC)
rvg-driver-rvg-cpp.o: INC += $(LUA_INC)
rvg-driver-nvpr.o: INC += $(LUA_INC) $(EGL_INC)
rvg-driver-cairo.o: INC += $(LUA_INC) $(CAIRO_INC)
rvg-driver-qt5.o: INC += $(LUA_INC) $(QT5_INC)
rvg-driver-skia.o: INC += $(LUA_INC) $(SKIA_INC)
rvg-driver-rg.o: INC += $(LUA_INC)

rvg-freetype.o: INC += $(FT_INC)
test-text.o: INC += $(FT_INC)

strokers/livarot/rvg-stroker-livarot.o: INC += $(ST_LIVAROT_INC)
strokers/cairo/rvg-stroker-cairo.o: INC += $(ST_CAIRO_INC)
strokers/gs/rvg-gs.o: INC += $(ST_GS_INC)
strokers/qt5/rvg-stroker-qt5.o: INC += $(ST_QT5_INC)
strokers/mupdf/rvg-stroker-mupdf.o: INC += $(ST_MUPDF_INC)
strokers/skia/rvg-stroker-skia.o: INC += $(ST_SKIA_INC)
strokers/agg/rvg-stroker-agg.o: INC += $(ST_AGG_INC)

DRV_OBJ := rvg-lua-base64.o \
	rvg-base64.o \
	rvg-lua-image.o \
	rvg-pngio.o \
	rvg-xform.o \
	rvg-xform-svd.o \
	rvg-path-data.o \
	rvg-shape.o \
	rvg-svg-path-commands.o \
	rvg-svg-path-token.o \
	rvg-unorm.o \
	rvg-util.o \
	rvg-stroke-style.o \
	rvg-named-colors.o \
	rvg-facade-scene-data.o \
	rvg-gaussian-quadrature.o \
	rvg-lua-facade.o \
	rvg-lua-scene-data.o \
	rvg-lua-path-data.o \
	rvg-lua-triangle-data.o \
	rvg-lua-polygon-data.o \
	rvg-lua-circle-data.o \
	rvg-lua-rect-data.o \
	rvg-lua-patch-data.o \
	rvg-lua-patch.o \
	rvg-lua-xform.o \
	rvg-lua-bbox.o \
	rvg-lua-stroke-style.o \
	rvg-lua-shape.o \
	rvg-lua-rgba.o \
	rvg-lua-winding-rule.o \
	rvg-lua-spread.o \
	rvg-lua-color-ramp.o \
	rvg-lua-linear-gradient-data.o \
	rvg-lua-radial-gradient-data.o \
	rvg-lua-texture-data.o \
	rvg-lua-paint.o \
	rvg-lua-scene.o \
	rvg-lua.o \
	strokers/rvg-stroker-rvg.o

SO_STROKERS_OBJ := strokers/rvg-lua-strokers.o $(DRV_OBJ) $(ST_OBJ)
SO_BASE64_OBJ:= rvg-lua-base64.o rvg-base64.o rvg-lua.o
SO_UTIL_OBJ:= rvg-lua-util.o rvg-lua.o rvg-xform.o rvg-xform-svd.o
SO_CHRONOS_OBJ:= rvg-lua-chronos.o rvg-chronos.o rvg-lua.o
SO_IMAGE_OBJ:= rvg-lua-image.o rvg-pngio.o rvg-lua.o
SO_FACADE_OBJ:= $(DRV_OBJ)

SO_FREETYPE_OBJ:= \
	rvg-freetype.o \
	rvg-lua-freetype.o \
	rvg-lua-freetype-typeface.o \
	rvg-lua-path-data.o \
	rvg-path-data.o \
	rvg-lua.o

SO_FILTER_OBJ:= \
	rvg-lua-filter.o \
	rvg-lua-path-filters.o \
	rvg-lua-scene-filters.o \
	$(DRV_OBJ) \
	$(ST_OBJ)

SO_HARFBUZZ_OBJ:= rvg-lua-harfbuzz.o rvg-lua.o

SO_PNG_DRV_OBJ:= rvg-driver-png.o $(DRV_OBJ)
SO_SVG_DRV_OBJ:= rvg-driver-svg.o $(DRV_OBJ)
SO_EPS_DRV_OBJ:= rvg-driver-eps.o $(DRV_OBJ)
SO_NVPR_DRV_OBJ:= rvg-driver-nvpr.o $(DRV_OBJ)
SO_RVG_LUA_DRV_OBJ:= rvg-driver-rvg-lua.o rvg-scene-f-print-rvg.o $(DRV_OBJ)
SO_RVG_CPP_DRV_OBJ:= rvg-driver-rvg-cpp.o rvg-scene-f-print-rvg.o $(DRV_OBJ)
SO_CAIRO_DRV_OBJ:= rvg-driver-cairo.o $(DRV_OBJ)
SO_QT5_DRV_OBJ:= rvg-driver-qt5.o $(DRV_OBJ)
SO_SKIA_DRV_OBJ:= rvg-driver-skia.o $(DRV_OBJ)
SO_RG_DRV_OBJ:= rvg-driver-rg.o $(DRV_OBJ)
SO_DISTROKE_DRV_OBJ:= rvg-driver-distroke.o $(DRV_OBJ)

T_TEXT_OBJ:= test-text.o rvg-freetype.o
T_TUPLE_OBJ:= test-tuple.o
T_UNORM_OBJ:= test-unorm.o rvg-unorm.o
T_GAUSSIAN_QUADRATURE_OBJ:= test-gaussian-quadrature.o rvg-gaussian-quadrature.o
T_ARC_LENGTH_OBJ:= test-arc-length.o rvg-gaussian-quadrature.o
T_UTIL_OBJ:= test-util.o rvg-util.o
T_PATH_OBJ:= test-path.o rvg-svg-path-commands.o rvg-svg-path-token.o rvg-path-data.o
T_XFORM_OBJ:= test-xform.o rvg-xform.o rvg-xform-svd.o rvg-path-data.o
T_TUPLE_BEZIER_OBJ:= test-bezier.o
T_POINT_OBJ:= test-point.o
T_COLOR_OBJ:= test-color.o rvg-named-colors.o
T_IMAGE_OBJ:= test-image.o rvg-pngio.o rvg-base64.o
T_PAINT_OBJ:= test-paint.o
T_SHAPE_OBJ:= test-shape.o
T_FACADE_OBJ:= test-facade.o rvg-facade.o rvg-facade-scene-data.o rvg-path-data.o
T_FIND_PARAMETERS_OBJ:= test-find-parameters.o rvg-path-data.o rvg-svg-path-commands.o rvg-svg-path-token.o rvg-stroke-style.o rvg-xform-svd.o rvg-util.o
T_OFFSET_OBJ:= test-offset.o rvg-path-data.o rvg-svg-path-commands.o rvg-svg-path-token.o rvg-stroke-style.o rvg-xform-svd.o rvg-util.o rvg-gaussian-quadrature.o
T_EVOLUTE_OBJ:= test-evolute.o rvg-path-data.o rvg-svg-path-commands.o rvg-svg-path-token.o rvg-stroke-style.o rvg-xform-svd.o rvg-util.o rvg-gaussian-quadrature.o
T_STROKE_OBJ := test-stroke.o rvg-util.o rvg-gaussian-quadrature.o rvg-path-data.o rvg-svg-path-commands.o rvg-svg-path-token.o rvg-stroke-style.o rvg-xform-svd.o

OBJ:= \
	$(SO_BASE64_OBJ) \
	$(SO_UTIL_OBJ) \
	$(SO_CHRONOS_OBJ) \
	$(SO_IMAGE_OBJ) \
	$(SO_FACADE_OBJ) \
	$(SO_FREETYPE_OBJ) \
	$(SO_HARFBUZZ_OBJ) \
	$(SO_FILTER_OBJ) \
	$(SO_PNG_DRV_OBJ) \
	$(SO_SVG_DRV_OBJ) \
	$(SO_EPS_DRV_OBJ) \
	$(SO_RVG_CPP_DRV_OBJ) \
	$(SO_RVG_LUA_DRV_OBJ) \
    $(SO_STROKERS_OBJ)

TARGETS:= \
	driver/rvg_cpp.so \
	driver/hadryan-salles.so \
	driver/rvg_lua.so \
	driver/eps.so \
	driver/svg.so \
	facade.so \
	filter.so \
	harfbuzz.so \
	freetype.so \
	image.so \
	base64.so \
	util.so \
	chronos.so \
	strokers.so

ifeq ($(vg_build_nvpr),yes)
OBJ += $(SO_NVPR_DRV_OBJ)
TARGETS += driver/nvpr.so
endif

ifeq ($(vg_build_png),yes)
OBJ+= $(SO_PNG_DRV_OBJ)
TARGETS += driver/hadryan-salles.so
endif

ifeq ($(vg_build_cairo),yes)
OBJ+= $(SO_CAIRO_DRV_OBJ)
TARGETS += driver/cairo.so
endif

ifeq ($(vg_build_qt5),yes)
OBJ+= $(SO_QT5_DRV_OBJ)
TARGETS += driver/qt5.so
endif

ifeq ($(vg_build_skia),yes)
OBJ+= $(SO_SKIA_DRV_OBJ)
TARGETS += driver/skia.so
endif


ifeq ($(vg_build_rg),yes)
OBJ+= $(SO_RG_DRV_OBJ)
TARGETS += driver/rg.so
endif

ifeq ($(vg_build_distroke),yes)
OBJ += $(SO_DISTROKE_DRV_OBJ)
TARGETS += driver/distroke.so
endif

ifeq ($(vg_build_tests),yes)
OBJ += \
	$(T_UTIL_OBJ) \
	$(T_TEXT_OBJ) \
	$(T_TUPLE_OBJ) \
	$(T_UNORM_OBJ) \
	$(T_GAUSSIAN_QUADRATURE_OBJ) \
	$(T_ARC_LENGTH_OBJ) \
	$(T_PATH_OBJ) \
	$(T_XFORM_OBJ) \
	$(T_TUPLE_BEZIER_OBJ) \
	$(T_PACKED_BEZIER_OBJ) \
	$(T_OFFSET_OBJ) \
	$(T_EVOLUTE_OBJ) \
	$(T_SPLIT_CUSPS_OBJ) \
	$(T_FIND_PARAMETERS_OBJ) \
	$(T_POINT_OBJ) \
	$(T_COLOR_OBJ) \
	$(T_IMAGE_OBJ) \
	$(T_PAINT_OBJ) \
	$(T_SHAPE_OBJ) \
	$(T_STROKE_OBJ) \
	$(T_FACADE_OBJ)

TARGETS += \
	test-paint \
	test-text \
	test-tuple \
	test-util \
	test-image \
	test-path \
	test-xform \
	test-bezier \
	test-point \
	test-color \
	test-shape \
	test-facade \
	test-find-parameters \
	test-offset \
	test-evolute \
	test-stroke \
	test-unorm \
	test-gaussian-quadrature \
	test-arc-length
endif

OBJ:=$(sort $(OBJ))

# The dependency file for each each OBJ
DEP:=$(OBJ:.o=.d)

# Do not delete dependency files
.PRECIOUS: $(DEP)

# Targets
all: $(TARGETS)
#all: test-offset

# Don't worry if dependency files are not found
$(DEP): ;

# Include all dependency files to make them active
-include $(DEP)

test-shape: $(T_SHAPE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-offset: $(T_OFFSET_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-evolute: $(T_EVOLUTE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-stroke: $(T_STROKE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-find-parameters: $(T_FIND_PARAMETERS_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-facade: $(T_FACADE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-paint: $(T_PAINT_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-unorm: $(T_UNORM_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-gaussian-quadrature: $(T_GAUSSIAN_QUADRATURE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-arc-length: $(T_ARC_LENGTH_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-tuple: $(T_TUPLE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-text: $(T_TEXT_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(FT_LIB)

test-util: $(T_UTIL_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-path: $(T_PATH_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-xform: $(T_XFORM_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LP_LIB)

test-bezier: $(T_TUPLE_BEZIER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-point: $(T_POINT_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-color: $(T_COLOR_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test-image: $(T_IMAGE_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB)

strokers.so: $(SO_STROKERS_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(ST_LIB) $(LP_LIB)

base64.so: $(SO_BASE64_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(B64_LIB)

util.so: $(SO_UTIL_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(LP_LIB) $(GPERFTOOLS_LIB)

chronos.so: $(SO_CHRONOS_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^

image.so: $(SO_IMAGE_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB)

facade.so: $(SO_FACADE_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB)

freetype.so: $(SO_FREETYPE_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(FT_LIB)

harfbuzz.so: $(SO_HARFBUZZ_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(HB_LIB)

filter.so: $(SO_FILTER_OBJ)
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(ST_LIB)

driver/distroke.so: $(SO_DISTROKE_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(OMP_LIB) $(LP_LIB)

driver/hadryan-salles.so: $(SO_PNG_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB) $

driver/svg.so: $(SO_SVG_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB)

driver/eps.so: $(SO_EPS_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB)

driver/rvg_lua.so: $(SO_RVG_LUA_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB)

driver/rvg_cpp.so: $(SO_RVG_CPP_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB)

driver/nvpr.so: $(SO_NVPR_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(EGL_LIB) $(B64_LIB) $(LP_LIB)

driver/cairo.so: $(SO_CAIRO_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(CAIRO_LIB) $(LP_LIB)

driver/qt5.so: $(SO_QT5_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(QT5_LIB) $(LP_LIB)

driver/skia.so: $(SO_SKIA_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(SKIA_LIB) $(LP_LIB)

driver/rg.so: $(SO_RG_DRV_OBJ)
	mkdir -p driver
	$(CXX) $(SOLDFLAGS) -o $@ $^ $(PNG_LIB) $(B64_LIB) $(LP_LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(DEF) $(INC_$(UNAME)) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INC) $(DEF) $(INC_$(UNAME)) -o $@ -c $<

%.o: %.mm
	$(CXX) $(CXXFLAGS) $(INC) $(DEF) $(INC_$(UNAME)) -o $@ -c $<

.PHONY: clean config

clean:
	\rm -rf $(OBJ) $(DEP) $(TARGETS) .vs Intermediate

# Show paths
config:
	@echo LUA_INC: $(LUA_INC)
	@echo FT_INC: $(FT_INC)
	@echo FT_LIB: $(FT_LIB)
	@echo PNG_INC: $(PNG_INC)
	@echo PNG_LIB: $(PNG_LIB)
	@echo B64_LIB: $(B64_LIB)
	@echo B64_INC: $(B64_INC)
	@echo HB_LIB: $(HB_LIB)
	@echo HB_INC: $(HB_INC)
	@echo LP_LIB: $(LP_LIB)
	@echo LP_INC: $(LP_INC)

DIST_CPP_SRC := \
	rvg-adjacent-range.h \
	rvg-align.h \
	rvg-arc-length.h \
	rvg-aspect.h \
	rvg-base64.cpp \
	rvg-base64.h \
	rvg-bbox.h \
	rvg-bezier-arc-length.h \
	rvg-bezier.h \
	rvg-bisect.h \
	rvg-canonize-rational-quadratic-bezier.h \
	rvg-chronos.cpp \
	rvg-chronos.h \
	rvg-circle-data.h \
	rvg-color-ramp.h \
	rvg-cubic-bezier-approximation.h \
	rvg-cubic-bezier-bezier-approximator.h \
	rvg-cubic-bezier-evolute-approximator.h \
	rvg-cubic-bezier-offset-approximator.h \
	rvg-cubic-parameters-f-forward-if.h \
	rvg-dashing-parameters-f-forward-if.h \
	rvg-decorated-path-f-forward-and-backward.h \
	rvg-decorated-path-f-forward-if.h \
	rvg-decorated-path-f-simplify-joins.h \
	rvg-decorated-path-f-thicken.h \
	rvg-driver-cairo.cpp \
	rvg-driver-cairo.h \
	rvg-driver-png.cpp \
	hadryan-path-builder.h \
	hadryan-path-builder.cpp \
	rvg-driver-png.h \
	rvg-driver-eps.cpp \
	rvg-driver-eps.h \
	rvg-driver-rvg-cpp.cpp \
	rvg-driver-rvg-cpp.h \
	rvg-driver-rvg-lua.cpp \
	rvg-driver-rvg-lua.h \
	rvg-driver-skia.cpp \
	rvg-driver-skia.h \
	rvg-driver-svg.cpp \
	rvg-driver-svg.h \
	rvg-facade.cpp \
	rvg-facade.h \
	rvg-facade-painted-compound.h \
	rvg-facade-scene-data.cpp \
	rvg-facade-scene-data.h \
	rvg-facade-stencil-compound.h \
	rvg-floatint.h \
	rvg-freetype.cpp \
	rvg-freetype.h \
	rvg-freetype-typeface.h \
	rvg-gaussian-quadrature.cpp \
	rvg-gaussian-quadrature.h \
	rvg-golden-section-search.h \
	rvg-harfbuzz-buffer.h \
	rvg-harfbuzz-features.h \
	rvg-harfbuzz-font.h \
	rvg-i-cubic-bezier-approximator.h \
	rvg-i-cubic-parameters-f-forwarder.h \
	rvg-i-cubic-parameters.h \
	rvg-i-dashing-parameters-f-forwarder.h \
	rvg-i-dashing-parameters.h \
	rvg-i-decorated-path-f-forwarder.h \
	rvg-i-decorated-path-f-thicken.h \
	rvg-i-decorated-path.h \
	rvg-i-image.h \
	rvg-i-input-path-f-forwarder.h \
	rvg-i-input-path.h \
	rvg-i-join-parameters-f-forwarder.h \
	rvg-i-join-parameters.h \
	rvg-image.h \
	rvg-i-monotonic-parameters-f-forwarder.h \
	rvg-i-monotonic-parameters.h \
	rvg-indent.h \
	rvg-inplace-polynomial.h \
	rvg-input-path-f-close-contours.h \
	rvg-input-path-f-downgrade-degenerate.h \
	rvg-input-path-f-find-cubic-parameters.h \
	rvg-input-path-f-find-monotonic-parameters.h \
	rvg-input-path-f-forward-if.h \
	rvg-input-path-f-monotonize.h \
	rvg-input-path-f-rational-quadratic-to-cubics.h \
	rvg-input-path-f-simplify.h \
	rvg-input-path-f-stroke.h \
	rvg-input-path-f-to-regular-path.h \
	rvg-input-path-f-to-svg-path.h \
	rvg-input-path-f-xform.h \
	rvg-i-offsetting-parameters-f-forwarder.h \
	rvg-i-offsetting-parameters.h \
	rvg-i-parameters-f-forwarder.h \
	rvg-i-parameters-f-hold.h \
	rvg-i-path-f-forwarder.h \
	rvg-i-path.h \
	rvg-i-point-decorated-path-f-forwarder.h \
	rvg-i-point-decorated-path.h \
	rvg-i-point.h \
	rvg-i-point-input-path-f-forwarder.h \
	rvg-i-point-input-path.h \
	rvg-i-point-path-f-forwarder.h \
	rvg-i-point-path.h \
	rvg-i-point-regular-path-f-forwarder.h \
	rvg-i-point-regular-path.h \
	rvg-i-quadratic-bezier-approximator.h \
	rvg-i-regular-path-f-forwarder.h \
	rvg-i-regular-path.h \
	rvg-i-scene-data-f-forwarder.h \
	rvg-i-scene-data.h \
	rvg-i-sink.h \
	rvg-i-strokable.h \
	rvg-i-svg-path.h \
	rvg-i-typeface.h \
	rvg-i-xformable.h \
	rvg-i-xform.h \
	rvg-join-parameters-f-forward-if.h \
	rvg-linear-gradient-data.h \
	rvg-lua-base64.cpp \
	rvg-lua-base64.h \
	rvg-lua-bbox.cpp \
	rvg-lua-bbox.h \
	rvg-lua-chronos.cpp \
	rvg-lua-chronos.h \
	rvg-lua-circle-data.cpp \
	rvg-lua-circle-data.h \
	rvg-lua-color-ramp.cpp \
	rvg-lua-color-ramp.h \
	rvg-lua.cpp \
	rvg-lua-facade.cpp \
	rvg-lua-facade.h \
	rvg-lua-filter.cpp \
	rvg-lua-freetype.cpp \
	rvg-lua-freetype.h \
	rvg-lua-freetype-typeface.cpp \
	rvg-lua-freetype-typeface.h \
	rvg-lua.h \
	rvg-lua-harfbuzz.cpp \
	rvg-lua-harfbuzz.h \
	rvg-lua-image.cpp \
	rvg-lua-image.h \
	rvg-lua-linear-gradient-data.cpp \
	rvg-lua-linear-gradient-data.h \
	rvg-lua-paint.cpp \
	rvg-lua-paint.h \
	rvg-lua-patch.cpp \
	rvg-lua-patch-data.cpp \
	rvg-lua-patch-data.h \
	rvg-lua-patch.h \
	rvg-lua-path-data.cpp \
	rvg-lua-path-data.h \
	rvg-lua-path-filters.cpp \
	rvg-lua-path-filters.h \
	rvg-lua-polygon-data.cpp \
	rvg-lua-polygon-data.h \
	rvg-lua-radial-gradient-data.cpp \
	rvg-lua-radial-gradient-data.h \
	rvg-lua-rect-data.cpp \
	rvg-lua-rect-data.h \
	rvg-lua-rgba.cpp \
	rvg-lua-rgba.h \
	rvg-lua-scene.cpp \
	rvg-lua-scene-data.cpp \
	rvg-lua-scene-data.h \
	rvg-lua-scene-filters.cpp \
	rvg-lua-scene-filters.h \
	rvg-lua-scene.h \
	rvg-lua-shape.cpp \
	rvg-lua-shape.h \
	rvg-lua-spread.cpp \
	rvg-lua-spread.h \
	rvg-lua-strokable.h \
	rvg-lua-stroke-style.cpp \
	rvg-lua-stroke-style.h \
	rvg-lua-texture-data.cpp \
	rvg-lua-texture-data.h \
	rvg-lua-triangle-data.cpp \
	rvg-lua-triangle-data.h \
	rvg-lua-util.cpp \
	rvg-lua-util.h \
	rvg-lua-winding-rule.cpp \
	rvg-lua-winding-rule.h \
	rvg-lua-xformable.h \
	rvg-lua-xform.cpp \
	rvg-lua-xform.h \
	rvg-meta.h \
	rvg-monotonic-parameters-f-forward-if.h \
	rvg-named-colors.cpp \
	rvg-named-colors.h \
	rvg-offsetting-parameters-f-forward-if.h \
	rvg-painted-shape-data.h \
	rvg-paint.h \
	rvg-patch-data.h \
	rvg-patch.h \
	rvg-path-data.cpp \
	rvg-path-data.h \
	rvg-path-data.hpp \
	rvg-path-f-find-offsetting-parameters.h \
	rvg-path-f-null.h \
	rvg-path-f-spy.h \
	rvg-path-f-to-lua-path.h \
	rvg-path-instruction.h \
	rvg-pngio.cpp \
	rvg-pngio.h \
	rvg-point.h \
	rvg-polygon-data.h \
	rvg-polynomial.h \
	rvg-ptr.h \
	rvg-quadratic-bezier-evolute-approximator.h \
	rvg-quadratic-bezier-offset-approximator.h \
	rvg-radial-gradient-data.h \
	rvg-rect-data.h \
	rvg-regular-path-f-evolute.h \
	rvg-regular-path-f-find-join-parameters.h \
	rvg-regular-path-f-forward-if.h \
	rvg-regular-path-f-offset.h \
	rvg-regular-path-f-orient.h \
	rvg-regular-path-f-to-decorated-path.h \
	rvg-regular-path-f-to-input-path.h \
	rvg-rgba.h \
	rvg-scene-bracket.h \
	rvg-scene-data.h \
	rvg-scene-element.h \
	rvg-scene-f-null.h \
	rvg-scene-f-print-rvg.cpp \
	rvg-scene-f-print-rvg.h \
	rvg-scene-f-spy.h \
	rvg-scene-f-stroke.h \
	rvg-scene-f-to-lua-scene.h \
	rvg-scene.h \
	rvg-shape.cpp \
	rvg-shape.h \
	rvg-spread.h \
	rvg-stencil-shape-data.h \
	rvg-stroke-cap.h \
	rvg-stroke-dashes.h \
	rvg-stroke-join.h \
	rvg-stroke-method.h \
	rvg-stroke-style.cpp \
	rvg-stroke-style.h \
	rvg-svg-path-commands.cpp \
	rvg-svg-path-commands.h \
	rvg-svg-path-f-command-printer.h \
	rvg-svg-path-f-null.h \
	rvg-svg-path-f-to-input-path.h \
	rvg-svg-path-parse.h \
	rvg-svg-path-token.cpp \
	rvg-svg-path-token.h \
	rvg-svg-path-tokenizer.h \
	rvg-texture-data.h \
	rvg-triangle-data.h \
	rvg-tuple.h \
	rvg-unit-test.h \
	rvg-unorm.cpp \
	rvg-unorm.h \
	rvg-util.cpp \
	rvg-util.h \
	rvg-viewport.h \
	rvg-winding-rule.h \
	rvg-window.h \
	rvg-xform-affinity.h \
	rvg-xform-affinity.hpp \
	rvg-xform.cpp \
	rvg-xform.h \
	rvg-xform.hpp \
	rvg-xform-identity.h \
	rvg-xform-identity.hpp \
	rvg-xform-linear.h \
	rvg-xform-linear.hpp \
	rvg-xform-product.hpp \
	rvg-xform-projectivity.h \
	rvg-xform-projectivity.hpp \
	rvg-xform-rotation.h \
	rvg-xform-rotation.hpp \
	rvg-xform-scaling.h \
	rvg-xform-scaling.hpp \
	rvg-xform-svd.cpp \
	rvg-xform-svd.h \
	rvg-xform-translation.h \
	rvg-xform-translation.hpp \
	rvg-xform-windowviewport.h \
	strokers/rvg-stroker-rvg.h \
	strokers/rvg-stroker-rvg.cpp \
	strokers/rvg-lua-strokers.cpp

DIST_LUA_SRC := \
	process.lua \
	text.lua \
	fonts.lua \
	blue.lua \
	describechar.lua \
	sentence.lua \
	describefont.lua \
	renderstring.lua \
	rvg.lua \
	svg.lua \
	png.lua \
	indent.lua

DIST_FONTS := \
	fonts/google \
	fonts/microsoft \
	fonts/urw

DIST_PRJ := \
	README \
	b64.pc \
	makefile \
	vg.sln \
	base64.vcxproj \
	chronos.vcxproj \
	direct2d.vcxproj \
	eps.vcxproj \
	facade.vcxproj \
	filter.vcxproj \
	freetype.vcxproj \
	harfbuzz.vcxproj \
	image.vcxproj \
	png.vcxproj \
	rvg_cpp.vcxproj \
	rvg_lua.vcxproj \
	strokers.vcxproj \
	svg.vcxproj \
	util.vcxproj \
	paths.props

DIST_RVGS := \
	rvgs/ampersand.rvg \
	rvgs/anatomical_heart.rvg \
	rvgs/arc1.rvg \
	rvgs/arc2.rvg \
	rvgs/arc3.rvg \
	rvgs/arc4.rvg \
	rvgs/arc5.rvg \
	rvgs/blue_butterfly.rvg \
	rvgs/bunny.rvg \
    rvgs/carrera.rvg \
	rvgs/circle.rvg \
	rvgs/cubic1.rvg \
	rvgs/cubic2.rvg \
	rvgs/cubic3.rvg \
	rvgs/cubic4.rvg \
	rvgs/cubic5.rvg \
	rvgs/cubic6.rvg \
	rvgs/cubic7.rvg \
	rvgs/cubic8.rvg \
	rvgs/cubic9.rvg \
	rvgs/dancer.rvg \
	rvgs/drops.rvg \
	rvgs/embrace.rvg \
	rvgs/eopolygon.rvg \
	rvgs/eyes.rvg \
	rvgs/hello_ttf.rvg \
	rvgs/hello_type1.rvg \
	rvgs/hole1.rvg \
	rvgs/hole2.rvg \
	rvgs/hole3.rvg \
	rvgs/hole4.rvg \
	rvgs/inkboard.rvg \
	rvgs/icozahedron.rvg \
	rvgs/leopard.rvg \
	rvgs/lineargradient.rvg \
	rvgs/lion.rvg \
	rvgs/nestedxform1.rvg \
	rvgs/nestedxform2.rvg \
	rvgs/page_1.rvg \
	rvgs/page_2.rvg \
	rvgs/parabola1.rvg \
	rvgs/parabola2.rvg \
	rvgs/parabola3.rvg \
	rvgs/parabola4.rvg \
	rvgs/parabola5.rvg \
	rvgs/parabola6.rvg \
	rvgs/paris-30k.rvg \
	rvgs/patheopolygon.rvg \
	rvgs/pathlion.rvg \
	rvgs/pathpolygon.rvg \
	rvgs/pathtriangle1.rvg \
	rvgs/pathtriangle2.rvg \
	rvgs/pathtriangle3.rvg \
	rvgs/pathtriangle4.rvg \
	rvgs/penguin.rvg \
	rvgs/pirate-with-knife.rvg \
	rvgs/polygon.rvg \
	rvgs/puss-in-boots.rvg \
	rvgs/quad1.rvg \
	rvgs/quad2.rvg \
	rvgs/quad3.rvg \
	rvgs/quad4.rvg \
	rvgs/radialgradient.rvg \
	rvgs/ramp1.rvg \
	rvgs/ramp2.rvg \
	rvgs/ramp3.rvg \
	rvgs/ramp4.rvg \
	rvgs/ramp5.rvg \
	rvgs/ramp6.rvg \
	rvgs/relatorio.rvg \
	rvgs/reschart.rvg \
	rvgs/samurai.rvg \
	rvgs/svgarc1.rvg \
	rvgs/svgarc2.rvg \
	rvgs/svgarc3.rvg \
	rvgs/table-1.rvg \
	rvgs/tangentradialgradient.rvg \
	rvgs/texture.rvg \
	rvgs/transparency.rvg \
	rvgs/transparentlineargradient.rvg \
	rvgs/transparentradialgradient.rvg \
	rvgs/triangle1.rvg \
	rvgs/triangle2.rvg \
	rvgs/triangulatedlion.rvg \
	rvgs/windowviewport1.rvg \
	rvgs/windowviewport2.rvg \
	rvgs/xformedcircle.rvg \
	rvgs/xformedlineargradient.rvg \
	rvgs/xformedradialgradient.rvg \
	rvgs/xformedtriangle.rvg

DIST_SVGS := $(addprefix svgs/, $(addsuffix .svg, $(basename $(notdir $(DIST_RVGS)))))
DIST_PNGS := $(addprefix pngs/, $(addsuffix .png, $(basename $(notdir $(DIST_RVGS)))))

DIST_WIN64_LUA := \
	luapp.exe \
	luapp53.dll

DIST_WIN64_BASE := \
	x64/Release/base64.dll \
	x64/Release/chronos.dll \
	x64/Release/facade.dll \
	x64/Release/filter.dll \
	x64/Release/freetype.dll \
	x64/Release/harfbuzz.dll \
	x64/Release/image.dll \
	x64/Release/strokers.dll \
	x64/Release/util.dll

DIST_WIN64_DRIVER := \
	x64/Release/driver/eps.dll \
	x64/Release/driver/svg.dll \
	x64/Release/driver/rvg_cpp.dll \
	x64/Release/driver/rvg_lua.dll

DIST_WIN64_VER := 1.0.0
DIST_WIN64_ZIP := win64-$(DIST_WIN64_VER).zip
DIST_WIN64_DIR := win64-$(DIST_WIN64_VER)

DIST_RVGS_VER := 1.0.0
DIST_RVGS_ZIP := rvgs-$(DIST_RVGS_VER).zip
DIST_RVGS_DIR := rvgs-$(DIST_RVGS_VER)

DIST_SVGS_VER := 1.0.0
DIST_SVGS_ZIP := svgs-$(DIST_SVGS_VER).zip
DIST_SVGS_DIR := svgs-$(DIST_SVGS_VER)

DIST_PNGS_VER := 1.0.0
DIST_PNGS_ZIP := pngs-$(DIST_PNGS_VER).zip
DIST_PNGS_DIR := pngs-$(DIST_PNGS_VER)

DIST_SRC_VER := 1.0.1
DIST_SRC_ZIP := src-$(DIST_SRC_VER).zip
DIST_SRC_DIR := src-$(DIST_SRC_VER)

DIST_FONTS_VER := 1.0.0
DIST_FONTS_ZIP := fonts-$(DIST_FONTS_VER).zip
DIST_FONTS_DIR := fonts-$(DIST_FONTS_VER)

distsrc: dist/$(DIST_SRC_ZIP)
distfonts: dist/$(DIST_FONTS_ZIP)
distwin: dist/$(DIST_WIN64_ZIP)
distrvgs: dist/$(DIST_RVGS_ZIP)
distsvgs: dist/$(DIST_SVGS_ZIP)
distpngs: dist/$(DIST_PNGS_ZIP)

versions:
	@echo $(DIST_SRC_ZIP) $(DIST_WIN32_ZIP) $(DIST_WIN64_ZIP) $(DIST_RVGS_ZIP) $(DIST_FONTS_ZIP) $(DIST_SVGS_ZIP)

dist/$(DIST_WIN64_ZIP): $(DIST_WIN64_BASE) $(DIST_WIN64_LUA) $(DIST_WIN64_DRIVER)
	mkdir -p dist/$(DIST_WIN64_DIR)
	mkdir -p dist/$(DIST_WIN64_DIR)/x64/Release
	mkdir -p dist/$(DIST_WIN64_DIR)/x64/Release/driver
	$(CP) $(DIST_WIN64_LUA) dist/$(DIST_WIN64_DIR)
	$(CP) $(DIST_WIN64_BASE) dist/$(DIST_WIN64_DIR)/x64/Release
	$(CP) $(DIST_WIN64_DRIVER) dist/$(DIST_WIN64_DIR)/x64/Release/driver
	cd dist && zip -r $(DIST_WIN64_ZIP) $(DIST_WIN64_DIR)

dist/$(DIST_RVGS_ZIP): $(DIST_RVGS)
	mkdir -p dist
	mkdir dist/$(DIST_RVGS_DIR)
	$(CP) -f $(DIST_RVGS) dist/$(DIST_RVGS_DIR)
	cd dist && zip -r $(DIST_RVGS_ZIP) $(DIST_RVGS_DIR)

dist/$(DIST_SVGS_ZIP): $(DIST_SVGS)
	mkdir -p dist
	mkdir dist/$(DIST_SVGS_DIR)
	$(CP) -f $(DIST_SVGS) dist/$(DIST_SVGS_DIR)
	cd dist && zip -r $(DIST_SVGS_ZIP) $(DIST_SVGS_DIR)

dist/$(DIST_PNGS_ZIP): $(DIST_PNGS)
	mkdir -p dist
	mkdir dist/$(DIST_PNGS_DIR)
	$(CP) -f $(DIST_PNGS) dist/$(DIST_PNGS_DIR)
	cd dist && zip -r $(DIST_PNGS_ZIP) $(DIST_PNGS_DIR)

svgs/%.svg: rvgs/%.rvg
	./process.lua driver.svg $< $@

pngs/%.png: rvgs/%.rvg
	./process.lua st -pattern:16 $< $@

dist/$(DIST_FONTS_ZIP):
	mkdir -p dist
	mkdir dist/$(DIST_FONTS_DIR)
	$(CP) -fr $(DIST_FONTS) dist/$(DIST_FONTS_DIR)
	cd dist && zip -r $(DIST_FONTS_ZIP) $(DIST_FONTS_DIR)

dist/$(DIST_SRC_ZIP): $(DIST_LUA_SRC) $(DIST_CPP_SRC) $(DIST_PRJ)
	mkdir -p dist
	mkdir dist/$(DIST_SRC_DIR)
	$(CP) --parents $(DIST_LUA_SRC) dist/$(DIST_SRC_DIR)
	$(CP) --parents $(DIST_PRJ) dist/$(DIST_SRC_DIR)
	$(CP) --parents $(DIST_CPP_SRC) dist/$(DIST_SRC_DIR)
	cd dist && zip -r $(DIST_SRC_ZIP) $(DIST_SRC_DIR)

.PHONY: test-stroker

STROKER?=rvg

test-stroker: strokers.so
	strokers/stroke-all $(STROKER)
	for t in a/distroke/*distroke.png; do \
		s=$$(basename $$t _distroke.png); \
		compare -metric rmse $$t a/$${s}.png null: 2>&1; \
		echo " "$$s; \
	done | sort -n | tail;

stats-stroker: strokers.so
	@\rm -f a/*png;
	@strokers/stroke-all $(STROKER) -no-background -driver:stats
	@for t in linear_segment quadratic_segment rational_quadratic_segment cubic_segment; do \
		echo $${t}s; \
		cat a/*.png | grep \\\<$$t\\\> | cut -f 2 -d ' ' | awk '{s+=$$1} END {print s}'; \
	done
	@echo total; \
	cat a/*.png | grep segment | cut -f 2 -d ' ' | awk '{s+=$$1} END {print s}';
