// Tests M3 du plugin. Critere global : code retour 0 si et seulement si aucun test n'echoue ET au moins un test a tourne.
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <set>
#include "PluginProcessor.h"
#include "Parameters/ParameterIDs.h"
#include "Presets/FactoryPresets.h"
#if BM_HTML_UI
#include "../UI/WebUiSmoke.h"
#endif

namespace
{
using juce::AudioChannelSet;

juce::AudioProcessor::BusesLayout layoutOf(const AudioChannelSet& in, const AudioChannelSet& out)
{
    juce::AudioProcessor::BusesLayout l;
    l.inputBuses.add(in);
    l.outputBuses.add(out);
    return l;
}

struct BloodyMasterTests final : public juce::UnitTest
{
    BloodyMasterTests() : juce::UnitTest("Bloody Master M3", "BloodyMaster") {}

    void runTest() override
    {
        testLayouts();
        testPassthrough();
        testParameters();
        testDefaults();
        testState();
        testLiveOversampling();
        testFactoryPrograms();
        testHostBypassAndMeters();
        testEditor();
    }

    void testLayouts()
    {
        beginTest("Layouts mono / stereo");
        BloodyMasterProcessor p;
        const auto mono = AudioChannelSet::mono();
        const auto stereo = AudioChannelSet::stereo();
        expect(p.isBusesLayoutSupported(layoutOf(mono, mono)), "mono->mono doit etre accepte");
        expect(p.isBusesLayoutSupported(layoutOf(stereo, stereo)), "stereo->stereo doit etre accepte");
        expect(!p.isBusesLayoutSupported(layoutOf(mono, stereo)), "mono->stereo doit etre refuse");
        expect(!p.isBusesLayoutSupported(layoutOf(AudioChannelSet::create5point1(), AudioChannelSet::create5point1())),
               "5.1 doit etre refuse");
        expect(p.setBusesLayout(layoutOf(mono, mono)), "setBusesLayout mono");
        expectEquals(p.getTotalNumInputChannels(), 1);
        expectEquals(p.getTotalNumOutputChannels(), 1);
        expect(p.setBusesLayout(layoutOf(stereo, stereo)), "setBusesLayout stereo");
        expectEquals(p.getTotalNumInputChannels(), 2);
        expectEquals(p.getTotalNumOutputChannels(), 2);
    }

    void testPassthrough()
    {
        beginTest("Bypass: latency-aligned, bit-exact, mono/stereo");
        for (int nch : {1,2}) for (double sr : {44100.,48000.,96000.,192000.})
        for (int block : {1,32,480,1024})
        {
            BloodyMasterProcessor p;
            const auto set=nch==1?AudioChannelSet::mono():AudioChannelSet::stereo();
            expect(p.setBusesLayout(layoutOf(set,set)));
            p.apvts.getParameter(bm::id::bypass)->setValueNotifyingHost(1.f);
            p.prepareToPlay(sr,block);
            const int delay=p.getLatencySamples();
            expectEquals(delay,192+static_cast<int>(std::ceil(sr*.003)));
            const int length=2000,total=length+delay;
            juce::AudioBuffer<float> input(nch,total),output(nch,total); input.clear();output.clear();
            juce::Random rng(12345);
            for(int c=0;c<nch;++c) for(int i=0;i<length;++i) input.setSample(c,i,rng.nextFloat()-.5f);
            juce::MidiBuffer midi;
            for(int pos=0;pos<total;pos+=block) {
                int size=juce::jmin(block,total-pos);juce::AudioBuffer<float> buf(nch,size);
                for(int c=0;c<nch;++c) buf.copyFrom(c,0,input,c,pos,size);
                p.processBlock(buf,midi);
                for(int c=0;c<nch;++c) output.copyFrom(c,pos,buf,c,0,size);
            }
            for(int c=0;c<nch;++c)
                expect(std::memcmp(input.getReadPointer(c),output.getReadPointer(c)+delay,sizeof(float)*length)==0);
        }
    }

