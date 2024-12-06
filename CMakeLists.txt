cmake_minimum_required(VERSION 3.28)
project(GAME)

set(CMAKE_CXX_STANDARD 20)

add_executable(GAME main.cpp
        MazeState.cpp
        Greedy.cpp
        BeamSearch.cpp
        BeamSearchWithTime.cpp
        ChokudaiSearch.cpp
        AutoMoveMazeState.cpp
        HillClimb.cpp
)
