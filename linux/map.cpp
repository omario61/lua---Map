
#include <luaT.h>
#include <TH.h>
#include <iostream>
#include <map>

using namespace std;

class Pixel{
public:
    int r, g, b;
    double h, s, l;
    Pixel(){}
    Pixel(int red, int green, int blue){
      this->r = red;
      this->g = green;
      this->b = blue;
      toHsl();
    }
    Pixel(const Pixel& p){
      this->r = p.r;
      this->g = p.g;
      this->b = p.b;
      toHsl();
    }
    void toHsl(){
      double dr = ((double) this->r) / 255.0;
      double dg = ((double) this->g) / 255.0;
      double db = ((double) this->b) / 255.0;
      double max = std::max(dr, std::max(dg, db));
      double min = std::min(dr, std::min(dg, db));
      this->h = this->l = this->s = (max + min) / 2.0;
      if(max == min){
          this->h = 0;      this->s = 0;
          this->l = 0;
      }else{
          double d = max - min;
          this->s = this->l > 0.5 ? (d / (2.0 - max - min)) : (d / (max + min));
          if(max == dr){
                this->h = (dg - db) / d + (dg < db ? 6: 0);
          } else if(max == dg){
                this->h = (db - dr) / d + 2;
          }else if( max == db){
                this->h = (dr - dg) / d + 4;
          }
          this->h /= 6;
      }
      this->h *= 360; this->s *= 100; this->l *= 100;
    }
    double compareValue() const{
      return h;
    }
};
class compareColors{
public:
    bool operator()(const Pixel& x, const Pixel& y) const{
        double eps = 0.001;
        if(fabs(x.compareValue() - y.compareValue()) < eps)
          return false;
        return x.compareValue() < y.compareValue();
    }
};

std::map<Pixel, Pixel, compareColors> pixel_map;

extern "C"{
  static int l_init (lua_State *L) {
      // pixel_map[5] = 10;
      // pixel_map[15] = 20;
      pixel_map.clear();
      lua_pushboolean(L, 1);
      return 1;
  }

  static int l_add(lua_State *L){

      if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)
          && lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6)) {
        //  printf("Entering l_add\n");
          Pixel key = Pixel(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
          Pixel value = Pixel(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
          pixel_map[key] = value;
        //  printf("the key and values are %d %d\n", key, value);
         lua_pushboolean(L, 1);
          return 1;
      }
      lua_pushboolean(L, 0);
      return 1;
  }

  static int l_get(lua_State * L){

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
      //  printf("Entering l_get\n");
      //  LuaObject table;
        // printf("map size is %d\n", pixel_map.size());
        // for(map<Pixel, Pixel>:: iterator it = pixel_map.begin(); it != pixel_map.end(); it++){
        //     printf("%d %d %d %d %d %d %f\n", it->first.r , it->first.g, it->first.b,
        //                                     it->second.r , it->second.g, it->second.b, it->first.h);
        // }
        Pixel key = Pixel(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
        // printf("key %d %d %d %f\n", key.r, key.g, key.b, key.h);
        Pixel value;
        map<Pixel, Pixel>::iterator lower = pixel_map.lower_bound(key);
        map<Pixel, Pixel>::iterator upper = pixel_map.upper_bound(key);
        if(pixel_map.find(key) != pixel_map.end()){
            value = pixel_map[key];
        } else{
            if(lower != pixel_map.end() && upper != pixel_map.end()){
                if(fabs(lower->first.h - key.h) < fabs(upper->first.h - key.h)){
                   value = lower->second;
                } else{
                    value = upper->second;
                }
            } else if(upper != pixel_map.end()){
                value = upper->second;
            } else if(lower != pixel_map.end()){
                value = lower->second;
            }
        }

        THFloatTensor * frame = (THFloatTensor *) luaT_toudata(L, 4, luaT_typenameid(L, "torch.FloatTensor"));
        THFloatTensor_resize2d(frame, 3, 1);
        float * dst = THFloatTensor_data(frame);
        dst[0] = value.r;
        dst[1] = value.g;
        dst[2] = value.b;
        // lua_pushnumber(L, value.r);
        // lua_pushnumber(L, value.g);
        // lua_pushnumber(L, value.b);
        //lua_pushboolean(L, 1);
        return 1;
    }
    //lua_pushboolean(L, 0);
    return 1;
  }


  // Register functions in LUA
  static const struct luaL_reg map_fun [] = {
      {"init"             , l_init},
      {"add"              , l_add},
      {"get"              , l_get},
      {NULL, NULL}  /* sentinel */
  };

  int luaopen_libmap (lua_State *L) {
      luaL_openlib(L, "libmap", map_fun, 0);
      return 1;
  }
}
