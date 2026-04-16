find_package(Qt6 REQUIRED COMPONENTS Test)

function(add_organizer_test name)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        Qt6::Core
        KF6::CoreAddons
        KF6::KIOCore
        KF6::IconThemes
        organizer_core
    )
    if(BUILD_COVERAGE)
        target_compile_options(${name} PRIVATE --coverage -O0 -g)
        target_link_options(${name} PRIVATE --coverage)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_organizer_test(test_fencemanager)
add_organizer_test(test_fencemodel)
add_organizer_test(test_fencefilemodel)
add_organizer_test(test_fencefilemodel_modifications)
add_organizer_test(test_organizersettings)
