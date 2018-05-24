// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 **********************************************************************************/
// This MVA program uses 1 BG (WZ) and 1 Off-Z signal process with 2 variables (LT, MT)
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
//#include "/cms/mchristos/software/CMSSW_8_0_24_patch1/src/pyPlotter/tools/pileupWeights.C"
#include "TMVA/TMVAGui.h"



#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif


void SeesawTMVAClassification( TString myMethodList = "" )
{
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/customFunctions.C+");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/customFudgeFactors_2016.C+");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/matrixMethod_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/leptonSFs_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/pileupWeights_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/software/pyPlotter/tools/triggerEffScaleFactor_2016.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars2.C");
   gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/inputVars3.C");
   
   //gROOT->ProcessLineSync(".L /cms/mchristos/ANN/Seesaw/2016/custom_functions/makeTLVector.C");


   std::string mass = "140";
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Boosted Decision Trees
   Use["BDT1"]             = 1; // uses Adaptive Boost
   Use["BDT2"]          =1;
   Use["BDT3"]    = 1;
   Use["BDT4"] = 1;
   Use["BDT5"] = 1;
   Use["BDT6"] = 1;
   Use["BDT7"] = 1;
   Use["BDTG"]            = 1; // uses Gradient Boost
   Use["BDTB"]            = 1; // uses Bagging
   Use["BDTD"]            = 1; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 1; // allow usage of fisher discriminant for node splitting 

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "/cms/mchristos/ANN/Seesaw/2016/92X/class_perf/2017_TMVA_newVars_optimized.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   TMVA::Factory *factory = new TMVA::Factory( "2017_TMVA_newVars_optimized", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

   //factory->AddVariable("LightLeptonLT := LightLeptonLT3[0]", 'F' );Alt$(arr[4],0)
   factory->AddVariable("PFMETType1 := Alt$(PFMETType1[0],0)",'F');
   factory->AddVariable("LightLeptonPt0 := Alt$(LightLeptonPt[0],0)",'F');
   factory->AddVariable("LightLeptonPt1 := Alt$(LightLeptonPt[1],0)",'F');
   factory->AddVariable("LightLeptonPt2 := Alt$(LightLeptonPt[2],0)",'F');
   factory->AddVariable("LightLeptonMT3 := Alt$(LightLeptonMT3[0],0)",'F');
   //factory->AddVariable("LightLeptonEta0 := LightLeptonEta[0]",'F');
   //factory->AddVariable("LightLeptonEta1 := LightLeptonEta[1]",'F');
   //factory->AddVariable("LightLeptonEta2 := LightLeptonEta[2]",'F');
   factory->AddVariable("LightLeptonMass := Alt$(LightLeptonMass[0],0)",'F');
   //factory->AddVariable("JetN := JetN[0]",'I');
   factory->AddVariable("LightLeptonLeptonDRmin0 := Alt$(LightLeptonLeptonDRmin[0],0)",'F');
   factory->AddVariable("LightLeptonBestMOSSF0:= Alt$(LightLeptonBestMOSSF[0],0)",'F');
   factory->AddVariable("PtRatio1 := PtRatio(LightLeptonPt[0],LightLeptonPt[1],LightLeptonPt[2])",'F');
   factory->AddVariable("PtRatio2 := PtRatio(LightLeptonPt[1],LightLeptonPt[0],LightLeptonPt[2])",'F');
   factory->AddVariable("PtRatio3 := PtRatio(LightLeptonPt[2],LightLeptonPt[0],LightLeptonPt[1])",'F');
   factory->AddVariable("DPhi1 := DeltaPhi(LightLeptonEta[0],LightLeptonEta[1])",'F');
   factory->AddVariable("DPhi2 := DeltaPhi(LightLeptonEta[1],LightLeptonEta[2])",'F');
   factory->AddVariable("DPhi3 := DeltaPhi(LightLeptonEta[2],LightLeptonEta[0])",'F');
   factory->AddVariable("LightLeptonDXY0 := LightLeptonDXY(0, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",'F');
   factory->AddVariable("LightLeptonDXY1 := LightLeptonDXY(1, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",'F');
   factory->AddVariable("LightLeptonDXY2 := LightLeptonDXY(2, Alt$(MuonDXY[0],0),Alt$(MuonDXY[1],0),Alt$(MuonDXY[2],0), Alt$(ElectronDXY[0],0),Alt$(ElectronDXY[1],0),Alt$(ElectronDXY[2],0), LightLeptonFlavor[0], LightLeptonNativeIndex[0])",'F');

   //factory->AddVariable("PtRatio1 := PtRatio(LightLeptonPt[0],LightLeptonPt[1],LightLeptonPt[2])",'F');
   //factory->AddVariable("PtRatio2 := PtRatio(LightLeptonPt[1],LightLeptonPt[0],LightLeptonPt[2])",'F');
   //factory->AddVariable("PtRatio3 := PtRatio(LightLeptonPt[2],LightLeptonPt[0],LightLeptonPt[1])",'F');
   //factory->AddVariable("DPhi1 := DeltaPhi(LightLeptonEta[0],LightLeptonEta[1])",'F');
   //factory->AddVariable("DPhi2 := DeltaPhi(LightLeptonEta[1],LightLeptonEta[2])",'F');
   //factory->AddVariable("DPhi3 := DeltaPhi(LightLeptonEta[2],LightLeptonEta[3])",'F');
   //factory->AddVariable("test4vect:= Alt$(makeTLVector(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPhi[0]),0)");
   //factory->AddVariable("LightLeptonLeptonDRmin1 := LightLeptonLeptonDRmin[1]",'F');
   //factory->AddVariable("LightLeptonLeptonDRmin2 := LightLeptonLeptonDRmin[2]",'F');
   //factory->AddVariable("LightLeptonCharge0 := LightLeptonCharge[0]",'I');
   //factory->AddVariable("LightLeptonCharge1 := LightLeptonCharge[1]",'I');
   //factory->AddVariable("LightLeptonCharge2 := LightLeptonCharge[2]",'I');
   //factory->AddVariable("LTplusMET3D := PFMETType1[0]+LightLeptonLT3[0]", 'F' );
   //factory->AddVariable("LightLeptonPt := LightLeptonPt[0]",'F');
   //factory->AddVariable("LightLeptonPt1 := LightLeptonPt[1]",'F');
   //factory->AddVariable("LightLeptonPt2 := LightLeptonPt[2]",'F');
   //factory->AddVariable("LightLeptonMass := LightLeptonMass[0]", 'F' );
   //factory->AddVariable("JetPt := JetPt[0]", 'F' );
   //factory->AddVariable("BJetCombinedCvsBJetTags := BJetCombinedCvsBJetTags[0]", 'F' );

   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables
   //factory->AddSpectator( "spec1 := var1*2",  "Spectator 1", "units", 'F' );
   //factory->AddSpectator( "spec2 := var1*3",  "Spectator 2", "units", 'F' );
   /*factory->AddSpectator("ElectronN := ElectronN[0]",'I');
   factory->AddSpectator("HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLDZvAccept := HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLDZvAccept[0]",'F');
   factory->AddSpectator("HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLDZvAccept := HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLDZvAccept[0]",'F');
   factory->AddSpectator("HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLDZvAccept := HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLDZvAccept[0]",'F');
   factory->AddSpectator("HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLvAccept := HLTMu23TrkIsoVVLEle12CaloIdLTrackIdLIsoVLvAccept[0]",'F');
   factory->AddSpectator("HLTMu17TrkIsoVVLMu8TrkIsoVVLDZvAccept := HLTMu17TrkIsoVVLMu8TrkIsoVVLDZvAccept[0]",'F');
   factory->AddSpectator("HLTMu17TrkIsoVVLMu8TrkIsoVVLvAccept := HLTMu17TrkIsoVVLMu8TrkIsoVVLvAccept[0]",'F');
   factory->AddSpectator("HLTMu17TrkIsoVVLTkMu8TrkIsoVVLDZvAccept := HLTMu17TrkIsoVVLTkMu8TrkIsoVVLDZvAccept[0]",'F');
   factory->AddSpectator("HLTMu17TrkIsoVVLTkMu8TrkIsoVVLvAccept := HLTMu17TrkIsoVVLTkMu8TrkIsoVVLvAccept[0]",'F');
   factory->AddSpectator("HLTEle23Ele12CaloIdLTrackIdLIsoVLDZvAccept := HLTEle23Ele12CaloIdLTrackIdLIsoVLDZvAccept",'F');
   factory->AddSpectator("HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLvAccept := HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLvAccept[0]",'F');
   
   factory->AddSpectator("HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLvAccept := HLTMu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVLvAccept[0]",'F');
   factory->AddSpectator("LightLeptonN := LightLeptonN[0]",'I');
   factory->AddSpectator("LightLeptonOnZN := LightLeptonOnZN[0]",'I');
   //factory->AddSpectator("LightLeptonTightType3D := LightLeptonTightType3D[0]",'I');
   factory->AddSpectator("MuonN := MuonN[0]",'I');
   factory->AddSpectator("PassBadChargedCandidateFilter := PassBadChargedCandidateFilter[0]",'I');
   factory->AddSpectator("PassBadPFMuonFilter := PassBadPFMuonFilter[0]",'I');
   //factory->AddSpectator("LightLeptonIsTight0 := LightLeptonIsTight[0]", 'I');
   //factory->AddSpectator("LightLeptonIsTight2 := LightLeptonIsTight[1]", 'I');
   //factory->AddSpectator("LightLeptonIsTight1 := LightLeptonIsTight[2]", 'I');*/

   //////////////////////////
   // Adding signal Trees //
   //////////////////////////

   std::ifstream infileXsecs("/cms/mchristos/ANN/Seesaw/2016/test/xsecfiles/XSEC_140.txt");
   std::string line;

   std::getline(infileXsecs, line);    //throws away first line which includes titles
   
   while (std::getline(infileXsecs, line)) {    //loops through remaining lines in xsecs file
      
      std::string process;
      double xsec, br,filter_eff, k_factor;     //variables to store xsec, branching ratio, filter efficiency, and k factor
      
      std::istringstream ss(line);       //takes each line and converts to string
      ss >> process;
      ss >> xsec;
      ss >> br;
      ss >> filter_eff;
      ss >> k_factor;         //reads process name and other numbers into respective variables
     

      TH1I *EventCountSignal1= new TH1I();

      std::string in = "/cms/mchristos/ANN/Seesaw/2016/92X/Signal/140/SeesawTypeIII_"+process+"_M-140_13TeV-madgraph/analysisTree.root";
      const char *sig_name = in.c_str();
      TFile *sig_input = TFile::Open( sig_name );     //opens signal file matching process name

      std::cout << "--- TMVAClassification       : Using signal input file: " << sig_input->GetName() <<         std::endl;
      // --- Register the training and test trees
      TTree *signal     = (TTree*)sig_input->Get("rootTupleTreeVeryLoose/tree");     //gets signal tree
      EventCountSignal1 = (TH1I*)sig_input->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
      double nSignal1 = EventCountSignal1->GetBinContent(1);
      
      cout<<"XSEC      BR          Filter_eff  K Factor"<<endl;
      cout << xsec <<"   "<< br << "    "<<filter_eff<<"   "<< k_factor <<endl;
      Double_t sig_xsec = xsec*br*filter_eff*k_factor;
   
      Double_t sig_getweight = signal->GetWeight();
      cout <<"Signal Weight: "<<sig_getweight<<endl;
   
      // global event weights per tree (see below for setting event-wise weights)
      Double_t signalWeight     = sig_xsec*sig_getweight;      //calculates signal weight

      // You can add an arbitrary number of signal or background trees
      TString signalExpression = "(9*(1>0))*(Event%9==1)*((pileupWeightsOffXsecMinus5(GenPileUpInteractionsTrue[0]))&&((trigEffSF3D(LightLeptonPt[0], LightLeptonPt[1], LightLeptonPt[2], LightLeptonEta[0], LightLeptonEta[1], LightLeptonEta[2], LightLeptonFlavorType3D[0])))&&((leptonSF3D(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPt[1],LightLeptonEta[1],LightLeptonPt[2],LightLeptonEta[2],(((RecoVertexN[0])*1000)+LightLeptonFlavorType3D[0])))) && (((((((Alt$(MuonIsTight[protectedLightLeptonNativeIndex0],0)==1)*(LightLeptonFlavor[0]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex0],0.))*(LightLeptonFlavor[0]==1)))*100+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex1],0)==1)*(LightLeptonFlavor[1]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex1],0.))*(LightLeptonFlavor[1]==1)))*10+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex2],0)==1)*(LightLeptonFlavor[2]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex2],0.))*(LightLeptonFlavor[2]==1))))))==111))*35867.060";
      factory->SetSignalWeightExpression(signalExpression);
      factory->AddSignalTree    ( signal,      signalWeight/nSignal1 );       //adds signal tree with weight to factory
   }

   /////////////////////////////
   // Add MC Background Trees //Alt$(ProxyPhotonLepDRmin[0],0)
   /////////////////////////////

