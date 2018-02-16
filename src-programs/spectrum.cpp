//
// Programmer:    Ziemowit Laski <zlaski@ziemas.net>
// Creation Date: Tue, 06 Feb 2018 15:01:47 -0800
// Filename:      midifile/src-programs/spectrum.cpp
// Syntax:        C++11
//
// Description:   Performs a Fourier analysis of the input
//                source file and writes the resulting
//                spectrum to a MIDI file, with key velocities
//                representing amplitude.  The noise 
//                threshold and gain can be adjusted for
//                better visualization and/or playback.
//
//                The MIDI file will maintain the tempo
//                of the original signal, but will not
//                contain any time signature or BPM
//                information.

#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <vector>
#include <deque>
#include <complex>
#include <numeric>
#include <utility>
#include <cmath>
#include <algorithm>

#include "SoundFile.h"
#include "MidiFile.h"
#include "MidiData.h"
#include "Kernel.h"

struct Cmplx {
	float re, im;
};

// 88-key piano
static const unsigned gMinNote = MIDI::note::A0;
static const unsigned gMaxNote = MIDI::note::C8;
static const unsigned gPianoNotes = gMaxNote - gMinNote + 1;
static unsigned gLowNote = MIDI::note::A0;
static unsigned gHighNote = MIDI::note::C8;
static unsigned gSampleRate;
static double gSampleRateDouble;
static unsigned gStartSample;
static unsigned gEndSample;
static unsigned gNumSamples;
static unsigned gNumChannels;
static SoundFileRead gSoundFile;
static MidiFile gMidiFile;
static FILE *gMeshFile; // mesh
static float gNoiseDecibels = - FLT_MAX;  // basically -infinity
static int gNoise = 0;
static float gExponent = 0.4f;
static float gGain = 1.f;
static float gCompression = 10.f;
static int gMakeMesh;
static const char *gMaterialsName = "_elev.mtl";
static std::string gOutputFileName;
static Kernel gKernel(16384);
static float gMinDuration = 0.05f;  
static float gMinDurationInTicks;

#define _clamp(v, l, h) std::min(std::max(v, l), h)
#define _valueat(v, i) (i < 0? v[v.size() + i]: v[i])


