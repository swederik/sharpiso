PROJECT(TEST_SHARPINFO)

#---------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF (NOT DEFINED ${SHARP_DIR})
  GET_FILENAME_COMPONENT(SHARP_ABSOLUTE_PATH "../.." ABSOLUTE)
  SET(SHARP_DIR ${SHARP_ABSOLUTE_PATH} CACHE PATH "SHARP directory")
ENDIF (NOT DEFINED ${SHARP_DIR})

SET(CMAKE_INSTALL_PREFIX "${SHARP_DIR}/")
SET(LIBRARY_OUTPUT_PATH ${SHARP_DIR}/lib CACHE PATH "Library directory")
SET(SHARPINFO_DIR "src/sharpinfo")
SET(NRRD_LIBDIR "${SHARP_DIR}/lib")
SET(SHARP_ISOTABLE_DIR "${SHARP_DIR}/isotable" CACHE PATH "Isotable directory")

#---------------------------------------------------------

IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE Release CACHE STRING 
       "Default build type: Release" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

INCLUDE_DIRECTORIES("${SHARP_DIR}/include")
INCLUDE_DIRECTORIES("../eigen")
LINK_DIRECTORIES("${NRRD_LIBDIR}")
LINK_LIBRARIES(ITKNrrdIO z)

ADD_EXECUTABLE(test_voxel test_voxel.cxx sharpiso_get_gradients.cxx
                          sharpiso_intersect.cxx)
