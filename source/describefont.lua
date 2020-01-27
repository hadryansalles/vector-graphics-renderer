if #arg < 1 then
    io.stderr:write("lua describefont.lua [<font-name> | <family-file>]\n")
    os.exit(1)
end

local fonts = require"fonts"
local freetype = require"freetype"
local name = arg[1]
local font = assert(fonts.loadface(name))
local face = font.freetypeface
local attrs = font.attributes
local unpack = unpack or table.unpack
io.stdout:write("Properties for face ", name, "\n")
io.stdout:write("Number of faces: ", attrs.num_faces, "\n")
local properties = {
    {"face_family", "Face family"},
    {"style_name", "Style name"},
    {"num_glyphs", "Number of glyphs"},
    {"units_per_EM", "Units per EM"},
    {"height", "Height"},
    {"max_advance_width", "Maximum advance width"},
    {"max_advance_height", "Maximum advance height"},
    {"ascender", "Ascender"},
    {"descender", "Descender"},
    {"underline_thickness", "Underline thickness"},
    {"underline_position", "Underline position"}
}
local maxname = 0
for i,v in ipairs(properties) do
    maxname = math.max(maxname, #(v[2]))
end
for i,v in ipairs(properties) do
    v[2] = v[2] .. ":" .. string.rep(" ", maxname-#(v[2])+2)
end
local bbox = "Bounding box"
    bbox = bbox .. ":" .. string.rep(" ", maxname-#bbox+2)
for i = 1, attrs.num_faces do
    io.stdout:write("  Face ", i-1, "\n")
    for j, v in ipairs(properties) do
        local index, name = unpack(v)
        io.stdout:write("  ", name, attrs[index], "\n")
    end
    io.stdout:write("  ", bbox, table.concat(attrs.bounding_box, " "), "\n")
end
