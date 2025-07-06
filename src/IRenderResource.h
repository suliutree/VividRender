#pragma once
struct IRenderResource {
    virtual ~IRenderResource() = default;
    // 必须在持有当前线程 Context 的情况下调用
    virtual void initializeGL() = 0;
};