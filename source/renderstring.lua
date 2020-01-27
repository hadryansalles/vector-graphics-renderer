if #arg < 5 then
    io.stderr:write("lua renderstring.lua <font file> <string> <ascender in EMs> <desdender in EMs> <height in pixels> <output PNG>\n")
    os.exit(1)
end

local freetype = require"freetype"
local image = require"image"
local harfbuzz = require"harfbuzz"

local unpack = unpack or table.unpack

-- glyph images are actually an alpha channel in linear space
-- this function blends a glyph image over a canvas
local function blendalpha(glyphalpha, x, y, canvas)
    for i = 1, glyphalpha:get_height() do
        for j = 1, glyphalpha:get_width() do
            local oi, oj = y+i-1, x+j-1
            local fore_alpha = glyphalpha:get_pixel(j, i)
            local back_alpha = canvas:get_pixel(oj, oi)
            canvas:set_pixel(oj, oi, back_alpha*(1-fore_alpha) + fore_alpha)
        end
    end
end

-- this is just so we can look at the text as if it was
-- rendered in black on a white background. for the learning
-- algorithm, we should probably leave the image alone as a linear
-- alpha channel
local function blendonwhite(alphaimage)
    local w = alphaimage:get_width()
    local h = alphaimage:get_height()
    local grayimage = image.image(w, h, 1)
    for i = 1, h do
        for j = 1, w do
            local alpha = alphaimage:get_pixel(j, i)
            -- invert and approximate sRGB gamma
            local value = (1.-alpha)^(1/2.2)
            grayimage:set_pixel(j, i, value)
        end
    end
    return grayimage
end

-- loads a glyph image into the cache at a given number of pixels per EM
local function loadglyphalpha(face, cache, codepoint, ppem)
    local g = cache[codepoint]
    if g then return g end
    g = assert(face:get_glyph_alpha(codepoint, ppem), "no glyph for codepoint")
    cache[codepoint] = g
    return g
end

-- get command-line arguments
local facename = assert(arg[1], "missing font file argument")
local text = assert(arg[2], "missing string argument")
local force_ascender = assert(tonumber(arg[3]), "missing ascender argument")
local force_descender = assert(tonumber(arg[4]), "missing descender argument")
local height = assert(tonumber(arg[5]), "missing height argument")
-- compute pixels per EM based on specified ascender, descender, and height
local pixels_per_EM = math.floor(height/(force_ascender-force_descender))

local outputname = arg[6] or "%f %s.png"

-- setup freetype and harfbuzz
local freetypeface = assert(freetype.face(facename, 0), "face not found")
io.stderr:write("loaded '", facename, "'\n")
local attrs = freetypeface:get_attributes()
assert(force_ascender*attrs.units_per_EM > attrs.ascender, "ascender too small for font")
assert(force_descender*attrs.units_per_EM < attrs.descender, "descender too small for font")

local harfbuzzfont = assert(harfbuzz.font(facename))
--local features = harfbuzz.features("liga=0") -- disable ligatures
--local features = harfbuzz.features("smcp=1") -- enable small caps
--local features = harfbuzz.features("-kern") -- disable kerning

harfbuzzfont:set_scale(attrs.units_per_EM)
harfbuzzfont:set_ft_funcs()
local buffer = harfbuzz.buffer()
local pixels_per_unit = pixels_per_EM/attrs.units_per_EM
buffer:add(text)
buffer:shape(harfbuzzfont, features)
local infos, nglyphs = buffer:get_glyph_infos()
local positions = buffer:get_glyph_positions()
local cache = {}
local xcursor, ycursor = 0, 0
local toblend = {}
-- get all glyph images and where to draw them
for i = 1, nglyphs do
    local glyph =
        loadglyphalpha(freetypeface, cache, infos[i].codepoint, pixels_per_EM)
    toblend[#toblend+1] = {
        xcursor + positions[i].x_offset*pixels_per_unit,
        ycursor + positions[i].y_offset*pixels_per_unit,
        glyph
    }
    xcursor = xcursor + positions[i].x_advance*pixels_per_unit
    ycursor = ycursor + positions[i].y_advance*pixels_per_unit
end
-- compute tight image dimensions
local xmin, ymin = math.huge, math.huge
local xmax, ymax = -math.huge, -math.huge
for i,entry in ipairs(toblend) do
    local x, y, glyph = unpack(entry)
    local alpha = glyph.alpha
    x, y = math.floor(x + 0.5), math.floor(y + 0.5)
    xmin = math.min(x+glyph.left, xmin)
    xmax = math.max(x+glyph.left+alpha:get_width(), xmax)
    ymin = math.min(y+glyph.bottom, ymin)
    ymax = math.max(y+glyph.bottom+alpha:get_height(), ymax)
end

force_ascender = math.ceil(force_ascender*pixels_per_EM)
assert(ymax <= force_ascender, "ascender too small")
ymax = force_ascender
--force_descender = math.floor(force_descender*pixels_per_EM)
force_descender = force_ascender - height + 1
assert(ymin >= force_descender, "descender too small")
ymin = force_descender

-- create canvas and blend all glyph images into the appropriate places
local canvas = image.image(xmax-xmin+1, ymax-ymin+1, 1)
for i,entry in ipairs(toblend) do
    local x, y, glyph = unpack(entry)
    local alpha = glyph.alpha
    x, y = math.floor(x + 0.5), math.floor(y + 0.5)
    x = x-xmin+1+glyph.left
    y = y-ymin+1+glyph.bottom
    blendalpha(alpha, x, y, canvas)
end
io.stderr:write("rendered '", text, "'\n")

-- hack to save image name based on family and style
local function cleanfilename(s)
    return string.gsub(s, "[/\\<>:\"|?*]", " ")
end
outputname = string.gsub(outputname, "%%(.)", function(c)
    if c == "f" then return cleanfilename(attrs.face_family)
    elseif c == "s" then return cleanfilename(attrs.style_name)
    elseif c == "t" then return cleanfilename(text) end
end)

-- save resulting image, annotated with all command line arguments
local f = assert(io.open(outputname, "wb"))
local comments = {}
local i = -1
while arg[i] do i = i-1 end
i = i + 1
while arg[i] do
    comments["arg["..i.."]"] = arg[i]
    i = i + 1
end
assert(image.png.store8(f, blendonwhite(canvas), comments))
f:close()
io.stderr:write("saved '", outputname, "'\n")
