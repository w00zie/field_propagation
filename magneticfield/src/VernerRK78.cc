/*
 ********************************************************************
 //                     |   Acknowledgement   |
 //                    -------------------------
 // The following code uses the work of Jim Verner, obtained from
 // http://people.math.sfu.ca/~jverner/
 //
 // Sets of all coefficients provided in attachments are copyrighted
 // as such by the author. They many not be published for general
 // distribution. They may be used for any research, industrial
 // application or development of software provided that any
 // product arising using any set of coefficients acknowledges this
 // source and includes the URL for this site within the produced
 // item.
 //
 ********************************************************************
 */

//  Verner - 8 - 5(4) implementation by Somnath Banerjee
//  Supervision / code review: John Apostolakis
//
//  Sponsored by Google in Google Summer of Code 2015.
// 
//  First version:  9 June 2015
//
//  This code is made available subject to the Geant4 license, a copy of
//  which is available at
//  http://geant4.org/license
//  
//  History
// -----------------------------
//  Created by Somnath on 9 June 2015.
//

#include "VernerRK78.hh"
#include "G4LineSection.hh"


//Constructor
VernerRK78::VernerRK78(G4EquationOfMotion *EqRhs,
                       G4int noIntegrationVariables,
                       G4bool primary)
: G4MagIntegratorStepper(EqRhs, noIntegrationVariables){
    
    const G4int numberOfVariables = noIntegrationVariables;
    
    //New Chunk of memory being created for use by the stepper
    
    //aki - for storing intermediate RHS
    ak2 = new G4double[numberOfVariables];
    ak3 = new G4double[numberOfVariables];
    ak4 = new G4double[numberOfVariables];
    ak5 = new G4double[numberOfVariables];
    ak6 = new G4double[numberOfVariables];
    ak7 = new G4double[numberOfVariables];
    ak8 = new G4double[numberOfVariables];
    ak9 = new G4double[numberOfVariables];
    ak10 = new G4double[numberOfVariables];
    ak11 = new G4double[numberOfVariables];
    ak12 = new G4double[numberOfVariables];
    ak13 = new G4double[numberOfVariables];
    
    yTemp = new G4double[numberOfVariables];
    yIn = new G4double[numberOfVariables] ;
    
    fLastInitialVector = new G4double[numberOfVariables] ;
    fLastFinalVector = new G4double[numberOfVariables] ;
    fLastDyDx = new G4double[numberOfVariables];
    
    fMidVector = new G4double[numberOfVariables];
    fMidError =  new G4double[numberOfVariables];
    if( primary )
    {
        fAuxStepper = new VernerRK78(EqRhs, numberOfVariables,
                                     !primary);
    }
}


//Destructor
VernerRK78::~VernerRK78(){
    //clear all previously allocated memory for stepper and DistChord
    delete[] ak2;
    delete[] ak3;
    delete[] ak4;
    delete[] ak5;
    delete[] ak6;
    delete[] ak7;
    delete[] ak8;
    delete[] ak9;
    delete[] ak10;
    delete[] ak11;
    delete[] ak12;
    delete[] ak13;
    delete[] yTemp;
    delete[] yIn;
    
    delete[] fLastInitialVector;
    delete[] fLastFinalVector;
    delete[] fLastDyDx;
    delete[] fMidVector;
    delete[] fMidError;
    
    delete fAuxStepper;
    
}


//Stepper :

// Passing in the value of yInput[],the first time dydx[] and Step length
// Giving back yOut and yErr arrays for output and error respectively

