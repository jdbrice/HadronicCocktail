// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdICintFunctionLibraryDictionary

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "CintFunctionLibrary.h"

// Header files passed via #pragma extra_include

namespace {
  void TriggerDictionaryInitialization_CintFunctionLibraryDictionary_Impl() {
    static const char* headers[] = {
"CintFunctionLibrary.h",
0
    };
    static const char* includePaths[] = {
"include/UnitTests",
"/Users/danielbrandenburg/bnl/local/work/RooBarb/include",
"/Users/danielbrandenburg/bnl/vendor/root-6.06.04/include",
"/Users/danielbrandenburg/bnl/local/work/HadronicCocktail/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "CintFunctionLibraryDictionary dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "CintFunctionLibraryDictionary dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "CintFunctionLibrary.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TF_BreitWigner", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("CintFunctionLibraryDictionary",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_CintFunctionLibraryDictionary_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_CintFunctionLibraryDictionary_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_CintFunctionLibraryDictionary() {
  TriggerDictionaryInitialization_CintFunctionLibraryDictionary_Impl();
}
