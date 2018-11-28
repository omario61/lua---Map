SET(src map.cpp)
SET(luasrc init.lua)

ADD_TORCH_PACKAGE(map "${src}" "${luasrc}" "Image Processing")

TARGET_LINK_LIBRARIES(map luaT TH)
