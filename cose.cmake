#
# Generated using zcbor version 0.5.1
# https://github.com/NordicSemiconductor/zcbor
#

add_library(cose)
target_sources(cose PRIVATE
    src/zcbor_decode.c
    src/zcbor_encode.c
    src/zcbor_common.c
    src/cose_decode.c
    src/cose_encode.c
    )
target_include_directories(cose PUBLIC
    include
    )
