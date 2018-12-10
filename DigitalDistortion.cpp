#include "DigitalDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 5;

enum EParams
{
  kThreshold = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kThresholdX = 69,
  kThresholdY = 69,
  kKnobFrames = 64
};

DigitalDistortion::DigitalDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mThreshold(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Distortion", 0.0, 0.0, 80.00, 0.01, "%");
  GetParam(kThreshold)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_RED);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  //MakeDefaultPreset((char *) "-", kNumPrograms);
  CreatePresets();
}

DigitalDistortion::~DigitalDistortion() {}

void DigitalDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

	int const channelCount = 2;
	for (int i = 0; i < channelCount; i++)
	{
		double* input = inputs[i];
		double* output = outputs[i];
		for (int s = 0; s < nFrames; ++s, ++input, ++output)
		{
			if (*input >= 0)
			{
				*output = fmin(*input, mThreshold);
			}
			else
			{
				*output = fmax(*input, -mThreshold);
			}
			*output /= mThreshold;
		}
	}
}

void DigitalDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void DigitalDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      mThreshold = 1 - ( GetParam(kThreshold)->Value() / 100. );
      break;

    default:
      break;
  }
}

void DigitalDistortion::CreatePresets()
{
	MakePreset("Clean", 80.0);
	MakePreset("Slightly distorted", 60.0);
	MakePreset("Wooo", 40.0);
	MakePreset("Wawi", 20.0);
	MakePreset("Why would you use this", 0.01);
}