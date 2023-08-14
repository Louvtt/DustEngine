#ifndef _DUST_CORE_LAYER_HPP_
#define _DUST_CORE_LAYER_HPP_

#include <string>

namespace dust {

class Layer
{
private:
    std::string m_name;

public:
    Layer(std::string name);
    virtual ~Layer() = default;

    virtual void update() = 0;
    virtual void preRender() = 0;
    virtual void render() = 0;
    virtual void postRender() = 0;

    std::string getName() const;
};

class DebugLayer
: public Layer
{
public:
    DebugLayer();
    virtual ~DebugLayer();

    void update() override;
    void preRender() override;
    virtual void render() override;
    void postRender() override;
};

}

#endif //_DUST_CORE_LAYER_HPP_