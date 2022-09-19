/*
    class represented graphical object in a scene 
    Sergey Zakharov, 2022
*/
#include "graphicGeometry.hpp"

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/math.hpp>

#include <vector>
#include <memory>

class GraphicNode {
public:
    GraphicNode() = default;
    virtual ~GraphicNode() = default;

    // renders itself and downgrown graph
    void render();

    // adds new child node
    void addChild(std::shared_ptr<GraphicNode> child);
    
    // returns child with index num, returns nullptr if no found
    std::shared_ptr<GraphicNode> getChild(size_t num);

    // returns count of the child nodes
    size_t getChildredCount() const;

    // removes all child nodes
    void removeChildren();

    // returns parent node, or nullptr if detached
    std::shared_ptr<GraphicNode> parent();

    // sets renderable geometry buffer
    void setModel(std::shared_ptr<GraphicGeometry> model);
    
protected:
    // sets parent node, can replace existing parent ot null it
    void setParent(std::weak_ptr<GraphicNode> parent);

private:
    // local transform
    mbgl::mat4 m_transform;

    // graph data
    std::vector<std::shared_ptr<GraphicNode>> m_children;
    std::weak_ptr<GraphicNode> m_parent;

    // buffer for render (can be empty, if this node used as pure group)
    std::shared_ptr<GraphicGeometry> m_geometry;
};