import ROOT


def summary(fname="qtnm.root"):
    inputFile = ROOT.TFile(fname)
    score_tree = inputFile.Get("Score")
    watch_tree = inputFile.Get("Watch")

    for i in range(score_tree.GetEntries()):
        score_tree.GetEntry(i)
        print('Event: %d' % score_tree.EventID)
        print('Track ID: %d' % score_tree.HitID)
        print('Deposited Energy: %.6E' % score_tree.Edep)
        print('Kinetic Energy pre: %.6F' % score_tree.KinE1)
        print('Kinetic Energy post: %.6F' % score_tree.KinE2)
        print('Global Time: %.10F' % score_tree.Time)

    for i in range(watch_tree.GetEntries()):
        watch_tree.GetEntry(i)
        print('Event: %d' % watch_tree.EventID)
        print('Track ID: %d' % watch_tree.ExitID)
        print('Stopwatch: %.10F' % watch_tree.ExitTime)


def summary_TTreeReader(fname="qtnm.root"):
    inputFile = ROOT.TFile(fname)
    nt1 = ROOT.TTreeReader("Score", inputFile)
    nt2 = ROOT.TTreeReader("Watch", inputFile)

    EventID = ROOT.TTreeReaderValue(int)(nt1, "EventID")
    # "double" as a type also works here
    Edep = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Edep")
    KinE1 = ROOT.TTreeReaderValue(ROOT.double)(nt1, "KinE1")
    KinE2 = ROOT.TTreeReaderValue(ROOT.double)(nt1, "KinE2")
    time = ROOT.TTreeReaderValue(ROOT.double)(nt1, "Time")
    HitID = ROOT.TTreeReaderValue(int)(nt1, "HitID")
    ParentID = ROOT.TTreeReaderValue(int)(nt1, "ParentID")

    while nt1.Next():
        print('Event: %d' % EventID.Get()[0])
        print('Track ID: %d' % HitID.Get()[0])
        print('Deposited Energy: %.6E' % Edep.Get()[0])
        print('Kinetic Energy pre: %.6F' % KinE1.Get()[0])
        print('Kinetic Energy post: %.6F' % KinE2.Get()[0])
        print('Global Time: %.10F' % time.Get()[0])

    EventID2 = ROOT.TTreeReaderValue(int)(nt2, "EventID")
    HitID2 = ROOT.TTreeReaderValue(int)(nt2, "HitID")
    time2 = ROOT.TTreeReaderValue(ROOT.double)(nt2, "Time")

    while nt2.Next():
        print('Event: %d' % EventID2.Get()[0])
        print('Track ID: %d' % HitI2.Get()[0])
        print('Stopwatch: %.10F' % time2.Get()[0])


if __name__ == "__main__":
    summary()
