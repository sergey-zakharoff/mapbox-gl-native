/*
    SightsManager implementation 
    Sergey Zakharov, 2022
*/
#include "sights_manager.hpp"
#include "point_of_interest_layer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <fstream>

namespace {
    const std::string assetsRootDir {MAPBOX_PUCK_ASSETS_PATH };
}

struct mapPointDescription 
{
    std::string name;
    double lat = 0.;
    double lon = 0.;
    int zoom = 50;
    std::string model;
};

SightsManager::SightsManager()
{
    loadData();
}

SightsManager::~SightsManager()
{
    m_sights.clear();
}

void SightsManager::next(mbgl::Map* map)
{
    assert(map);
    size_t nextSight = currentSight + 1;
    if (nextSight >= m_sights.size())
        nextSight = 0;
    mbgl::Log::Info(mbgl::Event::General, "jumping to: %d, size %d", nextSight, m_sights.size());

    jumpTo(nextSight, map);
}

void SightsManager::toggle(mbgl::Map* map)
{
    assert(map);

    using namespace mbgl;
    using namespace mbgl::style;

    auto &style = map->getStyle();
    auto layer = style.getLayer(layerName());
    if (!layer) {
        // if no one layer, create and add one 
        style.addLayer(std::make_unique<CustomLayer>(layerName(), std::make_unique<PointOfInterestLayer>()));
    }
    else {
        // if layer already exists, but invisible, try to show it
        if (layer->getVisibility() == VisibilityType::None){
            mbgl::Log::Info(mbgl::Event::General, "Exists but invisible");
            layer->setVisibility(VisibilityType::Visible);
        }
        else {
            // view is exists and visible, thus, user wants to toggle it off
            style.removeLayer(layerName());
            return;
        }
    }
    jumpTo(currentSight, map);
}

std::string SightsManager::layerName()
{
    return "poiLayer";
}

bool SightsManager::jumpTo(size_t nextPoint, mbgl::Map* map)
{
    assert(map);

    if (nextPoint >= m_sights.size()){
        mbgl::Log::Info(mbgl::Event::General, "No sight with this index.");

        return false;
    }

    auto& sight = *m_sights[nextPoint];
    
    mbgl::CameraOptions cameraOptions;
    cameraOptions.center = mbgl::LatLng{sight.lat, sight.lon};
    cameraOptions.zoom = sight.zoom;
    cameraOptions.pitch = 0;
    cameraOptions.bearing = 0;
    map->jumpTo(cameraOptions);    

    currentSight = nextPoint;
    return true;
}

void SightsManager::loadData()
{
    auto path = assetsRootDir + "sights.json";
    auto pointsTag = "points";

    mbgl::Log::Info(mbgl::Event::General, "loading sights: %s", path.c_str());

    std::fstream ifs(path);

    rapidjson::Document doc;
    rapidjson::IStreamWrapper rapidWrapper(ifs);

    doc.ParseStream(rapidWrapper);
    if (!doc.IsObject()) {
        mbgl::Log::Info(mbgl::Event::General, "cannot parse file");
        return;
    }

    if (!doc.HasMember(pointsTag) || !doc[pointsTag].IsArray()){
        mbgl::Log::Info(mbgl::Event::General, "bad format");
        return;
    }

    const rapidjson::Value& points = doc[pointsTag];
    for (rapidjson::SizeType i = 0; i < points.Size(); i++) {
        const rapidjson::Value& point = points[i];
        if (!point.HasMember("lat") || 
            !point.HasMember("lon") || 
            !point.HasMember("zoom") ||
            !point.HasMember("model_name") ||
            !point.HasMember("display_name"))
                continue;

        auto sight = std::make_unique<mapPointDescription>();

        sight->lat = point["lat"].GetDouble();
        sight->lon = point["lon"].GetDouble();
        sight->zoom = point["zoom"].GetInt();
        sight->model = point["model_name"].GetString();
        sight->name = point["display_name"].GetString();

        mbgl::Log::Info(mbgl::Event::General, "loaded place: %s", sight->name.c_str());

        m_sights.push_back(std::move(sight));
    }
}
