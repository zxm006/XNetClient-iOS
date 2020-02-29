#ifndef AUDIO_ENGINE_COMMON_TYPES_H
#define AUDIO_ENGINE_COMMON_TYPES_H

#ifdef AUDIO_ENGINE_EXPORT
#define AUDIO_ENGINE_DLLEXPORT _declspec(dllexport)
#elif AUDIO_ENGINE_DLL
#define AUDIO_ENGINE_DLLEXPORT _declspec(dllimport)
#else
#define AUDIO_ENGINE_DLLEXPORT extern
#endif

namespace me {

// Each codec supported can be described by this structure.
struct CodecInst
{
	int pltype;
	char plname[32];
	int plfreq;
	int pacsize;
	int channels;
	int rate;
};

enum NsModes    // type of Noise Suppression
{
	kNsUnchanged = 0,   // previously set mode
	kNsDefault,         // platform default
	kNsConference,      // conferencing default
	kNsLowSuppression,  // lowest suppression
	kNsModerateSuppression,
	kNsHighSuppression,
	kNsVeryHighSuppression,     // highest suppression
};

enum AgcModes                  // type of Automatic Gain Control
{
	kAgcUnchanged = 0,        // previously set mode
	kAgcDefault,              // platform default
	// adaptive mode for use when analog volume control exists (e.g. for
	// PC softphone)
	kAgcAdaptiveAnalog,
	// scaling takes place in the digital domain (e.g. for conference servers
	// and embedded devices)
	kAgcAdaptiveDigital,
	// can be used on embedded devices where the the capture signal is level
	// is predictable
	kAgcFixedDigital
};

// EC modes
enum EcModes                   // type of Echo Control
{
	kEcUnchanged = 0,          // previously set mode
	kEcDefault,                // platform default
	kEcConference,             // conferencing default (aggressive AEC)
	kEcAec,                    // Acoustic Echo Cancellation
	kEcAecm,                   // AEC mobile
};

// AECM modes
enum AecmModes                 // mode of AECM
{
	kAecmQuietEarpieceOrHeadset = 0,
	// Quiet earpiece or headset use
	kAecmEarpiece,             // most earpiece use
	kAecmLoudEarpiece,         // Loud earpiece or quiet speakerphone use
	kAecmSpeakerphone,         // most speakerphone use (default)
	kAecmLoudSpeakerphone      // Loud speakerphone
};

enum OnHoldModes            // On Hold direction
{
	kHoldSendAndPlay = 0,    // Put both sending and playing in on-hold state.
	kHoldSendOnly,           // Put only sending in on-hold state.
	kHoldPlayOnly            // Put only playing in on-hold state.
};


struct NetworkStatistics
{
	unsigned short fractionLost;
	unsigned int cumulativeLost;
	unsigned int extendedMax;
	unsigned int jitterSamples;
	int rttMs;
	int bytesSent;
	int packetsSent;
	int bytesReceived;
	int packetsReceived;
};


// External transport callback interface
class Transport
{
public:
    virtual int SendPacket(int channel, const void *data, int len) = 0;
    virtual int SendRTCPPacket(int channel, const void *data, int len) = 0;

protected:
    virtual ~Transport() {}
    Transport() {}
};

};
#endif // AUDIO_ENGINE_COMMON_TYPES_H