    void testParameters()
    {
        beginTest("Parametres : nombre, IDs uniques/versionnes, automatisables");
        BloodyMasterProcessor p;
        const auto& params = p.getParameters();
        expectEquals(params.size(), 48); // Original parameters, four trap controls and two color choices.

        std::set<juce::String> ids;
        for (auto* prm : params)
        {
            auto* withId = dynamic_cast<juce::AudioProcessorParameterWithID*>(prm);
            expect(withId != nullptr, "parametre sans ID");
            if (withId == nullptr)
                continue;
            const auto pid = withId->getParameterID();
            expect(ids.insert(pid).second, "ID duplique : " + pid);
            expect(pid.startsWith("bm_"), "prefixe bm_ manquant : " + pid);
            expectEquals(withId->getVersionHint(), 1);
            expect(prm->isAutomatable(), "non automatisable : " + pid);
            expect(!prm->isMetaParameter(), "meta-parametre : " + pid);
            expect(prm->getName(64).isNotEmpty(), "nom vide : " + pid);

            const float def = prm->getDefaultValue();
            expect(def >= 0.0f && def <= 1.0f, "defaut hors [0,1] : " + pid);

            prm->setValueNotifyingHost(0.5f);
            const float v = prm->getValue();
            expect(v >= 0.0f && v <= 1.0f, "valeur hors [0,1] : " + pid);
            expect(prm->getText(v, 32).isNotEmpty(), "texte vide : " + pid);

            if (dynamic_cast<juce::AudioParameterFloat*>(prm) != nullptr)
            {
                prm->setValueNotifyingHost(0.37f);
                expectWithinAbsoluteError(prm->getValue(), 0.37f, 1.0e-4f);
            }
        }
    }

    void testDefaults()
    {
        beginTest("Valeurs par defaut (issues du HTML) et bypass hote");
        BloodyMasterProcessor p;
        auto raw = [&](const char* id) { return p.apvts.getRawParameterValue(id)->load(); };
        expectWithinAbsoluteError(raw(bm::id::oversampling), 1.0f, 1.0e-6f); // index 1 = 4x
        expectWithinAbsoluteError(raw(bm::id::bypass), 0.0f, 1.0e-6f);
        expectWithinAbsoluteError(raw(bm::id::hpf), 28.0f, 1.0e-4f);
        expectWithinAbsoluteError(raw(bm::id::width), 110.0f, 1.0e-4f);
        expectWithinAbsoluteError(raw(bm::id::trim), -0.2f, 1.0e-4f);
        expectWithinAbsoluteError(raw(bm::id::profile), 1.0f, 1.0e-6f); // profil "2"
        expect(p.getBypassParameter() == p.apvts.getParameter(bm::id::bypass), "bypass hote mal relie");
    }

    void testLiveOversampling()
    {
        beginTest("Oversampling modifie apres prepareToPlay, latence constante");
        for(double rate:{44100.,48000.,96000.,192000.}) {
            BloodyMasterProcessor switched,reference;
            auto setMode=[](BloodyMasterProcessor& p,float index) {
                auto* parameter=p.apvts.getParameter(bm::id::oversampling);
                parameter->setValueNotifyingHost(parameter->convertTo0to1(index));
            };
            setMode(switched,0); setMode(reference,2);
            switched.prepareToPlay(rate,512); reference.prepareToPlay(rate,512);
            const int latency=switched.getLatencySamples();
            setMode(switched,2);
            juce::MidiBuffer midi;
            for(int block=0;block<24;++block) {
                juce::AudioBuffer<float> a(2,512),b(2,512); a.clear(); b.clear();
                switched.processBlock(a,midi); reference.processBlock(b,midi);
                expectEquals(switched.getLatencySamples(),latency);
            }
            for(int block=0;block<12;++block) {
                juce::AudioBuffer<float> a(2,512),b(2,512);
                for(int c=0;c<2;++c) for(int i=0;i<512;++i) {
                    const float x=.2f*static_cast<float>(std::sin(2*juce::MathConstants<double>::pi*997*(block*512+i)/rate));
                    a.setSample(c,i,x); b.setSample(c,i,x);
                }
                switched.processBlock(a,midi); reference.processBlock(b,midi);
                for(int c=0;c<2;++c)
                    expect(std::memcmp(a.getReadPointer(c),b.getReadPointer(c),512*sizeof(float))==0);
            }
        }
    }

    void testFactoryPrograms()
    {
        beginTest("Presets : plages physiques, rappel et sauvegarde");
        BloodyMasterProcessor p;
        expectEquals(p.getNumPrograms(),9);
        for(int i=0;i<p.getNumPrograms();++i) {
            p.setCurrentProgram(i);
            expectEquals(p.getCurrentProgram(),i);
            expect(p.getProgramName(i).isNotEmpty());
            for(const auto& [id,value]:bm::presetParameters(bm::factoryPresets()[static_cast<size_t>(i)].settings)) {
                auto* parameter=p.apvts.getParameter(id);expect(parameter!=nullptr);
                if(parameter==nullptr) continue;
                const auto range=parameter->getNormalisableRange();
                expect(value>=range.start && value<=range.end,juce::String("preset hors plage: ")+id);
                expectWithinAbsoluteError(p.apvts.getRawParameterValue(id)->load(),value,1.e-4f);
            }
            juce::MemoryBlock state;p.getStateInformation(state);
            BloodyMasterProcessor restored;restored.setStateInformation(state.getData(),static_cast<int>(state.getSize()));
            expectEquals(restored.getCurrentProgram(),i);
        }
    }

