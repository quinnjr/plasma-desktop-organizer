find_package(Qt6 REQUIRED COMPONENTS Test)

function(add_organizer_test name)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        Qt6::Core
        KF6::CoreAddons
        KF6::KIOCore
        organizer_core
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# Tests are added in subsequent tasks
