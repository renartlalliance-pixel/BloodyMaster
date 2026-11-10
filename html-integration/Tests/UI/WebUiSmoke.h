#pragma once
#include "PluginEditor.h"
#include <chrono>
#include <cstdio>
inline int runWebUiSmoke(const juce::String& reportPath) {
    using Clock=std::chrono::steady_clock;
    int checks=0;
    const auto pump=[](int ms){juce::MessageManager::getInstance()->runDispatchLoopUntil(ms);};
    const auto wait=[&](auto condition,int timeout=30000) {
        const auto end=Clock::now()+std::chrono::milliseconds(timeout);
        do {if(condition())return true;pump(25);}while(Clock::now()<end);
        return condition();
    };
    BloodyMasterProcessor p;p.setRateAndBufferSizeDetails(48000,512);p.prepareToPlay(48000,512);p.setCurrentProgram(3);
    auto editor=std::make_unique<BloodyMasterEditor>(p);
    editor->addToDesktop(juce::ComponentPeer::windowIsTemporary);editor->setVisible(true);
    const auto verify=[&](bool success,const char* name){++checks;std::printf("%s %s\n",success?"PASS":"FAIL",name);return success;};
    if(!verify(wait([&]{return editor->isUiReady();}),"WebView2 runtime page ready"))return 1;
    const auto evaluate=[&](const juce::String& script) -> juce::var {
        struct Capture { bool done=false;juce::var value; };
        auto capture=std::make_shared<Capture>();
        editor->evaluateForTesting(script,[capture](juce::WebBrowserComponent::EvaluationResult r){
            if(auto* result=r.getResult())capture->value=*result;capture->done=true;
        });
        if(!wait([&]{return capture->done;},10000))return {};
        return capture->value;
    };
    if(!verify(wait([&]{return static_cast<bool>(evaluate("!!window.bloodyMasterUi?.getState()"));}),"Native state reaches HTML"))return 2;
    if(!verify(static_cast<bool>(evaluate("document.querySelectorAll('.knob-base').length===36 && document.querySelector('#knob-drive').getBoundingClientRect().width===80 && getComputedStyle(document.querySelector('.wood-panel')).backgroundImage.includes('linear-gradient')")),"Approved layout and offline CSS loaded"))return 3;
    evaluate("document.querySelector('#knob-drive').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::drive)->load()>1.1f;}),"HTML knob updates APVTS"))return 4;
    auto* drive=p.apvts.getParameter(bm::id::drive);drive->setValueNotifyingHost(drive->convertTo0to1(6.3f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("Math.abs(Number(document.querySelector('#knob-drive').dataset.val)-6.3)<.001"));}),"Host automation updates knob"))return 5;
    evaluate("document.querySelector('#presetSelector').value='8';document.querySelector('#presetSelector').dispatchEvent(new Event('change'));true");
    if(!verify(wait([&]{return p.getCurrentProgram()==8;}),"HTML factory preset selection"))return 6;
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().preset===8"));}),"Preset recall returns to HTML"))return 7;
    evaluate("document.querySelector('#bypassBtn').click();true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::bypass)->load()>.5f;}),"HTML bypass reaches audio parameter"))return 8;
    evaluate("document.querySelector('#abBtn').click();true");pump(100);
    drive->setValueNotifyingHost(drive->convertTo0to1(7.f));
    evaluate("document.querySelector('#abBtn').click();true");
    if(!verify(wait([&]{return std::abs(p.apvts.getRawParameterValue(bm::id::drive)->load()-2.f)<.001f;}),"A/B recalls parameter snapshot"))return 9;
    p.apvts.getParameter(bm::id::bypass)->setValueNotifyingHost(0);
    juce::AudioBuffer<float> audio(2,512);juce::MidiBuffer midi;
    for(int block=0;block<3;++block) {
        for(int i=0;i<512;++i){const auto v=.2f*std::sin(2*juce::MathConstants<float>::pi*997*(block*512+i)/48000);audio.setSample(0,i,v);audio.setSample(1,i,v*.5f);}
        p.processBlock(audio,midi);
    }
    if(!verify(wait([&]{return static_cast<bool>(evaluate("(()=>{const s=window.bloodyMasterUi.getState();return s.meters.outputL>0 && s.scope.out[0].some(v=>Math.abs(v)>.001) && s.latency===336 && s.sampleRate===48000;})()"));}),"Real audio samples, meters, Fs and latency reach HTML"))return 10;
    if(!verify(static_cast<bool>(evaluate("document.querySelector('#knob-de-amount').getAttribute('aria-disabled')==='false' && window.bloodyMasterUi.getState().parameters.mud_active.value===0")),"Cleanup controls active and traps off by default"))return 11;
    evaluate("document.querySelector('#mudActiveBtn').click();document.querySelector('#knob-mud-amount').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::mudActive)->load()>.5f && p.apvts.getRawParameterValue(bm::id::mudAmount)->load()>25.f && p.apvts.getRawParameterValue(bm::id::ringActive)->load()<.5f;}),"Independent Mud activation and amount reach APVTS"))return 12;
    evaluate("document.querySelector('#ringActiveBtn').click();document.querySelector('#deSoloBtn').click();true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::ringActive)->load()>.5f && p.apvts.getRawParameterValue(bm::id::spikeListen)->load()>.5f;}),"Ring activation and delta listen reach APVTS"))return 13;
    auto* ring=p.apvts.getParameter(bm::id::ringAmount);ring->setValueNotifyingHost(ring->convertTo0to1(61.f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("Math.abs(Number(document.querySelector('#knob-ring-amount').dataset.val)-61)<.001"));}),"Host automation updates new Ring amount"))return 14;
    editor->setVisible(false);editor->removeFromDesktop();editor.reset();
    auto* result=new juce::DynamicObject;result->setProperty("passed",checks);result->setProperty("backend","WebView2 native Windows");
    if(reportPath.isNotEmpty())juce::File(reportPath).replaceWithText(juce::JSON::toString(juce::var(result)));
    else delete result;
    std::printf("PASS %d native WebView2 integration checks\n",checks);return 0;
}
