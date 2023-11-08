/*############################################################################
  # Copyright (C) 2005 Intel Corporation
  #
  # SPDX-License-Identifier: MIT
  ############################################################################*/

#ifndef __PARAMETERS_DUMPER_H__
#define __PARAMETERS_DUMPER_H__
#include "sample_defs.h"

class CParametersDumper {
protected:
    static void SerializeFrameInfoStruct(msdk_ostream& sstr,
                                         msdk_string prefix,
                                         mfxFrameInfo& info);
    static void SerializeMfxInfoMFXStruct(msdk_ostream& sstr, msdk_string prefix, mfxInfoMFX& info);
    static void SerializeExtensionBuffer(msdk_ostream& sstr,
                                         msdk_string prefix,
                                         mfxExtBuffer* pExtBuffer);
    static void SerializeVPPCompInputStream(msdk_ostream& sstr,
                                            msdk_string prefix,
                                            mfxVPPCompInputStream& info);

    template <class T>
    static mfxStatus GetUnitParams(T* pMfxUnit,
                                   const mfxVideoParam* pPresetParams,
                                   mfxVideoParam* pOutParams) {
        memset(pOutParams, 0, sizeof(mfxVideoParam));
        mfxExtBuffer** paramsArray = new mfxExtBuffer*[pPresetParams->NumExtParam];
        for (int paramNum = 0; paramNum < pPresetParams->NumExtParam; paramNum++) {
            mfxExtBuffer* buf    = pPresetParams->ExtParam[paramNum];
            mfxExtBuffer* newBuf = (mfxExtBuffer*)new mfxU8[buf->BufferSz];
            memset(newBuf, 0, buf->BufferSz);
            newBuf->BufferId      = buf->BufferId;
            newBuf->BufferSz      = buf->BufferSz;
            paramsArray[paramNum] = newBuf;
        }
        pOutParams->NumExtParam = pPresetParams->NumExtParam;
        pOutParams->ExtParam    = paramsArray;

        mfxStatus sts = pMfxUnit->GetVideoParam(pOutParams);
        MSDK_CHECK_STATUS_SAFE(sts,
                               "Cannot read configuration from encoder: GetVideoParam failed",
                               ClearExtBuffs(pOutParams));

        return MFX_ERR_NONE;
    }

    static void ClearExtBuffs(mfxVideoParam* params) {
        // Cleaning params array
        for (int paramNum = 0; paramNum < params->NumExtParam; paramNum++) {
            delete[] params->ExtParam[paramNum];
        }
        delete[] params->ExtParam;
        params->ExtParam    = NULL;
        params->NumExtParam = 0;
    }

public:
    static void SerializeVideoParamStruct(msdk_ostream& sstr,
                                          msdk_string sectionName,
                                          mfxVideoParam& info,
                                          bool shouldUseVPPSection = false);
    static mfxStatus DumpLibraryConfiguration(msdk_string fileName,
                                              MFXVideoDECODE* pMfxDec,
                                              MFXVideoVPP* pMfxVPP,
                                              MFXVideoENCODE* pMfxEnc,
                                              const mfxVideoParam* pDecoderPresetParams,
                                              const mfxVideoParam* pVPPPresetParams,
                                              const mfxVideoParam* pEncoderPresetParams);
    static void ShowConfigurationDiff(msdk_ostream& sstr1, msdk_ostream& sstr2);
};
#endif
