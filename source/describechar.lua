if #arg < 1 then
    io.stderr:write("lua describechar.lua <char> [<font-file> | <family-name>]\n")
    os.exit(1)
end

local utf8 = require"utf8"
local freetype = require"freetype"
local fonts = require"fonts"
local text = require"text"
local svg = require"driver.svg"
local code
for i, c in utf8.codes(arg[1]) do
    code = c
end
local unpack = table.unpack
local face = assert(fonts.loadface(arg[2] or "Times New Roman"))
local freetypeface = face.freetypeface
local faceattrs = face.attributes
local charindex = assert(freetypeface:get_char_index(code))
local glyphoutline = freetypeface:get_glyph_outline(charindex)
local glyphmetrics = freetypeface:get_glyph_metrics(charindex)
local s = {}
local e = faceattrs.units_per_EM
local ymin = faceattrs.descender-e/4
local ymax = ymin+3*e/2
local xmin = -e/4
local xmax = 5*e/4
local tscale = 1/30
local thskip = faceattrs.height*tscale


local color = svg.color
local circle = svg.circle
local scene = svg.scene
local render = svg.render
local window = svg.window
local viewport = svg.viewport
local fill = svg.fill
local path = svg.path
local M = svg.M
local L = svg.L
local Z = svg.Z
local w = window(xmin, ymin, xmax, ymax)
local vp = viewport(0, 0, xmax-xmin, ymax-ymin)

local function line(x0, y0, x1, y1)
    return path{M,x0,y0,x1,y1}:stroked(5)
end

local function cxttext(s, x, y)
    local p, f, xmin, ymin, xmax, ymax = text.text(svg, s)
    local xmed, ymed = 0.5*(xmax+xmin), 0.5*(ymax+ymin)
    return path(p):translated(-xmed,-ymed):
        scaled(tscale):
            translated(x,y-tscale*(ymax-ymed))
end

local function cxbtext(s, x, y)
    local p, f, xmin, ymin, xmax, ymax = text.text(svg, s)
    local xmed, ymed = 0.5*(xmax+xmin), 0.5*(ymax+ymin)
    return path(p):translated(-xmed,-ymed):
        scaled(tscale):
            translated(x,y+tscale*(ymed-ymin))
end

local function cyrtext(s, x, y)
    local p, f, xmin, ymin, xmax, ymax = text.text(svg, s)
    local xmed, ymed = 0.5*(xmax+xmin), 0.5*(ymax+ymin)
    return path(p):translated(-xmed,-ymed):
        scaled(tscale):
            translated(x-tscale*(xmax-xmed),y)
end

local function cyltext(s, x, y)
    local p, f, xmin, ymin, xmax, ymax = text.text(svg, s)
    local xmed, ymed = 0.5*(xmax+xmin), 0.5*(ymax+ymin)
    return path(p):translated(-xmed,-ymed):
        scaled(tscale):
            translated(x+tscale*(xmed-xmin),y)
end

-- draw EM box and legend
s[#s+1] = fill(path{M,0,0,e,0,e,e,0,e,Z}:translated(0, faceattrs.descender):stroked(5), color.red)
s[#s+1] = fill(cxttext("EM box", e/2, faceattrs.descender-e/100), color.red)
-- draw glyph
s[#s+1] = fill(path(glyphoutline), color.lightgray)
-- draw baseline
s[#s+1] = fill(line(0, 0, e, 0), color.brown)
s[#s+1] = fill(cyltext("baseline", e+e/50, 0), color.brown)
-- horizontal origin
s[#s+1] = fill(circle(0,0,e/100), color.black)
s[#s+1] = fill(cyrtext("origin", -e/50,0), color.black)
-- horizontal origin for next glyph
s[#s+1] = fill(circle(glyphmetrics.horiz_advance,0,e/100), color.black)
s[#s+1] = fill(cyltext("origin+advance", glyphmetrics.horiz_advance+e/50,0),
            color.black)
-- glyph bbox
gxmin = glyphmetrics.horiz_bearing_x
gxmax = glyphmetrics.horiz_bearing_x+glyphmetrics.width
gymin = glyphmetrics.horiz_bearing_y-glyphmetrics.height
gymax = glyphmetrics.horiz_bearing_y
s[#s+1] = fill(path{M,gxmin,gymin,gxmax,gymin,gxmax,gymax,gxmin,gymax,Z}:stroked(5), color.blue)
s[#s+1] = fill(cxbtext("Glyph bounding box", 0.5*(gxmax+gxmin), gymax+e/100), color.blue)
-- horizontal metrics
-- bearingX
s[#s+1] = fill(line(0,e/10,glyphmetrics.horiz_bearing_x,e/10), color.magenta)
s[#s+1] = fill(cyrtext("bearingX", -e/50,e/10), color.magenta)
-- width
s[#s+1] = fill(line(gxmin, 2*e/10, gxmax, 2*e/10), color.darkgreen)
s[#s+1] = fill(cyrtext("width", gxmin-e/50, 2*e/10), color.darkgreen)
-- bearingY
s[#s+1] = fill(line(2*e/10, 0, 2*e/10, glyphmetrics.horiz_bearing_y),
    color.coral)
s[#s+1] = fill(cxttext("bearingY", 2*e/10, -e/50), color.coral)
-- height
s[#s+1] = fill(line(e/10, gymin, e/10, gymax), color.cyan)
s[#s+1] = fill(cxttext("height", e/10, gymin-e/50), color.cyan)

render(scene(s), w, vp, io.stdout)

-- dump metrics to stderr
local metrics = {
    {"width", "Width"},
    {"height", "Height"},
    {"horiz_bearing_x", "Horizontal Bearing X"},
    {"horiz_bearing_y", "Horizontal Bearing Y"},
    {"horiz_advance", "Horizontal Advance"},
    {"vert_bearing_x", "Vertical Bearing X"},
    {"vert_bearing_y", "Vertical Bearing Y"},
    {"vert_advance", "Vertical Advance"},
}
local maxname = 0
for i,v in ipairs(metrics) do
    maxname = math.max(maxname, #(v[2]))
end
local linearHoriAdvance = "Linear Horizontal Advance"
maxname = math.max(maxname, #linearHoriAdvance)
local linearVertAdvance = "Linear Vertical Advance"
maxname = math.max(maxname, #linearVertAdvance)
for i,v in ipairs(metrics) do
    v[2] = v[2] .. ":" .. string.rep(" ", maxname-#(v[2])+2)
end
io.stderr:write("Glyph ", code, "\n")
for j, v in ipairs(metrics) do
    local index, name = unpack(v)
    io.stderr:write("  ", name, glyphmetrics[index], "\n")
end
