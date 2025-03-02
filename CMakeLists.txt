cmake_minimum_required(VERSION 3.10)

project(OOP_Task4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(OOP_Task4
    AddElementDialog.cpp
    AddElementDialog.h
    AddElementWindow.ui
    EditElementDialog.cpp
    EditElementDialog.h
    EditElementWindow.ui
    ElectricalCircuitCalculator.cpp
    ElectricalCircuitCalculator.h
    ElectricalElement.cpp
    ElectricalElement.h
    ElectricalElements/CurrentSource.cpp
    ElectricalElements/CurrentSource.h
    ElectricalElements/ElectricHeater.cpp
    ElectricalElements/ElectricHeater.h
    ElectricalElements/ElectricLamp.cpp
    ElectricalElements/ElectricLamp.h
    ElectricalElements/Resistor.cpp
    ElectricalElements/Resistor.h
    ElectricalElements/Switch.cpp
    ElectricalElements/Switch.h
    ElectricalElements/VoltageSource.cpp
    ElectricalElements/VoltageSource.h
    ElectricalElements/Wire.cpp
    ElectricalElements/Wire.h
    ElectricalElementsManager.cpp
    ElectricalElementsManager.h
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.qrc
    MainWindow.ui
    RenderArea.cpp
    RenderArea.h
)

target_link_libraries(OOP_Task4 Qt6::Core Qt6::Gui Qt6::Widgets)