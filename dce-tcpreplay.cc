//Exemplo utilizando o tcpreplay para gerar o tráfego entre dois pontos de forma simples
#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("tcpreplay e tcpdump");

int main (int argc, char *argv[]){

	std::string banda = "5Mbps";
	CommandLine cmd;
	cmd.AddValue("banda", "largura de banda padrao: 5Mbps", banda);
	cmd.Parse(argc,argv);

	NodeContainer node;
	node.Create(2);


	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
	csma.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer dev;
	dev = csma.Install (node);

	DceManagerHelper dceMan;
	dceMan.SetTaskManagerAttribute("FiberManagerType", StringValue ("UcontextFiberManager"));

	InternetStackHelper internet;
	internet.Install(node);
	dceMan.Install(node);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("192.168.0.0", "255.255.255.0");
	ipv4.Assign(dev);

	DceApplicationHelper dce;
	ApplicationContainer app;

	dce.SetBinary("tcpreplay");
	dce.ResetArguments();
	dce.ResetEnvironment();
	dce.AddArgument("-K");
	dce.AddArgument("-P");
	dce.AddArgument("-t");
	dce.AddArgument("-i");
	dce.AddArgument("eth0");
	dce.AddArgument("/home/dce/smallFlows.pcap");

	app = dce.Install(node.Get(0));
	app.Start(Seconds (1));
	app.Stop(Seconds (10));

	dce.SetBinary("tcpdump");
	dce.ResetArguments();
	dce.ResetEnvironment();
	app = dce.Install(node.Get(1));
	app.Start(Seconds (0.5));
	app.Stop(Seconds (15));

	csma.EnablePcapAll("tcpreplay-", true);

	Simulator::Stop(Seconds (30));
	Simulator::Run();
	Simulator::Destroy();


	return 0;



}
