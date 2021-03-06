#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <array>
#include <memory>
#include <Tempest/Point>

#include "utils/workers.h"

template<class T>
class SpaceIndex final {
  public:
    SpaceIndex()=default;

    void clear() {
      arr.clear();
      index.clear();
      }

    template<typename... Args>
    void emplace_back(Args&&... args) {
      index.clear();
      arr.emplace_back(std::forward<Args>(args)...);
      }

          T& operator[](size_t i)       { return arr[i]; }
    const T& operator[](size_t i) const { return arr[i]; }

    size_t size() const { return arr.size(); }

    T*    begin()        { return arr.data();            }
    T*    end()          { return arr.data()+arr.size(); }

    auto  begin()  const { return arr.data();     }
    auto  end()    const { return arr.data()+arr.size(); }

    auto& back() { return arr.back(); }

    void  pop_back() { arr.pop_back(); index.clear(); }

    void buildIndex() {
      index.resize(arr.size());
      for(size_t i=0;i<index.size();++i){
        index[i] = &arr[i];
        }
      std::sort(index.begin(),index.end(),[](const T* a,const T* b){
        return X(a) < X(b);
        });
      }

    template<class Func>
    void find(const Tempest::Vec3& p,float R,const Func& f) {
      return find(p.x,p.y,p.z,R,f);
      }

    template<class Func>
    void find(float x,float y,float z,float R,Func& f) {
      if(index.size()==0)
        buildIndex();

      auto s = std::lower_bound(index.begin(),index.end(),x-R,[](const T* a,float b){
        return X(a) < b;
        });
      auto e = std::upper_bound(index.begin(),index.end(),x+R,[](float b,const T* a){
        return b < X(a);
        });

      auto RQ = R*R;
      size_t count = std::distance(s,e); (void)count;
      for(auto i=s;i!=e;++i) {
        T* t = *i;
        if( qDist(x,y,z,t)<RQ ){
          if( f(**i) )
            return;
          }
        }
      }

    template<class F>
    void parallelFor(F func){
      Workers::parallelFor(arr,std::forward(func));
      }

  private:
    std::vector<T>  arr;
    std::vector<T*> index;

    template<class U>
    static float X(const std::unique_ptr<U>* t) {
      return X(t->get());
      }

    template<class U>
    static float qDist(float x,float y,float z,const std::unique_ptr<U>* t) {
      return qDist(x,y,z,t->get());
      }

    template<class U>
    static float X(const U* t) {
      return t->position().x;
      }

    template<class U>
    static float qDist(float x,float y,float z,const U* t) {
      return (Tempest::Vec3(x,y,z)-t->position()).quadLength();
      }
  };
