#include "CDActionInitialization.hh"
#include "CDEventAction.hh"
#include "CDPrimaryGeneratorAction.hh"
#include "CDRunAction.hh"


CDActionInitialization::CDActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

CDActionInitialization::~CDActionInitialization() = default;

void CDActionInitialization::BuildForMaster() const
{
  SetUserAction(new CDRunAction(foutname));
}

void CDActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new CDPrimaryGeneratorAction());
  auto event = new CDEventAction;
  SetUserAction(event);
  SetUserAction(new CDRunAction(foutname));
}
