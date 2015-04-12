//Primeiro exemplo testando IPERF emulado entre dois hosts ligados diretamente
#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/dce-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Exemplo IPERF emulado via DCE");

int main (int argc, char *argv[]) {
	
	std::string banda = "1Mbps";
	CommandLine cmd;
	cmd.AddValue ("banda", "Largura da banda padrão: 1 Megabit", banda);
	cmd.Parse (argc, argv);

	NS_LOG_INFO("Criando os nós");
	NodeConteiner hosts;
	hosts.Create (2);

	NS_LOG_INFO("Criando os canais de comunicação");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (banda));
	p2p.SetChannelAttibute ("Delay", StringValue ("1ms"));

	NS_LOG_INFO("Criando as interfaces de comunicação e adicionando aos hosts");
	NetDeviceContainer dev;
	dev = p2p.Install(hosts);

	NS_LOG_INFO("Adicionando o Helper DCE e um FiberManager que controla o contexto dos processos executados pelo DCE");
	DceManagerHelper dce;
	dce.SetTaskManagerAttribute ("FiberManagerType", StringValue ("UcontextFiberManager"));
	
	//Instalando as pilhas de protocolos e o DCE nos hosts que vão executar
	NS_LOG_INFO("Adicionando pilhas de protocolos Ipv4 e etc nos nós E a aplicação DCE");
	InternetStackHelper pilha;
	pilha.Install (hosts);
	dce.Install (hosts);

	//Adicionando endereços IP aos nós de forma específica e associando a uma interface no nó
	NS_LOG_INFO("Adicionando endereços as interfaces");
	Ipv4AddressHelper enderecos;
	enderecos.SetBase ("192.168.0.0", "255.255.255.0");
	enderecos.Assign (dev);
	
	//Um único set de devices com o mesmo endereço
	DceApplicationHelper dceH;
	ApplicationConteiner programa;

	dceH.SetStackSize (1 << 20);

	
	NS_LOG_INFO("Iniciando o iperf no nó 0");
	//Adicionando o Iperf em nó 0
	dceH.SetBinary("iperf");
	dceH.ResetArguments();
	dceH.ResetEnvironment();
	dceH.AddArgument ("-c");
	dceH.AddArgument ("192.168.0.2");
	dceH.AddArgument ("-i");
	dceH.AddArgument ("1");
	dceH.AddArgument ("--time");
	dceH.AddArgument ("25");

	programa = dceH.Install(hosts.Get(0));//Nó zero cliente
	programa.Start (Seconds (1));
	programa.Stop (Seconds (35));

	NS_LOG_INFO("Iniciando o iperf como servidor em nó 1");
	//Adicionando o Iperf em nó 1 como servidor
	dceH.Setbinary("iperf");
	dceH.ResetArguments();
	dceH.ResetEnvironment();
	dceH.AddArgument ("-s");
	dceH.AddArgument ("-P");
	dceH.AddArgument ("1");

	programa = dceH.Install(hosts.Get(1));//Nó um servidor
	programa.Start (Seconds (0.5));
	
	//Captura de pacotes desativada
	//p2p.EnablePcapAll("iperf-ns3", false);

	Simulator::Stop(Seconds (60.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
