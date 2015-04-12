//Exemplo parecido com o IPERF, adicionando um roteador entre os hosts.
#include "ns3/network-module.h" //Possui os recursos básicos de rede
#include "ns3/dce-module.h"	//Adiciona o suporte ao DCE
#include "ns3/core-module.h" //Possui os sistemas básicos de funcionamento do NS-3, como o sistema de tracing
#include "ns3/point-to-point-module.h" //Funções em csma, adiciona o helper de criação dos meios
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h" //Para a visualização do modo --viz
#include "ns3/internet-module.h" //Possui as pilhas neccessárias para funcionamento da rede, como Ethernet, IP, TCP e etc.
//#include "ns3/global-route-module.h" Retirei devido a complexidade de utilizar um roteador, será um nó mesmo por enquanto
//#include "ns3/ipv4-global-routing.h" //Cria as tabelas de roteamento automaticamente nos hosts

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("Teste de IPERF com um roteador entre os nós");

int main (int argc, char *argv[]){
	//Necessário para que as interfaces respondam aos eventos
	//Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

	std::string banda = "1Mbps";
	CommandLine cmd;
	cmd.AddValue ("banda", "Largura de banda default", banda);
	cmd.Parse (argc, argv);

	NS_LOG_INFO("Criando os hosts e o roteador");
	NodeContainer hosts;
	hosts.Create (3);
	//NodeContainer h1r0 = (hosts.Get(1), hosts.Get(0));
	//NodeContainer h2r0 = (hosts.Get(2), hosts.Get(0));
	
	NS_LOG_INFO("Criando o meio de comunicação");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (banda));
	p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
	
	NetDeviceContainer devH1, devH2;
	devH1 = p2p.Install(hosts.Get(0), hosts.Get(1));
	devH2 = p2p.Install(hosts.Get(2), hosts.Get(1));
	
	DceManagerHelper dceM;
        dceM.SetTaskManagerAttribute("FiberManagerType", StringValue ("UcontextFiberManager"));
	
        InternetStackHelper internet;
        dceM.Install(hosts);
        internet.Install(hosts);

	
	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("192.168.0.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(devH1);

	ipv4.SetBase ("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces2 = ipv4.Assign(devH2);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	DceApplicationHelper dce;
	ApplicationContainer binario;

	dce.SetBinary ("iperf");
	dce.ResetArguments();
	dce.ResetEnvironment();
	dce.AddArgument("-c");
	dce.AddArgument("192.168.1.1");
	dce.AddArgument("-i");
	dce.AddArgument("1");
	dce.AddArgument("--time");
	dce.AddArgument("10");

	binario = dce.Install(hosts.Get(0));
	binario.Start(Seconds (1));
	binario.Stop(Seconds (10));

	dce.SetBinary ("iperf");
	dce.ResetArguments ();
	dce.ResetEnvironment ();
	dce.AddArgument("-s");
	dce.AddArgument("-P");
	dce.AddArgument("1");
	binario = dce.Install(hosts.Get(2));
	binario.Start(Seconds (0.5));
	binario.Stop(Seconds (12));

	Simulator::Stop(Seconds(60.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;

}
