SET(src map.cpp)
SET(luasrc init.lua)
SET(OMP 1)
IF (OMP)
   FIND_PACKAGE(OpenMP)
   IF(OPENMP_FOUND)
      MESSAGE(STATUS "OpenMP Found with compiler flag : ${OpenMP_CXX_FLAGS}")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
   ENDIF(OPENMP_FOUND)
ENDIF (OMP)

ADD_TORCH_PACKAGE(map "${src}" "${luasrc}" "Image Processing")

TARGET_LINK_LIBRARIES(map luaT TH)
