cmake_minimum_required(VERSION 3.22)
project(BloodyMaster VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Runtime MSVC statique : pas de dependance a la redistribuable Visual C++ chez l'utilisateur
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# --- Chemins courts (limite MAX_PATH de Windows) ---
# JUCE (sources + build intermediaire) est place dans un dossier court, hors de l'arbre du projet.
if(WIN32)
    set(BM_DEPS_DIR "C:/bm_deps" CACHE PATH "Dossier court pour les dependances (JUCE)")
else()
    set(BM_DEPS_DIR "${CMAKE_BINARY_DIR}/_deps" CACHE PATH "Dossier des dependances (JUCE)")
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
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(BloodyMaster
    COMPANY_NAME "B.R.I Audio Systems"
    PRODUCT_NAME "Bloody Master"
    BUNDLE_ID "com.briaudiosystems.bloodymaster"
    PLUGIN_MANUFACTURER_CODE BRIA
    PLUGIN_CODE BlMa
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Dynamics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VERSION ${PROJECT_VERSION})

target_sources(BloodyMaster PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/MasteringChain.cpp
    Source/Parameters/ParameterLayout.cpp)

target_include_directories(BloodyMaster PRIVATE Source Source/Parameters Source/Utilities)

target_compile_definitions(BloodyMaster PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(BloodyMaster
    PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC  juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

# --- Tests DSP (sans JUCE) ---
enable_testing()
add_subdirectory(Tests)
