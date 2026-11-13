#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace bm {
// Message-thread-only repository. UUID filenames never use the display name.
// All parameters are validated before a recall can mutate APVTS.
class UserPresets {
public:
    struct Entry { juce::String id,name; std::vector<std::pair<juce::String,float>> parameters; };
    explicit UserPresets(juce::File directory=defaultDirectory()):folder(std::move(directory)) {}
    static juce::File defaultDirectory() {
        return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile("BRI Audio Systems/BloodyMaster/User Presets");
    }
    const std::vector<Entry>& entries() const { return bank; }
    juce::Result refresh(juce::AudioProcessorValueTreeState& state) {
        jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
        bank.clear();
        if(!folder.exists())return juce::Result::ok();
        if(!folder.isDirectory())return juce::Result::fail("Le dossier User est inaccessible.");
        int invalid=0;
        for(const auto& file:folder.findChildFiles(juce::File::findFiles,false,"*.bmpreset")) {
            Entry entry;
            if(file.getSize()>131072 || !decode(file.loadFileAsString(),state,entry)){++invalid;continue;}
            entry.id=file.getFileNameWithoutExtension();bank.push_back(std::move(entry));
        }
        std::sort(bank.begin(),bank.end(),[](const Entry& a,const Entry& b){
            const int order=a.name.compareIgnoreCase(b.name);return order==0?a.id<b.id:order<0;
        });
        return invalid?juce::Result::fail(juce::String(invalid)+" preset(s) User invalide(s) ignoré(s)."):juce::Result::ok();
    }
    juce::Result save(juce::String name,juce::AudioProcessorValueTreeState& state,juce::String& savedId) {
        jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
        name=name.trim();
        if(name.isEmpty()||name.length()>80 || name.containsAnyOf("\r\n\t"))
            return juce::Result::fail("Choisir un nom de 1 à 80 caractères, sur une seule ligne.");
        const auto creation=folder.createDirectory();if(creation.failed())return juce::Result::fail("Impossible de créer le dossier User : "+creation.getErrorMessage());
        (void)refresh(state);
        const auto original=name;int suffix=2;
        while(std::any_of(bank.begin(),bank.end(),[&](const Entry& e){return e.name.equalsIgnoreCase(name);}))name=original+" ("+juce::String(suffix++)+")";
        auto* root=new juce::DynamicObject;juce::var document(root);
        root->setProperty("format","BloodyMasterUserPreset");root->setProperty("version",1);root->setProperty("name",name);
        auto* parameters=new juce::DynamicObject;root->setProperty("parameters",juce::var(parameters));
        for(auto* base:state.processor.getParameters())if(auto* p=dynamic_cast<juce::RangedAudioParameter*>(base)) {
            const float value=state.getRawParameterValue(p->getParameterID())->load(std::memory_order_relaxed);
            if(!std::isfinite(value))return juce::Result::fail("Un réglage est invalide ; preset non enregistré.");
            parameters->setProperty(p->getParameterID(),value);
        }
        const auto id=juce::Uuid().toString();const auto target=folder.getChildFile(id+".bmpreset");
        juce::TemporaryFile temporary(target);
        if(!temporary.getFile().replaceWithText(juce::JSON::toString(document)) || !temporary.overwriteTargetFileWithTemporary())
            return juce::Result::fail("Échec de l'enregistrement User (droits ou espace disque).");
        savedId=id;(void)refresh(state);return juce::Result::ok();
    }
    juce::Result load(const juce::String& id,juce::AudioProcessorValueTreeState& state) const {
        jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
        const auto found=std::find_if(bank.begin(),bank.end(),[&](const Entry& e){return e.id==id;});
        if(found==bank.end())return juce::Result::fail("Preset User introuvable. Actualiser la liste.");
        // Revalidate the on-disk file; deleted/corrupt files never silently recall
        // stale cached data or partially apply a state.
        const auto file=folder.getChildFile(found->id+".bmpreset");Entry entry;
        if(!file.existsAsFile()||file.getSize()>131072||!decode(file.loadFileAsString(),state,entry))
            return juce::Result::fail("Preset User illisible ou invalide ; réglages conservés.");
        for(const auto& [parameterId,value]:entry.parameters)if(auto* p=state.getParameter(parameterId)) {
            p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(value));p->endChangeGesture();
        }
        return juce::Result::ok();
    }
    static bool matches(const Entry& e,juce::AudioProcessorValueTreeState& state) {
        for(const auto& [id,value]:e.parameters) {
            const auto* raw=state.getRawParameterValue(id);
            if(raw==nullptr||std::abs(raw->load(std::memory_order_relaxed)-value)>1.e-4f)return false;
        }
        return true;
    }
private:
    juce::File folder;
    std::vector<Entry> bank;
    static bool decode(const juce::String& json,juce::AudioProcessorValueTreeState& state,Entry& entry) {
        const auto doc=juce::JSON::parse(json);
        if(!doc.isObject()||doc["format"].toString()!="BloodyMasterUserPreset"||!doc["version"].isInt()||int(doc["version"])!=1)return false;
        const auto name=doc["name"];
        if(!name.isString()||name.toString().trim().isEmpty()||name.toString().length()>96||name.toString().containsAnyOf("\r\n\t"))return false;
        auto* parameters=doc["parameters"].getDynamicObject();if(parameters==nullptr)return false;
        int expected=0;
        for(auto* base:state.processor.getParameters())if(auto* p=dynamic_cast<juce::RangedAudioParameter*>(base)) {
            ++expected;const auto id=p->getParameterID();const auto value=parameters->getProperty(id);
            if(!(value.isInt()||value.isInt64()||value.isDouble()))return false;
            const double physical=static_cast<double>(value);const auto range=p->getNormalisableRange();
            if(!std::isfinite(physical)||physical<range.start||physical>range.end)return false;
            const auto snapped=range.snapToLegalValue(static_cast<float>(physical));
            if(std::abs(snapped-physical)>1.e-3)return false;
            entry.parameters.emplace_back(id,snapped);
        }
        if(parameters->getProperties().size()!=expected)return false;
        entry.name=name.toString();return true;
    }
};
}
