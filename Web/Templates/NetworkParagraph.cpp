//======================
// NetworkParagraph.cpp
//======================

#include "pch.h"


//=======
// Using
//=======

#include "Network/Connection.h"
#include "Web/Controls/Tables/Table.h"
#include "Web/Controls/Caption.h"
#include "Web/Controls/Label.h"
#include "Web/Controls/TextBlock.h"
#include "NetworkParagraph.h"

using namespace Web::Controls;
using namespace Web::Controls::Tables;


//===========
// Namespace
//===========

namespace Web {
	namespace Templates {


//==================
// Con-/Destructors
//==================

NetworkParagraph::NetworkParagraph()
{
auto hcon=Network::Connection::Current;
if(!hcon)
	{
	Parent->Select();
	return;
	}
new Caption("Netzwerk", 2);
Handle<Table> htable=new Table("simple");
	new Row(); new Cell("static"); new TextBlock("Ip-Adresse"); new Cell("num"); new Label(hcon->StationIp);
	new Row(); new Cell("static"); new TextBlock("Gateway"); new Cell("num"); new Label(hcon->StationGateway);
	new Row(); new Cell("static"); new TextBlock("Subnetz"); Handle<Cell> hcell=new Cell("num"); new Label(hcon->StationSubnet);
		hcell->Style="width:100px";
htable->Break();
Parent->Select();
}

}}