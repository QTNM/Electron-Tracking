#include "EGActionInitialization.hh"
#include "EGEventAction.hh"
#include "EGPrimaryGeneratorAction.hh"
#include "EGRunAction.hh"


EGActionInitialization::EGActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

EGActionInitialization::~EGActionInitialization() = default;

void EGActionInitialization::BuildForMaster() const
{
  SetUserAction(new EGRunAction(foutname));
}

void EGActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new EGPrimaryGeneratorAction());
  auto event = new EGEventAction;
  SetUserAction(event);
  SetUserAction(new EGRunAction(foutname));
}