// The following materials (colors) are used to construct 3D
// spectrum meshes.
static char gMaterials[] = {
	"# Elevation colors for .OBJ (Alias/Wavefront) meshes.\n"
	"# Each color corresponds to a MIDI velocity.\n"
	"# DO NOT EDIT\n"
	"\n"
	"newmtl Grey\n"
	"Kd 0.98	0.98	0.98\n"
	"newmtl Elev00\n"
	"Kd 0.643137255	0	1\n"
	"newmtl Elev01\n"
	"Kd 0.635294118	0.003921569	1\n"
	"newmtl Elev02\n"
	"Kd 0.631372549	0.011764706	1\n"
	"newmtl Elev03\n"
	"Kd 0.623529412	0.019607843	1\n"
	"newmtl Elev04\n"
	"Kd 0.615686275	0.035294118	1\n"
	"newmtl Elev05\n"
	"Kd 0.611764706	0.043137255	1\n"
	"newmtl Elev06\n"
	"Kd 0.607843137	0.047058824	1\n"
	"newmtl Elev07\n"
	"Kd 0.6	0.050980392	1\n"
	"newmtl Elev08\n"
	"Kd 0.576470588	0.058823529	1\n"
	"newmtl Elev09\n"
	"Kd 0.552941176	0.062745098	1\n"
	"newmtl Elev0A\n"
	"Kd 0.533333333	0.066666667	1\n"
	"newmtl Elev0B\n"
	"Kd 0.505882353	0.090196078	1\n"
	"newmtl Elev0C\n"
	"Kd 0.482352941	0.11372549	1\n"
	"newmtl Elev0D\n"
	"Kd 0.466666667	0.133333333	1\n"
	"newmtl Elev0E\n"
	"Kd 0.439215686	0.152941176	1\n"
	"newmtl Elev0F\n"
	"Kd 0.415686275	0.17254902	1\n"
	"newmtl Elev10\n"
	"Kd 0.4	0.2	1\n"
	"newmtl Elev11\n"
	"Kd 0.37254902	0.223529412	1\n"
	"newmtl Elev12\n"
	"Kd 0.352941176	0.239215686	1\n"
	"newmtl Elev13\n"
	"Kd 0.333333333	0.266666667	1\n"
	"newmtl Elev14\n"
	"Kd 0.305882353	0.290196078	1\n"
	"newmtl Elev15\n"
	"Kd 0.28627451	0.31372549	1\n"
	"newmtl Elev16\n"
	"Kd 0.266666667	0.333333333	1\n"
	"newmtl Elev17\n"
	"Kd 0.239215686	0.360784314	1\n"
	"newmtl Elev18\n"
	"Kd 0.223529412	0.380392157	1\n"
	"newmtl Elev19\n"
	"Kd 0.2	0.4	1\n"
	"newmtl Elev1A\n"
	"Kd 0.180392157	0.419607843	1\n"
	"newmtl Elev1B\n"
	"Kd 0.156862745	0.443137255	1\n"
	"newmtl Elev1C\n"
	"Kd 0.133333333	0.466666667	1\n"
	"newmtl Elev1D\n"
	"Kd 0.109803922	0.482352941	1\n"
	"newmtl Elev1E\n"
	"Kd 0.082352941	0.509803922	1\n"
	"newmtl Elev1F\n"
	"Kd 0.066666667	0.533333333	1\n"
	"newmtl Elev20\n"
	"Kd 0.043137255	0.552941176	1\n"
	"newmtl Elev21\n"
	"Kd 0.023529412	0.580392157	1\n"
	"newmtl Elev22\n"
	"Kd 0	0.6	1\n"
	"newmtl Elev23\n"
	"Kd 0.035294118	0.615686275	1\n"
	"newmtl Elev24\n"
	"Kd 0.070588235	0.62745098	1\n"
	"newmtl Elev25\n"
	"Kd 0.105882353	0.643137255	1\n"
	"newmtl Elev26\n"
	"Kd 0.141176471	0.654901961	1\n"
	"newmtl Elev27\n"
	"Kd 0.176470588	0.670588235	1\n"
	"newmtl Elev28\n"
	"Kd 0.211764706	0.68627451	1\n"
	"newmtl Elev29\n"
	"Kd 0.247058824	0.698039216	1\n"
	"newmtl Elev2A\n"
	"Kd 0.282352941	0.717647059	1\n"
	"newmtl Elev2B\n"
	"Kd 0.317647059	0.729411765	1\n"
	"newmtl Elev2C\n"
	"Kd 0.352941176	0.745098039	1\n"
	"newmtl Elev2D\n"
	"Kd 0.388235294	0.756862745	1\n"
	"newmtl Elev2E\n"
	"Kd 0.423529412	0.77254902	1\n"
	"newmtl Elev2F\n"
	"Kd 0.454901961	0.788235294	1\n"
	"newmtl Elev30\n"
	"Kd 0.490196078	0.803921569	1\n"
	"newmtl Elev31\n"
	"Kd 0.525490196	0.815686275	1\n"
	"newmtl Elev32\n"
	"Kd 0.560784314	0.82745098	1\n"
	"newmtl Elev33\n"
	"Kd 0.596078431	0.847058824	1\n"
	"newmtl Elev34\n"
	"Kd 0.631372549	0.858823529	1\n"
	"newmtl Elev35\n"
	"Kd 0.666666667	0.870588235	1\n"
	"newmtl Elev36\n"
	"Kd 0.701960784	0.88627451	1\n"
	"newmtl Elev37\n"
	"Kd 0.737254902	0.901960784	1\n"
	"newmtl Elev38\n"
	"Kd 0.77254902	0.917647059	1\n"
	"newmtl Elev39\n"
	"Kd 0.807843137	0.929411765	1\n"
	"newmtl Elev3A\n"
	"Kd 0.843137255	0.945098039	1\n"
	"newmtl Elev3B\n"
	"Kd 0.874509804	0.960784314	1\n"
	"newmtl Elev3C\n"
	"Kd 0.909803922	0.976470588	1\n"
	"newmtl Elev3D\n"
	"Kd 0.945098039	0.988235294	1\n"
	"newmtl Elev3E\n"
	"Kd 0.776470588	0.788235294	0.666666667\n"
	"newmtl Elev3F\n"
	"Kd 0.509803922	0.611764706	0.345098039\n"
	"newmtl Elev40\n"
	"Kd 0.2	0.4	0\n"
	"newmtl Elev41\n"
	"Kd 0.2	0.545098039	0.129411765\n"
	"newmtl Elev42\n"
	"Kd 0.2	0.678431373	0.262745098\n"
	"newmtl Elev43\n"
	"Kd 0.2	0.8	0.4\n"
	"newmtl Elev44\n"
	"Kd 0.368627451	0.831372549	0.458823529\n"
	"newmtl Elev45\n"
	"Kd 0.588235294	0.862745098	0.541176471\n"
	"newmtl Elev46\n"
	"Kd 0.733333333	0.894117647	0.57254902\n"
	"newmtl Elev47\n"
	"Kd 0.858823529	0.88627451	0.623529412\n"
	"newmtl Elev48\n"
	"Kd 0.898039216	0.874509804	0.674509804\n"
	"newmtl Elev49\n"
	"Kd 1	0.862745098	0.725490196\n"
	"newmtl Elev4A\n"
	"Kd 0.980392157	0.835294118	0.670588235\n"
	"newmtl Elev4B\n"
	"Kd 0.968627451	0.815686275	0.596078431\n"
	"newmtl Elev4C\n"
	"Kd 0.952941176	0.792156863	0.537254902\n"
	"newmtl Elev4D\n"
	"Kd 0.937254902	0.768627451	0.443137255\n"
	"newmtl Elev4E\n"
	"Kd 0.91372549	0.745098039	0.388235294\n"
	"newmtl Elev4F\n"
	"Kd 0.901960784	0.721568627	0.345098039\n"
	"newmtl Elev50\n"
	"Kd 0.88627451	0.694117647	0.247058824\n"
	"newmtl Elev51\n"
	"Kd 0.866666667	0.670588235	0.192156863\n"
	"newmtl Elev52\n"
	"Kd 0.850980392	0.650980392	0.152941176\n"
	"newmtl Elev53\n"
	"Kd 0.776470588	0.635294118	0.141176471\n"
	"newmtl Elev54\n"
	"Kd 0.709803922	0.619607843	0.129411765\n"
	"newmtl Elev55\n"
	"Kd 0.658823529	0.603921569	0.121568627\n"
	"newmtl Elev56\n"
	"Kd 0.650980392	0.588235294	0.11372549\n"
	"newmtl Elev57\n"
	"Kd 0.647058824	0.576470588	0.105882353\n"
	"newmtl Elev58\n"
	"Kd 0.643137255	0.564705882	0.098039216\n"
	"newmtl Elev59\n"
	"Kd 0.639215686	0.552941176	0.090196078\n"
	"newmtl Elev5A\n"
	"Kd 0.639215686	0.537254902	0.082352941\n"
	"newmtl Elev5B\n"
	"Kd 0.635294118	0.525490196	0.074509804\n"
	"newmtl Elev5C\n"
	"Kd 0.631372549	0.51372549	0.066666667\n"
	"newmtl Elev5D\n"
	"Kd 0.62745098	0.498039216	0.058823529\n"
	"newmtl Elev5E\n"
	"Kd 0.623529412	0.482352941	0.050980392\n"
	"newmtl Elev5F\n"
	"Kd 0.619607843	0.470588235	0.043137255\n"
	"newmtl Elev60\n"
	"Kd 0.615686275	0.458823529	0.035294118\n"
	"newmtl Elev61\n"
	"Kd 0.611764706	0.443137255	0.02745098\n"
	"newmtl Elev62\n"
	"Kd 0.607843137	0.42745098	0.019607843\n"
	"newmtl Elev63\n"
	"Kd 0.603921569	0.415686275	0.007843137\n"
	"newmtl Elev64\n"
	"Kd 0.6	0.4	0\n"
	"newmtl Elev65\n"
	"Kd 0.611764706	0.376470588	0.117647059\n"
	"newmtl Elev66\n"
	"Kd 0.623529412	0.356862745	0.235294118\n"
	"newmtl Elev67\n"
	"Kd 0.635294118	0.349019608	0.349019608\n"
	"newmtl Elev68\n"
	"Kd 0.654901961	0.388235294	0.388235294\n"
	"newmtl Elev69\n"
	"Kd 0.682352941	0.42745098	0.42745098\n"
	"newmtl Elev6A\n"
	"Kd 0.698039216	0.462745098	0.462745098\n"
	"newmtl Elev6B\n"
	"Kd 0.705882353	0.501960784	0.501960784\n"
	"newmtl Elev6C\n"
	"Kd 0.71372549	0.541176471	0.541176471\n"
	"newmtl Elev6D\n"
	"Kd 0.717647059	0.576470588	0.576470588\n"
	"newmtl Elev6E\n"
	"Kd 0.729411765	0.611764706	0.611764706\n"
	"newmtl Elev6F\n"
	"Kd 0.745098039	0.650980392	0.650980392\n"
	"newmtl Elev70\n"
	"Kd 0.760784314	0.690196078	0.690196078\n"
	"newmtl Elev71\n"
	"Kd 0.77254902	0.733333333	0.733333333\n"
	"newmtl Elev72\n"
	"Kd 0.788235294	0.768627451	0.768627451\n"
	"newmtl Elev73\n"
	"Kd 0.8	0.8	0.8\n"
	"newmtl Elev74\n"
	"Kd 0.831372549	0.831372549	0.831372549\n"
	"newmtl Elev75\n"
	"Kd 0.862745098	0.862745098	0.862745098\n"
	"newmtl Elev76\n"
	"Kd 0.898039216	0.898039216	0.898039216\n"
	"newmtl Elev77\n"
	"Kd 0.91372549	0.91372549	0.91372549\n"
	"newmtl Elev78\n"
	"Kd 0.937254902	0.937254902	0.937254902\n"
	"newmtl Elev79\n"
	"Kd 0.949019608	0.949019608	0.949019608\n"
	"newmtl Elev7A\n"
	"Kd 0.964705882	0.964705882	0.964705882\n"
	"newmtl Elev7B\n"
	"Kd 0.97254902	0.97254902	0.97254902\n"
	"newmtl Elev7C\n"
	"Kd 0.980392157	0.980392157	0.980392157\n"
	"newmtl Elev7D\n"
	"Kd 0.988235294	0.988235294	0.596078431\n"
	"newmtl Elev7E\n"
	"Kd 0.992156863	0.992156863	0.6\n"
	"newmtl Elev7F\n"
	"Kd 1	1	1\n"
};

