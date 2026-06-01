// macOS stub implementations for Windows Registry functions.
// Registry operations use an in-memory map so that session state written
// by the DSP layer (e.g. bypass, EQ, effect values) is visible to subsequent
// reads within the same process.  Values are NOT persisted to disk between
// runs — that is intentional for Phase 1.
#if defined(__APPLE__)

#include "codedefs.h"
#include "reg.h"

#include <map>
#include <string>
#include <mutex>

// ---------------------------------------------------------------------------
// In-memory registry store
// ---------------------------------------------------------------------------
namespace {

struct RegStore {
    std::mutex mtx;
    // key: "hive|path" -> wide-string value
    std::map<std::wstring, std::wstring> wide_store;
    // key: "hive|path" -> narrow-string value
    std::map<std::string, std::string>   narrow_store;

    static RegStore& instance() {
        static RegStore s;
        return s;
    }

    static std::wstring make_key(int hive, const wchar_t* path) {
        return std::wstring(L"h") + std::to_wstring(hive) + L"|" + (path ? path : L"");
    }
    static std::string make_key(int hive, const char* path) {
        return std::string("h") + std::to_string(hive) + "|" + (path ? path : "");
    }
};

} // namespace

// ---------------------------------------------------------------------------
// Wide-string registry functions (primary path used by DSP session layer)
// ---------------------------------------------------------------------------

