import ROOT

def shortsummary(fname='qtnm.root'):
    inputFile = ROOT.TFile(fname)
    nt1 = ROOT.TTreeReader("Score", inputFile)

    print('Counted gas hits: %d' % nt1.GetEntries())


def summary(fname='qtnm.root'):
    inputFile = ROOT.TFile(fname)
    nt1 = ROOT.TTreeReader("Score", inputFile)

    evid = ROOT.TTreeReaderValue(ROOT.int)(nt1, "EventID")
    edep = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Edep")
    kine = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Kine")
    px = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Px")
    py = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Py")
    pz = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Pz")
    tid = ROOT.TTreeReaderValue(ROOT.int)(nt1, "TrackID")

    ref = ROOT.TVector3(0.0, 0.0, 1.0)
    data = ROOT.TVector3()

    while nt1.Next():
        print('<<< event: %d' % evid.Get()[0])
        print('track id: %d' % tid.Get()[0])
        print('deposited energy: %.6f [keV]' % edep.Get()[0])
        print('post step kinetic energy: %.6f [keV]' % kine.Get()[0])
        data.SetXYZ(px.Get()[0], py.Get()[0], pz.Get()[0])
        print('momentum angle to ref: %.6f' % ref.Angle(data))


def analyse(fnamelist, angle):
    # Extract list of files
    try:
        with open(fnamelist, 'r') as flist:
            filelist = flist.readlines()
    except IOError:
        print('Failed to open file: %s' % fnamelist)
        return -1
    # Allow the user to pass in a list(like) argument
    except TypeError:
        filelist = list(fnamelist)
    # A (likely) cause of a UnicodeDecodeError is that the user has passed
    # the name of a root file directly
    except UnicodeDecodeError:
        try:
            # Not clear why this doesn't -raise- an exception if file
            # isn't valid ROOT file
            # Directly calling the constructor to TFile doesn't work either
            inputFile = ROOT.TFile.Open(fnamelist)
            inputFile.Get('')
            filelist = [fnamelist]
        except ReferenceError:
            print('Could not parse %s as either a text file or a ROOT file' % 'aaa')
            return -1


    chScore = ROOT.TChain("Score")

    for f in filelist:
        chScore.Add(f)

    nt1 = ROOT.TTreeReader(chScore)

    evid = ROOT.TTreeReaderValue(ROOT.int)(nt1, "EventID")
    edep = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Edep")
    kine = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Kine")
    px = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Px")
    py = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Py")
    pz = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Pz")
    tid = ROOT.TTreeReaderValue(ROOT.int)(nt1, "TrackID")

    # Loop over events
    anglerad = angle * ROOT.TMath.DegToRad()
    hk = ROOT.TH1D("energy", "Kinetic energy [keV]", 100, 0.0, 20.0)

    ref = ROOT.TVector3(0.0, 0.0, 1.0)
    data = ROOT.TVector3()
    anglerad = angle * ROOT.TMath.DegToRad()

    while nt1.Next():
        data.SetXYZ(px.Get()[0], py.Get()[0], pz.Get()[0])
        scatangle = ref.Angle(data)
        if (ROOT.TMath.PiOver2() - anglerad <= scatangle
            <= ROOT.TMath.PiOver2() + anglerad):
            hk.Fill(kine.Get()[0])


    print('Entries: %.6d' % hk.GetEntries())

    hk.DrawCopy()
    # Appears to be needed to display plot
    ROOT.gPad.Update()

if __name__ == "__main__":
    shortsummary()
    analyse('qtnm.root', 10)