static const char *current_time(void) {
	time_t t;
	time(&t);
	char *a = asctime(gmtime(&t));
	*strrchr(a, '\n') = 0;
	return a;
}

static float pcm16_to_db(int s) {
	float dB = 20.f * log10(abs(s) / 32767.f);
	return dB;
}

static int db_to_pcm16(float d) {
	int sig = (int)(32767.99 * pow(10.f, d / 10.f));
	return sig;
}

// find the peaks and valleys
static std::vector<int> PT(const std::vector<float> &QQ, float delta) {
	int peak = 0, hi = 0, lo = 0, Qsz = QQ.size();
	const float *Q = QQ.data();
	std::vector<int> OO(Qsz); // this is a heuristic, we could theoretically blow past this size
	int *O = OO.data(), Oi = 0;
	enum { None, Up, Down } direction = None;
	for (int index = 0; index < Qsz; ++index) {
		switch (direction) {
		case None:
			if (Q[hi] >= Q[index] + delta) {
				direction = Down;
			}
			else if (Q[index] >= Q[lo] + delta) {
				direction = Up;
			}
			if (Q[index] < Q[lo]) {
				lo = index;
			}
			else if (Q[hi] < Q[index]) {
				hi = index;
			}
			peak = index;
			break;
		case Up:
			if (Q[hi] < Q[index]) {
				peak = hi = index;
			}
			else if (Q[hi] == Q[index]) {
				peak = index;
			}
			else if (Q[hi] >= Q[index] + delta) {
				int U;

				// find the leading trough
				for (U = peak - 1; U && Q[U] < Q[U + 1]; --U);
				O[Oi++] = -U;

				O[Oi++] = peak;  // peak

				// find the trailing trough
				for (U = peak + 1; U < Qsz && Q[U - 1] > Q[U]; ++U);
				O[Oi++] = -U;

				// see if this segment is really close to the previous one
				// if so, merge them
				if (Oi >= 6 && (O[Oi - 3] - O[Oi - 4] > -10)) {
					if (Q[O[Oi - 2]] > Q[O[Oi - 5]]) {
						O[Oi - 5] = O[Oi - 2];
					}
					O[Oi - 4] = O[Oi - 1];
					Oi -= 3;
				}
				peak = lo = index; // reset peak to current trough
				direction = Down;
			}
			break;
		case Down:
			if (Q[index] < Q[lo]) {
				peak = lo = index;
			}
			else if (Q[index] == Q[lo]) {
				peak = index;
			}
			else if (Q[index] >= Q[lo] + delta) {
				peak = hi = index;
				direction = Up;
			}
		}
	}

	OO.resize(Oi);
	return OO;
}