int PT_DECLSPEC regCreateKey_Wide(int hive, wchar_t *wcp_path, wchar_t *wcp_value)
{
    if (!wcp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    store.wide_store[RegStore::make_key(hive, wcp_path)] = wcp_value ? wcp_value : L"";
    return OKAY;
}

int PT_DECLSPEC regReadKey_Wide(int hive, wchar_t *wcp_path, int *ip_exists,
                                wchar_t *wcp_value, unsigned long max_len)
{
    if (ip_exists) *ip_exists = 0;
    if (wcp_value && max_len > 0) wcp_value[0] = L'\0';
    if (!wcp_path) return OKAY;

    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    auto it = store.wide_store.find(RegStore::make_key(hive, wcp_path));
    if (it != store.wide_store.end()) {
        if (ip_exists) *ip_exists = 1;
        if (wcp_value && max_len > 0) {
            wcsncpy(wcp_value, it->second.c_str(), max_len - 1);
            wcp_value[max_len - 1] = L'\0';
        }
    }
    return OKAY;
}

int PT_DECLSPEC regCreateKeyWithKeyname_String_Wide(int hive, wchar_t *wcp_path,
                                                    wchar_t *wcp_keyname, wchar_t *wcp_value)
{
    if (!wcp_path || !wcp_keyname) return OKAY;
    std::wstring full = std::wstring(wcp_path) + L"\\" + wcp_keyname;
    return regCreateKey_Wide(hive, &full[0], wcp_value);
}

int PT_DECLSPEC regReadKeyWithKeyname_String_Wide(int hive, wchar_t *wcp_path,
                                                  wchar_t *wcp_keyname, int *ip_exists,
                                                  wchar_t *wcp_value, unsigned long max_len)
{
    if (ip_exists) *ip_exists = 0;
    if (wcp_value && max_len > 0) wcp_value[0] = L'\0';
    if (!wcp_path || !wcp_keyname) return OKAY;
    std::wstring full = std::wstring(wcp_path) + L"\\" + wcp_keyname;
    return regReadKey_Wide(hive, &full[0], ip_exists, wcp_value, max_len);
}

int PT_DECLSPEC regCreateKeyWithKeyname_Dword_Wide(int hive, wchar_t *wcp_path,
                                                   wchar_t *wcp_keyname, unsigned long value)
{
    if (!wcp_path || !wcp_keyname) return OKAY;
    std::wstring full = std::wstring(wcp_path) + L"\\" + wcp_keyname;
    wchar_t buf[32];
    swprintf(buf, 32, L"%lu", value);
    return regCreateKey_Wide(hive, &full[0], buf);
}

int PT_DECLSPEC regReadKeyWithKeyname_Dword_Wide(int hive, wchar_t *wcp_path,
                                                 wchar_t *wcp_keyname, int *ip_exists,
                                                 unsigned long *ulp_value)
{
    if (ip_exists) *ip_exists = 0;
    if (ulp_value) *ulp_value = 0;
    if (!wcp_path || !wcp_keyname) return OKAY;
    std::wstring full = std::wstring(wcp_path) + L"\\" + wcp_keyname;
    wchar_t buf[32] = {};
    int exists = 0;
    if (regReadKey_Wide(hive, &full[0], &exists, buf, 32) != OKAY) return NOT_OKAY;
    if (ip_exists) *ip_exists = exists;
    if (exists && ulp_value) *ulp_value = (unsigned long)wcstoul(buf, nullptr, 10);
    return OKAY;
}

int PT_DECLSPEC regCreateKeyTest_Wide(int hive, wchar_t *wcp_path, wchar_t *wcp_value,
                                      int *ip_success)
{
    if (ip_success) *ip_success = 1;
    if (!wcp_path) return OKAY;
    return regCreateKey_Wide(hive, wcp_path, wcp_value);
}

int PT_DECLSPEC regRemoveKey_Wide(int hive, wchar_t *wcp_path)
{
    if (!wcp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    store.wide_store.erase(RegStore::make_key(hive, wcp_path));
    return OKAY;
}

int PT_DECLSPEC regRecursiveDeleteFolder_Wide(int hive, wchar_t *wcp_path)
{
    if (!wcp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    std::wstring prefix = RegStore::make_key(hive, wcp_path);
    for (auto it = store.wide_store.begin(); it != store.wide_store.end(); ) {
        if (it->first.substr(0, prefix.size()) == prefix)
            it = store.wide_store.erase(it);
        else
            ++it;
    }
    return OKAY;
}

// ---------------------------------------------------------------------------
// Narrow-string registry functions (rarely used by DSP, kept as stubs)
// ---------------------------------------------------------------------------

int PT_DECLSPEC regCreateKey(int hive, char *cp_path, char *cp_value)
{
    if (!cp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    store.narrow_store[RegStore::make_key(hive, cp_path)] = cp_value ? cp_value : "";
    return OKAY;
}

int PT_DECLSPEC regReadKey(int hive, char *cp_path, int *ip_exists,
                           char *cp_value, unsigned long max_len)
{
    if (ip_exists) *ip_exists = 0;
    if (cp_value && max_len > 0) cp_value[0] = '\0';
    if (!cp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    auto it = store.narrow_store.find(RegStore::make_key(hive, cp_path));
    if (it != store.narrow_store.end()) {
        if (ip_exists) *ip_exists = 1;
        if (cp_value && max_len > 0) {
            strncpy(cp_value, it->second.c_str(), max_len - 1);
            cp_value[max_len - 1] = '\0';
        }
    }
    return OKAY;
}

int PT_DECLSPEC regRemoveKey(int hive, char *cp_path)
{
    if (!cp_path) return OKAY;
    auto& store = RegStore::instance();
    std::lock_guard<std::mutex> lk(store.mtx);
    store.narrow_store.erase(RegStore::make_key(hive, cp_path));
    return OKAY;
}

// ---------------------------------------------------------------------------
// Misc stubs that don't need in-memory storage
// ---------------------------------------------------------------------------

int PT_DECLSPEC regReadTopDir_Wide(wchar_t *, int, int, int, CSlout *) { return OKAY; }
int PT_DECLSPEC regReadRegisteredOwner(char *, int) { return OKAY; }
int PT_DECLSPEC regReadRegisteredOwner_Wide(wchar_t *, int) { return OKAY; }

#endif // __APPLE__
