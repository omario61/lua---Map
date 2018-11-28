
----------------------------------
-- dependencies
----------------------------------
require 'sys'
require 'xlua'


----------------------------------
-- package a little demo
----------------------------------
map = {}
map.testme = function()
                   print '--------------------------------------------------'
                   local map = image.Map{}
                   map:add(5, 10, 11, 3, 4, 5)
                   print('map get 5', map:get(5, 10, 11))
                   return map:get(5, 10, 11)
                end
