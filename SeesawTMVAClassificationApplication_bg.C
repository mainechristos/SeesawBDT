/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVAClassificationApplication                                      *
 *                                                                                *
 * This macro provides a simple example on how to use the trained classifiers     *
 * within an analysis module                                                      *
 **********************************************************************************/
#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "/cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars.C"
#include "/cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars2.C"
#include "/cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars3.C"

//#include "TMVA/TMVAGui.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

using namespace TMVA;

void runSignal(std::map<std::string,int> Use, TString ifile = "./validation_Dec9.root"){

   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/customFunctions.C+");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/customFudgeFactors_2016.C+");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/matrixMethod_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/leptonSFs_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/pileupWeights_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/triggerEffScaleFactor_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars2.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars3.C");

   TMVA::Tools::Instance();
  

   // --- Create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
      Float_t MuonDXY0, MuonDXY1, MuonDXY2, ElectronDXY0, ElectronDXY1, ElectronDXY2, LightLeptonPhi0, LightLeptonPhi1, LightLeptonPhi2;
   Int_t LightLeptonFlavor, LightLeptonNativeIndex;
   Float_t LightLeptonLT, LightLeptonMT3, LightLeptonPt0, LightLeptonPt1, LightLeptonPt2,  LightLeptonBestMOSSF, PFMETType1;
   Float_t LightLeptonEta0, LightLeptonEta1, LightLeptonEta2, LightLeptonMass, LightLeptonLeptonDRmin0, LightLeptonLeptonDRmin1, LightLeptonLeptonDRmin2;
   Float_t JetN;
   Int_t HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLDZvAccept, HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLDZvAccept, HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLDZvAccept;
   Int_t HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLvAccept;   Int_t HLTMu17TrkIsoVVLMu8TrkIsoVVLDZvAccept, HLTMu17TrkIsoVVLMu8TrkIsoVVLvAccept, HLTMu17TrkIsoVVLTkMu8TrkIsoVVLDZvAccept, HLTMu17TrkIsoVVLTkMu8TrkIsoVVLvAccept, HLTEle23Ele12CaloIdLTrackIdLIsoVLDZvAccept, HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLvAccept;
   Int_t HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLvAccept;
   Int_t LightLeptonN, LightLeptonOnZN, ElectronN, LightLeptonTightType3D, MuonN, PassBadChargedCandidateFilter, PassBadPFMuonFilter;
   Float_t PtRatio1, PtRatio2, PtRatio3, DPhi1, DPhi2, DPhi3, LightLeptonDXY0, LightLeptonDXY1, LightLeptonDXY2; 
    int n = 1;
   cout<<"STILL WORKING :)"<<endl;
   
  //reader->AddVariable("LTplusMET3D := PFMETType1[0]+LightLeptonLT3[0]", &LightLeptonLT);
   //reader->AddVariable("LightLeptonLT := LightLeptonLT3[0]", &LightLeptonLT);
   reader->AddVariable("PFMETType1 := Alt$(PFMETType1[0],0)", &PFMETType1);
   reader->AddVariable("LightLeptonPt0 := Alt$(LightLeptonPt[0],0)", &LightLeptonPt0 );
   reader->AddVariable("LightLeptonPt1 := Alt$(LightLeptonPt[1],0)",&LightLeptonPt1);
   reader->AddVariable("LightLeptonPt2 := Alt$(LightLeptonPt[2],0)",&LightLeptonPt2);
   reader->AddVariable("LightLeptonMT3 := Alt$(LightLeptonMT3[0],0)",&LightLeptonMT3);
   //reader->AddVariable("LightLeptonEta0 := LightLeptonEta[0]", &LightLeptonEta0 );
   //reader->AddVariable("LightLeptonEta1 := LightLeptonEta[1]",&LightLeptonEta1);
   //reader->AddVariable("LightLeptonEta2 := LightLeptonEta[2]",&LightLeptonEta2);
   reader->AddVariable("LightLeptonMass := Alt$(LightLeptonMass[0],0)",&LightLeptonMass);
   //reader->AddVariable("JetN := JetN[0]",&JetN);
   reader->AddVariable("LightLeptonLeptonDRmin0 := Alt$(LightLeptonLeptonDRmin[0],0)", &LightLeptonLeptonDRmin0 );
   reader->AddVariable("LightLeptonBestMOSSF := Alt$(LightLeptonBestMOSSF[0],0)", &LightLeptonBestMOSSF );

   //reader->AddVariable("PtRatio1 := PtRatio(LightLeptonPt[0],LightLeptonPt[1],LightLeptonPt[2])", &PtRatio1);
   //reader->AddVariable("PtRatio2 := PtRatio(LightLeptonPt[1],LightLeptonPt[0],LightLeptonPt[2])", &PtRatio2);
   //reader->AddVariable("PtRatio3 := PtRatio(LightLeptonPt[2],LightLeptonPt[0],LightLeptonPt[1])", &PtRatio3);

   //reader->AddVariable("DPhi1 := DeltaPhi(LightLeptonEta[0],LightLeptonEta[1])", &DPhi1);
   //reader->AddVariable("DPhi2 := DeltaPhi(LightLeptonEta[1],LightLeptonEta[2])", &DPhi2);
   //reader->AddVariable("DPhi3 := DeltaPhi(LightLeptonEta[2],LightLeptonEta[0])", &DPhi3);

   //reader->AddVariable("LightLeptonDXY0 := LightLeptonDXY(0, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",&LightLeptonDXY0);
   //reader->AddVariable("LightLeptonDXY1 := LightLeptonDXY(1, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",&LightLeptonDXY1);
   //reader->AddVariable("LightLeptonDXY2 := LightLeptonDXY(2, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",&LightLeptonDXY2);
   //reader->AddVariable("LightLeptonLeptonDRmin1 := LightLeptonLeptonDRmin[1]",&LightLeptonLeptonDRmin1);
   //reader->AddVariable("LightLeptonLeptonDRmin2 := LightLeptonLeptonDRmin[2]",&LightLeptonLeptonDRmin2);

   
   
   // --- Book the MVA methods

   TString dir    = "/cms/mchristos/ANN/Seesaw/2016/92X/weights/";
   TString prefix = "2017_TMVA_optimized";

   // Book method(s)
   /*for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = TString(it->first) + TString(" method");
         TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader->BookMVA( methodName, weightfile ); 
      }
   }*/

   reader->BookMVA( "BDTG method",  "/cms/mchristos/ANN/Seesaw/2016/92X/weights/2017_TMVA_test_depth_1.weights.xml");

   //make vectors to see passed events
   vector<double>  vecBDTG; 
  
   TFile *input(0);
   TString fname = ifile;   
   input = TFile::Open(fname);

   std::cout << "--- TMVAClassificationApp    : Using input file: " << input->GetName() << std::endl;
   
   // --- Event loop

   // Prepare the event tree
   std::cout << "--- Select Background sample" << std::endl;
   TTree *tree = (TTree*)input->Get("rootTupleTreeVeryLoose/tree");
   cout<<tree->GetEntries()<<endl;
   TTreeReader myReader(tree);
   TH1I *histo= new TH1I();
   histo = (TH1I*)input->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");  
   //histo = (TH1I*)input->Get("dileptonEventFilter/EventCounter"); 
    
 
   TTreeReaderValue<vector<Float_t>> inMuonDXY(myReader, "MuonDXY");
   TTreeReaderValue<vector<Float_t>> inElectronDXY(myReader, "ElectronDXY");
   TTreeReaderValue<vector<Float_t>> inLightLeptonPhi(myReader, "LightLeptonPhi");
   TTreeReaderValue<vector<Int_t>> inLightLeptonFlavor(myReader, "LightLeptonFlavor");
   TTreeReaderValue<vector<Int_t>> inLightLeptonNativeIndex(myReader, "LightLeptonNativeIndex");
   TTreeReaderValue<vector<Float_t>> inLightLeptonBestMOSSF(myReader, "LightLeptonBestMOSSF");
   TTreeReaderValue<vector<Float_t>> inPFMETType1(myReader, "PFMETType1");
   TTreeReaderValue<vector<Float_t>> inLightLeptonPt(myReader, "LightLeptonPt");
   TTreeReaderValue<vector<Float_t>> inLightLeptonEta(myReader, "LightLeptonEta");
   TTreeReaderValue<vector<Int_t>> inJetN(myReader, "JetN");
   TTreeReaderValue<vector<Float_t>> inLightLeptonLeptonDRmin(myReader, "LightLeptonLeptonDRmin");
   TTreeReaderValue<vector<Float_t>> inLightLeptonMass(myReader, "LightLeptonMass");
   TTreeReaderValue<vector<Float_t>> inLightLeptonLT(myReader, "LightLeptonLT3");
   TTreeReaderValue<vector<Float_t>> inLightLeptonMT3(myReader, "LightLeptonMT3");
   TTreeReaderValue<vector<Int_t>> inLightLeptonN(myReader, "LightLeptonN");
   TTreeReaderValue<vector<Int_t>> inLightLeptonOnZN(myReader, "LightLeptonOnZN");
   TTreeReaderValue<vector<Int_t>> inElectronN(myReader, "ElectronN");
   //TTreeReaderValue<vector<Int_t>> inLightLeptonTightType3D(myReader, "LightLeptonTightType3D");
   TTreeReaderValue<vector<Int_t>> inMuonN(myReader, "MuonN");
   TTreeReaderValue<vector<Int_t>> inPassBadChargedCandidateFilter(myReader, "PassBadChargedCandidateFilter");
   TTreeReaderValue<vector<Int_t>> inPassBadPFMuonFilter(myReader, "PassBadPFMuonFilter");


   TFile *endback = new TFile("/cms/mchristos/ANN/Seesaw/2016/92X/outFiles/SeesawTMVAClassificationApplicationVVV/Background/QQQ/Seesaw_BDTG_Background_1.root","RECREATE");   
   
   endback->cd();
   endback->mkdir("veryLooseDileptonEventFilter/");
   endback->cd();
   endback->mkdir("veryLooseDileptonEventFilter/EventCount/");
   endback->cd("veryLooseDileptonEventFilter/EventCount/");

   histo->Write();
   cout<<"STILL WORKING"<<endl;

   endback->cd();

   endback->mkdir("rootTupleTreeVeryLoose/");
   endback->cd("rootTupleTreeVeryLoose/");
   TTree *found_back_tree = tree->CloneTree(0);

   std::vector<double> *BDTResponse = new std::vector<double>(0);
   found_back_tree->Branch("BDT",&BDTResponse);

   std::vector<float> vecVar(4); // vector for EvaluateMVA tests

   std::cout << "--- Processing: " << tree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   int count = 0;
   
   while (myReader.Next()) {

      if (count%1000 == 0) std::cout << "--- ... Processing event: " << count << std::endl;
      
      //This will get entries to fill nonspecified branches in the cloned tree.
      tree->GetEntry(count);
//Float_t MuonDXY0, MuonDXY1, MuonDXY2, ElectronDXY0, ElectronDXY1, ElectronDXY2, LightLeptonPhi0, LightLeptonPhi1, LightLeptonPhi2;
   //Int_t LightLeptonFlavor, LightLeptonNativeIndex;
   
   if(inMuonDXY->size()>=3){
         MuonDXY0 = inMuonDXY->at(0);
         MuonDXY1 = inMuonDXY->at(1);
         MuonDXY2 = inMuonDXY->at(2);
      }else if(inMuonDXY->size()==2){
         MuonDXY0 = inMuonDXY->at(0);
         MuonDXY1 = inMuonDXY->at(1);
         MuonDXY2 = 0;
      }else if(inMuonDXY->size()==1){
         MuonDXY0 = inMuonDXY->at(0);
         MuonDXY1 = 0;
         MuonDXY2 = 0;
      }else{
         MuonDXY0 = 0;
         MuonDXY1 = 0;
         MuonDXY2 = 0;
      }

      if(inElectronDXY->size()>=3){
         ElectronDXY0 = inElectronDXY->at(0);
         ElectronDXY1 = inElectronDXY->at(1);
         ElectronDXY2 = inElectronDXY->at(2);
      }else if(inElectronDXY->size()==2){
         ElectronDXY0 = inElectronDXY->at(0);
         ElectronDXY1 = inElectronDXY->at(1);
         ElectronDXY2 = 0;
      }else if(inElectronDXY->size()==1){
         ElectronDXY0 = inElectronDXY->at(0);
         ElectronDXY1 = 0;
         ElectronDXY2 = 0;
      }else{
         ElectronDXY0 = 0;
         ElectronDXY1 = 0;
         ElectronDXY2 = 0;
      }
      if(inLightLeptonPhi->size()>=3){
         LightLeptonPhi0 = inLightLeptonPhi->at(0);
         LightLeptonPhi1 = inLightLeptonPhi->at(1);
         LightLeptonPhi2 = inLightLeptonPhi->at(2);
      }else if(inLightLeptonPhi->size()==2){
         LightLeptonPhi0 = inLightLeptonPhi->at(0);
         LightLeptonPhi1 = inLightLeptonPhi->at(1);
         LightLeptonPhi2 = 0;
      }else if(inLightLeptonPhi->size()==1){
         LightLeptonPhi0 = inLightLeptonPhi->at(0);
         LightLeptonPhi1 = 0;
         LightLeptonPhi2 = 0;
      }else{
         LightLeptonPhi0 = 0;
         LightLeptonPhi1 = 0;
         LightLeptonPhi2 = 0;
      }
      if(inLightLeptonFlavor->empty()){
         LightLeptonFlavor = 0;
      }else{
         LightLeptonFlavor = inLightLeptonFlavor->at(0);
      }
      if(inLightLeptonNativeIndex->empty()){
         LightLeptonNativeIndex = 0;
      }else{
         LightLeptonNativeIndex = inLightLeptonNativeIndex->at(0);
      }



      //LightLeptonBestMOSSF = inLightLeptonBestMOSSF->at(0);
      if(inLightLeptonBestMOSSF->empty()){
         LightLeptonBestMOSSF = 0;
      }else{
         LightLeptonBestMOSSF = inLightLeptonBestMOSSF->at(0);
      }
      if(inLightLeptonPt->size()>=3){
         LightLeptonPt0 = inLightLeptonPt->at(0);
         LightLeptonPt1 = inLightLeptonPt->at(1);
         LightLeptonPt2 = inLightLeptonPt->at(2);
      }else if(inLightLeptonPt->size()==2){
         LightLeptonPt0 = inLightLeptonPt->at(0);
         LightLeptonPt1 = inLightLeptonPt->at(1);
         LightLeptonPt2 = 0;
      }else if(inLightLeptonPt->size()==1){
         LightLeptonPt0 = inLightLeptonPt->at(0);
         LightLeptonPt1 = 0;
         LightLeptonPt2 = 0;
      }else{
         LightLeptonPt0 = 0;
         LightLeptonPt1 = 0;
         LightLeptonPt2 = 0;
      }
      if(inLightLeptonEta->size()>=3){
         LightLeptonEta0 = inLightLeptonEta->at(0);
         LightLeptonEta1 = inLightLeptonEta->at(1);
         LightLeptonEta2 = inLightLeptonEta->at(2);
      }else if(inLightLeptonEta->size()==2){
         LightLeptonEta0 = inLightLeptonEta->at(0);
         LightLeptonEta1 = inLightLeptonEta->at(1);
         LightLeptonEta2 = 0;
      }else if(inLightLeptonEta->size()==1){
         LightLeptonEta0 = inLightLeptonEta->at(0);
         LightLeptonEta1 = 0;
         LightLeptonEta2 = 0;
      }else{
         LightLeptonEta0 = 0;
         LightLeptonEta1 = 0;
         LightLeptonEta2 = 0;
      }
      
      if(inLightLeptonLeptonDRmin->size()>=3){
         LightLeptonLeptonDRmin0 = inLightLeptonLeptonDRmin->at(0);
         LightLeptonLeptonDRmin1 = inLightLeptonLeptonDRmin->at(1);
         LightLeptonLeptonDRmin2 = inLightLeptonLeptonDRmin->at(2);
      }else if(inLightLeptonLeptonDRmin->size()==2){
         LightLeptonLeptonDRmin0 = inLightLeptonLeptonDRmin->at(0);
         LightLeptonLeptonDRmin1 = inLightLeptonLeptonDRmin->at(1);
         LightLeptonLeptonDRmin2 = 0;
      }else if(inLightLeptonLeptonDRmin->size()==1){
         LightLeptonLeptonDRmin0 = inLightLeptonLeptonDRmin->at(0);
         LightLeptonLeptonDRmin1 = 0;
         LightLeptonLeptonDRmin2 = 0;
      }else{
         LightLeptonLeptonDRmin0 = 0;
         LightLeptonLeptonDRmin1 = 0;
         LightLeptonLeptonDRmin2 = 0;
      }
      if(inJetN->empty()){
         JetN = 0;
      }else{
         JetN = inJetN->at(0);
      }
      //cout<<"STILL WORKING :)"<<endl;
      PFMETType1 = inPFMETType1->at(0);
      LightLeptonMass = inLightLeptonMass->at(0);
      LightLeptonLT = inLightLeptonLT->at(0);
      if(inLightLeptonMT3->empty()){
         LightLeptonMT3 = 0;
      }else{
         LightLeptonMT3 = inLightLeptonMT3->at(0);
      }
      LightLeptonN = inLightLeptonN->at(0);
      LightLeptonOnZN = inLightLeptonOnZN->at(0);
      PassBadPFMuonFilter = inPassBadPFMuonFilter->at(0);
      PassBadChargedCandidateFilter = inPassBadChargedCandidateFilter->at(0);
      MuonN = inMuonN->at(0);
      //LightLeptonTightType3D = inLightLeptonTightType3D->at(0);
      ElectronN = inElectronN->at(0);

      PtRatio1 = PtRatio(LightLeptonPt0,LightLeptonPt1,LightLeptonPt2);
      PtRatio2 = PtRatio(LightLeptonPt1,LightLeptonPt0,LightLeptonPt2);
      PtRatio3 = PtRatio(LightLeptonPt2,LightLeptonPt0,LightLeptonPt1);

      DPhi1 = DeltaPhi(LightLeptonEta0,LightLeptonEta1);
      DPhi2 = DeltaPhi(LightLeptonEta1,LightLeptonEta2);
      DPhi3 = DeltaPhi(LightLeptonEta2,LightLeptonEta0);

      LightLeptonDXY0 = LightLeptonDXY(0, MuonDXY0,MuonDXY1,MuonDXY2, ElectronDXY0,ElectronDXY1,ElectronDXY2, LightLeptonFlavor, LightLeptonNativeIndex);
      LightLeptonDXY1 = LightLeptonDXY(1, MuonDXY0,MuonDXY1,MuonDXY2, ElectronDXY0,ElectronDXY1,ElectronDXY2, LightLeptonFlavor, LightLeptonNativeIndex);
      LightLeptonDXY2 = LightLeptonDXY(2, MuonDXY0,MuonDXY1,MuonDXY2, ElectronDXY0,ElectronDXY1,ElectronDXY2, LightLeptonFlavor, LightLeptonNativeIndex);
      
      if (Use["BDTG"          ]){
            double result =  reader->EvaluateMVA( "BDTG method" );
               //cout << result << endl;
               BDTResponse->clear();
               BDTResponse->push_back(result);
               if (result > -0.1153) vecBDTG.push_back(count);
               found_back_tree->Fill();
      }
      count++;
   }
   
   // Get elapsed time
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();

   
   std::cout << "Printing out all passed events... " << std::endl;
   

   std::cout << "BDT: \n"<< vecBDTG.size() << "\t passed events" << std::endl;

   
   found_back_tree->Write();
   

   
   endback->Close();               
  
   delete reader;
    


}

void SeesawTMVAClassificationApplication_bg( TString myMethodList = "" ) 
{   
#ifdef __CINT__
   gROOT->ProcessLine( ".O0" ); // turn off optimization in CINT
#endif

   //---------------------------------------------------------------

   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Boosted Decision Trees
   Use["BDTG"]             = 1; // uses Adaptive Boost


   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod 
                      << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------
   TString back_name = "/cms/mchristos/ANN/Seesaw/2016/92X/MC/QQQ/analysisTree_3L.root";
   //TString back_name = "/multilepton-2/2016/L3SkimMC/QQQ.root";

   runSignal(Use, back_name);

   std::cout << "==> TMVAClassificationApplication is done!" << endl << std::endl;
} 

