#pragma once

namespace Core
{

class ComponentSystem
{
public:
    //ComponentSystem();
    //~ComponentSystem();

    void Connect();

protected:
    virtual void OnAwake() {}
    virtual void OnStart() {}

    virtual void OnUpdate() {}
    virtual void OnLateUpdate() {}

    virtual void OnEnable() {}
    virtual void OnDisable() {}

    virtual void OnRender() {}
    virtual void OnGUIRender() {}

private:
    void HandleAwake() {
        OnAwake();
    }

    void HandleStart() {
        OnStart();
    }

    void HandleUpdate() {
        OnUpdate();
    }

    void HandleLateUpdate() {
        OnLateUpdate();
    }

    void HandleEnable() {
        OnEnable();
    }

    void HandleDisable() {
        OnDisable();
    }

    void HandleRender() {
        OnRender();
    }

    void HandleGUIRender() {
        OnGUIRender();
    }
};

}