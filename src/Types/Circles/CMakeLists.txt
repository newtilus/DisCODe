SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE( OpenCV REQUIRED )

FILE(GLOB circles *.cpp)
ADD_LIBRARY(TypesCircles SHARED ${circles})
TARGET_LINK_LIBRARIES(TypesCircles ${OpenCV_LIBRARIES})

INSTALL(
  TARGETS TypesCircles
  RUNTIME DESTINATION bin COMPONENT applications
  LIBRARY DESTINATION lib COMPONENT applications
  ARCHIVE DESTINATION lib COMPONENT sdk
)

FILE(GLOB circles *.hpp)
install(
    FILES ${circles}
    DESTINATION include/Types/Circles
    COMPONENT sdk
)
