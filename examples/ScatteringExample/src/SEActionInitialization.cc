#include "SEActionInitialization.hh"
#include "SEEventAction.hh"
#include "SEPrimaryGeneratorAction.hh"
#include "SERunAction.hh"


SEActionInitialization::SEActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

SEActionInitialization::~SEActionInitialization() = default;

void SEActionInitialization::BuildForMaster() const
{
  auto event = new SEEventAction;
  SetUserAction(new SERunAction(event, foutname));
}

void SEActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new SEPrimaryGeneratorAction());
  auto event = new SEEventAction;
  SetUserAction(event);
  SetUserAction(new SERunAction(event, foutname));
}
