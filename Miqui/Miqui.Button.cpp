#include "pch.h"
#include "Miqui.Button.h"


using namespace Miqui;


void BaseButton::OnPointerEvent(PointerEvent& pe)
{
	ContentControl::OnPointerEvent(pe);

	switch (pe.Mode())
	{
	case PointerMode::Presssed:
		this->m_pressed = true;
		pe.Handled(true);
		break;
	case PointerMode::Moved:
		if (!m_pressed) { m_hovered = true; this->_Hovered(); } else pe.Handled(true);
		break;
	case PointerMode::Released:
		this->_Clicked();
	case PointerMode::Exited:
	case PointerMode::Canceled:
		this->m_pressed = false;
		this->m_hovered = false;
		break;
	}
	Invalidate();
}

void BaseButton::_Clicked()
{
	this->OnClicked();
	this->Clicked(this);
}

void BaseButton::_Hovered()
{
	this->OnPointHovered();
	this->Hovered(this);
}



