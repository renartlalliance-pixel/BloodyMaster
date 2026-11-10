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
    ${CMAKE_SOURCE_DIR}/Source/DSP/MasteringChain.cpp
    ${CMAKE_SOURCE_DIR}/Source/Parameters/ParameterLayout.cpp)
target_include_directories(BloodyMasterTests PRIVATE ${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/Parameters)
target_compile_definitions(BloodyMasterTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(BloodyMasterTests
    PRIVATE juce::juce_audio_utils
    PUBLIC  juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
add_test(NAME plugin_tests COMMAND BloodyMasterTests)
