option(ENABLE_ASAN, "This indicates that the address sanitizer (ASan) should be enabled" false)
option(ENABLE_UBSAN, "This indicates that the undefined behaviour sanitizer (UBSan) should be enabled" false)

if (ENABLE_ASAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
		CMAKE_CXX_COMPILER_ID MATCHES Clang)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
	else()
		message(FATAL_ERROR "Only GCC and Clang are currently supported.")
	endif()
endif()

if (ENABLE_UBSAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
		CMAKE_CXX_COMPILER_ID MATCHES Clang)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
	else()
		message(FATAL_ERROR "Only GCC and Clang are currently supported.")
	endif()
endif()
