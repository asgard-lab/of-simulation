//Prospecto do que seria a simulação com o SoftSwitch compilado para o DCE
#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "netanim.h"
#include "ns3/fd-net-device-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DCE Openflow SoftSwitch");

int main (int argc, char *argv[]) {

	std::string banda = "5Mbps";

	NodeContainer node;
	EmuFdNetDeviceHelper emu;
	NetDeviceContainer dev;
	CsmaHelper csma;

	nodes.Create(3);
	emu.SetDeviceName("eth0");
	dev = emu.Install (nodes.Get (0), nodes.Get(2));
	Ptr<NetDevice> device = dev.Get(0);
	device = dev.Get(2);

	device->SetAttribute ("Address", Mac48AddressValue (Mac48Address::Allocate ()));

	DceManagerHelper dceM;
	dceM.SetTaskManagerAttribute ("FiberManagerType", StringValue ("UcontextFiberManager"));
	
	InternetStackHelper internet;
	internet.Install(node);
	dceM.Install(node);

	Ipv4AddressHelper ipv4;
  	ipv4.SetBase ("192.168.0.0", "255.255.255.0");
  	Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

  	DceApplicationHelper dce;
  	ApplicationContainer app;

  	dce.SetBinary("ofdatapath");
  	dce.ResetArguments();
  	dce.ResetEnviroment();
  	dce.AddArgument("--detach");
  	dce.AddArgument("punix:/var/run/dp0");
  	dce.AddArgument("--datapath-id=000000000001");
  	dce.AddArgument("--interfaces=eth0,eth0");//Neste ponto, talvez modificar para os sockets de comunicação ou ver se existe um método node.Get(EmuInterface)
  	dce.AddArgument("ptcp:6633");
  	dce.AddArgument("ofprotocol");
  	dce.AddArgument("unix:/var/run/dp0");
  	dce.AddArgument("tcp:192.168.0.10:6633");//Tentar compilar o Beacon como uma ferramenta do DCE, adicionar CmdValue para indiciar o controlador, talvez via socket também

  	app = dce.Install(node.Get(1));
  	app.Start(Seconds (0.2));
  	app.Stop(Seconds (80));

  	dce.SetBinary("iperf");
	dce.ResetArguments();
	dce.ResetEnviroment();
	dce.AddArgument ("-c");
	dce.AddArgument ("192.168.0.2");
	dce.AddArgument ("-i");
	dce.AddArgument ("1");
	dce.AddArgument ("--time");
	dce.AddArgument ("25");

	programa = dce.Install(hosts.Get(0));
	programa.Start (Seconds (1));
	programa.Stop (Seconds (35));

	dce.Setbinary("iperf");
	dce.ResetArguments();
	dce.ResetEnviroment();
	dce.AddArgument ("-s");
	dce.AddArgument ("-P");
	dce.AddArgument ("1");

	programa = dce.Install(hosts.Get(1));
	programa.Start (Seconds (0.5));


	emu.EnablePcap("openflow-emuNetDevice", dev.Get(1), true);

	Simulator::Stop(Seconds (120));
	Simulator::Run();
	Simulator::Destroy();

	return 0;

}
