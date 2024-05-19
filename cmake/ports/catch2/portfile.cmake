vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO catchorg/Catch2
    REF v3.0.1
    SHA512 065094c19cdf98b40f96a390e887542f895495562a91cdc28d68ce03690866d846ec87d320405312a2b97eacaa5351d3e55f0012bb9de40073c8d4444d82b0a1
    HEAD_REF devel
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCATCH_INSTALL_DOCS=OFF
		-DCMAKE_CXX_STANDARD=17	# WORKAROUND: catch2 conditionally enables new features we use, based on the c++ version
		-DCMAKE_CXX_STANDARD_REQUIRED=ON
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/Catch2)

file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/lib/pkgconfig")
file(RENAME "${CURRENT_PACKAGES_DIR}/share/pkgconfig/catch2-with-main.pc" "${CURRENT_PACKAGES_DIR}/lib/pkgconfig/catch2-with-main.pc")
file(RENAME "${CURRENT_PACKAGES_DIR}/share/pkgconfig/catch2.pc" "${CURRENT_PACKAGES_DIR}/lib/pkgconfig/catch2.pc")
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE
	"${CURRENT_PACKAGES_DIR}/debug/include"
	"${CURRENT_PACKAGES_DIR}/debug/share"
)

# We remove these folders because they are empty and cause warnings on the library installation
file(REMOVE_RECURSE
	"${CURRENT_PACKAGES_DIR}/include/catch2/benchmark/internal"
	"${CURRENT_PACKAGES_DIR}/include/catch2/generators/internal"
)

file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