TString backgroundExpression ="";
  backgroundExpression += "(dataType==1)*(9*(1>0))*(Event%9==1)*((pileupWeightsOffXsecMinus5(GenPileUpInteractionsTrue[0]))&&((trigEffSF3D(LightLeptonPt[0], LightLeptonPt[1], LightLeptonPt[2], LightLeptonEta[0], LightLeptonEta[1], LightLeptonEta[2], LightLeptonFlavorType3D[0])))&&((leptonSF3D(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPt[1],LightLeptonEta[1],LightLeptonPt[2],LightLeptonEta[2],(((RecoVertexN[0])*1000)+LightLeptonFlavorType3D[0])))) && (((((((Alt$(MuonIsTight[protectedLightLeptonNativeIndex0],0)==1)*(LightLeptonFlavor[0]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex0],0.))*(LightLeptonFlavor[0]==1)))*100+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex1],0)==1)*(LightLeptonFlavor[1]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex1],0.))*(LightLeptonFlavor[1]==1)))*10+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex2],0)==1)*(LightLeptonFlavor[2]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex2],0.))*(LightLeptonFlavor[2]==1))))))==111))*((((((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))*100+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))*10+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1)))*(LightLeptonN[0]==3))+((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[0],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))==0))*100+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[1],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))==0))*10+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[2],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1))==0)))*(LightLeptonN[0]==3)))*(1>0))==111)*35867.060";

  backgroundExpression +="+(dataType==4)*(9*(1>0))*(Event%9==1)*((pileupWeightsOffXsecMinus5(GenPileUpInteractionsTrue[0]))&&((trigEffSF3D(LightLeptonPt[0], LightLeptonPt[1], LightLeptonPt[2], LightLeptonEta[0], LightLeptonEta[1], LightLeptonEta[2], LightLeptonFlavorType3D[0])))&&((leptonSF3D(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPt[1],LightLeptonEta[1],LightLeptonPt[2],LightLeptonEta[2],(((RecoVertexN[0])*1000)+LightLeptonFlavorType3D[0]))))&&(((((((Alt$(MuonIsTight[protectedLightLeptonNativeIndex0],0)==1)*(LightLeptonFlavor[0]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex0],0.))*(LightLeptonFlavor[0]==1)))*100+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex1],0)==1)*(LightLeptonFlavor[1]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex1],0.))*(LightLeptonFlavor[1]==1)))*10+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex2],0)==1)*(LightLeptonFlavor[2]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex2],0.))*(LightLeptonFlavor[2]==1))))))==111))*(((1>0)*jetWeights_WZ_powheg_2016(JetN[0])))*((((((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))*100+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))*10+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1)))*(LightLeptonN[0]==3))+((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[0],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))==0))*100+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[1],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))==0))*10+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[2],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1))==0)))*(LightLeptonN[0]==3)))*(1>0))==111)*35867.060";

  backgroundExpression +="+(dataType==5)*(9*(1>0))*(Event%9==1)*((pileupWeightsOffXsecMinus5(GenPileUpInteractionsTrue[0]))&&((trigEffSF3D(LightLeptonPt[0], LightLeptonPt[1], LightLeptonPt[2], LightLeptonEta[0], LightLeptonEta[1], LightLeptonEta[2], LightLeptonFlavorType3D[0])))&&((leptonSF3D(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPt[1],LightLeptonEta[1],LightLeptonPt[2],LightLeptonEta[2],(((RecoVertexN[0])*1000)+LightLeptonFlavorType3D[0]))))&&(((((((Alt$(MuonIsTight[protectedLightLeptonNativeIndex0],0)==1)*(LightLeptonFlavor[0]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex0],0.))*(LightLeptonFlavor[0]==1)))*100+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex1],0)==1)*(LightLeptonFlavor[1]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex1],0.))*(LightLeptonFlavor[1]==1)))*10+(((Alt$(MuonIsTight[protectedLightLeptonNativeIndex2],0)==1)*(LightLeptonFlavor[2]==2)+(Alt$(ElectronIsTight[protectedLightLeptonNativeIndex2],0.))*(LightLeptonFlavor[2]==1))))))==111))*((((((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))*100+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))*10+(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1)))*(LightLeptonN[0]==3))+((((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[0],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[0],999))),99.))<0.1))==0))*100+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[1],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[1],999))),99.))<0.1))==0))*10+((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenPhotonDr[2],999))),99.)<0.1&&(((Alt$(((1>0)*(Alt$(LightLeptonMatchedPromptGenLeptonDr[2],999))),99.))<0.1))==0)))*(LightLeptonN[0]==3)))*(1>0))==111)*35867.060";

  //backgroundExpression+= "+(dataType==2)*(MatrixMethodWeight3D(LightLeptonPt[0],LightLeptonEta[0],LightLeptonPt[1],LightLeptonEta[1],LightLeptonPt[2],LightLeptonEta[2],(LightLeptonTightType3D[0]*1000*1000)+(LightLeptonFlavorType3D[0]*1000)+(((LightLeptonFlavor[0]==1)*(Alt$(ElectronJetNoOfConst[(LightLeptonNativeIndex[0]*(LightLeptonNativeIndex[0]>=0))],0)>11.5))+((LightLeptonFlavor[0]==2)*(Alt$(MuonJetNoOfConst[(LightLeptonNativeIndex[0]*(LightLeptonNativeIndex[0]>=0))],0)>11.5))+1)*100+(((LightLeptonFlavor[1]==1)*(Alt$(ElectronJetNoOfConst[(LightLeptonNativeIndex[1]*(LightLeptonNativeIndex[1]>=0))],0)>11.5))+((LightLeptonFlavor[1]==2)*(Alt$(MuonJetNoOfConst[(LightLeptonNativeIndex[1]*(LightLeptonNativeIndex[1]>=0))],0)>11.5))+1)*10+(((LightLeptonFlavor[2]==1)*(Alt$(ElectronJetNoOfConst[(LightLeptonNativeIndex[2]*(LightLeptonNativeIndex[2]>=0))],0)>11.5))+((LightLeptonFlavor[2]==2)*(Alt$(MuonJetNoOfConst[(LightLeptonNativeIndex[2]*(LightLeptonNativeIndex[2]>=0))],0)>11.5))+1),((RecoVertexN*10000)+(LightLeptonPt[0]<70)*1+(LightLeptonPt[0]>70)*2),0))";

  //backgroundExpression+="+(dataType==3)*((LightLeptonTightType3D[0]==111)*PhoFakeRates3D(LightLeptonLT3[0],(LightLeptonPt[0]*LightLeptonPt[1])/(Alt$(ProxyPhotonPt[0],0)*Alt$(ProxyPhotonPt[0],0)),Alt$(ProxyPhotonPt[0],0),Alt$(ProxyPhotonLepDRmin[0],0),LightLeptonProxyType3D[0],(LightLeptonFlavor[0]==1)*(LightLeptonRole[0]==1)+(LightLeptonFlavor[1]==1)*(LightLeptonRole[1]==1)+(LightLeptonFlavor[2]==1)*(LightLeptonRole[2]==1),(LightLeptonFlavor[0]==2)*(LightLeptonRole[0]==1)+(LightLeptonFlavor[1]==2)*(LightLeptonRole[1]==1)+(LightLeptonFlavor[2]==2)*(LightLeptonRole[2]==1))*(LightLeptonIsPhotonProxyEvent[0]==1&&ProxyPhotonJetDRmin>0.4&&ProxyPhotonN[0]==1))";

   factory->SetBackgroundWeightExpression(backgroundExpression);

   Double_t back_xsec1 = 117.864*0.11;
   Double_t back_xsec2 = 87.31*0.21;
   Double_t back_xsec3 = 87.31*0.79;
   Double_t back_xsec4 = 117.864*0.89; 
   Double_t back_xsec5 = 12.178;
   Double_t back_xsec6 = 19.56*0.62;
   Double_t back_xsec7 = 19.56*0.38;
   Double_t back_xsec8 = 19.56*0.63;
   Double_t back_xsec9 = 19.56*0.37;
   Double_t back_xsec10 = 0.564*0.16;
   Double_t back_xsec11 = 0.564*0.84;
   Double_t back_xsec12 = 3.22;
   Double_t back_xsec13 = 510.6*0.19;
   Double_t back_xsec14 = 510.6*0.34;
   Double_t back_xsec15 = 510.6*0.47;
   Double_t back_xsec16 = 0.007868;
   Double_t back_xsec17 = 0.02093;
   Double_t back_xsec18 = 0.1703;
   Double_t back_xsec19 = 5.606;
   Double_t back_xsec20 = 4.42965;
   Double_t back_xsec21 = 1.256*1.1*0.07;
   Double_t back_xsec22 = 1.256*1.1*0.93;
   Double_t back_xsec23 = 0.003194*1.7;
   Double_t back_xsec24 = 0.003194*1.7;
   Double_t back_xsec25 = 0.003194*1.7;
   Double_t back_xsec26 = 0.001586*1.7;
   Double_t back_xsec27 = 0.001586*1.7;
   Double_t back_xsec28 = 0.001586*1.7;
   Double_t back_xsec29 = 0.2086;
   Double_t back_xsec30 = 0.1651;
   Double_t back_xsec31 = 0.05565;
   Double_t back_xsec32 = 0.01398;
   Double_t back_xsec33 = 0.2043*0.45;
   Double_t back_xsec34 = 0.2043*0.55;
   Double_t back_xsec35 = 0.0493;
   Double_t back_xsec36 = 0.2529*0.15;
   Double_t back_xsec37 = 0.2529*0.44;
   Double_t back_xsec38 = 0.2529*0.41;
   Double_t back_xsec39 = 0.09418;
   Double_t back_xsec40 = 0.01103;
   Double_t back_xsec41 = 0.009103;
   Double_t back_xsec42 = 0.007834;
   Double_t back_xsec43 = 0.002938;
   Double_t back_xsec44 = 0.001563;
   Double_t back_xsec45 = 0.2151;
   Double_t back_xsec46 = 0.01212;
   Double_t back_xsec47 = 0.001034;
   Double_t back_xsec48 = 0.000324;
   Double_t back_xsec49 = 0.9561;

   Double_t back_xsec50 = 5765.4*0.19;
   Double_t back_xsec51 = 5765.4*0.81;
   Double_t back_xsec52 = 18610*0.61;
   Double_t back_xsec53 = 18610*0.39;
   Double_t back_xsec54 = 87.31*0.21;
   Double_t back_xsec55 = 87.31*0.79;


   TH1I *EventCount1= new TH1I();
   TString back_name1 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/analysisTree.root";
   TFile *back_input1 = TFile::Open( back_name1 );
   TTree *background1 = (TTree*)back_input1->Get("rootTupleTreeVeryLoose/tree");
   EventCount1 = (TH1I*)back_input1->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n1 = EventCount1->GetBinContent(1);
   Double_t backgroundWeight1 = back_xsec1/n1;
   factory->AddBackgroundTree( background1, backgroundWeight1 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input1->GetName() <<       std::endl;

   /*TH1I *EventCount2= new TH1I();
   TString back_name2 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTJets_Dilept_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8/analysisTree.root";
   TFile *back_input2 = TFile::Open( back_name2 );
   TTree *background2 = (TTree*)back_input2->Get("rootTupleTreeVeryLoose/tree");
   EventCount2 = (TH1I*)back_input2->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n2 = EventCount2->GetBinContent(1);
   Double_t backgroundWeight2 = back_xsec2/n2;
   factory->AddBackgroundTree( background2, backgroundWeight2 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input2->GetName() <<       std::endl;

   TH1I *EventCount3= new TH1I();
   TString back_name3 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTJets_Dilept_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8_ext1/analysisTree.root";
   TFile *back_input3 = TFile::Open( back_name3 );
   TTree *background3 = (TTree*)back_input3->Get("rootTupleTreeVeryLoose/tree");
   EventCount3 = (TH1I*)back_input3->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n3 = EventCount3->GetBinContent(1);
   Double_t backgroundWeight3 = back_xsec3/n3;
   factory->AddBackgroundTree( background3, backgroundWeight3 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input3->GetName() <<       std::endl;
*/
   TH1I *EventCount4= new TH1I();
   TString back_name4 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext1/analysisTree.root";
   TFile *back_input4 = TFile::Open( back_name4 );
   TTree *background4 = (TTree*)back_input4->Get("rootTupleTreeVeryLoose/tree");
   EventCount4 = (TH1I*)back_input4->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n4 = EventCount4->GetBinContent(1);
   Double_t backgroundWeight4 = back_xsec4/n4;
   factory->AddBackgroundTree( background4, backgroundWeight4 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input4->GetName() <<       std::endl;

   TH1I *EventCount5= new TH1I();
   TString back_name5 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WWTo2L2Nu_13TeV-powheg/analysisTree.root";
   TFile *back_input5 = TFile::Open( back_name5 );
   TTree *background5 = (TTree*)back_input5->Get("rootTupleTreeVeryLoose/tree");
   EventCount5 = (TH1I*)back_input5->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n5 = EventCount5->GetBinContent(1);
   Double_t backgroundWeight5 = back_xsec5/n5;
   factory->AddBackgroundTree( background5, backgroundWeight5 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input5->GetName() <<       std::endl;

   TH1I *EventCount6= new TH1I();
   TString back_name6 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ST_tW_top_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1/analysisTree.root";
   TFile *back_input6 = TFile::Open( back_name6 );
   TTree *background6 = (TTree*)back_input6->Get("rootTupleTreeVeryLoose/tree");
   EventCount6 = (TH1I*)back_input6->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n6 = EventCount6->GetBinContent(1);
   Double_t backgroundWeight6 = back_xsec6/n6;
   factory->AddBackgroundTree( background6, backgroundWeight6 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input6->GetName() <<       std::endl;

   TH1I *EventCount7= new TH1I();
   TString back_name7 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ST_tW_top_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1_ext1/analysisTree.root";
   TFile *back_input7 = TFile::Open( back_name7 );
   TTree *background7 = (TTree*)back_input7->Get("rootTupleTreeVeryLoose/tree");
   EventCount7 = (TH1I*)back_input7->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n7 = EventCount7->GetBinContent(1);
   Double_t backgroundWeight7 = back_xsec7/n7;
   factory->AddBackgroundTree( background7, backgroundWeight7 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input7->GetName() <<       std::endl;

   TH1I *EventCount8= new TH1I();
   TString back_name8 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ST_tW_antitop_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1/analysisTree.root";
   TFile *back_input8 = TFile::Open( back_name8 );
   TTree *background8 = (TTree*)back_input8->Get("rootTupleTreeVeryLoose/tree");
   EventCount8 = (TH1I*)back_input8->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n8 = EventCount8->GetBinContent(1);
   Double_t backgroundWeight8 = back_xsec8/n8;
   factory->AddBackgroundTree( background8, backgroundWeight8 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input8->GetName() <<       std::endl;

   TH1I *EventCount9= new TH1I();
   TString back_name9 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ST_tW_antitop_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1_ext1/analysisTree.root";
   TFile *back_input9 = TFile::Open( back_name9 );
   TTree *background9 = (TTree*)back_input9->Get("rootTupleTreeVeryLoose/tree");
   EventCount9 = (TH1I*)back_input9->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n9 = EventCount9->GetBinContent(1);
   Double_t backgroundWeight9 = back_xsec9/n9;
   factory->AddBackgroundTree( background9, backgroundWeight9 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input9->GetName() <<       std::endl;

   TH1I *EventCount10= new TH1I();
   TString back_name10 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZTo2L2Nu_13TeV_powheg_pythia8/analysisTree.root";
   TFile *back_input10 = TFile::Open( back_name10 );
   TTree *background10 = (TTree*)back_input10->Get("rootTupleTreeVeryLoose/tree");
   EventCount10 = (TH1I*)back_input10->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n10 = EventCount10->GetBinContent(1);
   Double_t backgroundWeight10 = back_xsec10/n10;
   factory->AddBackgroundTree( background10, backgroundWeight10 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input10->GetName() <<       std::endl;

   TH1I *EventCount11= new TH1I();
   TString back_name11 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZTo2L2Nu_13TeV_powheg_pythia8_ext1/analysisTree.root";
   TFile *back_input11 = TFile::Open( back_name11 );
   TTree *background11 = (TTree*)back_input11->Get("rootTupleTreeVeryLoose/tree");
   EventCount11 = (TH1I*)back_input11->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n11 = EventCount11->GetBinContent(1);
   Double_t backgroundWeight11 = back_xsec11/n11;
   factory->AddBackgroundTree( background11, backgroundWeight11 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input11->GetName() <<       std::endl;

   TH1I *EventCount12= new TH1I();
   TString back_name12 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZTo2L2Q_13TeV_powheg_pythia8/analysisTree.root";
   TFile *back_input12 = TFile::Open( back_name12 );
   TTree *background12 = (TTree*)back_input12->Get("rootTupleTreeVeryLoose/tree");
   EventCount12 = (TH1I*)back_input12->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n12 = EventCount12->GetBinContent(1);
   Double_t backgroundWeight12 = back_xsec12/n12;
   factory->AddBackgroundTree( background12, backgroundWeight12 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input12->GetName() <<       std::endl;

   TH1I *EventCount13= new TH1I();
   TString back_name13 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext1/analysisTree.root";
   TFile *back_input13 = TFile::Open( back_name13 );
   TTree *background13 = (TTree*)back_input13->Get("rootTupleTreeVeryLoose/tree");
   EventCount13 = (TH1I*)back_input13->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n13 = EventCount13->GetBinContent(1);
   Double_t backgroundWeight13 = back_xsec13/n13;
   factory->AddBackgroundTree( background13, backgroundWeight13 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input13->GetName() <<       std::endl;

   TH1I *EventCount14= new TH1I();
   TString back_name14 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext2/analysisTree.root";
   TFile *back_input14 = TFile::Open( back_name14 );
   TTree *background14 = (TTree*)back_input14->Get("rootTupleTreeVeryLoose/tree");
   EventCount14 = (TH1I*)back_input14->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n14 = EventCount14->GetBinContent(1);
   Double_t backgroundWeight14 = back_xsec14/n14;
   factory->AddBackgroundTree( background14, backgroundWeight14 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input14->GetName() <<       std::endl;

   TH1I *EventCount15= new TH1I();
   TString back_name15 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext3/analysisTree.root";
   TFile *back_input15 = TFile::Open( back_name15 );
   TTree *background15 = (TTree*)back_input15->Get("rootTupleTreeVeryLoose/tree");
   EventCount15 = (TH1I*)back_input15->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n15 = EventCount15->GetBinContent(1);
   Double_t backgroundWeight15 = back_xsec15/n15;
   factory->AddBackgroundTree( background15, backgroundWeight15 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input15->GetName() <<       std::endl;

   TH1I *EventCount16= new TH1I();
   TString back_name16 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WmWmJJ_13TeV-powheg-pythia8_TuneCUETP8M1/analysisTree.root";
   TFile *back_input16 = TFile::Open( back_name16 );
   TTree *background16 = (TTree*)back_input16->Get("rootTupleTreeVeryLoose/tree");
   EventCount16 = (TH1I*)back_input16->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n16 = EventCount16->GetBinContent(1);
   Double_t backgroundWeight16 = back_xsec16/n16;
   factory->AddBackgroundTree( background16, backgroundWeight16 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input16->GetName() <<       std::endl;

   TH1I *EventCount17= new TH1I();
   TString back_name17 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WpWpJJ_13TeV-powheg-pythia8_TuneCUETP8M1/analysisTree.root";
   TFile *back_input17 = TFile::Open( back_name17 );
   TTree *background17 = (TTree*)back_input17->Get("rootTupleTreeVeryLoose/tree");
   EventCount17 = (TH1I*)back_input17->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n17 = EventCount17->GetBinContent(1);
   Double_t backgroundWeight17 = back_xsec17/n17;
   factory->AddBackgroundTree( background17, backgroundWeight17 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input17->GetName() <<       std::endl;

   TH1I *EventCount18= new TH1I();
   TString back_name18 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WWTo2L2Nu_DoubleScattering_13TeV-pythia8/analysisTree.root";
   TFile *back_input18 = TFile::Open( back_name18 );
   TTree *background18 = (TTree*)back_input18->Get("rootTupleTreeVeryLoose/tree");
   EventCount18 = (TH1I*)back_input18->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n18 = EventCount18->GetBinContent(1);
   Double_t backgroundWeight18 = back_xsec18/n18;
   factory->AddBackgroundTree( background18, backgroundWeight18 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input18->GetName() <<       std::endl;

   TH1I *EventCount19= new TH1I();
   TString back_name19 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/analysisTree.root";
   TFile *back_input19 = TFile::Open( back_name19 );
   TTree *background19 = (TTree*)back_input19->Get("rootTupleTreeVeryLoose/tree");
   EventCount19 = (TH1I*)back_input19->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n19 = EventCount19->GetBinContent(1);
   Double_t backgroundWeight19 = back_xsec19/n19;
   factory->AddBackgroundTree( background19, backgroundWeight19 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input19->GetName() <<       std::endl;

   TH1I *EventCount20= new TH1I();
   TString back_name20 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/analysisTree.root";
   TFile *back_input20 = TFile::Open( back_name20 );
   TTree *background20 = (TTree*)back_input20->Get("rootTupleTreeVeryLoose/tree");
   EventCount20 = (TH1I*)back_input20->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n20 = EventCount20->GetBinContent(1);
   Double_t backgroundWeight20 = back_xsec20/n20*1.07;
   factory->AddBackgroundTree( background20, backgroundWeight20 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input20->GetName() <<       std::endl;

   TH1I *EventCount21= new TH1I();
   TString back_name21 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZTo4L_13TeV_powheg_pythia8/analysisTree.root";
   TFile *back_input21 = TFile::Open( back_name21 );
   TTree *background21 = (TTree*)back_input21->Get("rootTupleTreeVeryLoose/tree");
   EventCount21 = (TH1I*)back_input21->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n21 = EventCount21->GetBinContent(1);
   Double_t backgroundWeight21 = back_xsec21/n21*0.95;
   factory->AddBackgroundTree( background21, backgroundWeight21 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input21->GetName() <<       std::endl;

   TH1I *EventCount22= new TH1I();
   TString back_name22 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZTo4L_13TeV_powheg_pythia8_ext1/analysisTree.root";
   TFile *back_input22 = TFile::Open( back_name22 );
   TTree *background22 = (TTree*)back_input22->Get("rootTupleTreeVeryLoose/tree");
   EventCount22 = (TH1I*)back_input22->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n22 = EventCount22->GetBinContent(1);
   Double_t backgroundWeight22 = back_xsec22/n22*0.95;
   factory->AddBackgroundTree( background22, backgroundWeight22 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input22->GetName() <<       std::endl;

   TH1I *EventCount23= new TH1I();
   TString back_name23 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo2e2mu_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input23 = TFile::Open( back_name23 );
   TTree *background23 = (TTree*)back_input23->Get("rootTupleTreeVeryLoose/tree");
   EventCount23 = (TH1I*)back_input23->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n23 = EventCount23->GetBinContent(1);
   Double_t backgroundWeight23 = back_xsec23/n23*0.95;
   factory->AddBackgroundTree( background23, backgroundWeight23 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input23->GetName() <<       std::endl;

   TH1I *EventCount24= new TH1I();
   TString back_name24 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo2e2tau_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input24 = TFile::Open( back_name24 );
   TTree *background24 = (TTree*)back_input24->Get("rootTupleTreeVeryLoose/tree");
   EventCount24 = (TH1I*)back_input24->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n24 = EventCount24->GetBinContent(1);
   Double_t backgroundWeight24 = back_xsec24/n24*0.95;
   factory->AddBackgroundTree( background24, backgroundWeight24 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input24->GetName() <<       std::endl;

   TH1I *EventCount25= new TH1I();
   TString back_name25 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo2mu2tau_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input25 = TFile::Open( back_name25 );
   TTree *background25 = (TTree*)back_input25->Get("rootTupleTreeVeryLoose/tree");
   EventCount25 = (TH1I*)back_input25->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n25 = EventCount25->GetBinContent(1);
   Double_t backgroundWeight25 = back_xsec25/n25*0.95;
   factory->AddBackgroundTree( background25, backgroundWeight25 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input25->GetName() <<       std::endl;

   TH1I *EventCount26= new TH1I();
   TString back_name26 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo4e_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input26 = TFile::Open( back_name26 );
   TTree *background26 = (TTree*)back_input26->Get("rootTupleTreeVeryLoose/tree");
   EventCount26 = (TH1I*)back_input26->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n26 = EventCount26->GetBinContent(1);
   Double_t backgroundWeight26 = back_xsec26/n26*0.95;
   factory->AddBackgroundTree( background26, backgroundWeight26 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input26->GetName() <<       std::endl;

   TH1I *EventCount27= new TH1I();
   TString back_name27 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo4mu_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input27 = TFile::Open( back_name27 );
   TTree *background27 = (TTree*)back_input27->Get("rootTupleTreeVeryLoose/tree");
   EventCount27 = (TH1I*)back_input27->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n27 = EventCount27->GetBinContent(1);
   Double_t backgroundWeight27 = back_xsec27/n27*0.95;
   factory->AddBackgroundTree( background27, backgroundWeight27 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input27->GetName() <<       std::endl;

   TH1I *EventCount28= new TH1I();
   TString back_name28 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluToContinToZZTo4tau_13TeV_MCFM701_pythia8/analysisTree.root";
   TFile *back_input28 = TFile::Open( back_name28 );
   TTree *background28 = (TTree*)back_input28->Get("rootTupleTreeVeryLoose/tree");
   EventCount28 = (TH1I*)back_input28->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n28 = EventCount28->GetBinContent(1);
   Double_t backgroundWeight28 = back_xsec28/n28*0.95;
   factory->AddBackgroundTree( background28, backgroundWeight28 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input28->GetName() <<       std::endl;

   TH1I *EventCount29= new TH1I();
   TString back_name29 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WWW_4F_TuneCUETP8M1_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input29 = TFile::Open( back_name29 );
   TTree *background29 = (TTree*)back_input29->Get("rootTupleTreeVeryLoose/tree");
   EventCount29 = (TH1I*)back_input29->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n29 = EventCount29->GetBinContent(1);
   Double_t backgroundWeight29 = back_xsec29/n29;
   factory->AddBackgroundTree( background29, backgroundWeight29 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input29->GetName() <<       std::endl;

   TH1I *EventCount30= new TH1I();
   TString back_name30 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WWZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input30 = TFile::Open( back_name30 );
   TTree *background30 = (TTree*)back_input30->Get("rootTupleTreeVeryLoose/tree");
   EventCount30 = (TH1I*)back_input30->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n30 = EventCount30->GetBinContent(1);
   Double_t backgroundWeight30 = back_xsec30/n30;
   factory->AddBackgroundTree( background30, backgroundWeight30 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input30->GetName() <<       std::endl;

   TH1I *EventCount31= new TH1I();
   TString back_name31 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/WZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input31 = TFile::Open( back_name31 );
   TTree *background31 = (TTree*)back_input31->Get("rootTupleTreeVeryLoose/tree");
   EventCount31 = (TH1I*)back_input31->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n31 = EventCount31->GetBinContent(1);
   Double_t backgroundWeight31 = back_xsec31/n31;
   factory->AddBackgroundTree( background31, backgroundWeight31 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input31->GetName() <<       std::endl;

   TH1I *EventCount32= new TH1I();
   TString back_name32 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ZZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input32 = TFile::Open( back_name32 );
   TTree *background32 = (TTree*)back_input32->Get("rootTupleTreeVeryLoose/tree");
   EventCount32 = (TH1I*)back_input32->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n32 = EventCount32->GetBinContent(1);
   Double_t backgroundWeight32 = back_xsec32/n32;
   factory->AddBackgroundTree( background32, backgroundWeight32 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input32->GetName() <<       std::endl;

   TH1I *EventCount33= new TH1I();
   TString back_name33 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_ext1/analysisTree.root";
   TFile *back_input33 = TFile::Open( back_name33 );
   TTree *background33 = (TTree*)back_input33->Get("rootTupleTreeVeryLoose/tree");
   EventCount33 = (TH1I*)back_input33->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n33 = EventCount33->GetBinContent(1);
   Double_t backgroundWeight33 = back_xsec33/n33;
   factory->AddBackgroundTree( background33, backgroundWeight33 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input33->GetName() <<       std::endl;

   TH1I *EventCount34= new TH1I();
   TString back_name34 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_ext2/analysisTree.root";
   TFile *back_input34 = TFile::Open( back_name34 );
   TTree *background34 = (TTree*)back_input34->Get("rootTupleTreeVeryLoose/tree");
   EventCount34 = (TH1I*)back_input34->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n34 = EventCount34->GetBinContent(1);
   Double_t backgroundWeight34 = back_xsec34/n34;
   factory->AddBackgroundTree( background34, backgroundWeight34 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input34->GetName() <<       std::endl;

   TH1I *EventCount35= new TH1I();
   TString back_name35 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTZToLL_M-1to10_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/analysisTree.root";
   TFile *back_input35 = TFile::Open( back_name35 );
   TTree *background35 = (TTree*)back_input35->Get("rootTupleTreeVeryLoose/tree");
   EventCount35 = (TH1I*)back_input35->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n35 = EventCount35->GetBinContent(1);
   Double_t backgroundWeight35 = back_xsec35/n35;
   factory->AddBackgroundTree( background35, backgroundWeight35 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input35->GetName() <<       std::endl;

   TH1I *EventCount36= new TH1I();
   TString back_name36 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8_ext1/analysisTree.root";
   TFile *back_input36 = TFile::Open( back_name36 );
   TTree *background36 = (TTree*)back_input36->Get("rootTupleTreeVeryLoose/tree");
   EventCount36 = (TH1I*)back_input36->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n36 = EventCount36->GetBinContent(1);
   Double_t backgroundWeight36 = back_xsec36/n36;
   factory->AddBackgroundTree( background36, backgroundWeight36 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input36->GetName() <<       std::endl;

   TH1I *EventCount37= new TH1I();
   TString back_name37 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8_ext2/analysisTree.root";
   TFile *back_input37 = TFile::Open( back_name37 );
   TTree *background37 = (TTree*)back_input37->Get("rootTupleTreeVeryLoose/tree");
   EventCount37 = (TH1I*)back_input37->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n37 = EventCount37->GetBinContent(1);
   Double_t backgroundWeight37 = back_xsec37/n37;
   factory->AddBackgroundTree( background37, backgroundWeight37 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input37->GetName() <<       std::endl;

   TH1I *EventCount38= new TH1I();
   TString back_name38 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8_ext3/analysisTree.root";
   TFile *back_input38 = TFile::Open( back_name38 );
   TTree *background38 = (TTree*)back_input38->Get("rootTupleTreeVeryLoose/tree");
   EventCount38 = (TH1I*)back_input38->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n38 = EventCount38->GetBinContent(1);
   Double_t backgroundWeight38 = back_xsec38/n38;
   factory->AddBackgroundTree( background38, backgroundWeight38 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input38->GetName() <<       std::endl;

   TH1I *EventCount39= new TH1I();
   TString back_name39 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/tZq_ll_4f_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input39 = TFile::Open( back_name39 );
   TTree *background39 = (TTree*)back_input39->Get("rootTupleTreeVeryLoose/tree");
   EventCount39 = (TH1I*)back_input39->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n39 = EventCount39->GetBinContent(1);
   Double_t backgroundWeight39 = back_xsec39/n39;
   factory->AddBackgroundTree( background39, backgroundWeight39 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input39->GetName() <<       std::endl;

   TH1I *EventCount40= new TH1I();
   TString back_name40 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ST_tWll_5f_LO_13TeV_MadGraph_pythia8/analysisTree.root";
   TFile *back_input40 = TFile::Open( back_name40 );
   TTree *background40 = (TTree*)back_input40->Get("rootTupleTreeVeryLoose/tree");
   EventCount40 = (TH1I*)back_input40->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n40 = EventCount40->GetBinContent(1);
   Double_t backgroundWeight40 = back_xsec40/n40;
   factory->AddBackgroundTree( background40, backgroundWeight40 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input40->GetName() <<       std::endl;

   TH1I *EventCount41= new TH1I();
   TString back_name41 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTTT_TuneCUETP8M2T4_13TeV-amcatnlo-pythia8/analysisTree.root";
   TFile *back_input41 = TFile::Open( back_name41 );
   TTree *background41 = (TTree*)back_input41->Get("rootTupleTreeVeryLoose/tree");
   EventCount41 = (TH1I*)back_input41->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n41 = EventCount41->GetBinContent(1);
   Double_t backgroundWeight41 = back_xsec41/n41;
   factory->AddBackgroundTree( background41, backgroundWeight41 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input41->GetName() <<       std::endl;

   TH1I *EventCount42= new TH1I();
   TString back_name42 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTWW_TuneCUETP8M2T4_13TeV-madgraph-pythia8/analysisTree.root";
   TFile *back_input42 = TFile::Open( back_name42 );
   TTree *background42 = (TTree*)back_input42->Get("rootTupleTreeVeryLoose/tree");
   EventCount42 = (TH1I*)back_input42->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n42 = EventCount42->GetBinContent(1);
   Double_t backgroundWeight42 = back_xsec42/n42;
   factory->AddBackgroundTree( background42, backgroundWeight42 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input42->GetName() <<       std::endl;

   TH1I *EventCount43= new TH1I();
   TString back_name43 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTWZ_TuneCUETP8M2T4_13TeV-madgraph-pythia8/analysisTree.root";
   TFile *back_input43 = TFile::Open( back_name43 );
   TTree *background43 = (TTree*)back_input43->Get("rootTupleTreeVeryLoose/tree");
   EventCount43 = (TH1I*)back_input43->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n43 = EventCount43->GetBinContent(1);
   Double_t backgroundWeight43 = back_xsec43/n43;
   factory->AddBackgroundTree( background43, backgroundWeight43 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input43->GetName() <<       std::endl;

   TH1I *EventCount44= new TH1I();
   TString back_name44 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/TTZZ_TuneCUETP8M2T4_13TeV-madgraph-pythia8/analysisTree.root";
   TFile *back_input44 = TFile::Open( back_name44 );
   TTree *background44 = (TTree*)back_input44->Get("rootTupleTreeVeryLoose/tree");
   EventCount44 = (TH1I*)back_input44->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n44 = EventCount44->GetBinContent(1);
   Double_t backgroundWeight44 = back_xsec44/n44;
   factory->AddBackgroundTree( background44, backgroundWeight44 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input44->GetName() <<       std::endl;

   TH1I *EventCount45= new TH1I();
   TString back_name45 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/ttHToNonbb_M125_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/analysisTree.root";
   TFile *back_input45 = TFile::Open( back_name45 );
   TTree *background45 = (TTree*)back_input45->Get("rootTupleTreeVeryLoose/tree");
   EventCount45 = (TH1I*)back_input45->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n45 = EventCount45->GetBinContent(1);
   Double_t backgroundWeight45 = back_xsec45/n45;
   factory->AddBackgroundTree( background45, backgroundWeight45 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input45->GetName() <<       std::endl;

   TH1I *EventCount46= new TH1I();
   TString back_name46 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/GluGluHToZZTo4L_M125_13TeV_powheg2_JHUgenV6_pythia8/analysisTree.root";
   TFile *back_input46 = TFile::Open( back_name46 );
   TTree *background46 = (TTree*)back_input46->Get("rootTupleTreeVeryLoose/tree");
   EventCount46 = (TH1I*)back_input46->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n46 = EventCount46->GetBinContent(1);
   Double_t backgroundWeight46 = back_xsec46/n46;
   factory->AddBackgroundTree( background46, backgroundWeight46 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input46->GetName() <<       std::endl;

   TH1I *EventCount47= new TH1I();
   TString back_name47 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/VBF_HToZZTo4L_M125_13TeV_powheg2_JHUgenV6_pythia8/analysisTree.root";
   TFile *back_input47 = TFile::Open( back_name47 );
   TTree *background47 = (TTree*)back_input47->Get("rootTupleTreeVeryLoose/tree");
   EventCount47 = (TH1I*)back_input47->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n47 = EventCount47->GetBinContent(1);
   Double_t backgroundWeight47 = back_xsec47/n47;
   factory->AddBackgroundTree( background47, backgroundWeight47 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input47->GetName() <<       std::endl;

   TH1I *EventCount48= new TH1I();
   TString back_name48 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/bbH_HToZZTo4L_M125_13TeV_JHUgenV702_pythia8/analysisTree.root";
   TFile *back_input48 = TFile::Open( back_name48 );
   TTree *background48 = (TTree*)back_input48->Get("rootTupleTreeVeryLoose/tree");
   EventCount48 = (TH1I*)back_input48->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n48 = EventCount48->GetBinContent(1);
   Double_t backgroundWeight48 = back_xsec48/n48;
   factory->AddBackgroundTree( background48, backgroundWeight48 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input48->GetName() <<       std::endl;

   TH1I *EventCount49= new TH1I();
   TString back_name49 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_notTTbarDY/VHToNonbb_M125_13TeV_amcatnloFXFX_madspin_pythia8/analysisTree.root";
   TFile *back_input49 = TFile::Open( back_name49 );
   TTree *background49 = (TTree*)back_input49->Get("rootTupleTreeVeryLoose/tree");
   EventCount49 = (TH1I*)back_input49->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n49 = EventCount49->GetBinContent(1);
   Double_t backgroundWeight49 = back_xsec49/n49;
   factory->AddBackgroundTree( background49, backgroundWeight49 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input49->GetName() <<       std::endl;

   



   TH1I *EventCount50= new TH1I();
   TString back_name50 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/analysisTree.root";
   TFile *back_input50 = TFile::Open( back_name50 );
   TTree *background50 = (TTree*)back_input50->Get("rootTupleTreeVeryLoose/tree");
   EventCount50 = (TH1I*)back_input50->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n50 = EventCount50->GetBinContent(1);
   Double_t backgroundWeight50 = back_xsec50/n50;
   factory->AddBackgroundTree( background50, backgroundWeight50 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input50->GetName() <<       std::endl;

   TH1I *EventCount51= new TH1I();
   TString back_name51 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext1/analysisTree.root";
   TFile *back_input51 = TFile::Open( back_name51 );
   TTree *background51 = (TTree*)back_input51->Get("rootTupleTreeVeryLoose/tree");
   EventCount51 = (TH1I*)back_input51->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n51 = EventCount51->GetBinContent(1);
   Double_t backgroundWeight51 = back_xsec51/n51;
   factory->AddBackgroundTree( background51, backgroundWeight51 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input51->GetName() <<       std::endl;

   TH1I *EventCount52= new TH1I();
   TString back_name52 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/analysisTree.root";
   TFile *back_input52 = TFile::Open( back_name52 );
   TTree *background52 = (TTree*)back_input52->Get("rootTupleTreeVeryLoose/tree");
   EventCount52 = (TH1I*)back_input52->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n52 = EventCount52->GetBinContent(1);
   Double_t backgroundWeight52 = back_xsec52/n52;
   factory->AddBackgroundTree( background52, backgroundWeight52 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input52->GetName() <<       std::endl;

   TH1I *EventCount53= new TH1I();
   TString back_name53 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_ext2/analysisTree.root";
   TFile *back_input53 = TFile::Open( back_name53 );
   TTree *background53 = (TTree*)back_input53->Get("rootTupleTreeVeryLoose/tree");
   EventCount53 = (TH1I*)back_input53->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n53 = EventCount53->GetBinContent(1);
   Double_t backgroundWeight53 = back_xsec53/n53;
   factory->AddBackgroundTree( background53, backgroundWeight53 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input53->GetName() <<       std::endl;


   TH1I *EventCount54= new TH1I();
   TString back_name54 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/TTJets_Dilept_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8/analysisTree.root";
   TFile *back_input54 = TFile::Open( back_name54 );
   TTree *background54 = (TTree*)back_input54->Get("rootTupleTreeVeryLoose/tree");
   EventCount54 = (TH1I*)back_input54->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n54 = EventCount54->GetBinContent(1);
   Double_t backgroundWeight54 = back_xsec54/n54;
   factory->AddBackgroundTree( background54, backgroundWeight54 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input54->GetName() <<       std::endl;

   TH1I *EventCount55= new TH1I();
   TString back_name55 = "/cms/mchristos/ANN/Seesaw/2016/92X/MC_TTbarDY/TTJets_Dilept_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8_ext1/analysisTree.root";
   TFile *back_input55 = TFile::Open( back_name55 );
   TTree *background55 = (TTree*)back_input55->Get("rootTupleTreeVeryLoose/tree");
   EventCount55 = (TH1I*)back_input55->Get("veryLooseDileptonEventFilter/EventCount/EventCounter");
   double n55 = EventCount55->GetBinContent(1);
   Double_t backgroundWeight55 = back_xsec55/n55;
   factory->AddBackgroundTree( background55, backgroundWeight55 );
   std::cout << "--- TMVAClassification       : Using Background input file: "<< back_input55->GetName() <<       std::endl;

   

   


   //cout <<"BG Weight: " <<ConversionWeight<<endl;
   
   // Apply additional cuts on the signal and background samples (can be different)
   //TCut mycuts = "";
   //TCut mycutb = "";
   TCut mycuts = ""; //&&LightLeptonIsTight0==1&&LightLeptonIsTight1==1&&LightLeptonIsTight2==1";   ///TCut mycuts = "";
   mycuts += ""; //PassMETFilters
   
   mycuts += "(((((HLTIsoMu24vAccept[0]>0||HLTIsoTkMu24vAccept[0]>0)*(MuonN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==2)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2)+(Alt$(LightLeptonFlavor[2],0)==2))==0)))>25)*(SampleType[0]==4||SampleType[0]==-1))>0)||(((HLTEle27WPTightGsfvAccept[0]>0)*(ElectronN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==1)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1)+(Alt$(LightLeptonFlavor[1],0)==1))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1)+(Alt$(LightLeptonFlavor[1],0)==1)+(Alt$(LightLeptonFlavor[2],0)==1))==0)))>30)*(SampleType[0]==5||SampleType[0]==-1))>0&&((HLTIsoMu24vAccept[0]>0||HLTIsoTkMu24vAccept[0]>0)*(MuonN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==2)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2)+(Alt$(LightLeptonFlavor[2],0)==2))==0)))>25)*(SampleType[0]==5))==0))>0)"; //SingleIsoLeptonTrigAccept2016>0
   
   mycuts += "(((LightLeptonMinMOSSF[0]>12&&LightLeptonMinMOSSF[0]<=LightLeptonMinMSSSF[0])||(LightLeptonMinMSSSF[0]>12&&LightLeptonMinMSSSF[0]<LightLeptonMinMOSSF[0])))";//LowMassVeto==1
   
   mycuts += "(((((Sum$(ElectronDRHltEle27WPTightGsfv<0.2&&ElectronIsTight>0&&ElectronPt>30)))*(SampleType[0]==5||SampleType[0]==-1))>0)||((((Sum$(MuonDRHltIsoMu24OrTkMu24v<0.2&&MuonIsTight>0&&MuonPt>25)))*(SampleType[0]==4||SampleType[0]==-1))>0))";//HasTriggerMatch_2016>0
   
   mycuts += "(((((LightLeptonBestMOSSF[0]>(((91.1876*(1>0))+15.)*(1>0)))&&((LightLeptonN[0]==3)*(LightLeptonNOSSF[0]>0)>0)))==1&&LightLeptonN[0]==3)) || ((((LightLeptonBestMOSSF[0]>(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonBestMOSSF[0]<(((91.1876*(1>0))+15.)*(1>0)))*(1>0))>0&&(PFMETType1[0])>100&&LightLeptonN[0]==3)) || (((((LightLeptonBestMOSSF[0]<(((91.1876*(1>0))-15.)*(1>0)))&&((LightLeptonN[0]==3)*(LightLeptonNOSSF[0]>0)>0)))==1&&(PFMETType1[0])>50&&((LightLeptonN[0]==3)*(LightLeptonBestMOSSF[0]<(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonMass[0]<(((91.1876*(1>0))+15.)*(1>0))&&LightLeptonMass[0]>(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonNOSSF[0]>0))==0&&LightLeptonN[0]==3)) || ((LightLeptonNOSSF[0]==0&&LightLeptonN[0]==3))";


   //mycuts += "HLTMu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVLDZvAccept[0]>0";
  // mycuts += "((LightLeptonN[0]==3)*(LightLeptonOnZN[0]>0))&&PFMETType1[0]>100&&LightLeptonN[0]==3&&LightLeptonMinMOSSF[0]>12"//L3OnZ
   
   TCut mycutb = ""; //&&LightLeptonIsTight0==1&&LightLeptonIsTight1==1&&LightLeptonIsTight2==1";   ///TCut mycuts = "";
   mycutb += ""; //PassMETFilters
   
   mycutb += "(((((HLTIsoMu24vAccept[0]>0||HLTIsoTkMu24vAccept[0]>0)*(MuonN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==2)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2)+(Alt$(LightLeptonFlavor[2],0)==2))==0)))>25)*(SampleType[0]==4||SampleType[0]==-1))>0)||(((HLTEle27WPTightGsfvAccept[0]>0)*(ElectronN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==1)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1)+(Alt$(LightLeptonFlavor[1],0)==1))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==1)*(((Alt$(LightLeptonFlavor[0],0)==1)+(Alt$(LightLeptonFlavor[1],0)==1)+(Alt$(LightLeptonFlavor[2],0)==1))==0)))>30)*(SampleType[0]==5||SampleType[0]==-1))>0&&((HLTIsoMu24vAccept[0]>0||HLTIsoTkMu24vAccept[0]>0)*(MuonN[0]>0&&((Alt$(LightLeptonPt[0],0)*(Alt$(LightLeptonFlavor[0],0)==2)+Alt$(LightLeptonPt[1],0)*(Alt$(LightLeptonFlavor[1],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2))==0)+Alt$(LightLeptonPt[2],0)*(Alt$(LightLeptonFlavor[2],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2))==0)+Alt$(LightLeptonPt[3],0)*(Alt$(LightLeptonFlavor[3],0)==2)*(((Alt$(LightLeptonFlavor[0],0)==2)+(Alt$(LightLeptonFlavor[1],0)==2)+(Alt$(LightLeptonFlavor[2],0)==2))==0)))>25)*(SampleType[0]==5))==0))>0)"; //SingleIsoLeptonTrigAccept2016>0
   
   mycutb += "(((LightLeptonMinMOSSF[0]>12&&LightLeptonMinMOSSF[0]<=LightLeptonMinMSSSF[0])||(LightLeptonMinMSSSF[0]>12&&LightLeptonMinMSSSF[0]<LightLeptonMinMOSSF[0])))";//LowMassVeto==1
   
   mycutb += "(((((Sum$(ElectronDRHltEle27WPTightGsfv<0.2&&ElectronIsTight>0&&ElectronPt>30)))*(SampleType[0]==5||SampleType[0]==-1))>0)||((((Sum$(MuonDRHltIsoMu24OrTkMu24v<0.2&&MuonIsTight>0&&MuonPt>25)))*(SampleType[0]==4||SampleType[0]==-1))>0))";//HasTriggerMatch_2016>0
   
   mycutb += "(((((LightLeptonBestMOSSF[0]>(((91.1876*(1>0))+15.)*(1>0)))&&((LightLeptonN[0]==3)*(LightLeptonNOSSF[0]>0)>0)))==1&&LightLeptonN[0]==3)) || ((((LightLeptonBestMOSSF[0]>(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonBestMOSSF[0]<(((91.1876*(1>0))+15.)*(1>0)))*(1>0))>0&&(PFMETType1[0])>100&&LightLeptonN[0]==3)) || (((((LightLeptonBestMOSSF[0]<(((91.1876*(1>0))-15.)*(1>0)))&&((LightLeptonN[0]==3)*(LightLeptonNOSSF[0]>0)>0)))==1&&(PFMETType1[0])>50&&((LightLeptonN[0]==3)*(LightLeptonBestMOSSF[0]<(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonMass[0]<(((91.1876*(1>0))+15.)*(1>0))&&LightLeptonMass[0]>(((91.1876*(1>0))-15.)*(1>0))&&LightLeptonNOSSF[0]>0))==0&&LightLeptonN[0]==3)) || ((LightLeptonNOSSF[0]==0&&LightLeptonN[0]==3))";

   //mycutb += "(LightLeptonNOSSF[0]<=0)&&LightLeptonN[0]==3";
   //TCut mycutb = "";


   factory->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        "nTrain_Signal=61690:nTrain_Background=2016120:SplitMode=Random:NormMode=NumEvents:!V" );

   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
   factory->BookMethod( TMVA::Types::kBDT, "Optimized BDT",
                           "!H:!V:NTrees=10000:MinNodeSize=1.0%:BoostType=Grad:Shrinkage=1:nCuts=10:MaxDepth=20::BaggedSampleFraction=0.5" );
   //ncuts
   /*factory->BookMethod( TMVA::Types::kBDT, "100 cuts",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=100:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "50 cuts",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=50:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "20 cuts",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "10 cuts",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=10:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "5 cuts",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=5:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "1 cut",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=1:MaxDepth=3::BaggedSampleFraction=0.5" );
   */
   //depth
   /*factory->BookMethod( TMVA::Types::kBDT, "depth 1",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=1::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 2",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=2::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 4",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=4::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 6",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=6::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 8",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=8::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 10",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=10::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 20",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=20::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "depth 100",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=100::BaggedSampleFraction=0.5" );
   */
   //ntrees
   /*factory->BookMethod( TMVA::Types::kBDT, "150 trees",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "450 trees",
                           "!H:!V:NTrees=450:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "850 trees",
                           "!H:!V:NTrees=850:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "1000 trees",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "2000 trees",
                           "!H:!V:NTrees=2000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "10000 trees",
                           "!H:!V:NTrees=10000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   */
   //minNodePercentage
   /*factory->BookMethod( TMVA::Types::kBDT, "1%",
                           "!H:!V:NTrees=150:MinNodeSize=1.0%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "2.5%",
                           "!H:!V:NTrees=150:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "5.0%",
                           "!H:!V:NTrees=150:MinNodeSize=5.0%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "7.5%",
                           "!H:!V:NTrees=150:MinNodeSize=7.5%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "20%",
                           "!H:!V:NTrees=150:MinNodeSize=20.0%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );
   factory->BookMethod( TMVA::Types::kBDT, "10%",
                           "!H:!V:NTrees=150:MinNodeSize=30.0%:BoostType=Grad:Shrinkage=1:nCuts=20:MaxDepth=3::BaggedSampleFraction=0.5" );*/
   
// For an example of the category classifier usage, see: TMVAClassificationCategory

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   //factory->OptimizeAllMethods("SigEffAt001","Scan");
   //factory->OptimizeAllMethods();
   //factory->OptimizeAllMethods("MaxDepth","FitGA");

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;

   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()){
         //gROOT->LoadMacro("mvaeffs.C++");
         //gROOT->LoadMacro("BDT.C++");
         //gROOT->LoadMacro("BDTControlPlots.C++");
         TMVA::TMVAGui( outfileName );
   }
}

