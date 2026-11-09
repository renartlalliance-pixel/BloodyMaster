#pragma once
// Identifiants de parametres Bloody Master (versionnes).
// NE JAMAIS renommer un ID publie : presets et automations en dependent.
namespace bm::id
{
    inline constexpr int kVersion = 1;
    inline constexpr const char* defreq = "bm_defreq";   // HTML: #knob-de-freq
    inline constexpr const char* deamount = "bm_deamount";   // HTML: #knob-de-amount
    inline constexpr const char* lowres = "bm_lowres";   // HTML: #knob-lowres
    inline constexpr const char* hires = "bm_hires";   // HTML: #knob-hires
    inline constexpr const char* hpf = "bm_hpf";   // HTML: #knob-hpf
    inline constexpr const char* lpf = "bm_lpf";   // HTML: #knob-lpf
    inline constexpr const char* xl = "bm_xl";   // HTML: #knob-xl
    inline constexpr const char* thd = "bm_thd";   // HTML: #knob-thd
    inline constexpr const char* bass = "bm_bass";   // HTML: #knob-bass
    inline constexpr const char* mid = "bm_mid";   // HTML: #knob-mid
    inline constexpr const char* high = "bm_high";   // HTML: #knob-high
    inline constexpr const char* presence = "bm_presence";   // HTML: #knob-presence
    inline constexpr const char* p1f = "bm_p1f";   // HTML: #knob-p1-f
    inline constexpr const char* p1g = "bm_p1g";   // HTML: #knob-p1-g
    inline constexpr const char* p2f = "bm_p2f";   // HTML: #knob-p2-f
    inline constexpr const char* p2g = "bm_p2g";   // HTML: #knob-p2-g
    inline constexpr const char* p3f = "bm_p3f";   // HTML: #knob-p3-f
    inline constexpr const char* p3g = "bm_p3g";   // HTML: #knob-p3-g
    inline constexpr const char* drive = "bm_drive";   // HTML: #knob-drive
    inline constexpr const char* link = "bm_link";   // HTML: #knob-link
    inline constexpr const char* mix = "bm_mix";   // HTML: #knob-mix
    inline constexpr const char* squash = "bm_squash";   // HTML: #knob-squash
    inline constexpr const char* recovery = "bm_recovery";   // HTML: #knob-recovery
    inline constexpr const char* glue = "bm_glue";   // HTML: #knob-glue
    inline constexpr const char* clipper = "bm_clipper";   // HTML: #knob-clipper
    inline constexpr const char* monomaker = "bm_monomaker";   // HTML: #knob-monomaker
    inline constexpr const char* width = "bm_width";   // HTML: #knob-width
    inline constexpr const char* enhance = "bm_enhance";   // HTML: #knob-enhance
    inline constexpr const char* pan = "bm_pan";   // HTML: #knob-pan
    inline constexpr const char* foundation = "bm_foundation";   // HTML: #knob-foundation
    inline constexpr const char* trim = "bm_trim";   // HTML: #knob-trim
    inline constexpr const char* turbo = "bm_turbo";   // HTML: #knob-turbo
    inline constexpr const char* limrel = "bm_limrel";   // HTML: #knob-limrel
    inline constexpr const char* limlink = "bm_limlink";   // HTML: #knob-limlink
    inline constexpr const char* bypass = "bm_bypass";
    inline constexpr const char* valve = "bm_valve";
    inline constexpr const char* isp = "bm_isp";
    inline constexpr const char* eqActive = "bm_eq_active";
    inline constexpr const char* spikeListen = "bm_spike_listen";
    inline constexpr const char* profile = "bm_profile";
    inline constexpr const char* oversampling = "bm_oversampling";
    inline constexpr const char* lufsTarget = "bm_lufs_target";
}
