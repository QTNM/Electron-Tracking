#include "PEActionInitialization.hh"
#include "PEEventAction.hh"
#include "PEPrimaryGeneratorAction.hh"
#include "PERunAction.hh"


PEActionInitialization::PEActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

PEActionInitialization::~PEActionInitialization() = default;

void PEActionInitialization::BuildForMaster() const
{
  SetUserAction(new PERunAction(foutname));
}

void PEActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new PEPrimaryGeneratorAction());
  auto event = new PEEventAction;
  SetUserAction(event);
  SetUserAction(new PERunAction(foutname));
}
