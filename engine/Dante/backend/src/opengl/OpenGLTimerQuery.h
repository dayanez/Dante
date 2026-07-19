
#ifndef TNT_DANTE_BACKEND_OPENGL_TIMERQUERY_H
#define TNT_DANTE_BACKEND_OPENGL_TIMERQUERY_H

#include "DriverBase.h"
#include "gl_headers.h"

#include <backend/DriverEnums.h>

#include <utils/AsyncJobQueue.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <stdint.h>

namespace dante::backend {

class OpenGLPlatform;
class OpenGLContext;
class OpenGLDriver;
class TimerQueryFactoryInterface;

struct GLTimerQuery : public HwTimerQuery {
    struct State {
        struct {
            GLuint query;
        } gl;
        int64_t then{};
        std::atomic<int64_t> elapsed{};
    };
    std::shared_ptr<State> state;
};

/*
 * We need two implementation of timer queries (only elapsed time), because
 * on some gpu disjoint_timer_query/arb_timer_query is much less accurate than
 * using fences.
 *
 * These classes implement the various strategies...
 */

class TimerQueryFactory {
public:
    static TimerQueryFactoryInterface* init(
            OpenGLPlatform& platform, OpenGLContext& context);
};

class TimerQueryFactoryInterface {
protected:
    using GLTimerQuery = dante::backend::GLTimerQuery;
    using clock = std::chrono::steady_clock;

public:
    virtual ~TimerQueryFactoryInterface();
    virtual void createTimerQuery(GLTimerQuery* query) = 0;
    virtual void destroyTimerQuery(GLTimerQuery* query) = 0;
    virtual void beginTimeElapsedQuery(GLTimerQuery* query) = 0;
    virtual void endTimeElapsedQuery(OpenGLDriver& driver, GLTimerQuery* query) = 0;

    static TimerQueryResult getTimerQueryValue(GLTimerQuery* tq, uint64_t* elapsedTime) noexcept;
};

#if defined(BACKEND_OPENGL_VERSION_GL) || defined(GL_EXT_disjoint_timer_query)

class TimerQueryNativeFactory final : public TimerQueryFactoryInterface {
public:
    explicit TimerQueryNativeFactory(OpenGLContext& context);
    ~TimerQueryNativeFactory() override;
private:
    void createTimerQuery(GLTimerQuery* query) override;
    void destroyTimerQuery(GLTimerQuery* query) override;
    void beginTimeElapsedQuery(GLTimerQuery* query) override;
    void endTimeElapsedQuery(OpenGLDriver& driver, GLTimerQuery* query) override;
    OpenGLContext& mContext;
};

#endif

class TimerQueryFenceFactory final : public TimerQueryFactoryInterface {
public:
    explicit TimerQueryFenceFactory(OpenGLPlatform& platform);
    ~TimerQueryFenceFactory() override;
private:
    void createTimerQuery(GLTimerQuery* query) override;
    void destroyTimerQuery(GLTimerQuery* query) override;
    void beginTimeElapsedQuery(GLTimerQuery* tq) override;
    void endTimeElapsedQuery(OpenGLDriver& driver, GLTimerQuery* tq) override;

    OpenGLPlatform& mPlatform;
    utils::AsyncJobQueue mJobQueue;
};

class TimerQueryFallbackFactory final : public TimerQueryFactoryInterface {
public:
    explicit TimerQueryFallbackFactory();
    ~TimerQueryFallbackFactory() override;
private:
    void createTimerQuery(GLTimerQuery* query) override;
    void destroyTimerQuery(GLTimerQuery* query) override;
    void beginTimeElapsedQuery(GLTimerQuery* query) override;
    void endTimeElapsedQuery(OpenGLDriver& driver, GLTimerQuery* query) override;
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_TIMERQUERY_H
