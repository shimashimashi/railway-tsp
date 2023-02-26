cmake_minimum_required(VERSION 3.24)
enable_language(CXX)
enable_testing()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(railway)

include(FetchContent)
FetchContent_Populate(
  ac_library
  URL https://github.com/atcoder/ac-library/releases/download/v1.4/ac-library.zip
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

include(ExternalProject)
ExternalProject_Add(
  LKH
  URL http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0.10.tgz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make
  BUILD_IN_SOURCE true
  INSTALL_COMMAND cp ./LKH ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(
    /usr/local/include
    ${ac_library_SOURCE_DIR}
)

add_executable(
    tsp
    src/tsp.cc
)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(tsp PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(
    group
    src/group.cc
)

add_executable(
    tour
    src/tour.cc
)
target_link_libraries(
    tour
    PRIVATE nlohmann_json::nlohmann_json
)

add_executable(
    station
    src/station.cc
)
target_link_libraries(
    station
    PRIVATE nlohmann_json::nlohmann_json
)

add_executable(
    line
    src/line.cc
)
target_link_libraries(
    line
    PRIVATE nlohmann_json::nlohmann_json
)

add_test(
    NAME tsp_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_tsp.sh $<TARGET_FILE:tsp> ${CMAKE_CURRENT_SOURCE_DIR} ./test
)

add_test(
    NAME group_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_group.sh $<TARGET_FILE:group> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME tour_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_tour.sh $<TARGET_FILE:tour> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME station_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_station.sh $<TARGET_FILE:station> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME line_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_line.sh $<TARGET_FILE:line> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT group.csv
    DEPENDS group
    COMMAND $<TARGET_FILE:group> ${CMAKE_CURRENT_SOURCE_DIR}/data/station20230105free.csv ${CMAKE_CURRENT_SOURCE_DIR}/data/join20220921.csv > ./group.csv
)
add_custom_target(
    generate_group
    DEPENDS group.csv
)

add_custom_command(
    OUTPUT node.csv shortest_path.csv railway.tsp
    DEPENDS tsp group.csv
    COMMAND $<TARGET_FILE:tsp> ${CMAKE_CURRENT_SOURCE_DIR}/data/station20230105free.csv ${CMAKE_CURRENT_SOURCE_DIR}/data/join20220921.csv ./group.csv ./
)
add_custom_target(
    generate_tsp
    DEPENDS railway.tsp
)

add_custom_command(
    OUTPUT railway.lkh
    DEPENDS LKH railway.tsp ${CMAKE_CURRENT_SOURCE_DIR}/config/railway.par
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/config/railway.par ./
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/LKH ./railway.par
)
add_custom_target(
    generate_lkh
    DEPENDS railway.lkh
)

add_custom_command(
    OUTPUT tour.json
    DEPENDS tour railway.lkh
    COMMAND $<TARGET_FILE:tour> ${CMAKE_CURRENT_SOURCE_DIR}/data/station20230105free.csv ./node.csv ./railway.lkh ./shortest_path.csv > ./tour.json
)
add_custom_target(
    generate_tour
    DEPENDS tour.json
)

add_custom_command(
    OUTPUT station.json
    DEPENDS station
    COMMAND $<TARGET_FILE:station> ${CMAKE_CURRENT_SOURCE_DIR}/data/station20230105free.csv > ./station.json
)
add_custom_target(
    generate_station
    DEPENDS station.json
)

add_custom_command(
    OUTPUT line.json
    DEPENDS line
    COMMAND $<TARGET_FILE:line> ${CMAKE_CURRENT_SOURCE_DIR}/data/line20230105free.csv > ./line.json
)
add_custom_target(
    generate_line
    DEPENDS line.json
)
