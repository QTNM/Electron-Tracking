#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>

//  use output TTree for short summary
void shortsummary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
   
  std::cout << "counted gas hits: " << nt1.GetEntries() << std::endl;
}

//  use output TTree for summary
void summary(TString fname) {
  if (fname.IsNull()) fname = "qtnm.root";
  TFile *fin = new TFile(fname.Data(), "READ");
  TTreeReader nt1("Score", fin);
  TTreeReaderValue<int>    evid(nt1, "EventID");
  TTreeReaderValue<double> edep(nt1, "Edep");
  TTreeReaderValue<double> kine(nt1, "Kine");
  TTreeReaderValue<double> px(nt1, "Px");
  TTreeReaderValue<double> py(nt1, "Py");
  TTreeReaderValue<double> pz(nt1, "Pz");
  TTreeReaderValue<int>    tid(nt1, "TrackID");

  // event loop
  TVector3 ref(0.0,0.0,1.0); // z-direction reference
  TVector3 data;
  while (nt1.Next())
  {
    std::cout << "<<< event " << *evid << std::endl;
    std::cout << "track id: " << *tid << std::endl;
    std::cout << "deposited energy: " << *edep << " [keV]" << std::endl;
    std::cout << "post step kinetic energy: " << *kine << " [keV]" << std::endl;
    data.SetXYZ(*px, *py, *pz);
    std::cout << "momentum angle to ref: " << ref.Angle(data) << std::endl;
  }
}


//  analyse for angle range; +- angle in degree
void analyse(std::string fnamelist, double angle) {
  std::vector<std::string> rootfiles;
  std::string fname;
  ifstream infile; // read list of root files from file
  infile.open(fnamelist.data());
  if (!infile) {
    cout << "Error file list " << fnamelist.data() << endl;
    exit(1);
  }
  while (infile >> fname)
    rootfiles.push_back(fname);
  infile.close();

  TChain* chScore = new TChain("Score");
  for (auto fn : rootfiles) 
    chScore->Add(fn.data());

  TTreeReader nt1(chScore);
  TTreeReaderValue<int>    evid(nt1, "EventID");
  TTreeReaderValue<double> edep(nt1, "Edep");
  TTreeReaderValue<double> kine(nt1, "Kine");
  TTreeReaderValue<double> px(nt1, "Px");
  TTreeReaderValue<double> py(nt1, "Py");
  TTreeReaderValue<double> pz(nt1, "Pz");
  TTreeReaderValue<int>    tid(nt1, "TrackID");

  // event loop
  double scatangle;
  double anglerad = angle * TMath::DegToRad();
  TH1D* hk = new TH1D("energy","Kinetic energy [keV]", 100, 0.0, 20.0);
  TVector3 ref(0.0,0.0,1.0); // z-direction reference
  TVector3 data;
  while (nt1.Next())
  {
    data.SetXYZ(*px, *py, *pz);
    scatangle = ref.Angle(data);
    if (scatangle >= TMath::PiOver2()-anglerad && scatangle<=TMath::PiOver2()+anglerad)
      hk->Fill(*kine);
  }
  std::cout << "Entries: " << hh->GetEntries() << std::endl;
  hk->DrawCopy();
  
}
