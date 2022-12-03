#include "stdafx.h"
#include "Boat.h"
#include "Boom.h"
#include "MainsheetCommand.h"

void MainsheetCommand::Execute()
{
  m_pBoat->Boom.setMaxAngleSpeed(m_value);
}