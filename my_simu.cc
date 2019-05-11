#include "ns3/core-module.h"
#include "ns3/lte-module.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/animation-interface.h"

NS_LOG_COMPONENT_DEFINE("HelloSimulator");

using namespace ns3 ;
using namespace std ;

void CoutSeconds(){
    cout << Simulator::Now ().GetSeconds () << endl;
}


int main(int argc, char *argv[]){
Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));

/**控制trace时间间隔**/
Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration",TimeValue(Seconds(1))) ;
	NodeContainer ueNodes ;
	NodeContainer enbNodes ;

	MobilityHelper mobility ;

	enbNodes.Create(1);
	ueNodes.Create(100);
/*
	Ptr<ListPositionAllocator> uePos = CreateObject<ListPositionAllocator>() ;
	uePos->Add(Vector(100,100,35)) ;
	uePos->Add(Vector(100,50,0)) ;
	uePos->Add(Vector(0,0,10)) ;
*/
	Ptr<ListPositionAllocator> enbPos = CreateObject<ListPositionAllocator>() ;
	enbPos->Add(Vector(50,50,50)) ;


	
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (10.0),
                                  "MinY", DoubleValue (10.0),
                                  "DeltaX", DoubleValue (20.0),
                                  "DeltaY", DoubleValue (20.0),
                                  "GridWidth", UintegerValue (10),
                                  "LayoutType", StringValue ("RowFirst"));

/*
	mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Mode", StringValue ("Time"),
                              "Time", StringValue ("s"),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                              "Bounds", RectangleValue (Rectangle (0.0,200.0,0.0,200.0)));
	
		mobility.Install(ueNodes) ;
*/

	for(unsigned int i=0; i<ueNodes.GetN(); i++){
		


		std::cout <<"left:"<< (i%10)*20<<" right:"<<((i%10)+1)*20<<" top:"<<(i/10)*20<<" botom:"<<((i/10)+1)*20<<std::endl ;

 		mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Mode", StringValue ("Time"),
                              "Time", StringValue ("22s"),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                              "Bounds", RectangleValue (Rectangle ((i%10)*20,((i%10)+1)*20,(i/10)*20,((i/10)+1)*20)));
	
		mobility.Install(ueNodes.Get(i)) ;

	}

	/**
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator(uePos) ;
	mobility.Install(ueNodes) ;
**/

	MobilityHelper mobility2 ;
	mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility2.SetPositionAllocator(enbPos) ;
	mobility2.Install(enbNodes) ;

	/**
	AnimationInterface::SetConstantPosition(ueNodes.Get(0),100,100,35) ;
	AnimationInterface::SetConstantPosition(ueNodes.Get(1),100,50,0) ;
	AnimationInterface::SetConstantPosition(ueNodes.Get(2),0,0,10) ;
**/

	

	AnimationInterface::SetConstantPosition(enbNodes.Get(0),50,50,50) ;

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper>() ;
	NetDeviceContainer enbDevs ;
	NetDeviceContainer ueDevs ;

	enbDevs = lteHelper->InstallEnbDevice(enbNodes) ;
	ueDevs = lteHelper->InstallUeDevice(ueNodes) ;

	lteHelper->Attach(ueDevs,enbDevs.Get(0)) ;

	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE ;
	EpsBearer bearer(q) ;
	lteHelper->ActivateDataRadioBearer(ueDevs,bearer);

	std::cout<<"test1========="<<std::endl ;

	AnimationInterface anim("lte.xml");
	anim.SetMobilityPollInterval(Seconds(1)) ;
	for(uint32_t i=0; i<ueNodes.GetN(); i++){
		anim.UpdateNodeColor(ueNodes.Get(i),255,0,0) ;

	}

	lteHelper->EnablePhyTraces() ;
	std::cout<<"test2========="<<std::endl ;
	
	Simulator::Stop(Seconds(10)) ;
//	Simulator::Schedule(Seconds(1), &CoutSeconds);
	Simulator::Run();
//	Simulator::Destroy() ;
	NS_LOG_UNCOND("Hello Simulator");
//	return 0 ;
}
