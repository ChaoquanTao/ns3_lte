#include "ns3/core-module.h"
#include "ns3/lte-module.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/animation-interface.h"
#include "ns3/box.h"
NS_LOG_COMPONENT_DEFINE("HelloSimulator");

using namespace ns3 ;
using namespace std ;

#define GRID_WIDTH 20 
#define ROW_INTERVAL 50 
#define COLUMN_INTERVAL 50

void CoutSeconds(){
	cout << Simulator::Now ().GetSeconds () << endl;
}


int main(int argc, char *argv[]){
	Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (320));

	/**控制trace时间间隔**/
	//Config::setDefault("ns3::LteEnbPhy::UeSinrSamplePeriod",UintegerValue(100));
	//Config::SetDefault("ns3::UdpClient::Interval",TimeValue(MilliSeconds(1)));
	//Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration",TimeValue(Seconds(1))) ;
	NodeContainer ueNodes ;
	NodeContainer enbNodes ;

	NodeContainer skyNodes ;

	MobilityHelper mobility ;

	enbNodes.Create(1);
	ueNodes.Create(400);
	skyNodes.Create(400) ;

	/*
	   Ptr<ListPositionAllocator> uePos = CreateObject<ListPositionAllocator>() ;
	   uePos->Add(Vector(100,100,35)) ;
	   uePos->Add(Vector(100,50,0)) ;
	   uePos->Add(Vector(0,0,10)) ;
	   */
	Ptr<ListPositionAllocator> enbPos = CreateObject<ListPositionAllocator>() ;
	enbPos->Add(Vector(500,500,100)) ;



	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue (COLUMN_INTERVAL/2),
			"MinY", DoubleValue (ROW_INTERVAL/2),
			"DeltaX", DoubleValue (COLUMN_INTERVAL),
			"DeltaY", DoubleValue (ROW_INTERVAL),
			"GridWidth", UintegerValue (GRID_WIDTH),
			"LayoutType", StringValue ("RowFirst"));


	for(unsigned int i=0; i<ueNodes.GetN(); i++){

		std::cout <<"left:"<< (i%GRID_WIDTH)*COLUMN_INTERVAL<<" right:"<<((i%GRID_WIDTH)+1)*COLUMN_INTERVAL<<" top:"<<(i/GRID_WIDTH)*ROW_INTERVAL<<" botom:"<<((i/GRID_WIDTH)+1)*ROW_INTERVAL<<std::endl ;

		mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
				"Mode", StringValue ("Time"),
				"Time", StringValue ("0.001s"),
				"Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
				"Bounds", RectangleValue (Rectangle ((i%GRID_WIDTH)*COLUMN_INTERVAL,((i%GRID_WIDTH)+1)*COLUMN_INTERVAL,(i/GRID_WIDTH)*ROW_INTERVAL,((i/GRID_WIDTH)+1)*ROW_INTERVAL)));

		mobility.Install(ueNodes.Get(i)) ;

	}


	/**
	 *设置天空节点属性
	 * */
	Ptr<ListPositionAllocator> skyPos = CreateObject<ListPositionAllocator>() ;
	for(unsigned int i=0; i<skyNodes.GetN(); i++){
		int row = i/GRID_WIDTH ;
		int column = i%GRID_WIDTH ;
		skyPos->Add(Vector(row*COLUMN_INTERVAL+(COLUMN_INTERVAL/2), column*ROW_INTERVAL+ROW_INTERVAL/2,50)) ;
		std::cout<<"x:"<<row*COLUMN_INTERVAL+(COLUMN_INTERVAL/2)<<" y:"<< column*ROW_INTERVAL+ROW_INTERVAL/2<<" z:"<<50<<std::endl ; 
	}
	mobility.SetPositionAllocator(skyPos) ;

	for(unsigned int i=0; i<skyNodes.GetN(); i++){
		/**
		  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
		  "Mode", StringValue ("Time"),
		  "Time", StringValue ("22s"),
		  "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
		  "Bounds", RectangleValue (Rectangle ((i%10)*20,((i%10)+1)*20,(i/10)*20,((i/10)+1)*20)));

		  mobility.Install(skyNodes.Get(i)) ;
		 **/


		mobility.SetMobilityModel ("ns3::GaussMarkovMobilityModel",
				"Bounds", BoxValue(Box((i%GRID_WIDTH)*COLUMN_INTERVAL, ((i%GRID_WIDTH)+1)*COLUMN_INTERVAL, (i/GRID_WIDTH)*ROW_INTERVAL, ((i/GRID_WIDTH)+1)*ROW_INTERVAL,50,50)),
				"TimeStep", TimeValue (Seconds (0.5)),
				"Alpha", DoubleValue (0.85),
				"MeanVelocity", StringValue ("ns3::UniformRandomVariable[Min=800|Max=1200]"),
				"MeanDirection", StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"),
				"MeanPitch", StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"),
				"NormalVelocity", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"),
				"NormalDirection", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"),
				"NormalPitch", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"));
		mobility.Install(skyNodes.Get(i)) ;
	}



	MobilityHelper mobility2 ;
	mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility2.SetPositionAllocator(enbPos) ;
	mobility2.Install(enbNodes) ;




	AnimationInterface::SetConstantPosition(enbNodes.Get(0),500,500,50) ;

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper>() ;
	NetDeviceContainer enbDevs ;
	NetDeviceContainer ueDevs ;
	NetDeviceContainer skyDevs ;

	enbDevs = lteHelper->InstallEnbDevice(enbNodes) ;
	ueDevs = lteHelper->InstallUeDevice(ueNodes) ;
	skyDevs = lteHelper->InstallUeDevice(skyNodes) ;

	lteHelper->Attach(ueDevs,enbDevs.Get(0)) ;
	lteHelper->Attach(skyDevs,enbDevs.Get(0)) ;

	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE ;
	EpsBearer bearer(q) ;
	lteHelper->ActivateDataRadioBearer(ueDevs,bearer);
	lteHelper->ActivateDataRadioBearer(skyDevs,bearer);

	std::cout<<"test1========="<<std::endl ;

	AnimationInterface anim("lte.xml");
	anim.SetMobilityPollInterval(Seconds(1)) ;
/**	for(uint32_t i=0; i<ueNodes.GetN(); i++){
		anim.UpdateNodeColor(ueNodes.Get(i),255,0,0) ;

	}
	for(uint32_t i=0; i<skyNodes.GetN(); i++){
		anim.UpdateNodeColor(skyNodes.Get(i),0,0,205) ;

	}
**/
	lteHelper->EnablePhyTraces() ;
	//lteHelper->EnableMacTraces() ;

	std::cout<<"test2========="<<std::endl ;

	Simulator::Stop(Seconds(10)) ;
	Simulator::Schedule(Seconds(1), &CoutSeconds);
	Simulator::Run();
	//	Simulator::Destroy() ;
	NS_LOG_UNCOND("Hello Simulator");
	//	return 0 ;
}
