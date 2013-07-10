/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <ahmad@amireh.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include <algol/lua/engine.hpp>
#include <algol/utility.hpp>
#include <stdarg.h>
#include <boost/bind.hpp>

extern "C" {
  void lua_pushusertype (lua_State* L, void* value, const char* type)
  {
   if (value == NULL)
    lua_pushnil(L);
   else
   {
    luaL_getmetatable(L, type);
    lua_pushstring(L,"tolua_ubox");
    lua_rawget(L,-2);        /* stack: mt ubox */
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      lua_pushstring(L, "tolua_ubox");
      lua_rawget(L, LUA_REGISTRYINDEX);
    };
    lua_pushlightuserdata(L,value);
    lua_rawget(L,-2);                       /* stack: mt ubox ubox[u] */
    if (lua_isnil(L,-1))
    {
     lua_pop(L,1);                          /* stack: mt ubox */
     lua_pushlightuserdata(L,value);
     *(void**)lua_newuserdata(L,sizeof(void *)) = value;   /* stack: mt ubox u newud */
     lua_pushvalue(L,-1);                   /* stack: mt ubox u newud newud */
     lua_insert(L,-4);                      /* stack: mt newud ubox u newud */
     lua_rawset(L,-3);                      /* stack: mt newud ubox */
     lua_pop(L,1);                          /* stack: mt newud */
     /*luaL_getmetatable(L,type);*/
     lua_pushvalue(L, -2);      /* stack: mt newud mt */
     lua_setmetatable(L,-2);      /* stack: mt newud */

     #ifdef LUA_VERSION_NUM
     lua_pushvalue(L, LUA_REGISTRYINDEX);
     lua_setfenv(L, -2);
     #endif
    }
    else
    {
     /* check the need of updating the metatable to a more specialized class */
     lua_insert(L,-2);                       /* stack: mt ubox[u] ubox */
     lua_pop(L,1);                           /* stack: mt ubox[u] */
     lua_pushstring(L,"tolua_super");
     lua_rawget(L,LUA_REGISTRYINDEX);        /* stack: mt ubox[u] super */
     lua_getmetatable(L,-2);                 /* stack: mt ubox[u] super mt */
     lua_rawget(L,-2);                       /* stack: mt ubox[u] super super[mt] */
        if (lua_istable(L,-1))
        {
          lua_pushstring(L,type);                 /* stack: mt ubox[u] super super[mt] type */
          lua_rawget(L,-2);                       /* stack: mt ubox[u] super super[mt] flag */
          if (lua_toboolean(L,-1) == 1)   /* if true */
          {
            lua_pop(L,3); /* mt ubox[u]*/
            lua_remove(L, -2);
            return;
          }
        }
        /* type represents a more specilized type */
        /*luaL_getmetatable(L,type);             // stack: mt ubox[u] super super[mt] flag mt */
        lua_pushvalue(L, -5);         /* stack: mt ubox[u] super super[mt] flag mt */
        lua_setmetatable(L,-5);                /* stack: mt ubox[u] super super[mt] flag */
        lua_pop(L,3);                          /* stack: mt ubox[u] */
    }
    lua_remove(L, -2);  /* stack: ubox[u]*/
   }
  }
}

namespace algol {
namespace lua {

  engine::engine()
  : logger("lua engine"),
    configurable({ "lua engine" }),
    lua_(nullptr),
    is_corrupt_(false)
  {
  }

  engine::~engine() {
    stop();

    log_->infoStream() << "shutting down.";
  }

  bool engine::start(string_t in_script_path) {
    string_t entry_script;
    if (lua_)
      return true;

    if (!in_script_path.empty()) {
      entry_script = in_script_path;
    }

    log_->infoStream() << "Setting up";
    lua_ = lua_open();
    luaL_openlibs(lua_);

    is_corrupt_ = false;

    if (entry_script.empty()) {
      entry_script = (file_manager::singleton().root_path() / "scripts" / "algol.lua").string();
    }

    if (!run(entry_script)) {
      on_error();
      return false;
    }

    lua_getglobal(lua_, "arbitrator");
    if(!lua_isfunction(lua_, -1))
    {
      log_->errorStream() << "could not find Lua arbitrator functor! Corrupt state?";
      on_error();
      return false;
    }

    invoke("Algol.start", 0);

    return !!lua_;
  }

