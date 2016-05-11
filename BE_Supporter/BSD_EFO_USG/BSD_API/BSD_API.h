// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BSD_API_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BSD_API_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BSD_API_EXPORTS
#define BSD_API_API __declspec(dllexport)
#else
#define BSD_API_API __declspec(dllimport)
#endif

// This class is exported from the BSD_API.dll
class BSD_API_API CBSD_API {
public:
	CBSD_API(void);
	// TODO: add your methods here.
};

extern BSD_API_API int nBSD_API;

BSD_API_API int fnBSD_API(void);
//BSD_API_DLL short bsd_api_gen_debug_file(char strDebugBsdData[], int iFilenameLen);