    void testHostBypassAndMeters()
    {
        beginTest("Bypass de l'hote et mesures reelles");
        BloodyMasterProcessor p;p.prepareToPlay(48000,512);
        juce::AudioBuffer<float> input(2,4096),output(2,4096);
        juce::Random random(51);juce::MidiBuffer midi;
        for(int c=0;c<2;++c) for(int i=0;i<4096;++i) input.setSample(c,i,random.nextFloat()-.5f);
        for(int pos=0;pos<4096;pos+=512) {
            juce::AudioBuffer<float> block(2,512);
            for(int c=0;c<2;++c) block.copyFrom(c,0,input,c,pos,512);
            p.processBlockBypassed(block,midi);
            for(int c=0;c<2;++c) output.copyFrom(c,pos,block,c,0,512);
        }
        const int delay=p.getLatencySamples(),start=delay+240;
        for(int c=0;c<2;++c)
            expect(std::memcmp(input.getReadPointer(c)+start-delay,output.getReadPointer(c)+start,(4096-start)*sizeof(float))==0);
        const auto meters=p.getMeterValues();expect(meters.inputPeak>0 && meters.outputPeak>0);
    }
    void testEditor()
    {
        beginTest("Editeur : controles actifs, presets et rendu");
        BloodyMasterProcessor p;
        std::unique_ptr<juce::AudioProcessorEditor> editor(p.createEditor());
        expect(editor!=nullptr);if(!editor) return;
        expect(editor->getWidth()>0 && editor->getHeight()>0);
#if !BM_HTML_UI
        bool foundPresets=false,foundControls=false;
        for(auto* child:editor->getChildren()) {
            if(auto* combo=dynamic_cast<juce::ComboBox*>(child)) {
                if(combo->getNumItems()==9) {
                    combo->setSelectedId(4,juce::sendNotificationSync);
                    expectEquals(p.getCurrentProgram(),3);foundPresets=true;
                }
            }
            if(auto* viewport=dynamic_cast<juce::Viewport*>(child)) {
                expectEquals(viewport->getViewedComponent()->getNumChildComponents(),45);foundControls=true;
            }
        }
        expect(foundPresets && foundControls);
        const auto snapshot=editor->createComponentSnapshot(editor->getLocalBounds());
        expect(snapshot.isValid());
        const auto path=juce::SystemStats::getEnvironmentVariable("BM_EDITOR_SNAPSHOT",{});
        if(path.isNotEmpty()) {
            juce::FileOutputStream output{juce::File(path)};
            juce::PNGImageFormat png;expect(output.openedOk() && png.writeImageToStream(snapshot,output));
        }
#endif
    }

    void testState()
    {
        beginTest("Sauvegarde / restauration d'etat");
        BloodyMasterProcessor a;
        auto* hpf = a.apvts.getParameter(bm::id::hpf);
        auto* os = a.apvts.getParameter(bm::id::oversampling);
        hpf->setValueNotifyingHost(hpf->convertTo0to1(60.0f));
        os->setValueNotifyingHost(os->convertTo0to1(2.0f));

        juce::MemoryBlock mb;
        a.getStateInformation(mb);

        BloodyMasterProcessor b;
        b.setStateInformation(mb.getData(), (int) mb.getSize());
        expectWithinAbsoluteError(b.apvts.getRawParameterValue(bm::id::hpf)->load(), 60.0f, 1.0e-3f);
        expectEquals((int) b.apvts.getRawParameterValue(bm::id::oversampling)->load(), 2);

        b.setStateInformation("pas-du-xml", 10); // ne doit ni planter ni changer l'etat
        expectWithinAbsoluteError(b.apvts.getRawParameterValue(bm::id::hpf)->load(), 60.0f, 1.0e-3f);
    }
};
} // namespace

int main(int argc,char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
#if BM_HTML_UI
    if(argc>1 && juce::String(argv[1])=="--html-ui-smoke")return runWebUiSmoke(argc>2?juce::String(argv[2]):juce::String{});
#else
    juce::ignoreUnused(argc,argv);
#endif
    BloodyMasterTests tests;
    juce::UnitTestRunner runner;
    runner.setAssertOnFailure(false);
    runner.setPassesAreLogged(false);
    runner.runTestsInCategory("BloodyMaster");

    int failures = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
        failures += runner.getResult(i)->failures;

    if (runner.getNumResults() == 0)
        return 2; // aucun test execute = echec
    std::printf("JUCE plugin tests: %d groups, %d failures\n",runner.getNumResults(),failures);
    return failures == 0 ? 0 : 1;
}
