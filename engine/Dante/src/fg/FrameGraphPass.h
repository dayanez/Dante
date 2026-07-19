
#ifndef TNT_DANTE_FG_FRAMEGRAPHPASS_H
#define TNT_DANTE_FG_FRAMEGRAPHPASS_H

#include "fg/FrameGraphResources.h"

#include <backend/DriverApiForward.h>

#include <utils/Allocator.h>

#include <type_traits>

namespace dante {

class FrameGraphPassExecutor {
    friend class FrameGraph;
    friend class PassNode;
    friend class RenderPassNode;

protected:
    virtual void execute(FrameGraphResources const& resources, backend::DriverApi& driver) noexcept = 0;

public:
    FrameGraphPassExecutor() noexcept = default;
    virtual ~FrameGraphPassExecutor() noexcept;
    FrameGraphPassExecutor(FrameGraphPassExecutor const&) = delete;
    FrameGraphPassExecutor& operator = (FrameGraphPassExecutor const&) = delete;
};

class FrameGraphPassBase : protected FrameGraphPassExecutor {
    friend class FrameGraph;
    friend class PassNode;
    friend class RenderPassNode;
    PassNode* mNode = nullptr;
    void setNode(PassNode* node) noexcept { mNode = node; }
    PassNode const& getNode() const noexcept { return *mNode; }

public:
    using FrameGraphPassExecutor::FrameGraphPassExecutor;
    ~FrameGraphPassBase() noexcept override;
};

template<typename Data, typename Execute>
class FrameGraphPass final : public FrameGraphPassBase {
    friend class FrameGraph;

    // allow our allocators to instantiate us
    template<typename, typename, typename, typename>
    friend class utils::Arena;

    explicit FrameGraphPass(Execute&& execute) noexcept
            : mExecute(std::move(execute)) {
    }

    void execute(FrameGraphResources const& resources,
            backend::DriverApi& driver) noexcept override {
        // execute can omit the DriverApi parameter
        if constexpr (std::is_invocable_v<Execute, FrameGraphResources const&, Data&>) {
            mExecute(resources, mData);
        } else {
            mExecute(resources, mData, driver);
        }
    }

    Data mData;
    Execute mExecute;

public:
    Data const& getData() const noexcept { return mData; }
    Data const* operator->() const { return &mData; }
};

} // namespace dante

#endif //TNT_DANTE_FG_FRAMEGRAPHPASS_H
