#include "BIActionInitialization.hh"
#include "BIEventAction.hh"
#include "BIPrimaryGeneratorAction.hh"
#include "BIRunAction.hh"


BIActionInitialization::BIActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

BIActionInitialization::~BIActionInitialization() = default;

void BIActionInitialization::BuildForMaster() const
{
  SetUserAction(new BIRunAction(foutname));
}

void BIActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new BIPrimaryGeneratorAction());
  auto event = new BIEventAction;
  SetUserAction(event);
  SetUserAction(new BIRunAction(foutname));
}
