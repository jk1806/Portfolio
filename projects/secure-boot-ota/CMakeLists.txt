cmake_minimum_required(VERSION 3.10)
project(SecureBootOTA VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find mbedTLS
find_package(PkgConfig REQUIRED)
pkg_check_modules(MBEDTLS REQUIRED mbedtls)

# Include directories
include_directories(
    bootloader/include
    firmware/include
    crypto/include
    ota_client/include
    config
)

# Bootloader
add_library(bootloader STATIC
    bootloader/src/boot.c
    bootloader/src/verify.c
    bootloader/src/decrypt.c
    bootloader/src/version.c
    bootloader/src/integrity.c
    bootloader/src/jump.c
)
target_link_libraries(bootloader ${MBEDTLS_LIBRARIES})
target_include_directories(bootloader PUBLIC bootloader/include)

# Crypto library
add_library(crypto_lib STATIC
    crypto/src/sign.c
    crypto/src/verify.c
    crypto/src/encrypt.c
    crypto/src/decrypt.c
    crypto/src/hash.c
    crypto/src/key_derivation.c
)
target_link_libraries(crypto_lib ${MBEDTLS_LIBRARIES})
target_include_directories(crypto_lib PUBLIC crypto/include)

# Firmware
add_executable(firmware
    firmware/src/main.c
    firmware/src/anti_tamper.c
    firmware/src/mpu_sim.c
)
target_link_libraries(firmware crypto_lib ${MBEDTLS_LIBRARIES})

# OTA Client
add_executable(ota_client
    ota_client/src/ota_client.c
    ota_client/src/download.c
)
target_link_libraries(ota_client crypto_lib ${MBEDTLS_LIBRARIES})

# Tests
enable_testing()
add_executable(test_boot tests/test_boot.c)
target_link_libraries(test_boot bootloader crypto_lib ${MBEDTLS_LIBRARIES})
add_test(NAME BootTest COMMAND test_boot)

add_executable(test_crypto tests/test_crypto.c)
target_link_libraries(test_crypto crypto_lib ${MBEDTLS_LIBRARIES})
add_test(NAME CryptoTest COMMAND test_crypto)

add_executable(test_ota tests/test_ota.c)
target_link_libraries(test_ota ota_client crypto_lib ${MBEDTLS_LIBRARIES})
add_test(NAME OTATest COMMAND test_ota)

# Install targets
install(TARGETS firmware ota_client DESTINATION bin)

