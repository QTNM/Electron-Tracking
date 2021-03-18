#ifndef SEPSTime_h
#define SEPSTime_h 1

#include "G4THitsMap.hh"
#include "G4VPrimitiveScorer.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring global step time
//
///////////////////////////////////////////////////////////////////////////////

class SEPSTime : public G4VPrimitiveScorer
{
public:                                        // with description
  SEPSTime(G4String name, G4int depth = 0);  // default unit
  SEPSTime(G4String name, const G4String& unit, G4int depth = 0);
  virtual ~SEPSTime();

protected:  // with description
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
  virtual void Initialize(G4HCofThisEvent*);
  virtual void EndOfEvent(G4HCofThisEvent*);
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();

  virtual void SetUnit(const G4String& unit);

private:
  G4int                          HCID;
  G4THitsMap<G4double>*          EvtMap;

};
#endif
