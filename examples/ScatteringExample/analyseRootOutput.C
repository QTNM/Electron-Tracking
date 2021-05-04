#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>

//  use output TTree for short summary
void shortsummary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
   
  TTreeReader nt2("Watch", fin);
      
  std::cout << "counted gas hits: " << nt1.GetEntries() << std::endl;
  std::cout << "counted watch hits: " << nt2.GetEntries() << std::endl;
}

//  use output TTree for summary
void summary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
  TTreeReaderValue<int>    evid1(nt1, "EventID");
  TTreeReaderValue<double> edep(nt1, "Edep");
  TTreeReaderValue<double> kine(nt1, "KinE");
  TTreeReaderValue<double> time(nt1, "Time");
  TTreeReaderValue<int>    tid(nt1, "HitID");
  TTreeReaderValue<int>    pid(nt1, "ParentID");

  TTreeReader nt2("Watch", fin);
  TTreeReaderValue<int>    evid2(nt2, "EventID");
  TTreeReaderValue<double> btime(nt2, "ExitTime");
  TTreeReaderValue<double> bxpos(nt2, "Posx");
  TTreeReaderValue<double> bypos(nt2, "Posy");
  TTreeReaderValue<int>    btid(nt2, "ExitID");

  // event loop
  while (nt1.Next())
  {
    std::cout << "<<< event " << *evid1 << std::endl;
    std::cout << "track id: " << *tid << std::endl;
    std::cout << "deposited energy: " << *edep << std::endl;
    std::cout << "kinetic energy post : " << *kine << std::endl;
    std::cout << "global time: " << *time << std::endl;
  }
  while (nt2.Next())
  {
    std::cout << "<<< event " << *evid2 << std::endl;
    std::cout << "track id : " << *btid << std::endl;
    std::cout << "stopwatch: " << *btime << std::endl;
    std::cout << "x pos: " << *bxpos << std::endl;
    std::cout << "y pos: " << *bypos << std::endl;
  }
}