void VernerRK78::Stepper(const G4double yInput[],
                         const G4double dydx[],
                         G4double Step,
                         G4double yOut[],
                         G4double yErr[])
{
    G4int i;
    
    //The various constants defined on the basis of butcher tableu
    //G4double - only once
    const G4double
    
    b21 =  .5000000000000000000000000000000000000000e-1,
    
    b31 = -.6993164062500000000000000000000000000000e-2,
    b32 =  .1135556640625000000000000000000000000000,
    
    b41 =  .3996093750000000000000000000000000000000e-1,
    b42 =  0.,
    b43 =  .1198828125000000000000000000000000000000,
    
    b51 =  .3613975628004575124052940721184028345129,
    b52 =  0.,
    b53 = -1.341524066700492771819987788202715834917,
    b54 =  1.370126503900035259414693716084313000404,
    
    b61 =  .4904720279720279720279720279720279720280e-1,
    b62 =  0.,
    b63 =  0.,
    b64 =  .2350972042214404739862988335493427143122,
    b65 =  .1808555929813567288109039636534544884850,
    
    b71 =  .6169289044289044289044289044289044289044e-1,
    b72 =  0.,
    b73 =  0.,
    b74 =  .1123656831464027662262557035130015442303,
    b75 = -.3885046071451366767049048108111244567456e-1,
    b76 =  .1979188712522045855379188712522045855379e-1,
    
    b81 = -1.767630240222326875735597119572145586714,
    b82 =  0.,
    b83 =  0.,
    b84 = -62.50000000000000000000000000000000000000,
    b85 = -6.061889377376669100821361459659331999758,
    b86 =  5.650823198222763138561298030600840174201,
    b87 =  65.62169641937623283799566054863063741227,
    
    b91 = -1.180945066554970799825116282628297957882,
    b92 =  0.,
    b93 =  0.,
    b94 = -41.50473441114320841606641502701994225874,
    b95 = -4.434438319103725011225169229846100211776,
    b96 =  4.260408188586133024812193710744693240761,
    b97 =  43.75364022446171584987676829438379303004,
    b98 =  .7871425489912310687446475044226307550860e-2,
    
    b101 = -1.281405999441488405459510291182054246266,
    b102 =  0.,
    b103 =  0.,
    b104 = -45.04713996013986630220754257136007322267,
    b105 = -4.731362069449576477311464265491282810943,
    b106 =  4.514967016593807841185851584597240996214,
    b107 =  47.44909557172985134869022392235929015114,
    b108 =  .1059228297111661135687393955516542875228e-1,
    b109 = -.5746842263844616254432318478286296232021e-2,
    
    b111 = -1.724470134262485191756709817484481861731,
    b112 =  0.,
    b113 =  0.,
    b114 = -60.92349008483054016518434619253765246063,
    b115 = -5.951518376222392455202832767061854868290,
    b116 =  5.556523730698456235979791650843592496839,
    b117 =  63.98301198033305336837536378635995939281,
    b118 =  .1464202825041496159275921391759452676003e-1,
    b119 =  .6460408772358203603621865144977650714892e-1,
    b1110 = -.7930323169008878984024452548693373291447e-1,
    
    b121 = -3.301622667747079016353994789790983625569,
    b122 =  0.,
    b123 =  0.,
    b124 = -118.0112723597525085666923303957898868510,
    b125 = -10.14142238845611248642783916034510897595,
    b126 =  9.139311332232057923544012273556827000619,
    b127 =  123.3759428284042683684847180986501894364,
    b128 =  4.623244378874580474839807625067630924792,
    b129 = -3.383277738068201923652550971536811240814,
    b1210 =  4.527592100324618189451265339351129035325,
    b1211 = -5.828495485811622963193088019162985703755,
    
    b131 = -3.039515033766309030040102851821200251056,
    b132 =  0.,
    b133 =  0.,
    b134 = -109.2608680894176254686444192322164623352,
    b135 = -9.290642497400293449717665542656897549158,
    b136 =  8.430504981764911142134299253836167803454,
    b137 =  114.2010010378331313557424041095523427476,
    b138 = -.9637271342145479358162375658987901652762,
    b139 = -5.034884088802189791198680336183332323118,
    b1310 =  5.958130824002923177540402165388172072794,
    b1311 =  0.,
    b1312 =  0.,
    
    c1 =  .4427989419007951074716746668098518862111e-1,
    c2 =  0.,
    c3 =  0.,
    c4 =  0.,
    c5 =  0.,
    c6 =  .3541049391724448744815552028733568354121,
    c7 =  .2479692154956437828667629415370663023884,
    c8 = -15.69420203883808405099207034271191213468,
    c9 =  25.08406496555856261343930031237186278518,
    c10 = -31.73836778626027646833156112007297739997,
    c11 =  22.93828327398878395231483560344797018313,
    c12 = -.2361324633071542145259900641263517600737,
   

    //Redundancy here
    dc1 = .4427989419007951074716746668098518862111e-1 - .4431261522908979212486436510209029764893e-1,
    dc2 =  0.,
    dc3 =  0.,
    dc4 =  0.,
    dc5 =  0.,
    dc6 =  .3541049391724448744815552028733568354121 - .3546095642343226447863179350895055038855,
    dc7 = .2479692154956437828667629415370663023884 - .2478480431366653069619986721504458660016,
    dc8 =  -15.69420203883808405099207034271191213468 - 4.448134732475784492725128317159648871312,
    dc9 =  25.08406496555856261343930031237186278518 - 19.84688636611873369930932399297687935291,
    dc10 =  -31.73836778626027646833156112007297739997 + 23.58162337746561841969517960870394965085,
    dc11 =  22.93828327398878395231483560344797018313,
    dc12 =  -.2361324633071542145259900641263517600737,
    dc13 =  +.3601679437289775162124536737746202409110 ;
    
    
    
    //end of declaration !
    
    
    const G4int numberOfVariables= this->GetNumberOfVariables();
    
    // The number of variables to be integrated over
    yOut[7] = yTemp[7]  = yIn[7];
    //  Saving yInput because yInput and yOut can be aliases for same array
    
    for(i=0;i<numberOfVariables;i++)
    {
        yIn[i]=yInput[i];
    }
    
    
    
    // RightHandSide(yIn, dydx) ;
    // 1st Step - Not doing, getting passed
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + b21*Step*dydx[i] ;
    }
    RightHandSide(yTemp, ak2) ;              // 2nd Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b31*dydx[i] + b32*ak2[i]) ;
    }
    RightHandSide(yTemp, ak3) ;              // 3rd Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b41*dydx[i] + b42*ak2[i] + b43*ak3[i]) ;
    }
    RightHandSide(yTemp, ak4) ;              // 4th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b51*dydx[i] + b52*ak2[i] + b53*ak3[i] +
                                  b54*ak4[i]) ;
    }
    RightHandSide(yTemp, ak5) ;              // 5th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b61*dydx[i] + b62*ak2[i] + b63*ak3[i] +
                                  b64*ak4[i] + b65*ak5[i]) ;
    }
    RightHandSide(yTemp, ak6) ;              // 6th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b71*dydx[i] + b72*ak2[i] + b73*ak3[i] +
                                  b74*ak4[i] + b75*ak5[i] + b76*ak6[i]);
    }
    RightHandSide(yTemp, ak7);				//7th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b81*dydx[i] + b82*ak2[i] + b83*ak3[i] +
                                  b84*ak4[i] + b85*ak5[i] + b86*ak6[i] +
                                  b87*ak7[i]);
    }
    RightHandSide(yTemp, ak8);				//8th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b91*dydx[i] + b92*ak2[i] + b93*ak3[i] +
                                  b94*ak4[i] + b95*ak5[i] + b96*ak6[i] +
                                  b97*ak7[i] + b98*ak8[i] );
    }
    RightHandSide(yTemp, ak9);          //9th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b101*dydx[i] + b102*ak2[i] + b103*ak3[i] +
                                  b104*ak4[i] + b105*ak5[i] + b106*ak6[i] +
                                  b107*ak7[i] + b108*ak8[i] + b109*ak9[i]);
    }
    RightHandSide(yTemp, ak10);          //10th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b111*dydx[i] + b112*ak2[i] + b113*ak3[i] +
                                  b114*ak4[i] + b115*ak5[i] + b116*ak6[i] +
                                  b117*ak7[i] + b118*ak8[i] + b119*ak9[i] +
                                  b1110*ak10[i]);
    }
    RightHandSide(yTemp, ak11);			//11th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b121*dydx[i] + b122*ak2[i] + b123*ak3[i] +
                                  b124*ak4[i] + b125*ak5[i] + b126*ak6[i] +
                                  b127*ak7[i] + b128*ak8[i] + b129*ak9[i] +
                                  b1210*ak10[i] + b1211*ak11[i]);
    }
    RightHandSide(yTemp, ak12);			//12th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b131*dydx[i] + b132*ak2[i] + b133*ak3[i] +
                                  b134*ak4[i] + b135*ak5[i] + b136*ak6[i] +
                                  b137*ak7[i] + b138*ak8[i] + b139*ak9[i] +
                                  b1310*ak10[i] + b1311*ak11[i] + b1312*ak12[i]);
    }
    RightHandSide(yTemp, ak13);			//13th Step
    
    for(i=0;i<numberOfVariables;i++)
    {
        // Accumulate increments with proper weights
        
        yOut[i] = yIn[i] + Step*(c1*dydx[i] + c2*ak2[i] + c3*ak3[i] +
                                 c4*ak4[i] + c5*ak5[i] + c6*ak6[i] +
                                 c7*ak7[i] + c8*ak8[i] +c9*ak9[i] + c10*ak10[i]
                                 + c11*ak11[i] + c12*ak12[i]) ;
        
        // Estimate error as difference between 4th and
        // 5th order methods
        
        yErr[i] = Step*(dc1*dydx[i] + dc2*ak2[i] + dc3*ak3[i] + dc4*ak4[i] +
                        dc5*ak5[i] + dc6*ak6[i] + dc7*ak7[i] + dc8*ak8[i] +
                        dc9*ak9[i] + dc10*ak10[i] + dc11*ak11[i] + dc12*ak12[i]
                        + dc13*ak13[i] ) ;
        
        
    }
    
    fLastStepLength = Step;
    
    return ;
}


