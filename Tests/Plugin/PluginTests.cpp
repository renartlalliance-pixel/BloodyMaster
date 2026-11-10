// Tests du plugin. Le code retour est non nul en cas d'echec ou si aucun test ne tourne.
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <cmath>
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
        testProcessing();
        testFilterResponse();
        testBypass();
        testParameters();
        testEditor();
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

    void testProcessing()
    {
        beginTest("Traitement stable et limite (mono/stereo, SR et tailles de bloc varies)");
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

                    for (int ch = 0; ch < nch; ++ch)
                    {
                        bool valid = true;
                        for (int i = 0; i < bs; ++i)
                        {
                            const auto sample = buf.getSample(ch, i);
                            valid = valid && std::isfinite(sample) && std::abs(sample) <= 1.0f;
                        }
                        expect(valid, "sortie non finie ou hors plage");
                    }
                }
            }
        }
    }

    void testFilterResponse()
    {
        beginTest("Le passe-haut modifie effectivement le signal");
        BloodyMasterProcessor p;
        expect(p.setBusesLayout(layoutOf(AudioChannelSet::mono(), AudioChannelSet::mono())));
        p.setRateAndBufferSizeDetails(48000.0, 512);
        p.prepareToPlay(48000.0, 512);
        auto set = [&p](const char* id, float value)
        {
            auto* parameter = p.apvts.getParameter(id);
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
        };
        set(bm::id::hpf, 100.0f);
        set(bm::id::valve, 0.0f);
        set(bm::id::drive, 0.0f);
        set(bm::id::clipper, 0.0f);
        set(bm::id::turbo, 0.0f);
        set(bm::id::mix, 100.0f);

        constexpr int sampleCount = 24000;
        juce::AudioBuffer<float> filtered(1, sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            filtered.setSample(0, i, 0.1f * std::sin(2.0f * juce::MathConstants<float>::pi * 40.0f * i / 48000.0f));
        juce::MidiBuffer midi;
        p.processBlock(filtered, midi);

        double filteredEnergy = 0.0;
        for (int i = sampleCount / 2; i < sampleCount; ++i)
            filteredEnergy += std::pow(filtered.getSample(0, i), 2.0);
        const auto filteredRms = std::sqrt(filteredEnergy / (sampleCount / 2));
        expect(filteredRms < 0.04, "100 Hz passe-haut doit attenuer une tonalite a 40 Hz");

        set(bm::id::eqActive, 0.0f);
        p.prepareToPlay(48000.0, 512);
        juce::AudioBuffer<float> unfiltered(1, sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            unfiltered.setSample(0, i, 0.1f * std::sin(2.0f * juce::MathConstants<float>::pi * 40.0f * i / 48000.0f));
        p.processBlock(unfiltered, midi);
        double unfilteredEnergy = 0.0;
        for (int i = sampleCount / 2; i < sampleCount; ++i)
            unfilteredEnergy += std::pow(unfiltered.getSample(0, i), 2.0);
        const auto unfilteredRms = std::sqrt(unfilteredEnergy / (sampleCount / 2));
        expect(unfilteredRms > filteredRms * 2.0, "desactiver l'EQ doit retablir le grave");
    }

    void testBypass()
    {
        beginTest("Bypass bit-exact");
        BloodyMasterProcessor p;
        p.setRateAndBufferSizeDetails(48000.0, 128);
        p.prepareToPlay(48000.0, 128);
        p.apvts.getParameter(bm::id::bypass)->setValueNotifyingHost(1.0f);
        juce::AudioBuffer<float> buffer(2, 128), reference(2, 128);
        juce::Random random(654321);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                buffer.setSample(ch, i, random.nextFloat() * 2.0f - 1.0f);
        reference.makeCopyOf(buffer);
        juce::MidiBuffer midi;
        p.processBlock(buffer, midi);
        for (int ch = 0; ch < 2; ++ch)
            expect(std::memcmp(reference.getReadPointer(ch), buffer.getReadPointer(ch),
                               sizeof(float) * static_cast<size_t>(buffer.getNumSamples())) == 0,
                   "le bypass doit conserver les echantillons d'origine");
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

    void testEditor()
    {
        beginTest("Creation de l'editeur natif et de ses controles");
        BloodyMasterProcessor p;
        auto* editor = p.createEditor();
        expect(editor != nullptr, "editeur non cree");
        if (editor != nullptr)
        {
            expect(editor->getWidth() >= 850, "taille minimale de l'editeur");
            delete editor;
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
