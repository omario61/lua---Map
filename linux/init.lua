
----------------------------------
-- dependencies
----------------------------------
require 'torch'
require 'xlua'
require 'image'
require 'libmap'

----------------------------------
-- a map class
----------------------------------
local Map = torch.class('image.Map')

function Map:__init()
   libmap.init()
end

function Map:add(r1, g1, b1, r2, g2, b2)
   return libmap.add(r1, g1, b1, r2, g2, b2)
end

function Map:get(r1, g1, b1)
   tensor = torch.DoubleTensor(3)
   local b, g, r = libmap.get(r1, g1, b1, tensor)
   return tensor
end

function Map:add_all(content, style)
    return libmap.add_all(content, style, content:size(2), content:size(3))
end

function Map:get_all(content, style)
   tensor = torch.DoubleTensor(style:size())
   local kkk = libmap.get_all(content, tensor, content:size(2), content:size(3))
   return tensor
end
