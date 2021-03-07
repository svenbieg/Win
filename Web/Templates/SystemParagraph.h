//===================
// SystemParagraph.h
//===================

#pragma once


//=======
// Using
//=======

#include "Web/Controls/Button.h"
#include "Web/Controls/Paragraph.h"


//===========
// Namespace
//===========

namespace Web {
	namespace Templates {


//==================
// System-Paragraph
//==================

class SystemParagraph: public Web::Controls::Paragraph
{
private:
	// Using
	using Button=Web::Controls::Button;

public:
	// Con-/Destructors
	SystemParagraph();

private:
	// Common
	VOID OnLogBoxClearClick(Handle<Button> Button);
};

}}