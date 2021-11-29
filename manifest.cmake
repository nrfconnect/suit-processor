#
# Generated using zcbor version 0.5.1
# https://github.com/NordicSemiconductor/zcbor
#

add_library(manifest)
target_sources(manifest PRIVATE
    src/zcbor_decode.c
    src/zcbor_encode.c
    src/zcbor_common.c
    src/manifest_decode.c
    )
target_include_directories(manifest PUBLIC
    include
    )
