if #arg < 1 then
    io.stderr:write("lua sentence.lua <sentence> [<font-file> | <family-name>] [<features>]\n")
    os.exit(1)
end

local svg = require"driver.svg"
setmetatable(_G, { __index = svg} )
local text = require"text"
local facename = arg[2] or "Times New Roman Regular"
local p, f, xmin, ymin, xmax, ymax = text.text(svg, arg[1], facename, arg[3])
local a = f.attributes
local wnd = window(xmin, -.7*a.units_per_EM, xmax, 1.3*a.units_per_EM)
local h = 256
local w = math.ceil((xmax-xmin)*h/(2*a.units_per_EM))
local vp = viewport(0, 0, w, h)
-- draw bounding box
local s = {}
local e = a.units_per_EM
local d = a.descender
s[#s+1] = fill(path{M,0,0,e,0,e,e,0,e,Z}:translated(0,d):stroked(5), color.red)
s[#s+1] = fill(path{M,xmin,ymin,xmax,ymin,xmax,ymax,xmin,ymax,Z}:stroked(3), color.blue)
s[#s+1] = fill(path(p), color.black)
render(accelerate(scene(s), wnd, vp), wnd, vp, io.stdout)