//The following has not been tested

//The DistChord() function fot the class - must define it here.
G4double  VernerRK78::DistChord() const
{
    G4double distLine, distChord;
    G4ThreeVector initialPoint, finalPoint, midPoint;
    
    // Store last initial and final points (they will be overwritten in self-Stepper call!)
    initialPoint = G4ThreeVector( fLastInitialVector[0],
                                 fLastInitialVector[1], fLastInitialVector[2]);
    finalPoint   = G4ThreeVector( fLastFinalVector[0],
                                 fLastFinalVector[1],  fLastFinalVector[2]);
    
    // Do half a step using StepNoErr
    
    fAuxStepper->Stepper( fLastInitialVector, fLastDyDx, 0.5 * fLastStepLength,
                         fMidVector,   fMidError );
    
    midPoint = G4ThreeVector( fMidVector[0], fMidVector[1], fMidVector[2]);
    
    // Use stored values of Initial and Endpoint + new Midpoint to evaluate
    //  distance of Chord
    
    
    if (initialPoint != finalPoint)
    {
        distLine  = G4LineSection::Distline( midPoint, initialPoint, finalPoint );
        distChord = distLine;
    }
    else
    {
        distChord = (midPoint-initialPoint).mag();
    }
    return distChord;
}


void VernerRK78::SetupInterpolate( const G4double yInput[],
                                  const G4double dydx[],
                                  const G4double Step ){


    //  FOUR ADDITIONAL STAGES FOR INTERPOLANT OF ORDER  7

    G4double
    //Coupling coefficients for
    //c14 = 1.0
    
    b141 =  .4427989419007951074716746668098518862111e-1 ,
    b142 =  0. ,
    b143 =  0. ,
    b144 =  0. ,
    b145 =  0. ,
    b146 =  .3541049391724448744815552028733568354121 ,
    b147 =  .2479692154956437828667629415370663023884 ,
    b148 = -15.69420203883808405099207034271191213468 ,
    b149 =  25.08406496555856261343930031237186278518 ,
    b1410 = -31.73836778626027646833156112007297739997 ,
    b1411 =  22.93828327398878395231483560344797018313 ,
    b1412 = -.2361324633071542145259900641263517600737 ,
    b1413 =  0. ,
    //
    //********************************************************
    //
    //  Coupling coefficients for
    //c15 =  .3110177634953863863927417318829099695921
    //  ----------------------------------------------------
    b151 =  .4620700646754963101730413150238116432863e-1 ,
    b152 =  0. ,
    b153 =  0. ,
    b154 =  0. ,
    b155 =  0. ,
    b156 =  .4503904160842480866828520384400679697151e-1 ,
    b157 =  .2336816697713424410788701065340221126565 ,
    b158 =  37.83901368421067410780338220861855254153 ,
    b159 = -15.94911328945424610266139490307397370835 ,
    b1510 =  23.02836835181610285142510596329590091940 ,
    b1511 = -44.85578507769412524816130998016948002745 ,
    b1512 = -.6379858768647444009509067402330140781326e-1 ,
    b1513 =  0. ,
    b1514 = -.1259503554386166268241032464519842162533e-1 ,
    //
    //********************************************************
    //
    //  Coupling coefficients for
    //c16 =  .1725
    //  ----------------------------------------------------
    b161 =  .5037946855482040993065158747220696112586e-1 ,
    b162 =  0. ,
    b163 =  0. ,
    b164 =  0. ,
    b165 =  0. ,
    b166 =  .4109836131046079339916530614028848248545e-1 ,
    b167 =  .1718054153348195783296309209549424619697 ,
    b168 =  4.61410531998151886974342237185977124648 ,
    b169 = -1.791667883085396449712744996746836471721 ,
    b1610 =  2.531658930485041408462243518792913614971 ,
    b1611 = -5.32497786020573071925718815977276269909 ,
    b1612 = -.3065532595385634734924449496356513113607e-1 ,
    b1613 =  0. ,
    b1614 = -.5254479979429613570549519094377878106127e-2 ,
    b1615 = -.8399194644224792997538653464258058697156e-1 ,
    //
    //********************************************************
    //
    //  Coupling coefficients for
    //c17 =  .7846
    //  ----------------------------------------------------
    b171 =  .4082897132997079620207118756242653796386e-1 ,
    b172 =  0. ,
    b173 =  0. ,
    b174 =  0. ,
    b175 =  0. ,
    b176 =  .4244479514247632218892086657732332485609 ,
    b177 =  .2326091531275234539465100096964845486081 ,
    b178 =  2.677982520711806062780528871014035962908 ,
    b179 =  .7420826657338945216477607044022963622057 ,
    b1710 =  .1460377847941461193920992339731312296021 ,
    b1711 = -3.579344509890565218033356743825917680543 ,
    b1712 =  .1138844389600173704531638716149985665239 ,
    b1713 =  0. ,
    b1714 =  .1267790651033190047378693537615687232109e-1 ,
    b1715 = -.7443436349946674429752785032561552478382e-1 ,
    b1716 =  .4782748079757851554575511473876987663388e-1 ;
    
    
    const G4int numberOfVariables= this->GetNumberOfVariables();
    
    //  Saving yInput because yInput and yOut can be aliases for same array
    for(int i=0;i<numberOfVariables;i++)
    {
        yIn[i]=yInput[i];
    }
    
    yTemp[7]  = yIn[7];

    
    //TODo
    ak14 = new G4double[numberOfVariables];
    ak15 = new G4double[numberOfVariables];
    ak16 = new G4double[numberOfVariables];
    ak17 = new G4double[numberOfVariables];
    
    
    
    //    calculating extra stage functions

    //Evaluate the stages :
    
    for(G4int i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b141*dydx[i] + b142*ak2[i] + b143*ak3[i] +
                                  b144*ak4[i] + b145*ak5[i] + b146*ak6[i] +
                                  b147*ak7[i] + b148*ak8[i] + b149*ak9[i] +
                                  b1410*ak10[i] + b1411*ak11[i] + b1412*ak12[i] +
                                  b1413*ak13[i] );
    }
    RightHandSide(yTemp, ak14);			//14th Stage
    
    for(G4int i=0; i<numberOfVariables; i++)
    {
        yTemp[i] = yIn[i] + Step*(b151*dydx[i] + b152*ak2[i] + b153*ak3[i] +
                                  b154*ak4[i] + b155*ak5[i] + b156*ak6[i] +
                                  b157*ak7[i] + b158*ak8[i] + b159*ak9[i] +
                                  b1510*ak10[i] + b1511*ak11[i] + b1512*ak12[i] +
                                  b1513*ak13[i] + b1514*ak14[i]);
    }
    RightHandSide(yTemp, ak15);			//15th Stage

    
    for(G4int i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b161*dydx[i] + b162*ak2[i] + b163*ak3[i] +
                                  b164*ak4[i] + b165*ak5[i] + b166*ak6[i] +
                                  b167*ak7[i] + b168*ak8[i] + b169*ak9[i] +
                                  b1610*ak10[i] + b1611*ak11[i] + b1612*ak12[i] +
                                  b1613*ak13[i] + b1614*ak14[i] +b1615*ak15[i]
                                  );
    }
    RightHandSide(yTemp, ak16);			//16th Stage

    for(G4int i=0;i<numberOfVariables;i++)
    {
        yTemp[i] = yIn[i] + Step*(b171*dydx[i] + b172*ak2[i] + b173*ak3[i] +
                                  b174*ak4[i] + b175*ak5[i] + b176*ak6[i] +
                                  b177*ak7[i] + b178*ak8[i] + b179*ak9[i] +
                                  b1710*ak10[i] + b1711*ak11[i] + b1712*ak12[i] +
                                  b1713*ak13[i] + b1714*ak14[i] +b1715*ak15[i] +
                                  b1716*ak16[i]);
    }
    RightHandSide(yTemp, ak17);			//17th Stage

    
}

