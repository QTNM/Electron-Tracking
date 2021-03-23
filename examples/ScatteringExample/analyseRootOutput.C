#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>
#include <vector>

//  use output TTree for summary
void summary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader myreader("Score", fin);
  TTreeReaderValue<double> kine(myreader, "KinE");
  TTreeReaderValue<double> time(myreader, "Time");
  TTreeReaderValue<double> btime(myreader, "ExitTime");
  TTreeReaderValue<int>    tid(myreader, "HitID");
  TTreeReaderValue<int>    pid(myreader, "ParentID");
  TTreeReaderValue<int>    btid(myreader, "ExitID");

  // event loop
  int nev = 0;
  while (myreader.Next())
  {
    std::cout << "<<< event " << nev << std::endl;

    if (*tid>0) { // primary particle has id=1
      std::cout << "track id: " << *tid << std::endl;
      std::cout << "kinetic energy: " << *kine << std::endl;
      std::cout << "global time: " << *time << std::endl;
    }
    if (*btid>0) {
        std::cout << "stopwatch: " << *btime << std::endl;
    }
    nev++;
  }
}
