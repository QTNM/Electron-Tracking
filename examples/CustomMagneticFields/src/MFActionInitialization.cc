#include "MFActionInitialization.hh"
#include "MFEventAction.hh"
#include "MFPrimaryGeneratorAction.hh"
#include "MFRunAction.hh"


MFActionInitialization::MFActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

MFActionInitialization::~MFActionInitialization() = default;

void MFActionInitialization::BuildForMaster() const
{
  SetUserAction(new MFRunAction(foutname));
}

void MFActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new MFPrimaryGeneratorAction());
  auto event = new MFEventAction;
  SetUserAction(event);
  SetUserAction(new MFRunAction(foutname));
}
