#pragma once

#include "faust/gui/UI.h"
#include "faust/gui/MidiUI.h"
#include "assocarray.h"

#include "IPlugBase.h"

#if defined OS_MAC || defined OS_LINUX
#define DEFAULT_FAUST_LIBRARY_PATH "/usr/local/share/faust/"
#else
#define DEFAULT_FAUST_LIBRARY_PATH //TODO
#endif

class IPlugFaust : public UI, public Meta
{
public:
  
  IPlugFaust(const char* name, int nVoices = 1)
  : mNVoices(nVoices)
  {
    mName.Set(name);
  }
  
  virtual ~IPlugFaust()
  {
    // Has to be done *before* RemoveInstance that may free mFactory and thus mFactory->mMidiHandler
    //delete mMidiUI;

    mParams.Empty(true);
  }
  
  virtual void Init(const char* sourceStr = "", int maxNInputs = -1, int maxNOutputs = -1) = 0;
  
  virtual void GetSVGPath(WDL_String& path) {}
  virtual bool CompileArchitectureFile() { return true; }
  
  void FreeDSP()
  {
    DELETE_NULL(mDSP);
  }
  
  // Unique
  void SetSampleRate(double sampleRate)
  {
    if (mDSP)
      mDSP->init((int) sampleRate);
  }
  
  void ProcessMidiMsg(const IMidiMsg& msg)
  {
//    TODO:
  }
  
  virtual void ProcessBlock(sample** inputs, sample** outputs, int nFrames)
  {
    if (mDSP)
      mDSP->compute(nFrames, inputs, outputs);
  }
  
  void SetParameterValue(int paramIdx, double normalizedValue)
  {
    assert(paramIdx < NParams());
    
    *(mZones.Get(paramIdx)) = normalizedValue;
  }
  
  void SetParameterValue(const char* labelToLookup, double normalizedValue)
  {
    FAUSTFLOAT* dest = nullptr;
    dest = mMap.Get(labelToLookup, nullptr);
    
    if(dest)
      *dest = normalizedValue;
  }
  
  int CreateIPlugParameters(IPlugBase& plug, int startIdx = 0)
  {
    int plugParamIdx = startIdx;

    for (auto p = 0; p < NParams(); p++)
    {
      plugParamIdx += p;
      assert(plugParamIdx < plug.NParams());
      
      plug.GetParam(plugParamIdx)->Init(*mParams.Get(p));
    }
    
    return plugParamIdx;
  }

  int NParams()
  {
    return mParams.GetSize();
  }
  
  // Meta
  void declare(const char *key, const char *value) override
  {
    // TODO:
  }
  
  // UI
  
  // TODO:
  void openTabBox(const char *label) override {}
  void openHorizontalBox(const char *label) override {}
  void openVerticalBox(const char *label) override {}
  void closeBox() override {}
  
  void addButton(const char *label, FAUSTFLOAT *zone) override
  {
    IParam* pParam = new IParam();
    pParam->InitBool(label, 0);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }
  
  void addCheckButton(const char *label, FAUSTFLOAT *zone) override
  {
    IParam* pParam = new IParam();
    pParam->InitBool(label, 0);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }
  
  void addVerticalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitDouble(label, init, min, max, step);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }
  
  void addHorizontalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitDouble(label, init, min, max, step);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }
  
  void addNumEntry(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    IParam* pParam = new IParam();
    pParam->InitEnum(label, init, max - min);
    mParams.Add(pParam);
    mZones.Add(zone);
    mMap.Insert(label, zone);
  }
  
  // TODO:
  void addHorizontalBargraph(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT min, FAUSTFLOAT max) override {}
  void addVerticalBargraph(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT min, FAUSTFLOAT max) override {}
  void addSoundfile(const char *label, const char *filename, Soundfile **sf_zone) override {}
  
protected:
  WDL_String mName;
  int mNVoices;
  ::dsp* mDSP = nullptr;
  MidiUI* mMidiUI = nullptr;
  WDL_PtrList<IParam> mParams;
  WDL_PtrList<FAUSTFLOAT> mZones;
  WDL_StringKeyedArray<FAUSTFLOAT*> mMap;
};




