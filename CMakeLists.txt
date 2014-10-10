cmake_minimum_required(VERSION 2.8)
SET(PROJECTNAME onlineStereoCalibration)
PROJECT(${PROJECTNAME})

find_package(OpenCV)
FIND_PACKAGE(YARP)

file(GLOB folder_source src/*.cpp
			src/threads/*.cpp
			src/encoders/*.cpp
			src/images/*.cpp
			src/filter/*.cpp
			src/features/*.cpp)
file(GLOB folder_header include/threads/*.h
			include/encoders/*.h
			include/images/*.h
			include/filter/*.h
			include/features/*.h)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include
                    ${OpenCV_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(${PROJECTNAME} ${folder_source} ${folder_header})

TARGET_LINK_LIBRARIES(${PROJECTNAME} ${OpenCV_LIBRARIES}
                                     ${YARP_LIBRARIES})

INSTALL(TARGETS ${PROJECTNAME} DESTINATION bin)
