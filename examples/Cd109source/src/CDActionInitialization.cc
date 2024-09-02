#include "CDActionInitialization.hh"
#include "CDEventAction.hh"
#include "CDPrimaryGeneratorAction.hh"
#include "CDRunAction.hh"


CDActionInitialization::CDActionInitialization(G4String name, CDDetectorConstruction* detector)
: G4VUserActionInitialization()
, foutname(std::move(name))
, _detector(detector)
{}

CDActionInitialization::~CDActionInitialization() = default;

void CDActionInitialization::BuildForMaster() const
{
  SetUserAction(new CDRunAction(foutname));
}

void CDActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new CDPrimaryGeneratorAction(_detector));
  auto event = new CDEventAction;
  SetUserAction(event);
  SetUserAction(new CDRunAction(foutname));
}
