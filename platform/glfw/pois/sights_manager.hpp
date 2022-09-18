/*
    class for managing sights
    Sergey Zakharov, 2022
*/
#include <string>
#include <vector>
#include <memory>

namespace mbgl {
class Map;
}
struct mapPointDescription;

class SightsManager {
public:
    // lifetime
    SightsManager();
    ~SightsManager();

    // jumps to the next sight
    void next(mbgl::Map* map);

    // toggles sights on and off depending of current state
    void toggle(mbgl::Map* map);

private:
    // custom layer ID
    static std::string layerName();

    // loads sight places descriptions from resource file
    void loadData();

    // jumps to the sight with mextPoint index for map
    // returns true if transition was occured
    bool jumpTo(size_t nextPoint, mbgl::Map* map);

    std::vector<std::unique_ptr<mapPointDescription> > m_sights;
    size_t currentSight = 0;
};
