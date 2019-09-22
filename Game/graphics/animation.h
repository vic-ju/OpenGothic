#pragma once

#include <zenload/modelScriptParser.h>

class Animation final {
  public:
    enum Flags : uint32_t {
      None         = 0,
      Move         = 0x00000001,
      Rotate       = 0x00000002,
      QueueAnim    = 0x00000004,
      Fly          = 0x00000008,
      Idle         = 0x00000010
      };

    enum AnimClass : uint8_t {
      UnknownAnim=0,
      Transition,
      Loop
      };

    struct EvCount final {
      uint8_t def_opt_frame=0;
      uint8_t def_draw=0;
      uint8_t def_undraw=0;
      };

    struct Sequence final {
      Sequence(const std::string& name);

      bool                                   isMove() const { return bool(flags&Flags::Move); }
      bool                                   isFly()  const { return bool(flags&Flags::Fly);  }
      bool                                   isFinished(uint64_t t) const;
      bool                                   isAtackFinished(uint64_t t) const;
      bool                                   isParWindow(uint64_t t) const;
      float                                  totalTime() const;
      void                                   processEvents(uint64_t barrier, uint64_t sTime, uint64_t now, EvCount& ev) const;

      std::string                            name;
      float                                  fpsRate=60.f;
      uint32_t                               numFrames=0;
      uint32_t                               firstFrame=0;
      uint32_t                               lastFrame =0;
      uint32_t                               layer     =0;
      Flags                                  flags=Flags::None;
      AnimClass                              animCls=UnknownAnim;
      ZMath::float3                          translate={};

      std::vector<ZenLoad::zCModelAniSample> samples;
      std::vector<uint32_t>                  nodeIndex;

      std::string                            nextStr;
      const Sequence*                        next=nullptr;
      ZenLoad::zCModelAniSample              moveTr={};

      std::vector<ZMath::float3 >            tr;

      std::vector<ZenLoad::zCModelScriptEventSfx> sfx, gfx;
      std::vector<ZenLoad::zCModelEvent>          tag;
      std::vector<ZenLoad::zCModelEvent>          events;

      std::vector<uint64_t>                       defHitEnd;   // hit-end time
      std::vector<uint64_t>                       defOptFrame; // damage commitment timings
      std::vector<uint64_t>                       defParFrame; // block timings
      std::vector<uint64_t>                       defDraw;     // draw-weapon sound
      std::vector<uint64_t>                       defUndraw;   // undraw sound

      ZMath::float3                          translation(uint64_t dt) const;
      ZMath::float3                          speed(uint64_t at, uint64_t dt) const;
      ZMath::float3                          translateXZ(uint64_t at) const;

      private:
        void setupMoveTr();
      };

    Animation(ZenLoad::MdsParser &p, const std::string &name, bool ignoreErrChunks);

    const Sequence *sequence(const char* name) const;
    void            debug() const;

  private:
    std::vector<Sequence> sequences;
    Sequence& loadMAN(const std::string &name);
    void setupIndex();
  };
