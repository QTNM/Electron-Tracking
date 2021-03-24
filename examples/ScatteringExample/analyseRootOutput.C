#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>

//  use output TTree for short summary
void shortsummary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
  TTreeReaderValue<int>    evid1(nt1, "EventID");
  TTreeReaderValue<double> kine(nt1, "KinE");
  TTreeReaderValue<double> time(nt1, "Time");
  TTreeReaderValue<int>    tid(nt1, "HitID");
  TTreeReaderValue<int>    pid(nt1, "ParentID");
   
  TTreeReader nt2("Watch", fin);
  TTreeReaderValue<int>    evid2(nt2, "EventID");
  TTreeReaderValue<double> btime(nt2, "ExitTime");
  TTreeReaderValue<int>    btid(nt2, "ExitID");
      
  // event loop
  int counterh = 0;
  int counterw = 0;
  int prevev   = -1;
  while (nt1.Next())
  {
    if (*evid1 != prevev) { // primary particle has id=1
      counterh++;
      prevev = *evid1;
    }
  }
  prevev = -1;
  while (nt2.Next())
  {
    if (*evid2 != prevev) {
      counterw++;
      prevev = *evid2;
    }
  }
  std::cout << "counted gas hits: " << counterh << std::endl;
  std::cout << "counted watch hits: " << counterw << std::endl;
}

//  use output TTree for summary
void summary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
  TTreeReaderValue<int>    evid1(nt1, "EventID");
  TTreeReaderValue<double> kine(nt1, "KinE");
  TTreeReaderValue<double> time(nt1, "Time");
  TTreeReaderValue<int>    tid(nt1, "HitID");
  TTreeReaderValue<int>    pid(nt1, "ParentID");

  TTreeReader nt2("Watch", fin);
  TTreeReaderValue<int>    evid2(nt2, "EventID");
  TTreeReaderValue<double> btime(nt2, "ExitTime");
  TTreeReaderValue<int>    btid(nt2, "ExitID");

  // event loop
  while (nt1.Next())
  {
    if (*tid>0) { // primary particle has id=1
      std::cout << "<<< event " << *evid1 << std::endl;
      std::cout << "track id: " << *tid << std::endl;
      std::cout << "kinetic energy: " << *kine << std::endl;
      std::cout << "global time: " << *time << std::endl;
    }
  }
  while (nt2.Next())
  {
    if (*btid>0) {
      std::cout << "<<< event " << *evid2 << std::endl;
      std::cout << "stopwatch: " << *btime << std::endl;
    }
  }
}
