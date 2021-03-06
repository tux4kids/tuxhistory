#The following isn't necessary because of the SDL workaround
#cmake_minimum_required(VERSION 2.4.7 FATAL_ERROR)

## Libraries
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_gfx)
find_package(SDL_net)

if (NOT SDLGFX_FOUND)
  message("Adding rotozoom")
  set(TUXMATH_EXTRA_SRC ${TUXMATH_EXTRA_SRC} SDL_rotozoom.c)
endif (NOT SDLGFX_FOUND)


## Define the source files used for each executable
# tuxmath
set(SOURCES_TUXMATH
  audio.c
  convert_utf.c
  credits.c
  factoroids.c
  fileops.c
  fileops_media.c
  game.c
  menu.c
  highscore.c
  lessons.c
  loaders.c
  mathcards.c
  network.c
  options.c
  pixels.c
  scandir.c
  SDL_extras.c
  setup.c
  throttle.c
  titlescreen.c
  multiplayer.c
  campaign.c
  tuxmath.c
  linewrap.c
  ${TUXMATH_EXTRA_SRC}
  )

message("${SOURCES_TUXMATH}")

# tuxmathadmin
set(SOURCES_TUXMATHADMIN
  tuxmathadmin.c
  )

if (NOT SDL_FOUND)
  # Workaround for REQUIRED flag not working with cmake < 2.4.7.
  # Should put other libraries in, too.
  message(FATAL_ERROR "SDL not found!")
endif (NOT SDL_FOUND)

if (NOT APPLE)
  link_libraries (SDLmain)
endif (NOT APPLE)

## Include files
if (NOT BUILD_INTL)
  # Generate the config.h file
  configure_file(${TuxMath_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
endif (NOT BUILD_INTL)
check_include_file(${CMAKE_CURRENT_BINARY_DIR}/config.h HAVE_CONFIG_H)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${TUXMATH_EXTRA_INCLUDES} ${INTL_BINARY_DIR})
if (SDLGFX_FOUND)
   include_directories(${SDLGFX_INCLUDE_DIR})
endif (SDLGFX_FOUND)

if (TUXMATH_BUILD_INTL)
  link_directories(${INTL_BINARY_DIR})
endif(TUXMATH_BUILD_INTL)

if (TUXMATH_BUILD_LINEBREAK)
  link_directories(${LINEBREAK_BINARY_DIR})
endif (TUXMATH_BUILD_LINEBREAK)

## Define the executables
add_executable (
  tuxmath
  MACOSX_BUNDLE
  WIN32
  ${SOURCES_TUXMATH}
  )

add_executable (
  tuxmathadmin
  ${SOURCES_TUXMATHADMIN}
  )

# getting rid of semicolons
set(_rsvg_cflags "")
foreach(f ${RSVG_CFLAGS})
  set(_rsvg_cflags "${_rsvg_cflags} ${f}")
endforeach(f)

set(_cairo_cflags "")
foreach(f ${CAIRO_CFLAGS})
  set(_cairo_cflags "${_cairo_cflags} ${f}")
endforeach(f)

set(_rsvg_ldflags "")
foreach(f ${RSVG_LDFLAGS})
  set(_rsvg_ldflags "${_rsvg_ldflags} ${f}")
endforeach(f)

set(_cairo_ldflags "")
foreach(f ${CAIRO_LDFLAGS})
  set(_cairo_ldflags "${_cairo_ldflags} ${f}")
endforeach(f)

set_target_properties (
  tuxmath
  PROPERTIES COMPILE_FLAGS
  "-DDATA_PREFIX=\\\"${TUXMATH_DATA_PREFIX}\\\" -DVERSION=\\\"${TUXMATH_VERSION}\\\" -DLOCALEDIR=\\\"${LOCALE_DIR}\\\" -DPACKAGE=\\\"tuxmath\\\" ${_rsvg_cflags} ${_cairo_cflags}"
  LINK_FLAGS "${_rsvg_ldflags} ${_cairo_ldflags}"
  )

target_link_libraries (tuxmath
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${SDLMIXER_LIBRARY}
  ${SDLNET_LIBRARY}
  )

if (SDLPANGO_FOUND)
  target_link_libraries (tuxmath
    ${SDLPANGO_LIBRARY}
    )
endif (SDLPANGO_FOUND)

if (SDLGFX_FOUND)
   target_link_libraries (tuxmath
   ${SDLGFX_LIBRARY}
   )
endif (SDLGFX_FOUND)

if (APPLE)
  # The following seems to be needed to compile under 10.5
  set_target_properties(tuxmath tuxmathadmin
    PROPERTIES
    LINK_FLAGS "-mmacosx-version-min=10.4")
endif(APPLE)

if (TUXMATH_BUILD_INTL)
  message("Linking iconv: ${ICONV_LIBRARIES}")
  target_link_libraries(tuxmath ${ICONV_LIBRARIES} libintl.a)
endif(TUXMATH_BUILD_INTL)

if (TUXMATH_BUILD_LINEBREAK)
  target_link_libraries(tuxmath ${ICONV_LIBRARIES} liblinebreak.a)
endif(TUXMATH_BUILD_LINEBREAK)

set_target_properties (
  tuxmathadmin
  PROPERTIES COMPILE_FLAGS 
  "-DDATA_PREFIX=\\\"${TUXMATH_DATA_PREFIX}\\\" -DVERSION=\\\"${TUXMATHADMIN_VERSION}\\\" -DLOCALEDIR=\\\"${LOCALE_DIR}\\\" -DPACKAGE=\\\"tuxmathadmin\\\""
  )

## Installation specifications
if (UNIX AND NOT APPLE)
  install (TARGETS tuxmath tuxmathadmin
    RUNTIME DESTINATION bin)
endif(UNIX AND NOT APPLE)
