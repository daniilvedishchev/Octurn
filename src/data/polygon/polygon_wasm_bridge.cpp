#include <string>
#include <unordered_map>
#include <utility>
#include <iostream>

#include <emscripten.h>
#include <emscripten/bind.h>

#include "polygon_wasm_bridge.hpp"

/**
 @brief C++ wrapper to JS
       -- allows to call js functions from native C++ 
       -- ohlc is transmitted to other modules
       -- polygon_data_ready is called from js to pass data to C++ engine
       -- make local types, functions available in js thanks to bindings
       -- fetch from polygon in parallel manner -> free memory when fetched  
 @result OHLC pointer to 2D vector containing ohlc data used in further calculations 
*/

EM_JS(void, js_fetch_polygon_data,
    (const char* ticker, int multiplier, const char* timespan,
    const char* from, const char* to, int callback_id),
{
    window.fetch_polygon_data(
        UTF8ToString(ticker),
        multiplier,
        UTF8ToString(timespan),
        UTF8ToString(from),
        UTF8ToString(to),
        callback_id
    );
});

static int callBackId = 1;

static std::unordered_map<int,bool> fetch_status_ready;
static std::unordered_map<int,Vec2D> fetch_queue;

// --------------------------------------- //
//        Free memory when fetched
// --------------------------------------- //

Vec2D get_ohlc(int id){
    auto it = fetch_queue.find(id);
    if (it == fetch_queue.end()) return {};

    Vec2D out = std::move(it->second);
    fetch_queue.erase(it);
    fetch_status_ready.erase(id);
    return out;
}

bool ohlc_ready(int id) {
    std::cout<<std::format("OHCL for {} status {}\n",id,fetch_status_ready[id]);
    auto it = fetch_status_ready.find(id);
    if (it != fetch_status_ready.end()){
        return it->second;
    }else {return false;}
}

// --------------------------------------- //
//        Send request to JS
// --------------------------------------- //

int request_ohlc(const std::string& ticker, int multiplier, const std::string& timespan,
    const std::string& from, const std::string& to){
    
    std::cout<<std::format("Requesting for :{}\n",ticker);
    const int id = callBackId++;
    js_fetch_polygon_data(ticker.c_str(),multiplier,timespan.c_str(),from.c_str(),to.c_str(),id);

    return id;
}

void polygon_data_ready(const int callbackId,const Vec2D& ohlc){
    
    fetch_queue[callbackId] = ohlc;
    fetch_status_ready[callbackId] = true;
    std::cout << "ohlc size: " << ohlc.size()
          << " open size: " << (ohlc.empty() ? 0 : ohlc[0].size()) << "\n";

    // std::cout<<std::format("Polygon data ready! callbackId:{},status:{}\n",callbackId,fetch_status_ready[callbackId]?"true":"false");
    // std::cout<<std::format("isReady for id:{},status:{}\n",callbackId,ohlc_ready(callbackId) ? "true":"false");
}

// --------------------------------------- //
//        Make accessible in JS
// --> vector<double>
// --> vector<vector<double>>
// --> polygon_data_ready()
// --------------------------------------- //

EMSCRIPTEN_BINDINGS(polygon_bindings)
{
    emscripten::register_vector<double>("VecD");
    emscripten::register_vector<VecD>("Vec2D");

    emscripten::function("polygon_data_ready", &polygon_data_ready);
    emscripten::function("get_ohlc", &get_ohlc);
}
