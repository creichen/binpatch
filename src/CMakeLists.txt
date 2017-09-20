set(HEADERS
  transform.h
  )

add_executable(binpatch binpatch.c transform.c)

# ----------------------------------------
# install

install(TARGETS binpatch
        RUNTIME DESTINATION bin)

# ----------------------------------------
# tests
IF(NOT CMAKE_CROSSCOMPILING)
  add_executable(test_transform test_transform.c transform.c)
  #target_link_libraries(test_datastructures zysbus ${CHECK_LIBRARIES})
  add_test(Transform test_transform)
ENDIF(NOT CMAKE_CROSSCOMPILING)
