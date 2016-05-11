
#ifdef MOTALGO_DLL_EXPORTS
#define MOTALGO_DLL_API __declspec(dllexport)
#else
#define MOTALGO_DLL_API __declspec(dllimport)
#endif

//////// Only for internal
extern MOTALGO_DLL_API int mtnapi_confirm_para_base_path_exist();
