function(pad_string var_name val digits)
    string(LENGTH "${val}" val_len)
    
    set(temp_val "${val}")
    while(val_len LESS digits)
        set(temp_val "0${temp_val}")
        string(LENGTH "${temp_val}" val_len)
    endwhile()
    
    set(${var_name} "${temp_val}" PARENT_SCOPE)
endfunction()

string(TIMESTAMP BUILD_YEAR "%Y")
pad_string(TYPECOUNTER_VERSION_MAJOR_PAD_2DIG ${PROJECT_VERSION_MAJOR} 2)
pad_string(TYPECOUNTER_VERSION_MINOR_PAD_2DIG ${PROJECT_VERSION_MINOR} 2)
pad_string(TYPECOUNTER_VERSION_PATCH_PAD_2DIG ${PROJECT_VERSION_PATCH} 2)
set(TYPECOUNTER_VERSION_CODE "${BUILD_YEAR}${TYPECOUNTER_VERSION_MAJOR_PAD_2DIG}${TYPECOUNTER_VERSION_MINOR_PAD_2DIG}${TYPECOUNTER_VERSION_PATCH_PAD_2DIG}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
	${CMAKE_SOURCE_DIR}/include/version.hpp
	@ONLY
)