void VernerRK78::Interpolate( const G4double yInput[],
                              const G4double dydx[],
                              const G4double Step,
                             		G4double yOut[],
                             		G4double tau ){
 	//Define the coefficients for the polynomials
    G4double bi7[18][8], b[18];
    G4int numberOfVariables = this->GetNumberOfVariables();


    //
    //  --------------------------------------------------------
    //  COEFFICIENTS FOR INTERPOLANT bi7  WITH  17  STAGES
    //  --------------------------------------------------------
    
    //  COEFFICIENTS OF   bi7[ 1]
    bi7[1][1] =  1. ,
    bi7[1][2] = -7.238550783576432811855355839508646327161 ,
    bi7[1][3] =  26.00913483254676138219215542805486438340 ,
    bi7[1][4] = -50.23684777762566731759165474184543812128 ,
    bi7[1][5] =  52.12072084601022449485077581012685809554 ,
    bi7[1][6] = -27.06472451211777193118825764262673140465 ,
    bi7[1][7] =  5.454547288952965694339504452480078562780 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[2]
    bi7[2][1] =  0. ,
    bi7[2][2] =  0. ,
    bi7[2][3] =  0. ,
    bi7[2][4] =  0. ,
    bi7[2][5] =  0. ,
    bi7[2][6] =  0. ,
    bi7[2][7] =  0. ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[3]
    bi7[3][1] =  0. ,
    bi7[3][2] =  0. ,
    bi7[3][3] =  0. ,
    bi7[3][4] =  0. ,
    bi7[3][5] =  0. ,
    bi7[3][6] =  0. ,
    bi7[3][7] =  0. ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[4]
    bi7[4][1] =  0. ,
    bi7[4][2] =  0. ,
    bi7[4][3] =  0. ,
    bi7[4][4] =  0. ,
    bi7[4][5] =  0. ,
    bi7[4][6] =  0. ,
    bi7[4][7] =  0. ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[5]
    bi7[5][1] =  0. ,
    bi7[5][2] =  0. ,
    bi7[5][3] =  0. ,
    bi7[5][4] =  0. ,
    bi7[5][5] =  0. ,
    bi7[5][6] =  0. ,
    bi7[5][7] =  0. ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[6]
    bi7[6][1] =  0. ,
    bi7[6][2] =  11.15330887588935170976376962782446833855 ,
    bi7[6][3] = -91.7609656398961659890179437322816238711 ,
    bi7[6][4] =  291.7074241722059450113911477530513089255 ,
    bi7[6][5] = -430.4096692910862817449451677633631387823 ,
    bi7[6][6] =  299.4531188198997479843407054776900024282 ,
    bi7[6][7] = -79.78911199784015209705095616004766020335 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[7]
    bi7[7][1] =  0. ,
    bi7[7][2] =  2.34875229807309355640904629061136935335 ,
    bi7[7][3] = -11.6724894172018429369093778842231443146 ,
    bi7[7][4] = -3.339139076505928386509206543237093540 ,
    bi7[7][5] =  94.885262249720610030798242337479596095 ,
    bi7[7][6] = -143.071126583012024456409244370652716962 ,
    bi7[7][7] =  61.0967097444217359754873031115590556707 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[8]
    bi7[8][1] =  0. ,
    bi7[8][2] = -1027.321675339240679090464776362465090654 ,
    bi7[8][3] =  9198.71432360760879019681406218311101879 ,
    bi7[8][4] = -33189.78048157363822223641020734287802492 ,
    bi7[8][5] =  57750.0831348887181073584126028277545727 ,
    bi7[8][6] = -47698.93315706261990169947144294597707756 ,
    bi7[8][7] =  14951.54365344033382142012769129774268946 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[9]
    bi7[9][1] =  0. ,
    bi7[9][2] =  1568.546608927281956416687915664731868885 ,
    bi7[9][3] = -13995.38852541600542155322174511897930298 ,
    bi7[9][4] =  50256.2124698102445419491620666726469821 ,
    bi7[9][5] = -86974.5128036219909523950692144595063700 ,
    bi7[9][6] =  71494.7977095997701213661747332399327008 ,
    bi7[9][7] = -22324.57139433374168317029445568645401598 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[10]
    bi7[10][1] =  0. ,
    bi7[10][2] = -2000.882061921041961546811133479107090218 ,
    bi7[10][3] =  17864.36380347691630038038755096765127729 ,
    bi7[10][4] = -64205.1907515562863000297926577113695108 ,
    bi7[10][5] =  111224.8489930378077126420609392735999202 ,
    bi7[10][6] = -91509.3392102130338542605593697286718077 ,
    bi7[10][7] =  28594.46085938937782634638310955782423389 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[11]
    bi7[11][1] =  0. ,
    bi7[11][2] =  1496.620400693446268810344884971434468267 ,
    bi7[11][3] = -13397.55405171476021512904990709508924800 ,
    bi7[11][4] =  48323.5602199437493999696912750109765015 ,
    bi7[11][5] = -84051.4283423393032636942266780744607468 ,
    bi7[11][6] =  69399.8582111570893316100585838633124312 ,
    bi7[11][7] = -21748.11815446623273761450332307272543593 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[12]
    bi7[12][1] =  0. ,
    bi7[12][2] = -16.41320775560933621675902845723196069900 ,
    bi7[12][3] =  147.6097045407002371315249807692915435608 ,
    bi7[12][4] = -535.719963714732106447158760197417632645 ,
    bi7[12][5] =  938.286247077820650371318861625025573381 ,
    bi7[12][6] = -779.438309639349328345148153897689081893 ,
    bi7[12][7] =  245.4393970278627292916961100938952065362 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[13]
    bi7[13][1] =  0. ,
    bi7[13][2] =  0.,
    bi7[13][3] =  0. ,
    bi7[13][4] =  0. ,
    bi7[13][5] =  0. ,
    bi7[13][6] =  0. ,
    bi7[13][7] =  0. ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[14]
    bi7[14][1] =  0. ,
    bi7[14][2] = -4.29672443178246482824254064733546854251 ,
    bi7[14][3] =  38.6444746111678092366406218271498656093 ,
    bi7[14][4] = -140.3503471762808981414524290552248895548 ,
    bi7[14][5] =  246.3954669697502467443139611011701827640 ,
    bi7[14][6] = -205.8341686964167118696204191085878165880 ,
    bi7[14][7] =  65.44129872356201885836080588282812631205 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[15]
    bi7[15][1] =  0. ,
    bi7[15][2] = -20.41628069294821485579834313809132051248 ,
    bi7[15][3] =  153.5213232524836445391962375168798263930 ,
    bi7[15][4] = -436.5502610211220460266289847121377276100 ,
    bi7[15][5] =  598.214644262650861959065070073603792110 ,
    bi7[15][6] = -398.7823950071290897160364203878571043995 ,
    bi7[15][7] =  104.0129692060648441002024406476025340187 ,
    //  --------------------------------------------------------
    //
    //  COEFFICIENTS OF  bi7[16]
    bi7[16][1] =  0. ,
    bi7[16][2] =  16.53007184264271512356106095760699278945 ,
    bi7[16][3] = -96.6861433615782065041742809436987893361 ,
    bi7[16][4] =  268.959934219531723149495873437076657635 ,
    bi7[16][5] = -428.681909788964647271837835032326719249 ,
    bi7[16][6] =  354.578231152433375494079868740183658991 ,
    bi7[16][7] = -114.7001840640649599911246871588418008302 ,
    //  --------------------------------------------------------
    // 
    //  COEFFICIENTS OF  bi7[17]
    bi7[17][1] =  0. ,
    bi7[17][2] = -18.63064171313429626683549958846959067803 ,
    bi7[17][3] =  164.1994112280183092456176460821337125030 ,
    bi7[17][4] = -579.272256249540441494196462569641132906 ,
    bi7[17][5] =  980.198255708866731505258442280896479501 ,
    bi7[17][6] = -786.224179015513894176220583239056456901 ,
    bi7[17][7] =  239.7294100413035911863764570341369884827 ;
    
    
    for(G4int i = 0; i< numberOfVariables; i++)
        yIn[i] = yInput[i];
    
    G4double tau0 = tau;
    //    Calculating the polynomials :

    for(int i=1; i<=17; i++){	//Here i is NOT the coordinate no. , it's stage no.
        b[i] = 0;
        tau = tau0;
        for(int j=1; j<=7; j++){
            b[i] += bi7[i][j]*tau;
            tau*=tau0;
        }
    }
    
    for(int i=0; i<numberOfVariables; i++){		//Here is IS the cooridnate no.
        yOut[i] = yIn[i] + Step*(b[1]*dydx[i] + b[2]*ak2[i] + b[3]*ak3[i] +
                                 b[4]*ak4[i] + b[5]*ak5[i] + b[6]*ak6[i] +
                                 b[7]*ak7[i] + b[8]*ak8[i] + b[9]*ak9[i] +
                                 b[10]*ak10[i] + b[11]*ak11[i] + b[12]*ak12[i] +
                                 b[13]*ak13[i] + b[14]*ak14[i] + b[15]*ak15[i] +
                                 b[16]*ak16[i] + b[17]*ak17[i]);
    }

    
}






