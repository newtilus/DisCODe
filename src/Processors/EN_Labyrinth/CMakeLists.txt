# Include the directory itself as a path to include directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find OpenCV library files
FIND_PACKAGE( OpenCV REQUIRED )
FIND_PACKAGE( Boost REQUIRED serialization)

# Create a variable containing all .cpp files:
FILE(GLOB files *.cpp)

# Create an executable file from sources:
ADD_LIBRARY(EN_Labyrinth SHARED ${files})
TARGET_LINK_LIBRARIES(EN_Labyrinth ${OpenCV_LIBS} ${DisCODe_LIBRARIES} ${Boost_SERIALIZATION_LIBRARY})

INSTALL_PROCESSOR(EN_Labyrinth)
