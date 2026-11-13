#include "DSP/Engine.h"
#include "Presets/FactoryPresets.h"
#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstring>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#endif
namespace fs=std::filesystem;
using namespace bm::dsp;
std::string utf8(const fs::path& p) { auto s=p.u8string(); return std::string(s.begin(),s.end()); }
uint16_t u16(const unsigned char* p) { return uint16_t(p[0])|uint16_t(p[1])<<8; }
uint32_t u32(const unsigned char* p) { return uint32_t(p[0])|uint32_t(p[1])<<8|uint32_t(p[2])<<16|uint32_t(p[3])<<24; }
struct Audio { int rate=0,channels=0; std::vector<std::vector<float>> data; };
Audio readWave(const fs::path& path) {
    std::ifstream f(path,std::ios::binary|std::ios::ate);
    if(!f) throw std::runtime_error("Impossible d'ouvrir le fichier.");
    const auto length=f.tellg();
    if(length<44 || length>std::streamoff(1024ULL*1024*1024)) throw std::runtime_error("WAV invalide ou fichier de plus de 1 Go.");
    std::vector<unsigned char> b(static_cast<size_t>(length)); f.seekg(0); f.read(reinterpret_cast<char*>(b.data()),length);
    if(!f || std::memcmp(b.data(),"RIFF",4) || std::memcmp(b.data()+8,"WAVE",4)) throw std::runtime_error("Format attendu: WAV RIFF PCM ou float, mono/stereo.");
    if(uint64_t(u32(b.data()+4))+8!=b.size()) throw std::runtime_error("Taille RIFF incoherente.");
    const unsigned char* fmt=nullptr; size_t fmtSize=0,dataOffset=0,dataSize=0;
    for(size_t p=12;p<b.size();) {
        if(b.size()-p<8) throw std::runtime_error("En-tete de chunk WAV tronque.");
        const size_t size=u32(b.data()+p+4),start=p+8;
        if(size>b.size()-start || (size&1)>b.size()-start-size) throw std::runtime_error("Chunk WAV tronque.");
        if(!std::memcmp(b.data()+p,"fmt ",4)) {
            if(fmt) throw std::runtime_error("Plusieurs chunks fmt WAV.");
            fmt=b.data()+start; fmtSize=size;
        }
        if(!std::memcmp(b.data()+p,"data",4)) {
            if(dataOffset) throw std::runtime_error("Plusieurs chunks data WAV.");
            dataOffset=start; dataSize=size;
        }
        p=start+size+(size&1);
    }
    if(!fmt || fmtSize<16 || !dataOffset) throw std::runtime_error("WAV sans fmt/data valide.");
    int format=u16(fmt),channels=u16(fmt+2),rate=static_cast<int>(u32(fmt+4)),align=u16(fmt+12),bits=u16(fmt+14);
    if(format==65534) {
        if(fmtSize<40 || u16(fmt+16)<22 || u16(fmt+18)!=bits) throw std::runtime_error("WAV extensible: precision non prise en charge.");
        static const unsigned char tail[]{0,0,0x10,0,0x80,0,0,0xaa,0,0x38,0x9b,0x71};
        if(std::memcmp(fmt+28,tail,12)) throw std::runtime_error("WAV extensible: codec non reconnu.");
        format=static_cast<int>(u32(fmt+24));
    }
    if(channels<1 || channels>2 || rate<8000 || rate>192000) throw std::runtime_error("Utiliser un WAV mono/stereo, 8 a 192 kHz.");
    if(!((format==1&&(bits==16||bits==24||bits==32)) || (format==3&&(bits==32||bits==64)))) throw std::runtime_error("PCM 16/24/32 ou float 32/64 requis.");
    if(align!=channels*(bits/8) || dataSize%align) throw std::runtime_error("Alignement WAV invalide.");
    Audio a; a.rate=rate;a.channels=channels; const size_t frames=dataSize/align;
    a.data.assign(channels,std::vector<float>(frames));
    for(size_t i=0;i<frames;++i) for(int c=0;c<channels;++c) {
        const auto* p=b.data()+dataOffset+i*align+c*(bits/8); double x=0;
        if(format==1) {
            if(bits==16) x=std::bit_cast<int16_t>(u16(p))/32768.;
            if(bits==24) { uint32_t v=uint32_t(p[0])|uint32_t(p[1])<<8|uint32_t(p[2])<<16; if(v&0x800000) v|=0xff000000; x=std::bit_cast<int32_t>(v)/8388608.; }
            if(bits==32) x=std::bit_cast<int32_t>(u32(p))/2147483648.;
        } else if(bits==32) x=std::bit_cast<float>(u32(p));
        else { uint64_t v=uint64_t(u32(p))|(uint64_t(u32(p+4))<<32); x=std::bit_cast<double>(v); }
        if(!std::isfinite(x) || std::abs(x)>1.e6) throw std::runtime_error("WAV contenant des valeurs audio invalides.");
        a.data[c][i]=static_cast<float>(x);
    }
    return a;
}
void write16(std::ostream& f,uint16_t v) { char b[]{char(v),char(v>>8)}; f.write(b,2); }
void write32(std::ostream& f,uint32_t v) { char b[]{char(v),char(v>>8),char(v>>16),char(v>>24)}; f.write(b,4); }
void writeWave(const fs::path& p,const Audio& a) {
    if(fs::exists(p)) throw std::runtime_error("Le fichier de sortie existe deja: aucune ecriture.");
    const uint64_t bytes=uint64_t(a.data[0].size())*a.channels*4;
    if(bytes>0xffffffffULL-48) throw std::runtime_error("Sortie WAV trop grande.");
    std::ofstream f(p,std::ios::binary); if(!f) throw std::runtime_error("Impossible de creer la sortie.");
    f.write("RIFF",4); write32(f,static_cast<uint32_t>(bytes)+48); f.write("WAVEfmt ",8); write32(f,16);
    write16(f,3);write16(f,static_cast<uint16_t>(a.channels));write32(f,a.rate);write32(f,a.rate*a.channels*4);
    write16(f,static_cast<uint16_t>(a.channels*4));write16(f,32);
    f.write("fact",4);write32(f,4);write32(f,static_cast<uint32_t>(a.data[0].size()));
    f.write("data",4);write32(f,static_cast<uint32_t>(bytes));
    for(size_t i=0;i<a.data[0].size();++i) for(int c=0;c<a.channels;++c) write32(f,std::bit_cast<uint32_t>(a.data[c][i]));
    if(!f) throw std::runtime_error("Erreur d'ecriture WAV (espace disque ?).");
}
int run(std::vector<std::string> args) {
    if(args.size()>1 && args[1]=="--presets-json") {
        std::cout<<"[\n";
        bool first=true;
        for(const auto& p:bm::factoryPresets()) {
            if(!first) std::cout<<",\n";
            first=false;
            std::cout<<"  {\"slug\":\""<<p.slug<<"\",\"name\":\""<<p.name<<"\",\"category\":\""<<p.category<<"\",\"parameters\":{";
            bool parameterFirst=true;
            for(const auto& [id,value]:bm::presetParameters(p.settings)) {
                if(!parameterFirst) std::cout<<',';
                parameterFirst=false;
                std::cout<<'"'<<id<<"\":"<<value;
            }
            std::cout<<"}}";
        }
        std::cout<<"\n]\n"; return 0;
    }
    if(args.size()>1 && args[1]=="--list-presets") { for(const auto& p:bm::factoryPresets()) std::cout<<p.slug<<" : "<<p.name<<'\n'; return 0; }
    if(args.size()<2 || args[1]=="--help") { std::cout<<"BloodyMaster M3 test WAV\nUsage: BloodyMasterTest.exe fichier.wav --preset nom (voir --list-presets) ou bypass\nOptions: --drive 0..12 --clip 0..10 --push 0..6 --ceiling -6..0 --os 2|4|8\n";return 0; }
    const fs::path input=fs::path(reinterpret_cast<const char8_t*>(args[1].c_str())); std::string preset="clean"; int os=8;
    for(size_t i=2;i<args.size();i+=2) { if(i+1>=args.size()) throw std::runtime_error("Valeur d'option manquante."); if(args[i]=="--preset") preset=args[i+1]; }
    EngineSettings s;
    if(preset=="bypass") s.bypass=true;
    else if(const auto* factory=bm::findPreset(preset)) s=factory->settings;
    else throw std::runtime_error("Preset inconnu. Utiliser --list-presets.");
    for(size_t i=2;i<args.size();i+=2) {
        const auto& key=args[i]; if(key=="--preset") continue;
        size_t used=0; const double value=std::stod(args[i+1],&used);
        if(used!=args[i+1].size() || !std::isfinite(value)) throw std::runtime_error("Valeur numerique invalide.");
        const auto range=[&](double lo,double hi){ if(value<lo || value>hi) throw std::runtime_error("Valeur hors plage: "+key); };
        if(key=="--drive") { range(0,12);s.dynamics.drive=value; }
        else if(key=="--clip") { range(0,10);s.peaks.clipDb=value; }
        else if(key=="--push") { range(0,6);s.peaks.pushDb=value; }
        else if(key=="--ceiling") { range(-6,0);s.peaks.ceilingDb=value; }
        else if(key=="--os") { if(value!=2 && value!=4 && value!=8) throw std::runtime_error("OS: 2, 4 ou 8."); os=static_cast<int>(value); }
        else if(key=="--bass") { range(-6,6); s.tone.bass=value; }
        else if(key=="--high") { range(-6,6); s.tone.high=value; }
        else if(key=="--air") { range(-6,6); s.tone.presence=value; }
        else if(key=="--width") { range(0,200); s.tone.width=value; }
        else if(key=="--mono-bass") { range(0,400); s.tone.monoBass=value; }
        else if(key=="--dynamics-off") { range(0,1); s.dynamics.enabled=value==0; }
        else if(key=="--limiter-off") { range(0,1); s.peaks.limiterEnabled=value==0; }
        else throw std::runtime_error("Option inconnue: "+key);
    }
    s.oversampling=os;
    Audio a=readWave(input); const size_t frames=a.data[0].size();
    Engine engine;engine.prepare(a.rate,os);engine.update(s); const int latency=engine.latency();
    for(auto& c:a.data) c.resize(frames+latency,0);
    MeterValues meter{};
    std::cout<<"Traitement "<<preset<<" / "<<a.rate<<" Hz / "<<a.channels<<" canaux / "<<os<<"x...\n";
    for(size_t pos=0;pos<frames+latency;pos+=512) {
        float* p[]{a.data[0].data()+pos,a.channels==2?a.data[1].data()+pos:nullptr};
        engine.process(p,a.channels,static_cast<int>(std::min(size_t(512),frames+latency-pos)));
        meter.inputPeak=std::max(meter.inputPeak,engine.meters.inputPeak);meter.outputPeak=std::max(meter.outputPeak,engine.meters.outputPeak);
        meter.compressorGr=std::max(meter.compressorGr,engine.meters.compressorGr);meter.glueGr=std::max(meter.glueGr,engine.meters.glueGr);
        meter.clipGr=std::max(meter.clipGr,engine.meters.clipGr);meter.limiterGr=std::max(meter.limiterGr,engine.meters.limiterGr);
    }
    for(auto& c:a.data) c.erase(c.begin(),c.begin()+latency);
    fs::path output;int suffix=0;
    do { std::string name=utf8(input.stem())+"_BloodyMaster_"+preset+(suffix?"_"+std::to_string(suffix):"")+".wav";output=input.parent_path()/fs::path(reinterpret_cast<const char8_t*>(name.c_str()));++suffix; } while(fs::exists(output));
    writeWave(output,a);
    std::ostringstream report;
    report<<std::fixed<<std::setprecision(3)<<"BloodyMaster M3 alpha - test WAV\nPreset: "<<preset<<"\nSample rate: "<<a.rate<<"\nChannels: "<<a.channels<<"\nFrames: "<<frames<<"\nLatency compensated: "<<latency<<" samples\nOversampling: "<<os<<"x\nInput sample peak: "<<gainDb(meter.inputPeak)<<" dBFS\nOutput sample peak: "<<gainDb(meter.outputPeak)<<" dBFS\nCompressor maximum GR: "<<meter.compressorGr<<" dB\nGlue maximum GR: "<<meter.glueGr<<" dB\nClipper maximum reduction: "<<meter.clipGr<<" dB\nLimiter maximum GR: "<<meter.limiterGr<<" dB\nPeak values above are SAMPLE peaks, not LUFS or true-peak measurements.\nWAV output: float32, original rate/channels/frame count. Source not modified.\n";
    auto log=output;log+=".txt"; if(!fs::exists(log)) {std::ofstream f(log);f<<report.str();}
    std::cout<<report.str()<<"\nSortie: "<<utf8(output)<<'\n';return 0;
}
#ifdef _WIN32
std::string narrow(const wchar_t* s) {
    int size=WideCharToMultiByte(CP_UTF8,0,s,-1,nullptr,0,nullptr,nullptr);
    std::string result(size,'\0');WideCharToMultiByte(CP_UTF8,0,s,-1,result.data(),size,nullptr,nullptr);result.pop_back();return result;
}
int wmain(int argc,wchar_t** argv) {
    bool interactive=argc==1; int result=0;
    try {
        std::vector<std::string> args;for(int i=0;i<argc;++i) args.push_back(narrow(argv[i]));
        if(interactive) {
            std::vector<wchar_t> name(32768);OPENFILENAMEW ofn{};ofn.lStructSize=sizeof(ofn);ofn.lpstrFilter=L"WAV audio\0*.wav\0\0";ofn.lpstrFile=name.data();ofn.nMaxFile=static_cast<DWORD>(name.size());ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;ofn.lpstrTitle=L"BloodyMaster - choisir un WAV";
            if(!GetOpenFileNameW(&ofn)) return 0;
            args.push_back(narrow(name.data()));
            std::cout<<"BloodyMaster M3 - test audio\n";
            const auto& bank=bm::factoryPresets();
            for(size_t i=0;i<bank.size();++i) std::cout<<i+1<<". "<<bank[i].name<<'\n';
            std::cout<<"Choix [1] : ";
            std::string choice;std::getline(std::cin,choice);
            size_t selected=0;
            if(!choice.empty()) {
                size_t used=0;const int value=std::stoi(choice,&used);
                if(used!=choice.size() || value<1 || value>static_cast<int>(bank.size())) throw std::runtime_error("Choix de preset invalide.");
                selected=static_cast<size_t>(value-1);
            }
            args.push_back("--preset");args.push_back(bank[selected].slug);
        }
        result=run(args);
    } catch(const std::exception& e) { std::cerr<<"ERREUR: "<<e.what()<<'\n';result=1; }
    if(interactive) { std::cout<<"\nAppuyer sur Entree pour fermer.";std::string line;std::getline(std::cin,line); }
    return result;
}
#else
int main(int argc,char** argv) { try { return run({argv,argv+argc}); } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; } }
#endif
