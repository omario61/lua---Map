
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
   tensor = torch.FloatTensor(3)
   local b, g, r = libmap.get(r1, g1, b1, tensor)
   return tensor
end
