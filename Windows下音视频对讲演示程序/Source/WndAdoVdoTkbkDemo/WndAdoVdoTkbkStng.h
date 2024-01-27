#pragma once

#include "TinyXml2.h"

#ifdef __cplusplus
extern "C"
{
#endif

void SaveStngToXmlFile();
void ReadStngFromXmlFile();
void DelStngXmlFile();
void ResetStng();
void EffectLow();
void EffectMid();
void EffectHigh();
void EffectSuper();
void EffectPremium();

#ifdef __cplusplus
}
#endif
