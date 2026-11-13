cmake_minimum_required(VERSION 3.22)
project(BloodyMaster VERSION 0.3.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Runtime MSVC statique : pas de dependance a la redistribuable Visual C++ chez l'utilisateur
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
option(BM_HTML_UI "Use the approved HTML via WebView2 on Windows" ${WIN32})
set(BM_WEBVIEW2_BOOTSTRAPPER "" CACHE FILEPATH "Microsoft Evergreen bootstrapper included by the installer")
if(BM_HTML_UI AND NOT WIN32)
    message(FATAL_ERROR "This HTML edition currently targets Windows WebView2")
endif()

# --- Chemins courts (limite MAX_PATH de Windows) ---
# JUCE (sources + build intermediaire) est place dans un dossier court, hors de l'arbre du projet.
if(WIN32)
    set(BM_DEPS_DIR "C:/bm_deps" CACHE PATH "Dossier court pour les dependances (JUCE)")
else()
    set(BM_DEPS_DIR "${CMAKE_BINARY_DIR}/_deps" CACHE PATH "Dossier des dependances (JUCE)")
endif()

if(WIN32)
    find_program(BM_MAKENSIS makensis HINTS "C:/Program Files (x86)/NSIS")
    if(BM_MAKENSIS)
        file(TO_NATIVE_PATH "${BM_WEBVIEW2_BOOTSTRAPPER}" BM_BOOTSTRAPPER_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/installer" BM_INSTALLER_OUTPUT_NATIVE)
        add_custom_target(BloodyMasterInstaller
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/installer"
            COMMAND "${BM_MAKENSIS}"
                "-DPLUGIN_DIR=$<SHELL_PATH:$<TARGET_FILE_DIR:BloodyMaster_VST3>/../..>"
                "-DWAV_EXE=$<SHELL_PATH:$<TARGET_FILE:BloodyMasterTest>>"
                "-DOUT_DIR=${BM_INSTALLER_OUTPUT_NATIVE}"
                "-DHTML_UI=$<BOOL:${BM_HTML_UI}>"
                "-DWEBVIEW_PROBE=$<SHELL_PATH:$<TARGET_FILE:bm_webview_probe>>"
                "-DWEBVIEW_BOOTSTRAPPER=${BM_BOOTSTRAPPER_NATIVE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/packaging/BloodyMaster.nsi"
            DEPENDS BloodyMaster_VST3 BloodyMasterTest bm_webview_probe
            VERBATIM)
    endif()
endif()
set(FETCHCONTENT_BASE_DIR "${BM_DEPS_DIR}")

if(WIN32)
    string(LENGTH "${CMAKE_BINARY_DIR}" _bm_len)
    if(_bm_len GREATER 60)
        message(WARNING "Chemin de build long (${_bm_len} car.) : risque de depasser MAX_PATH (260) sous Windows. "
                        "Deplacez le projet vers un dossier court, par ex. C:/BM, ou activez les chemins longs Windows.")
    endif()
endif()

# --- JUCE : sous-module ./JUCE s'il existe, sinon telechargement (tag epingle) ---
set(BM_JUCE_VERSION "8.0.4" CACHE STRING "Version JUCE pour cette compilation")
if(JUCE_DIR)
    find_package(JUCE ${BM_JUCE_VERSION} EXACT CONFIG REQUIRED)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        ${BM_JUCE_VERSION}
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

if(BM_HTML_UI)
    if(BM_JUCE_VERSION VERSION_LESS "8.0.4")
        message(FATAL_ERROR "The HTML native bridge requires JUCE 8.0.4 or newer")
    endif()
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/UI/index.html"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_web_ui.py")
    execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/build_web_ui.py"
        "${CMAKE_CURRENT_BINARY_DIR}/web-runtime" COMMAND_ERROR_IS_FATAL ANY)
    juce_add_binary_data(BloodyMasterWebResources NAMESPACE BloodyMasterWebResources
        HEADER_NAME BloodyMasterWebResources.h SOURCES
        "${CMAKE_CURRENT_BINARY_DIR}/web-runtime/index.html"
        Resources/Web/tailwind.css Resources/Web/bridge.js)
endif()

if(CMAKE_CROSSCOMPILING)
    set(BM_AUTO_MANIFEST FALSE)
else()
    set(BM_AUTO_MANIFEST TRUE)
endif()

juce_add_plugin(BloodyMaster
    COMPANY_NAME "B.R.I Audio Systems"
    PRODUCT_NAME "Bloody Master"
    BUNDLE_ID "com.briaudio.bloodymaster"
    PLUGIN_MANUFACTURER_CODE BRIA
    PLUGIN_CODE BlMa
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Dynamics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    NEEDS_WEBVIEW2 ${BM_HTML_UI}
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_AUTO_MANIFEST ${BM_AUTO_MANIFEST}
    VERSION ${PROJECT_VERSION})

target_sources(BloodyMaster PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/HtmlPluginEditor.cpp
    Source/Parameters/ParameterLayout.cpp)

target_include_directories(BloodyMaster PRIVATE Source Source/Parameters Source/Utilities)

function(bm_configure_ui target)
    target_compile_definitions(${target} PUBLIC BM_HTML_UI=$<BOOL:${BM_HTML_UI}>
        JUCE_WEB_BROWSER=$<BOOL:${BM_HTML_UI}> JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
    if(BM_HTML_UI)
        target_compile_definitions(${target} PUBLIC JUCE_USE_WIN_WEBVIEW2=1 JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1)
        target_link_libraries(${target} PRIVATE BloodyMasterWebResources juce::juce_webview2)
    endif()
endfunction()
bm_configure_ui(BloodyMaster)

target_link_libraries(BloodyMaster
    PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC  juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

# --- Tests DSP (sans JUCE) ---
enable_testing()
add_subdirectory(Tests)

# Offline WAV audition app, using the identical production DSP core.
add_executable(BloodyMasterTest tools/wav_test.cpp)
target_include_directories(BloodyMasterTest PRIVATE Source)
if(WIN32)
    target_link_libraries(BloodyMasterTest PRIVATE comdlg32)
    if(MINGW)
        target_link_options(BloodyMasterTest PRIVATE -municode)
    endif()
endif()

if(WIN32)
    add_executable(bm_webview_probe tools/webview_probe.cpp)
    if(BM_HTML_UI)
        target_compile_definitions(bm_webview_probe PRIVATE BM_HTML_UI=1)
        target_link_libraries(bm_webview_probe PRIVATE juce::juce_webview2 ole32 shlwapi version)
    endif()
    if(MINGW)
        target_link_options(bm_webview_probe PRIVATE -municode)
    endif()
endif()
