DEPENDENCIES:
Linux: None

All: Torch7 (follow instructions here: www.torch.ch)

INSTALL:
$ git clone https://github.com/omario61/lua---Map
$ luarocks make map-1.1-0.rockspec

STATE:

USE:
$ torch
> require 'map'
> map.testme() 

