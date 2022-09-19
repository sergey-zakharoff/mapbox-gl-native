/*
    cGraphicNode implementation
    Sergey Zakharov, 2022
*/

#include "graphicNode.hpp"


void GraphicNode::render() 
{
    for (auto child: m_children){
        child->render();
    }
}

void GraphicNode::addChild(std::shared_ptr<GraphicNode> child)
{
    m_children.push_back(child);
}

std::shared_ptr<GraphicNode> GraphicNode::getChild(size_t num)
{
    if (m_children.size() <= num)
        return m_children[num];

    return nullptr;
}

size_t GraphicNode::getChildredCount() const
{
    return m_children.size();
}

void GraphicNode::removeChildren()
{
    m_children.clear();
}

std::shared_ptr<GraphicNode> GraphicNode::parent()
{
    return m_parent.lock();
}

void GraphicNode::setParent(std::weak_ptr<GraphicNode> parent)
{
    m_parent = parent;
}

void GraphicNode::setModel(std::shared_ptr<GraphicGeometry> model)
{
    m_geometry = model;
}
