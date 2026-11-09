// Tests M1 du plugin. Critere global : code retour 0 si et seulement si aucun test n'echoue ET au moins un test a tourne.
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <cstring>
#include <set>
#include "PluginProcessor.h"
#include "Parameters/ParameterIDs.h"

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
    BloodyMasterTests() : juce::UnitTest("Bloody Master M1", "BloodyMaster") {}

    void runTest() override
    {
        testLayouts();
        testPassthrough();
        testParameters();
        testDefaults();
        testState();
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
        beginTest("Passthrough bit-exact (mono/stereo, SR et tailles de bloc varies)");
        const std::array<AudioChannelSet, 2> sets { AudioChannelSet::mono(), AudioChannelSet::stereo() };
        const std::array<double, 4> rates { 44100.0, 48000.0, 96000.0, 192000.0 };
        const std::array<int, 5> blocks { 1, 32, 480, 1024, 4096 };
        juce::Random rng(12345);

        for (const auto& set : sets)
        {
            BloodyMasterProcessor p;
            expect(p.setBusesLayout(layoutOf(set, set)));
            const int nch = set.size();

            for (const double sr : rates)
            {
                p.setRateAndBufferSizeDetails(sr, 1024);
                p.prepareToPlay(sr, 1024);
                expectEquals(p.getLatencySamples(), 0);

                for (const int bs : blocks)
                {
                    juce::AudioBuffer<float> buf(nch, bs), ref(nch, bs);
                    for (int ch = 0; ch < nch; ++ch)
                        for (int i = 0; i < bs; ++i)
                            buf.setSample(ch, i, rng.nextFloat() * 2.0f - 1.0f);
                    ref.makeCopyOf(buf);

                    juce::MidiBuffer midi;
                    p.processBlock(buf, midi);

                    bool same = true;
                    for (int ch = 0; ch < nch; ++ch)
                        same = same && std::memcmp(ref.getReadPointer(ch), buf.getReadPointer(ch), sizeof(float) * (size_t) bs) == 0;
                    expect(same, "sortie differente de l'entree : canaux=" + juce::String(nch) + " sr=" + juce::String(sr)
                                     + " bloc=" + juce::String(bs));
                }
            }
        }
    }

    void testParameters()
    {
        beginTest("Parametres : nombre, IDs uniques/versionnes, automatisables");
        BloodyMasterProcessor p;
        const auto& params = p.getParameters();
        expectEquals(params.size(), 42); // 34 float + 5 bool + 3 choix

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

int main()
{
    juce::ScopedJuceInitialiser_GUI init;
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
    return failures == 0 ? 0 : 1;
}
