#include "applicationmodel.h"

ApplicationModel::ApplicationModel(QObject* parent) : QObject(parent)
{
  update_settings(new Settings());
  get_settings()->load();

  m_labyrinthModel.reset(new LabyrinthModel);
  m_roomModel.reset(new RoomModel);
  m_connectionModel.reset(new ConnectionModel);
  m_plannedRouteModel.reset(new PlannedRouteModel);
  m_goldenDoorModel.reset(new GoldenDoorModel);
  m_instructionModel.reset(new InstructionModel);
  m_planSummaryModel.reset(new PlanSummaryModel);

  m_newVersionAvailable = false;
  m_labMapOutdated = true;
  m_logFileOpen = true;
  m_inLab = false;
  m_currentRoomDetermined = true;
  m_isValid = false;
}

bool ApplicationModel::loadFromFile(const QString& file)
{
  LabyrinthData newLabyrinthData;
  if (newLabyrinthData.loadFromFile(file)) {
    labyrinthData = newLabyrinthData;
    update_labMapOutdated(labyrinthData.date != QDateTime::currentDateTimeUtc().date());
    updatePlanData(PlanData());

    update_inLab(false);
    update_isValid(true);
    resetModels();
    return true;
  }
  return false;
}

void ApplicationModel::updatePlanData(const PlanData& planData)
{
  this->planData = planData;
  NavigationData navigationData;
  navigationData.loadFromData(&labyrinthData, planData);
  navigationData.updatePlannedRouteAndInstructions();

  get_planSummaryModel()->loadFromData(navigationData);
  updateNavigationData(navigationData);
}

void ApplicationModel::updateNavigationData(const NavigationData& navigationData)
{
  this->navigationData = navigationData;
  update_currentRoomDetermined(navigationData.currentRoomDetermined);
  updateModels();
}

void ApplicationModel::resetModels()
{
  get_labyrinthModel()->loadFromData(labyrinthData);
  get_roomModel()->loadFromData(labyrinthData, navigationData);
  get_connectionModel()->loadFromData(labyrinthData, navigationData);
  get_goldenDoorModel()->loadFromData(labyrinthData);
  get_plannedRouteModel()->loadFromData(navigationData);
  get_instructionModel()->loadFromData(navigationData);
}

void ApplicationModel::updateModels()
{
  get_roomModel()->updateFromData(navigationData);
  get_connectionModel()->updateFromData(navigationData);
  get_plannedRouteModel()->loadFromData(navigationData);
  get_instructionModel()->loadFromData(navigationData);
}
