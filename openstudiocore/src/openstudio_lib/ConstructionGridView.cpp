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

#include "ConstructionGridView.hpp"

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

ConstructionGridView::ConstructionGridView(bool isIP, const model::Model & model, QWidget * parent)
  : QWidget(parent),
    m_isIP(isIP)
{
  auto mainLayout = new QVBoxLayout();
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(mainLayout);

  auto constructionSets = model.getModelObjects<model::DefaultConstructionSet>();
  auto constructionSetModelObjects = subsetCastVector<model::ModelObject>(constructionSets);

  auto constructionSetGridController = new ConstructionGridController(m_isIP, "Construction Sets", IddObjectType::OS_DefaultConstructionSet, model, constructionSetModelObjects);
  auto gridView = new OSGridView(constructionSetGridController, "Construction Sets", "Drop\nZone", false, parent);

  // Load Filter

  QLabel * label = nullptr;

  QVBoxLayout * layout = nullptr;

  bool isConnected = false;
  
  auto filterGridLayout = new QGridLayout();
  filterGridLayout->setContentsMargins(7, 4, 0, 8);
  filterGridLayout->setSpacing(5);

  label = new QLabel();
  label->setText("Filter:");
  label->setObjectName("H2");
  filterGridLayout->addWidget(label, filterGridLayout->rowCount(), 0, Qt::AlignTop | Qt::AlignLeft);

  layout = new QVBoxLayout();

  m_filterLabel = new QLabel();
  m_filterLabel->setText("Load Type");
  m_filterLabel->setObjectName("H3");
  layout->addWidget(m_filterLabel, Qt::AlignTop | Qt::AlignLeft);

  m_filters = new QComboBox();
  m_filters->setFixedWidth(OSItem::ITEM_WIDTH);

 
  disableFilter();
  layout->addWidget(m_filters, Qt::AlignTop | Qt::AlignLeft);

  layout->addStretch();

  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, 1);

  filterGridLayout->setRowStretch(filterGridLayout->rowCount(), 100);
  filterGridLayout->setColumnStretch(filterGridLayout->columnCount(), 100);

  mainLayout->addStretch(1);

  // GridController

  m_gridController = constructionSetGridController;
  
  OS_ASSERT(m_gridController);
  isConnected = connect(m_filters, &QComboBox::currentTextChanged, m_gridController, &openstudio::ConstructionGridController::filterChanged);
  OS_ASSERT(isConnected);

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

std::vector<model::ModelObject> ConstructionGridView::selectedObjects() const
{
  const auto os = m_gridController->getObjectSelector()->getSelectedObjects();

  return std::vector<model::ModelObject>(os.cbegin(), os.cend());
}

void ConstructionGridView::enableFilter()
{
  m_filterLabel->setEnabled(true);
  m_filters->setEnabled(true);
}

void ConstructionGridView::disableFilter()
{
  m_filterLabel->setEnabled(false);
  m_filters->setEnabled(false);
}

void ConstructionGridView::onDropZoneItemClicked(OSItem* item)
{
}

ConstructionGridController::ConstructionGridController(bool isIP,
  const QString & headerText,
  IddObjectType iddObjectType,
  model::Model model,
  std::vector<model::ModelObject> modelObjects) :
  OSGridController(isIP, headerText, iddObjectType, model, modelObjects)
{
  setCategoriesAndFields();
}

