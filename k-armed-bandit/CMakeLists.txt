cmake_minimum_required(VERSION 3.22)
project(karmedbandit)
set(CMAKE_CXX_STANDARD 17)

set(HEADERS
        headers/GaussianMAB.h
        headers/MAB.h
        headers/ActionChooser.h
        headers/ValueEstimator.h
        headers/Agent.h
        headers/Experiment.h
        headers/GreedyActionChooser.h
        headers/EpsilonGreedyActionChooser.h
        headers/RandomActionChooser.h
        headers/SampleAvgValueEstimator.h
        headers/SimpleExperiment.h
        headers/ComparisonExperiment.h)

set(SOURCES
        headers/ConstantStepSizeValueEstimator.h
        headers/UCBActionChooser.h
        headers/NonStationaryMABDecorator.h
        cmake-build-debug/sdasds.cpp
        cmake-build-debug/sdasds.h)

find_package(Catch2 3 REQUIRED)

# Main executable
add_executable(karmedbandit.out main.cpp ${HEADERS} ${SOURCES})
target_compile_options(karmedbandit.out PRIVATE -I/usr/include/python3.10 -I/usr/lib/python3.10/site-packages/numpy/core/include -lpython3.10)
target_link_libraries(karmedbandit.out PRIVATE libpython3.10.so)

set(EXPERIMENT_SOURCES
        experiments/SampleAvgACComparison.cpp
        experiments/ConstantStepSizeSampleAvgComparison.cpp
        experiments/UCBComparison.cpp
        experiments/OIVComparisons.cpp
        experiments/NonStationaryComparison.cpp
)

foreach (EXPERIMENT_SOURCE ${EXPERIMENT_SOURCES})
    # Extract the name of the executable from the source file
    get_filename_component(EXECUTABLE_NAME ${EXPERIMENT_SOURCE} NAME_WE)

    # Add the executable
    add_executable(${EXECUTABLE_NAME}.out ${EXPERIMENT_SOURCE})

    target_compile_options(${EXECUTABLE_NAME}.out PRIVATE -I/usr/include/python3.10 -I/usr/lib/python3.10/site-packages/numpy/core/include -lpython3.10)
    target_link_libraries(${EXECUTABLE_NAME}.out PRIVATE libpython3.10.so)
endforeach ()