void analyze_file(void) {
	std::vector<float> inp(gNumSamples);
	for (unsigned s = 0; s < gNumSamples; ++s) {
		// mix the channels together
		float val = (float)gSoundFile.getCurrentMixDouble();
		// eliminate noise and rescale
		if (fabs(val) > gNoise) {
			inp[s] = (val < 0? val + gNoise: val - gNoise) / (1.f - gNoise);
		}
		gSoundFile.incrementSample();
	}
	//inp.maximize();

	unsigned vertexNum = 1;
	// compute filter kernels and amplitude streams for each note in succession
	for (unsigned k = gLowNote; k <= gHighNote; ++k) {
		///*DEBUG*/ if (k == gLowNote + 2) break;
		float f = (float)MIDI::note_freq[k];
		printf("Processing note %s (MIDI %d) (%.3f Hz) \r", MIDI::note_name[k], k, f);
		float frac = f / (float)gSampleRate;
		gKernel.sinc_blackman((float)(frac / MIDI::filter_band_1), (float)(frac * MIDI::filter_band_1));  // band-pass
		std::vector<float> tmp = gKernel.fft_convolve(inp);
		gKernel.sinc_blackman(frac / 2.0f);                          // low-pass
		std::vector<float> out = gKernel.fft_envelope(tmp);
		//unsigned oCut = (out.size() - inp.size()) / 2;
		float *o = out.data();

		float delta = 0.1f / k;
		//std::vector<char> qv = quantize(k, out);

		std::vector<int> pt = PT(out, delta);

		int pt_sz = pt.size();
		for (int s = 1; s < pt_sz; s += 3) {
			float peak = out[pt[s]];
			int start = -pt[s - 1];
			int end = -pt[s + 1];
			if ((end - start) / gSampleRateDouble >= gMinDuration) {
				int velo = (int)(127.f * std::pow(peak, 0.4f));
				velo = _clamp(velo + ((int)k - (int)gLowNote - 30), 0, 127);
				if (gMakeMesh) {
					float s = (float)(start / gSampleRateDouble) * 100;
					float e = (float)(end / gSampleRateDouble) * 100;
					unsigned mf0 = k << 4;
					fprintf(gMeshFile, "v %f %d %d\n", s, mf0, 0);
					unsigned v = vertexNum++;
					vertexNum += 7;
					fprintf(gMeshFile, "v %f %d %d\n", s, mf0, velo);
					fprintf(gMeshFile, "v %f %d %d\n", s, mf0 + 12, velo);
					fprintf(gMeshFile, "v %f %d %d\n", s, mf0 + 12, 0);
					fprintf(gMeshFile, "v %f %d %d\n", e, mf0, 0);
					fprintf(gMeshFile, "v %f %d %d\n", e, mf0, velo);
					fprintf(gMeshFile, "v %f %d %d\n", e, mf0 + 12, velo);
					fprintf(gMeshFile, "v %f %d %d\n", e, mf0 + 12, 0);
					fprintf(gMeshFile, "usemtl Grey\n");
					fprintf(gMeshFile, "f %d %d %d\n", v, v + 1, v + 2);
					fprintf(gMeshFile, "f %d %d %d\n", v + 2, v + 3, v);
					fprintf(gMeshFile, "f %d %d %d\n", v + 4, v + 5, v + 6);
					fprintf(gMeshFile, "f %d %d %d\n", v + 6, v + 7, v + 4);
					fprintf(gMeshFile, "f %d %d %d\n", v, v + 1, v + 5);
					fprintf(gMeshFile, "f %d %d %d\n", v + 5, v + 4, v);
					fprintf(gMeshFile, "f %d %d %d\n", v + 3, v + 2, v + 6);
					fprintf(gMeshFile, "f %d %d %d\n", v + 6, v + 7, v + 3);
					fprintf(gMeshFile, "usemtl Elev%02X\n", velo);
					fprintf(gMeshFile, "f %d %d %d\n", v + 1, v + 2, v + 6);
					fprintf(gMeshFile, "f %d %d %d\n", v + 6, v + 5, v + 1);
					fprintf(gMeshFile, "\n");
				}
				else {
					gMidiFile.addNoteOn(0, gMidiFile.timeToTicks(start / gSampleRateDouble), 0, k, velo);
					MidiEvent *n_on = gMidiFile.getEventPtr(0);
					gMidiFile.addNoteOff(0, gMidiFile.timeToTicks(end / gSampleRateDouble), 0, k, velo);
					MidiEvent *n_off = gMidiFile.getEventPtr(0);
					n_on->linkEvent(n_off);
				}
			}
		}
	}

}

