# SPDX-FileCopyrightText: (C) 2024 Chris Rizzitello <sithlord48@gmail.com>
# SPDX-License-Identifier: MIT

# HACK This is set when the files is included so its the real path
# calling CMAKE_CURRENT_LIST_DIR after include would return the wrong scope var
set(MY_DIR ${CMAKE_CURRENT_LIST_DIR})
set(OSX_BUNDLE ${BUILD_OSX_BUNDLE})

set(OS_STRING "macos-${BUILD_ARCHITECTURE}")

if (OSX_BUNDLE)
  set(_codesign_options -codesign=-)
  if (APPLE_CODESIGN_DEV)
    set(_codesign_options "-codesign=${APPLE_CODESIGN_DEV}" -hardened-runtime -timestamp)
  endif()

  # Avoid unrelated plugins from shared Homebrew Qt installations.
  foreach(_plugin QCocoaIntegrationPlugin QMacStylePlugin QSvgIconPlugin QSvgPlugin QSecureTransportBackendPlugin)
    get_target_property(_plugin_type Qt6::${_plugin} QT_PLUGIN_TYPE)
    get_target_property(_plugin_file Qt6::${_plugin} IMPORTED_LOCATION)
    file(REAL_PATH "${_plugin_file}" _plugin_file)
    set(_plugin_dest "${MAC_APP_NAME}.app/Contents/PlugIns/${_plugin_type}")
    install(FILES "${_plugin_file}" DESTINATION "${_plugin_dest}")
    list(APPEND _deployqt_executables
      "\"-executable=\${CMAKE_INSTALL_PREFIX}/${_plugin_dest}/$<TARGET_FILE_NAME:Qt6::${_plugin}>\"")
  endforeach()
  string(JOIN " " _deployqt_executables ${_deployqt_executables})
  string(JOIN "\" \"" _codesign_options ${_codesign_options})
  file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" CONTENT "[Paths]\nPlugins=PlugIns\n")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" DESTINATION "${MAC_APP_NAME}.app/Contents/Resources")

  install(CODE "execute_process(COMMAND
    \"${DEPLOYQT}\"
    \"\${CMAKE_INSTALL_PREFIX}/${MAC_APP_NAME}.app\"
    -no-plugins
    \"-executable=\${CMAKE_INSTALL_PREFIX}/${MAC_APP_NAME}.app/Contents/MacOS/${CORE_BINARY}\"
    ${_deployqt_executables}
    \"${_codesign_options}\"
    COMMAND_ERROR_IS_FATAL ANY
  )")
  install(CODE "execute_process(COMMAND
    codesign --verify --deep --strict \"\${CMAKE_INSTALL_PREFIX}/${MAC_APP_NAME}.app\"
    COMMAND_ERROR_IS_FATAL ANY
  )")
  set(CPACK_PACKAGE_ICON "${MY_DIR}/dmg-volume.icns")
  set(CPACK_DMG_BACKGROUND_IMAGE "${MY_DIR}/dmg-background.tiff")
  set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${MY_DIR}/generate_ds_store.applescript")
  set(CPACK_DMG_VOLUME_NAME "${MAC_APP_NAME}")
  set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON)
  set(CPACK_GENERATOR "DragNDrop")
endif()
