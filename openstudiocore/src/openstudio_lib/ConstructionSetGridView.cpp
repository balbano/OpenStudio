/**********************************************************************
*  Copyright (c) 2008-2015, Alliance for Sustainable Energy.
*  All rights reserved.
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#include "ConstructionSetGridView.hpp"

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelSubTabView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"

#include "../shared_gui_components/OSGridView.hpp"

#include "../model/ComponentData.hpp"
#include "../model/ConstructionBase.hpp"
#include "../model/ConstructionBase_Impl.hpp"
#include "../model/DefaultConstructionSet.hpp"
#include "../model/DefaultConstructionSet_Impl.hpp"
#include "../model/DefaultSubSurfaceConstructions.hpp"
#include "../model/DefaultSubSurfaceConstructions_Impl.hpp"
#include "../model/DefaultSurfaceConstructions.hpp"
#include "../model/DefaultSurfaceConstructions_Impl.hpp"
#include "../model/Model_Impl.hpp"

#include "../utilities/core/Assert.hpp"

#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/OS_DefaultConstructionSet_FieldEnums.hxx>
#include <utilities/idd/OS_DefaultSubSurfaceConstructions_FieldEnums.hxx>
#include <utilities/idd/OS_DefaultSurfaceConstructions_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QSettings>
#include <QTimer>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Construction Set Name"
#define SELECTED "All"

// EXTERIORSURFACECONSTRUCTIONS
#define WALLS_EXTERIOR "Walls"
#define FLOORS_EXTERIOR "Floors"
#define ROOFS "Roofs"

// INTERIORSURFACECONSTRUCTIONS
#define WALLS_INTERIOR "Walls"
#define FLOORS_INTERIOR "Floors"
#define CEILINGS_INTERIOR "Ceilings"

// GROUNDCONTACTSURFACECONSTRUCTIONS
#define WALLS_GROUND "Walls"
#define FLOORS_GROUND "Floors"
#define CEILINGS_GROUND "Ceilings"

// EXTERIORSUBSURFACECONSTRUCTIONS
#define FIXEDWINDOWS_EXTERIOR "Fixed Windows"
#define OPERABLEWINDOWS_EXTERIOR "Operable Windows"
#define DOORS_EXTERIOR "Doors"
#define GLASSDOORS "Glass Doors"
#define OVERHEADDOORS "Overhead Doors"
#define SKYLIGHTS "Skylights"
#define TUBULARDAYLIGHTDOMES "Tubular Daylight Domes"
#define TUBULARDAYLIGHTDIFFUSERS "Tubular Daylight Diffusers"

// INTERIORSUBSURFACECONSTRUCTIONS
#define FIXEDWINDOWS_INTERIOR "Fixed Windows"
#define OPERABLEWINDOWS_INTERIOR "Operable Windows"
#define DOORS_INTERIOR "Doors"

// OTHERCONSTRUCTIONS
#define SPACESHADING "Space Shading"
#define BUILDINGSHADING "Building Shading"
#define SITESHADING "Site Shading"
#define INTERIORPARTITIONS "Interior Partitions"

namespace openstudio {

  struct ModelObjectNameSorter{
    // sort by name
    bool operator()(const model::ModelObject & lhs, const model::ModelObject & rhs){
      return (lhs.name() < rhs.name());
    }
  };

  ConstructionSetGridView::ConstructionSetGridView(bool isIP, const model::Model & model, QWidget * parent)
    : QWidget(parent),
    m_isIP(isIP)
  {
    auto mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    auto constructionSets = model.getModelObjects<model::DefaultConstructionSet>();
    auto constructionSetModelObjects = subsetCastVector<model::ModelObject>(constructionSets);

    auto constructionSetGridController = new ConstructionSetGridController(m_isIP, "Construction Sets", IddObjectType::OS_DefaultConstructionSet, model, constructionSetModelObjects);
    auto gridView = new OSGridView(constructionSetGridController, "Construction Sets", "Drop\nZone", false, parent);

    QLabel * label = nullptr;

    QVBoxLayout * layout = nullptr;

    bool isConnected = false;

    gridView->m_contentLayout->addSpacing(7);

    mainLayout->addWidget(gridView, 0, Qt::AlignTop);

    mainLayout->addStretch(1);

    // GridController

    m_gridController = constructionSetGridController;

    isConnected = connect(gridView, SIGNAL(dropZoneItemClicked(OSItem*)), this, SIGNAL(dropZoneItemClicked(OSItem*)));
    OS_ASSERT(isConnected);

    isConnected = connect(this, SIGNAL(itemSelected(OSItem *)), gridView, SIGNAL(itemSelected(OSItem *)));
    OS_ASSERT(isConnected);

    isConnected = connect(this, SIGNAL(selectionCleared()), gridView, SLOT(onSelectionCleared()));
    OS_ASSERT(isConnected);

    isConnected = connect(gridView, SIGNAL(gridRowSelected(OSItem*)), this, SIGNAL(gridRowSelected(OSItem*)));
    OS_ASSERT(isConnected);

    isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), constructionSetGridController, SIGNAL(toggleUnitsClicked(bool)));
    OS_ASSERT(isConnected);

    isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), constructionSetGridController, SLOT(toggleUnits(bool)));
    OS_ASSERT(isConnected);

    std::vector<model::DefaultSurfaceConstructions> defaultSurfaceConstructions = model.getModelObjects<model::DefaultSurfaceConstructions>(); // NOTE for horizontal system lists
  }

  std::vector<model::ModelObject> ConstructionSetGridView::selectedObjects() const
  {
    const auto os = m_gridController->getObjectSelector()->getSelectedObjects();

    return std::vector<model::ModelObject>(os.cbegin(), os.cend());
  }

  void ConstructionSetGridView::onDropZoneItemClicked(OSItem* item)
  {
  }

  ConstructionSetGridController::ConstructionSetGridController(bool isIP,
    const QString & headerText,
    IddObjectType iddObjectType,
    model::Model model,
    std::vector<model::ModelObject> modelObjects) :
    OSGridController(isIP, headerText, iddObjectType, model, modelObjects)
  {
    setCategoriesAndFields();
  }

  void ConstructionSetGridController::setCategoriesAndFields()
  {

    {
      std::vector<QString> fields;
      fields.push_back(WALLS_EXTERIOR);
      fields.push_back(FLOORS_EXTERIOR);
      fields.push_back(ROOFS);
      std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Exterior\nSurface\nConstructions"), fields);
      m_categoriesAndFields.push_back(categoryAndFields);
    }

  {
    std::vector<QString> fields;
    fields.push_back(WALLS_INTERIOR);
    fields.push_back(FLOORS_INTERIOR);
    fields.push_back(CEILINGS_INTERIOR);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Interior\nSurface\nConstructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(WALLS_GROUND);
    fields.push_back(FLOORS_GROUND);
    fields.push_back(CEILINGS_GROUND);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Ground Contact\nSurface\nConstructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(FIXEDWINDOWS_EXTERIOR);
    fields.push_back(OPERABLEWINDOWS_EXTERIOR);
    fields.push_back(DOORS_EXTERIOR);
    fields.push_back(GLASSDOORS);
    fields.push_back(OVERHEADDOORS);
    fields.push_back(SKYLIGHTS);
    fields.push_back(TUBULARDAYLIGHTDOMES);
    fields.push_back(TUBULARDAYLIGHTDIFFUSERS);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Exterior\nSub Surface\nConstructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;

    fields.push_back(FIXEDWINDOWS_INTERIOR);
    fields.push_back(OPERABLEWINDOWS_INTERIOR);
    fields.push_back(DOORS_INTERIOR);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Interior\nSub Surface\nConstructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(SPACESHADING);
    fields.push_back(BUILDINGSHADING);
    fields.push_back(SITESHADING);
    fields.push_back(INTERIORPARTITIONS);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Other\nConstructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();

  }

  ConstructionSetGridView * ConstructionSetGridController::constructionSetGridView()
  {
    auto gridView = qobject_cast<OSGridView *>(this->parent());
    OS_ASSERT(gridView);

    auto constructionSetGridView = qobject_cast<ConstructionSetGridView *>(gridView->parent());

    return constructionSetGridView;
  }

  void ConstructionSetGridController::categorySelected(int index)
  {
    OSGridController::categorySelected(index);
  }

  void ConstructionSetGridController::addColumns(const QString &category, std::vector<QString> & fields)
  {
    // always show name and selected columns
    fields.insert(fields.begin(), { NAME, SELECTED });

    m_baseConcepts.clear();

    for (const auto &field : fields) {

      if (field == NAME) {
        auto getter = CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::name);
        auto setter = CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setName);
        addNameLineEditColumn(Heading(QString(NAME), false, false),
          false,
          false,
          getter,
          setter);
      }
      else if (field == SELECTED && category != "Loads") {
        auto checkbox = QSharedPointer<QCheckBox>(new QCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &QCheckBox::stateChanged, this, &ConstructionSetGridController::selectAllStateChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
      }

      else if (field == WALLS_EXTERIOR){
        addDropZoneColumn(Heading(QString(WALLS_EXTERIOR)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::wallConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setWallConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetWallConstruction)));
      }
      else if (field == FLOORS_EXTERIOR){
        addDropZoneColumn(Heading(QString(FLOORS_EXTERIOR)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::floorConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setFloorConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetFloorConstruction)));
      }
      else if (field == ROOFS){
        addDropZoneColumn(Heading(QString(ROOFS)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::roofCeilingConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setRoofCeilingConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetRoofCeilingConstruction)));
      }

      else if (field == WALLS_INTERIOR){
        addDropZoneColumn(Heading(QString(WALLS_INTERIOR)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::wallConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setWallConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetWallConstruction)));
      }
      else if (field == FLOORS_INTERIOR){
        addDropZoneColumn(Heading(QString(FLOORS_INTERIOR)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::floorConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setFloorConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetFloorConstruction)));
      }
      else if (field == CEILINGS_INTERIOR){
        addDropZoneColumn(Heading(QString(CEILINGS_INTERIOR)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::roofCeilingConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setRoofCeilingConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetRoofCeilingConstruction)));
      }
      
      else if (field == WALLS_GROUND){
        addDropZoneColumn(Heading(QString(WALLS_GROUND)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::wallConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setWallConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetWallConstruction)));
      }
      else if (field == FLOORS_GROUND){
        addDropZoneColumn(Heading(QString(FLOORS_GROUND)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::floorConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setFloorConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetFloorConstruction)));
      }
      else if (field == CEILINGS_GROUND){
        addDropZoneColumn(Heading(QString(CEILINGS_GROUND)),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::roofCeilingConstruction),
          CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setRoofCeilingConstruction),
          boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetRoofCeilingConstruction)));
      }
      
      else if (field == FIXEDWINDOWS_EXTERIOR){
        addDropZoneColumn(Heading(QString(FIXEDWINDOWS_EXTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::fixedWindowConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setFixedWindowConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetFixedWindowConstruction)));
      }
      else if (field == OPERABLEWINDOWS_EXTERIOR){
        addDropZoneColumn(Heading(QString(OPERABLEWINDOWS_EXTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::operableWindowConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setOperableWindowConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetOperableWindowConstruction)));
      }
      else if (field == DOORS_EXTERIOR){
        addDropZoneColumn(Heading(QString(DOORS_EXTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::doorConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setDoorConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetDoorConstruction)));
      }
      else if (field == GLASSDOORS){
        addDropZoneColumn(Heading(QString(GLASSDOORS)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::glassDoorConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setGlassDoorConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetGlassDoorConstruction)));
      }
      else if (field == OVERHEADDOORS){
        addDropZoneColumn(Heading(QString(OVERHEADDOORS)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::overheadDoorConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setOverheadDoorConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetOverheadDoorConstruction)));
      }
      else if (field == SKYLIGHTS){
        addDropZoneColumn(Heading(QString(SKYLIGHTS)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::skylightConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setSkylightConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetSkylightConstruction)));
      }
      else if (field == TUBULARDAYLIGHTDOMES){
        addDropZoneColumn(Heading(QString(TUBULARDAYLIGHTDOMES)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::tubularDaylightDomeConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setTubularDaylightDomeConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetTubularDaylightDomeConstruction)));
      }
      else if (field == TUBULARDAYLIGHTDIFFUSERS){
        addDropZoneColumn(Heading(QString(TUBULARDAYLIGHTDIFFUSERS)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::tubularDaylightDiffuserConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setTubularDaylightDiffuserConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetTubularDaylightDiffuserConstruction)));
      }
      
      else if (field == FIXEDWINDOWS_INTERIOR){
        addDropZoneColumn(Heading(QString(FIXEDWINDOWS_INTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::fixedWindowConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setFixedWindowConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetFixedWindowConstruction)));
      }
      else if (field == OPERABLEWINDOWS_INTERIOR){
        addDropZoneColumn(Heading(QString(OPERABLEWINDOWS_INTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::operableWindowConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setOperableWindowConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetOperableWindowConstruction)));
      }
      else if (field == DOORS_INTERIOR){
        addDropZoneColumn(Heading(QString(DOORS_INTERIOR)),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::doorConstruction),
          CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::setDoorConstruction),
          boost::optional<std::function<void(model::DefaultSubSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSubSurfaceConstructions>(&model::DefaultSubSurfaceConstructions::resetDoorConstruction)));
      }

      else if (field == SPACESHADING){
        addDropZoneColumn(Heading(QString(SPACESHADING)),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::spaceShadingConstruction),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setSpaceShadingConstruction),
          boost::optional<std::function<void(model::DefaultConstructionSet*)>>(CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::resetSpaceShadingConstruction)));
      }
      else if (field == BUILDINGSHADING){
        addDropZoneColumn(Heading(QString(BUILDINGSHADING)),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::buildingShadingConstruction),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setBuildingShadingConstruction),
          boost::optional<std::function<void(model::DefaultConstructionSet*)>>(CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::resetBuildingShadingConstruction)));
      }
      else if (field == SITESHADING){
        addDropZoneColumn(Heading(QString(SITESHADING)),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::siteShadingConstruction),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setSiteShadingConstruction),
          boost::optional<std::function<void(model::DefaultConstructionSet*)>>(CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::resetSiteShadingConstruction)));
      }
      else if (field == INTERIORPARTITIONS){
        addDropZoneColumn(Heading(QString(INTERIORPARTITIONS)),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::interiorPartitionConstruction),
          CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setInteriorPartitionConstruction),
          boost::optional<std::function<void(model::DefaultConstructionSet*)>>(CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::resetInteriorPartitionConstruction)));
      }

      else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }

  QString ConstructionSetGridController::getColor(const model::ModelObject & modelObject)
  {
    QColor defaultColor(Qt::lightGray);
    QString color(defaultColor.name());
    return color;
  }

  void ConstructionSetGridController::checkSelectedFields()
  {
    if (!this->m_hasHorizontalHeader) return;

    OSGridController::checkSelectedFields();
  }

  void ConstructionSetGridController::onItemDropped(const OSItemId& itemId)
  {
    boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
    if (modelObject){
      if (modelObject->optionalCast<model::DefaultSurfaceConstructions>()){
        modelObject->clone(m_model);
        emit modelReset();
      }
    }
  }

  void ConstructionSetGridController::refreshModelObjects()
  {
    std::vector<model::DefaultSurfaceConstructions> spaceTypes = m_model.getModelObjects<model::DefaultSurfaceConstructions>();
    m_modelObjects = subsetCastVector<model::ModelObject>(spaceTypes);
    std::sort(m_modelObjects.begin(), m_modelObjects.end(), ModelObjectNameSorter());
  }

  void ConstructionSetGridController::onComboBoxIndexChanged(int index)
  {
  }

} // openstudio
