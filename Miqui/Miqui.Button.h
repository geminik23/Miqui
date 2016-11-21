#pragma once

#include "BaseHeader.h"


namespace Miqui
{


class BaseButton: public ContentControl
{
public:
	using ClickEvent = Miqui::SignalEvent<Miqui::IControl *>;
	using HoverEvent = Miqui::SignalEvent<Miqui::IControl *>;

	ClickEvent Clicked;
	HoverEvent Hovered;

	virtual void OnPointerEvent(PointerEvent&)override;

	bool IsHovered() const noexcept { return m_hovered; }
	bool IsPressed() const noexcept { return m_pressed; }

protected:
	virtual void OnPointHovered() {}
	virtual void OnClicked() {}

	bool m_pressed{ false };
	bool m_hovered{ false };

private:
	void _Clicked();
	void _Hovered();
};


}