void ConstructionGridController::setCategoriesAndFields()
{

  {
    std::vector<QString> fields;
    fields.push_back(WALLS_EXTERIOR);
    fields.push_back(FLOORS_EXTERIOR);
    fields.push_back(ROOFS);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Exterior Surface Constructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(WALLS_INTERIOR);
    fields.push_back(FLOORS_INTERIOR);
    fields.push_back(CEILINGS_INTERIOR);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Interior Surface Constructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(WALLS_GROUND);
    fields.push_back(FLOORS_GROUND);
    fields.push_back(CEILINGS_GROUND);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Ground Contact Surface Constructions"), fields);
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
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Exterior Sub Surface Constructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;

    fields.push_back(FIXEDWINDOWS_INTERIOR);
    fields.push_back(OPERABLEWINDOWS_INTERIOR);
    fields.push_back(DOORS_INTERIOR);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Interior Sub Surface Constructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(SPACESHADING);
    fields.push_back(BUILDINGSHADING);
    fields.push_back(SITESHADING);
    fields.push_back(INTERIORPARTITIONS);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("Other Constructions"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();

}

void ConstructionGridController::filterChanged(const QString & text)
{
}

ConstructionGridView * ConstructionGridController::constructionGridView()
{
  auto gridView = qobject_cast<OSGridView *>(this->parent());
  OS_ASSERT(gridView);

  auto constructionGridView = qobject_cast<ConstructionGridView *>(gridView->parent());

  return constructionGridView;
}

void ConstructionGridController::categorySelected(int index)
{
  OSGridController::categorySelected(index);
}

void ConstructionGridController::addColumns(const QString &category, std::vector<QString> & fields)
{
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  m_baseConcepts.clear();

  for(const auto &field : fields) {

    if ( field == NAME ) {
      auto getter = CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::name);
      auto setter = CastNullAdapter<model::DefaultConstructionSet>(&model::DefaultConstructionSet::setName);

      addNameLineEditColumn(Heading(QString(NAME), false, false),
        false,
        false,
        getter,
        setter);

    } else if (field == SELECTED && category != "Loads") {
      auto checkbox = QSharedPointer<QCheckBox>(new QCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &QCheckBox::stateChanged, this, &ConstructionGridController::selectAllStateChanged);

      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");

    }
    else if (field == WALLS_EXTERIOR){
      addDropZoneColumn(Heading(QString(WALLS_EXTERIOR)),
        CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::wallConstruction),
        CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::setWallConstruction),
        boost::optional<std::function<void(model::DefaultSurfaceConstructions*)>>(CastNullAdapter<model::DefaultSurfaceConstructions>(&model::DefaultSurfaceConstructions::resetWallConstruction)));
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString ConstructionGridController::getColor(const model:: ModelObject & modelObject)
{
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  // TODO: The code below is currently commented out because a refresh crash bug is precluding rack color
  // updates due to rack assignments to cases and walk-ins.  No colors are better than wrong colors.

  //std::vector<model::RefrigerationSystem> refrigerationSystems = m_model.getModelObjects<model::RefrigerationSystem>();

  //boost::optional<model::SpaceType> refrigerationCase = modelObject.optionalCast<model::SpaceType>();
  //OS_ASSERT(refrigerationCase);

  //boost::optional<model::RefrigerationSystem> refrigerationSystem = refrigerationCase->system();
  //if(!refrigerationSystem){
  //  return color;
  //}

  //std::vector<model::RefrigerationSystem>::iterator it;
  //it = std::find(refrigerationSystems.begin(), refrigerationSystems.end(), refrigerationSystem.get());
  //if(it != refrigerationSystems.end()){
  //  int index = std::distance(refrigerationSystems.begin(), it);
  //  if(index >= static_cast<int>(m_colors.size())){
  //    index = m_colors.size() - 1; // similar to scheduleView's approach
  //  }
  //  color = this->m_colors.at(index).name();
  //}

  return color;
}

void ConstructionGridController::checkSelectedFields()
{
  if(!this->m_hasHorizontalHeader) return;
  
  OSGridController::checkSelectedFields();
}

void ConstructionGridController::onItemDropped(const OSItemId& itemId)
{
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject){
    if (modelObject->optionalCast<model::DefaultSurfaceConstructions>()){
      modelObject->clone(m_model);
      emit modelReset();
    }
  }
}

void ConstructionGridController::refreshModelObjects()
{
  std::vector<model::DefaultSurfaceConstructions> spaceTypes = m_model.getModelObjects<model::DefaultSurfaceConstructions>();
  m_modelObjects = subsetCastVector<model::ModelObject>(spaceTypes);
  std::sort(m_modelObjects.begin(), m_modelObjects.end(), ModelObjectNameSorter());
}

void ConstructionGridController::onComboBoxIndexChanged(int index)
{
}

} // openstudio
