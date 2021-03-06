android {
  # To build for android you need OSGeo libraries
  OSGEO4A_DIR = /home/input-sdk
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  # we try to use it as /sdcard/path and if not writable, use /storage/emulated/0/path (user home path)
  GEODIFF_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
  GEODIFF_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
}

win32 {
  QGIS_INSTALL_PATH =  C:/projects/input-sdk/x86_64/stage
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  GEODIFF_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
  GEODIFF_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
}

ios {
  QGIS_INSTALL_PATH =  /opt/INPUT/input-sdk-ios-$$(SDK_VERSION)/stage/arm64
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  QMAKE_IOS_DEPLOYMENT_TARGET = 12.0
  GEODIFF_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
  GEODIFF_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
}

macx:!android {
  QGIS_INSTALL_PATH = /opt/INPUT/input-sdk-mac-$$(SDK_VERSION)/stage/mac
  QGIS_QUICK_DATA_PATH = $$(ROOT_DIR)/input/app/android/assets/qgis-data
  GEODIFF_INCLUDE_DIR = /opt/INPUT/input-sdk-mac-$$(SDK_VERSION)/stage/mac/include
  GEODIFF_LIB_DIR = /opt/INPUT/input-sdk-mac-$$(SDK_VERSION)/stage/mac/lib

  # also setup coverall for macos build
  # --coverage option is synonym for: -fprofile-arcs -ftest-coverage -lgcov
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS += --coverage
}
