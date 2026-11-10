# 1) Test sans JUCE (utilitaires dB)
add_executable(bm_test_dbutils DSP/test_dbutils.cpp)
target_include_directories(bm_test_dbutils PRIVATE ${CMAKE_SOURCE_DIR}/Source/Utilities)
add_test(NAME dbutils COMMAND bm_test_dbutils)

# 2) Tests du plugin (layouts mono/stereo, passthrough bit-exact, parametres/automation, etat)
juce_add_console_app(BloodyMasterTests PRODUCT_NAME "BloodyMasterTests")
target_sources(BloodyMasterTests PRIVATE
    Plugin/PluginTests.cpp
    ${CMAKE_SOURCE_DIR}/Source/PluginProcessor.cpp
    ${CMAKE_SOURCE_DIR}/Source/PluginEditor.cpp
    ${CMAKE_SOURCE_DIR}/Source/HtmlPluginEditor.cpp
    ${CMAKE_SOURCE_DIR}/Source/Parameters/ParameterLayout.cpp)
target_include_directories(BloodyMasterTests PRIVATE ${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/Parameters)
bm_configure_ui(BloodyMasterTests)
target_compile_definitions(BloodyMasterTests PRIVATE BM_UI_TESTING=1 JUCE_MODAL_LOOPS_PERMITTED=1)
target_link_libraries(BloodyMasterTests
    PRIVATE juce::juce_audio_utils
    PUBLIC  juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
add_test(NAME plugin_tests COMMAND BloodyMasterTests)
if(BM_HTML_UI)
    add_test(NAME html_webview_smoke COMMAND BloodyMasterTests --html-ui-smoke "${CMAKE_CURRENT_BINARY_DIR}/web-ui-report.json")
    set_tests_properties(html_webview_smoke PROPERTIES TIMEOUT 120)
endif()

# Independent DSP core, no JUCE required by this executable.
add_executable(bm_test_mastering DSP/test_mastering.cpp)
target_include_directories(bm_test_mastering PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME mastering_core COMMAND bm_test_mastering)

add_executable(bm_test_dynamics DSP/test_dynamics.cpp)
target_include_directories(bm_test_dynamics PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME dynamics_peak COMMAND bm_test_dynamics)

add_executable(bm_test_switching DSP/test_switching.cpp)
target_include_directories(bm_test_switching PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME oversampling_switching COMMAND bm_test_switching)

add_executable(bm_test_analog_behaviors DSP/test_analog_behaviors.cpp)
target_include_directories(bm_test_analog_behaviors PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME analog_behaviors COMMAND bm_test_analog_behaviors)

add_executable(bm_test_presets DSP/test_presets.cpp)
target_include_directories(bm_test_presets PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME presets COMMAND bm_test_presets)

if(WIN32)
    add_executable(bm_vst3_smoke Host/vst3_smoke.cpp)
    target_include_directories(bm_vst3_smoke PRIVATE
        "${JUCE_MODULES_DIR}/juce_audio_processors/format_types/VST3_SDK")
    if(MINGW)
        target_link_options(bm_vst3_smoke PRIVATE -municode)
    endif()
    add_dependencies(bm_vst3_smoke BloodyMaster_VST3)
    add_test(NAME vst3_binary_smoke COMMAND bm_vst3_smoke "$<TARGET_FILE:BloodyMaster_VST3>")
endif()

add_executable(bm_test_cleanup DSP/test_cleanup.cpp)
target_include_directories(bm_test_cleanup PRIVATE ${CMAKE_SOURCE_DIR}/Source)
add_test(NAME dynamic_cleanup COMMAND bm_test_cleanup)
