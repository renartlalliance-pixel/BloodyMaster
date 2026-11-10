#include <windows.h>
#if BM_HTML_UI
#include <WebView2.h>
#endif
int wmain() {
#if BM_HTML_UI
    LPWSTR version=nullptr;
    const auto result=GetAvailableCoreWebView2BrowserVersionString(nullptr,&version);
    const bool available=SUCCEEDED(result) && version!=nullptr && *version!=L'\0';
    if(version)CoTaskMemFree(version);
    return available?0:1;
#else
    return 0;
#endif
}
