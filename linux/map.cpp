
#include <luaT.h>
#include <TH.h>
#include <iostream>
#include <omp.h>
#include <map>

using namespace std;

class Pixel{
public:
    double r, g, b;
    double h, s, l;
    int idx_i, idx_j;
    Pixel(){}
    Pixel(double red, double green, double blue){
      this->r = red;
      this->g = green;
      this->b = blue;
      this->idx_i = this->idx_j = -1;
      toHsl();
    }
    Pixel(double red, double green, double blue, int idx_i, int idx_j){
      this->r = red;
      this->g = green;
      this->b = blue;
      this->idx_i = idx_i;
      this->idx_j = idx_j;
      toHsl();
    }
    Pixel(const Pixel& p){
      this->r = p.r;
      this->g = p.g;
      this->b = p.b;
      this->idx_i = p.idx_i;
      this->idx_j = p.idx_j;
      toHsl();
    }
    void toHsl(){
      double dr = ((double) this->r);
      double dg = ((double) this->g);
      double db = ((double) this->b);
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
    double magnitude() const{
        return r*r + g*g + b*b;
    }
    Pixel operator+(const Pixel pix) const{
        return Pixel(r + pix.r, g + pix.g, b + pix.b);
    }
    Pixel operator/(const int n) const{
        return Pixel(r / n, g / n, b / n);
    }
    bool spatialDataAvailable() const{
        return this->idx_i == -1;
    }
};
class compareColors{
public:
    bool operator()(const Pixel& x, const Pixel& y) const{
        double epsillon = 10e-4;
        if(fabs(x.compareValue() - y.compareValue()) < epsillon){
            return false;
        }
        double rdiff = fabs(x.r - y.r);
        double gdiff = fabs(x.g - y.g);
        double bdiff = fabs(x.b - y.b);
        epsillon = 10e-9;
        if(fabs(x.magnitude()- y.magnitude()) > epsillon || !(x.spatialDataAvailable() && y.spatialDataAvailable())){
          return x.magnitude() < y.magnitude();
        } else{
          return (x.idx_i - y.idx_i) * (x.idx_i - y.idx_i) + (x.idx_j - y.idx_j) * (x.idx_j - y.idx_j) ;
        }
    }
};

std::map<Pixel, Pixel, compareColors> pixel_map;
int threshold = 0;

extern "C"{
  static int l_init (lua_State *L) {
      // pixel_map[5] = 10;
      // pixel_map[15] = 20;
      pixel_map.clear();
      threshold = 10000000;
      lua_pushboolean(L, 1);
      return 1;
  }

  static int l_add(lua_State *L){

      if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)
          && lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6)) {
        //  printf("Entering l_add\n");
          Pixel key = Pixel(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
          if(pixel_map.find(key) != pixel_map.end()){
              // printf("Duplicates found==========================================\n");
          }

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
        //     printf("%lf %lf %lf %lf %lf %lf %lf\n", it->first.r , it->first.g, it->first.b,
        //                                     it->second.r , it->second.g, it->second.b, it->first.h);
        // }
        Pixel key = Pixel(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
        // printf("key %d %d %d %f\n", key.r, key.g, key.b, key.h);
        Pixel value;
        map<Pixel, Pixel>::iterator lower = pixel_map.lower_bound(key);
        map<Pixel, Pixel>::iterator upper = pixel_map.upper_bound(key);
        if(pixel_map.find(key) != pixel_map.end()){
            value = pixel_map[key];
            // printf("Found the needed value\n");
        } else{
            // printf("Not Found the needed value %lf %lf %lf %lf\n", key.r, key.g, key.b, key.h);
            if(lower != pixel_map.end() && upper != pixel_map.end()){
                if(fabs(lower->first.h - key.h) < fabs(upper->first.h - key.h)){
                   value = lower->second;
                } else{
                    value = upper->second;
                }
                // printf("lower is %lf %lf %lf %lf %lf %lf %lf\n", lower->first.r , lower->first.g, lower->first.b,
                //                                     lower->second.r , lower->second.g, lower->second.b, lower->first.h);
                // printf("upper is %lf %lf %lf %lf %lf %lf %lf\n", upper->first.r , upper->first.g, upper->first.b,
                //                                     upper->second.r , upper->second.g, upper->second.b, upper->first.h);
            } else if(upper != pixel_map.end()){
                value = upper->second;
                // printf("upper is %lf %lf %lf %lf %lf %lf %lf\n", upper->first.r , upper->first.g, upper->first.b,
                //                                     upper->second.r , upper->second.g, upper->second.b, upper->first.h);
            } else if(lower != pixel_map.end()){
                value = lower->second;
                // printf("lower is %lf %lf %lf %lf %lf %lf %lf\n", lower->first.r , lower->first.g, lower->first.b,
                //                                     lower->second.r , lower->second.g, lower->second.b, lower->first.h);
            }
        }

        THDoubleTensor * frame = (THDoubleTensor *) luaT_toudata(L, 4, luaT_typenameid(L, "torch.DoubleTensor"));
        THDoubleTensor_resize2d(frame, 3, 1);
        double * dst = THDoubleTensor_data(frame);
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

  static int l_add_all(lua_State *L){
    // printf("Entering add all function\n");
    int height  = lua_tonumber(L, 3);
    int width  = lua_tonumber(L, 4);
    if(pixel_map.size() > threshold){
        return 1;
    }
    THDoubleTensor * content_frame = (THDoubleTensor *) luaT_toudata(L, 1, luaT_typenameid(L, "torch.DoubleTensor"));
    THDoubleTensor_resize3d(content_frame, 3, height, width);
    double * content_data = THDoubleTensor_data(content_frame);

    THDoubleTensor * style_frame = (THDoubleTensor *) luaT_toudata(L, 2, luaT_typenameid(L, "torch.DoubleTensor"));
    THDoubleTensor_resize3d(style_frame, 3, height, width);
    double * style_data = THDoubleTensor_data(style_frame);
    std::map<Pixel, int, compareColors> counter;
    int totalSize = height * width;
    std::map<Pixel, Pixel, compareColors> tmp;
    for(int i = 0; i < height; i++){
      for(int j = 0; j < width; j++){
          int idx =  i * width + j;
          double r1 = content_data[idx];
          double g1 = content_data[idx + totalSize];
          double b1 = content_data[idx + 2 * totalSize];
          double r2 = style_data[idx];
          double g2 = style_data[idx + totalSize];
          double b2 = style_data[idx + 2 * totalSize];
          Pixel key = Pixel(r1, g1, b1);
          Pixel value = Pixel(r2, g2, b2);
          if(tmp.find(key) != tmp.end()){
              Pixel currVal = tmp[key];
              Pixel new_val = currVal + value;
              counter[key] = counter[key] + 1;
              tmp[key] = new_val;
          }else{
              counter[key] = 1;
              tmp[key] = value;
          }
      }
    }
    // printf("size of map is %d\n", tmp.size());
    for(map<Pixel, Pixel>::iterator it = tmp.begin(); it != tmp.end(); it++){
        // printf("unnormalized value equals %lf %lf %lf %d\n", it->second.r, it->second.g, it->second.b, counter[it->first]);
        Pixel norm_val = it->second / counter[it->first];
        // printf("normalized value equals %lf %lf %lf\n", norm_val.r, norm_val.g, norm_val.b);
        pixel_map[it->first] = norm_val;
    }
    return 1;
  }

  static Pixel get(double r, double g, double b){
    Pixel key = Pixel(r, g, b);
    Pixel value;
    map<Pixel, Pixel>::iterator lower = pixel_map.lower_bound(key);
    map<Pixel, Pixel>::iterator upper = pixel_map.upper_bound(key);
    if(pixel_map.find(key) != pixel_map.end()){
        value = pixel_map.at(key);
        // printf("Found the needed value\n");
    } else{
        // printf("Not Found the needed value %lf %lf %lf %lf\n", key.r, key.g, key.b, key.h);
        if(lower != pixel_map.end() && upper != pixel_map.end()){
            if(fabs(lower->first.h - key.h) < fabs(upper->first.h - key.h)){
               value = lower->second;
            } else{
                value = upper->second;
            }
            // printf("lower is %lf %lf %lf %lf %lf %lf %lf\n", lower->first.r , lower->first.g, lower->first.b,
            //                                     lower->second.r , lower->second.g, lower->second.b, lower->first.h);
            // printf("upper is %lf %lf %lf %lf %lf %lf %lf\n", upper->first.r , upper->first.g, upper->first.b,
            //                                     upper->second.r , upper->second.g, upper->second.b, upper->first.h);
        } else if(upper != pixel_map.end()){
            value = upper->second;
            // printf("upper is %lf %lf %lf %lf %lf %lf %lf\n", upper->first.r , upper->first.g, upper->first.b,
            //                                     upper->second.r , upper->second.g, upper->second.b, upper->first.h);
        } else if(lower != pixel_map.end()){
            value = lower->second;
            // printf("lower is %lf %lf %lf %lf %lf %lf %lf\n", lower->first.r , lower->first.g, lower->first.b,
            //                                     lower->second.r , lower->second.g, lower->second.b, lower->first.h);
        }
    }

    return value;
  }
  static int l_get_all(lua_State *L){
        // printf("Entering get all\n");
        int height = lua_tonumber(L, 3);
        int width  = lua_tonumber(L, 4);
        // printf("the width is %d the height is %d\n", width, height);
        THDoubleTensor * content_frame = (THDoubleTensor *) luaT_toudata(L, 1, luaT_typenameid(L, "torch.DoubleTensor"));
        THDoubleTensor_resize3d(content_frame, 3, height, width);
        double * content_data = THDoubleTensor_data(content_frame);

        THDoubleTensor * style_frame = (THDoubleTensor *) luaT_toudata(L, 2, luaT_typenameid(L, "torch.DoubleTensor"));
        THDoubleTensor_resize3d(style_frame, 3, height, width);
        // THDoubleTensor_resizezd(style_frame, 3, 1);
        double * style_data = THDoubleTensor_data(style_frame);
        int totalSize = height * width;
        // style_data[0] = style_data[1] =  style_data[2] = 0.55555555;
        // return 1;
#pragma omp parallel for shared(style_data, content_data, height, width) collapse(2)
        for(int i = 0; i < height; i++){
          for(int j = 0; j < width; j++){
              int idx =  i * width + j;
              double r1 = content_data[idx];
              double g1 = content_data[idx + totalSize];
              double b1 = content_data[idx + 2 * totalSize];
              Pixel value = get(r1, g1, b1);
              style_data[idx] = value.r;
              style_data[idx + totalSize] = value.g;
              style_data[idx + 2 * totalSize] = value.b;
              // printf("Indices %d %d %d %d %d\n", i, j, idx, idx + totalSize, idx + 2 * totalSize);
              // style_data[idx] = style_data[idx + totalSize] = style_data[idx + 2 * totalSize] = 0.5555555;
          }
        }
        // for(int i = 0; i < height; i++){
        //   for(int j = 0; j < width; j++){
        //       int idx =  i * width + height;
        //       printf("%lf %lf %lf\n", style_data[idx], style_data[idx + totalSize], style_data[idx + 2 * totalSize]);
        //   }
        // }
        return 1;
  }

  static int l_clear(lua_State *L){
      pixel_map.clear();
      return 1;
  }


  // Register functions in LUA
  static const struct luaL_reg map_fun [] = {
      {"init"             , l_init},
      {"add"              , l_add},
      {"add_all"          , l_add_all},
      {"get"              , l_get},
      {"get_all"          , l_get_all},
      {"clear"          , l_clear},
      {NULL, NULL}  /* sentinel */
  };

  int luaopen_libmap (lua_State *L) {
      luaL_openlib(L, "libmap", map_fun, 0);
      return 1;
  }
}