void create_mesh(void) {

	// export materials
	const char *fname = gOutputFileName.c_str();
	const char *delim = fname + strlen(fname);
	while (--delim > fname && !strchr("/\\:", delim[-1]));
	std::string mtlFileName;
	if (delim > fname) {
		mtlFileName += gOutputFileName.substr(0, delim - fname);
	}
	mtlFileName += gMaterialsName;
	FILE *mtl = fopen(mtlFileName.c_str(), "wb");
	if (mtl) {
		// materials are optional
		fprintf(mtl, "%s\n", gMaterials);
		fclose(mtl);
	}

	gMeshFile = fopen(gOutputFileName.c_str(), "wb");
	if (!gMeshFile) {
		return;
	}
	fprintf(gMeshFile, "# Autogenerated: %s\n# DO NOT EDIT\nmtllib %s\n\no Object.1\n\n", current_time(), gMaterialsName);
}


int main(int argc, char** argv) {
	Options options;
	options.define("m|mesh=b:false", "generate a mesh instead of a MIDI file");
	options.define("s|sustain=d:0.05", "only include notes sustained thusly (sec)");
	options.define("b|begin=d:0.0", "beginning of selection (sec)");
	options.define("d|duration=d:-1.0", "length of selection (sec)");
	options.define("n|noise=f:-1.0E35", "input noise threshold");
	options.define("l|low=s:A0", "lowest MIDI note to analyze");
	options.define("h|high=s:C8", "highest MIDI note to analyze");
	options.process(argc, argv);
	if (options.getArgCount() != 2) {
		fprintf(stderr, 
			"Usage: %s [-b begin_sec][-d duration_sec]\n"
			"       [-n noise_level][-m][-l low_note][-h high_note][-s sustain_sec]\n"
			"       wav_file (output_midi_file | output_mesh_file)\n\n", options.getCommand().c_str());
		fprintf(stderr, 
			"-b | --begin <sec>         beginning of selection (sec)\n"
			"-d | --duration <sec>      duration of selection (sec)\n"
			"-n | --noise <dB>          input noise threshold (dB)\n"
			"-s | --sustain <sec>       minimum length of notes (sec)\n"
			"-l | --low <note>          lowest note to analyze (name or MIDI number)\n"
			"-h | --high <note>         highest note to analyze (name or MIDI number)\n"
			"-m | --mesh                generate OBJ/MTL mesh instead of MIDI file\n\n");
		return 1;
	}

	gSoundFile.setFile(options.getArg(1).c_str(), options.getInteger("begin"), options.getInteger("duration"));
	printf("Input file: %s\n", options.getArg(1).c_str());
	gNoiseDecibels = _clamp(options.getFloat("noise"), - FLT_MAX, 0.f);
	gNoise = db_to_pcm16(gNoiseDecibels);
	gMakeMesh = options.getBoolean("mesh");
	gOutputFileName = options.getArg(2);
	std::string loNote = options.getString("low");
	std::string hiNote = options.getString("high");
	gLowNote = _clamp(MIDI::note_from_name(loNote), MIDI::note::A0, MIDI::note::C8);
	gHighNote = _clamp(MIDI::note_from_name(hiNote), MIDI::note::A0, MIDI::note::C8);
	if (gLowNote > gHighNote) {
		std::swap(gLowNote, gHighNote);
	}
	gSampleRateDouble = gSampleRate = gSoundFile.getSrate();
	gNumSamples = gSoundFile.getSamples();
	gNumChannels = gSoundFile.getChannels();
	gMinDuration = _clamp(options.getFloat("sustain"), 0.01f, 1.f);
	gMinDurationInTicks = gMinDuration * gSampleRate;

	printf("Sampling rate: %d Hz\n", gSampleRate);
	printf("Number of samples: %d (%.3f sec)\n", gNumSamples, (float)gNumSamples / (float)gSampleRate);
	printf("Number of channels: %d (mixed down)\n\n", gNumChannels);

	printf("Lowest note: %s (%.3f Hz)\n", MIDI::note_name[gLowNote], (float)MIDI::note_freq[gLowNote]);
	printf("Highest note: %s (%.3f Hz)\n", MIDI::note_name[gHighNote], (float)MIDI::note_freq[gHighNote]);
	printf("Beginning of selection: %d (%.3f sec)\n", gSoundFile.getSelectionStart(), (float)gSoundFile.getSelectionStart() / (float)gSampleRate);
	printf("Length of selection: %d (%.3f sec)\n\n", gSoundFile.getSelectionLength(), (float)gSoundFile.getSelectionLength() / (float)gSampleRate);

	if (gMakeMesh) {
		create_mesh();
		if (!gMeshFile) {
			fprintf(stderr, "*** Could not write to OBJ file\n");
			return 1;
		}
	}
	else {
		//gMidiFile.setTicksPerQuarterNote(gResolution);  // ticks per beat, now need to make a beat last 1 sec = 60BPM
		//gMidiFile.addTempoMicroseconds(0, 0, 1000000);  // 60 BPM

	}
	analyze_file();

	if (gMakeMesh) {
		printf("Output OBJ file: %s    \n", gOutputFileName.c_str());
		fprintf(gMeshFile, "\n # end\n");
		fclose(gMeshFile);
	}
	else {
		printf("Output MIDI file: %s    \n", gOutputFileName.c_str());
		gMidiFile.sortTracks();         // make sure data is in correct order
		if (!gMidiFile.write(gOutputFileName)) {
			fprintf(stderr, "*** Could not write to MIDI file\n");
			return 1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
