cmake_minimum_required(VERSION 3.0)
project(php)

set(KDEVPHP_VERSION_MAJOR 5)
set(KDEVPHP_VERSION_MINOR 2)
set(KDEVPHP_VERSION_PATCH 40)
# KDevplatform dependency version
set( KDEVPLATFORM_VERSION "${KDEVPHP_VERSION_MAJOR}.${KDEVPHP_VERSION_MINOR}.${KDEVPHP_VERSION_PATCH}" )

configure_file( "${php_SOURCE_DIR}/kdevphpversion.h.cmake" "${php_BINARY_DIR}/kdevphpversion.h" @ONLY )

find_package (ECM "5.14.0" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)
include(FeatureSummary)

set(QT_MIN_VERSION "5.5.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Test)
set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS
    Archive
    ThreadWeaver
    TextEditor
    I18n
    KCMUtils
)
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)
find_package(KDevelop-PG-Qt REQUIRED)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdocumentation")
endif()

add_definitions( -DTRANSLATION_DOMAIN=\"kdevphp\" )

include_directories(
    ${KDEVPGQT_INCLUDE_DIR}
)

add_subdirectory(app_templates)
add_subdirectory(parser)
add_subdirectory(duchain)
add_subdirectory(completion)
add_subdirectory(testprovider)
add_subdirectory(docs)

set(kdevphplanguagesupport_PART_SRCS
    phplanguagesupport.cpp
    phpparsejob.cpp
    phphighlighting.cpp

    codegen/refactoring.cpp
)
ecm_qt_declare_logging_category(kdevphplanguagesupport_PART_SRCS
    HEADER phpdebug.h
    IDENTIFIER PHP
    CATEGORY_NAME "kdevelop.languages.php"
)

kdevplatform_add_plugin(kdevphplanguagesupport JSON kdevphpsupport.json
    SOURCES ${kdevphplanguagesupport_PART_SRCS})
target_link_libraries(kdevphplanguagesupport
    KDev::Interfaces
    KDev::Language
    kdevphpduchain
    kdevphpparser
    kdevphpcompletion

    KF5::I18n
    KF5::TextEditor
)

# not writeable so that the refactoring actions get hidden
install(FILES phpfunctions.php DESTINATION ${KDE_INSTALL_DATADIR}/kdevphpsupport PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

install(FILES org.kde.kdev-php.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# kdebugsettings file
install(FILES kdevphpsupport.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
