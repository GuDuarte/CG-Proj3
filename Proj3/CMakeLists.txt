project(Proj3)
add_executable(${PROJECT_NAME} main.cpp model.cpp openglwindow.cpp
                               trackball.cpp smodel.cpp star.cpp)
enable_abcg(${PROJECT_NAME})
