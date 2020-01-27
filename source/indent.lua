local _M = {}

local indent_meta = { __index = {} }

function indent_meta:__call()
    return self.v
end

function indent_meta.__index:inc()
    local v = self.v
    self.n = self.n + 1
    self.v = "\n" .. self.s:rep(self.n)
    return v
end

function indent_meta.__index:dec()
    self.n = self.n - 1
    self.v = "\n" .. self.s:rep(self.n)
    return self.v
end

function indent_meta.__index:write(file)
    file:write(self.v)
end

function indent_meta.__index:write_inc(file)
    file:write(self:inc())
end

function indent_meta.__index:dec_write(file)
    file:write(self:dec())
end

function _M.indent(n, s)
    return setmetatable({ n = n, s = s, v = "\n" .. s:rep(n) }, indent_meta)
end

return _M