  bool engine::stop() {
    if (!lua_)
      return true;

    log_->infoStream() << "Lua engine is stopping...";

    lua_getglobal(lua_, "arbitrator");
    if(lua_isfunction(lua_, -1)) {
      invoke("Algol.stop", 0);
    } else {
      log_->errorStream() << "could not find Lua arbitrator! Corrupt state?";
    }

    lua_close(lua_);
    lua_ = nullptr;

    log_->infoStream() << "Lua engine is off.";

    return true;
  }


  void engine::restart() {
    if (!lua_)
      return;

    log_->infoStream() << "restarting Lua engine...";

    stop();
    start();

    log_->infoStream() << "Lua engine restarted.";
  }

  void engine::update( unsigned long dt ) {
    if (mConfig.TickScript)
      invoke("Algol.update", 1, "unsigned long", dt);
  }

  lua_State* engine::getLuaState() {
    return lua_;
  }

  bool engine::run(string_t const& in_script) {
    log_->infoStream() << "Running script '" << in_script << "'";

    int errCode = luaL_dofile(lua_, in_script.c_str());
    if (errCode == 1) {
      string_t err = lua_tostring(lua_, -1);
      log_->errorStream() << "Lua: " << err;
      lua_pop(lua_, -1);

      throw script_error("Unable to load script '" + in_script + "'; cause: " + err);

      return false;
    }

    return true;
  }

  bool engine::invoke(const char* in_func, int argc, ...) {
    if (is_corrupt_)
    {
      log_->warnStream() << "Lua state is corrupt, bailing out on method call " << in_func;
      return false;
    }

    va_list argp;
    va_start(argp, argc);

    lua_getfield(lua_, LUA_GLOBALSINDEX, "algol.arbitrator");
    if(!lua_isfunction(lua_, 1))
    {
      log_->errorStream() << "could not find the Lua arbitrator to invoke: " << in_func << "!";
      on_error();
      return false;
    }

    lua_pushfstring(lua_, in_func);
    for (int i=0; i < argc; ++i) {
      const char* argtype = (const char*)va_arg(argp, const char*);
      void* argv = (void*)va_arg(argp, void*);
      if (string_t(argtype) == "std::string")
        lua_pushfstring(lua_, ((string_t*)argv)->c_str());
      else
        lua_pushusertype(lua_,argv,argtype);
    }

    int ec = lua_pcall(lua_, argc+1, 1, 0);
    if (ec != 0)
    {
      // there was a lua error, dump the state and shut down the instance
      on_error();
      return false;
    }

    bool result = lua_toboolean(lua_, lua_gettop(lua_));
    lua_remove(lua_, lua_gettop(lua_));

    va_end(argp);
    return result;
  }

  void engine::on_error()
  {
    // pop the error msg from the stack
    const char* error_c = lua_tostring(lua_, -1);
    string_t error;
    if (error_c) {
      // string_t error = lua_tostring(lua_, -1);
      error = string_t(error_c);
      log_->errorStream() << "Lua error: " << error;
      lua_pop(lua_, -1);
    }

    try { stop(); } catch (...) {}

    is_corrupt_ = true;

    switch (mConfig.ErrorHandling) {
      // DEBUG: just die
      case CATCH_AND_DIE:
        assert(false);
        break;
      case CATCH_AND_THROW:
        throw script_error("A Lua error occured: " + error);
        break;
      default:
        assert(false);
    }

    return;
  }

  void engine::set_defaults() {
    mConfig.TickScript = true;
    mConfig.InterceptEvents = true;
    mConfig.ErrorHandling = CATCH_AND_DIE;
  }

  void engine::set_option(string_t const& k, string_t const& v) {
    if (k == "Intercept Events") {
      mConfig.InterceptEvents = utility::boolify(v);
    }
    else if (k == "Error Handling") {
      if (v == "Exception")
        mConfig.ErrorHandling = CATCH_AND_THROW;
      else if (v == "Die")
        mConfig.ErrorHandling = CATCH_AND_DIE;
      else {
        log_->noticeStream() << "Invalid 'Error Handling' setting '" << v << "', defaulting to 'Die'";
      }
    }
    else if (k == "Tick Script") {
      mConfig.TickScript = utility::boolify(v);
    }
    else {
      log_->noticeStream() << "Unknown Script Engine setting '" << k << "', ignoring.";
    }
  }

} // namespace lua
} // namespace algol
