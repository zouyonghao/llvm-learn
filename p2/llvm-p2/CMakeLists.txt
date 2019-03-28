
llvm_map_components_to_libnames(LLVM_LIBS_ 
                                bitreader bitwriter interpreter core irreader mcjit native option support)

set(SOURCE_FILES 
    main.cpp)
add_executable(llvm-p2 ${SOURCE_FILES})
target_link_libraries(llvm-p2 ${LLVM_LIBS_})
